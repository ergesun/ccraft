/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../common/common-def.h"
#include "../../common/global-vars.h"
#include "../../common/timer.h"
#include "../../common/codec-utils.h"
#include "../../common/buffer.h"

#include "../request.h"
#include "../../net/socket-service-factory.h"
#include "../../net/net-protocal-stacks/msg-worker-managers/unique-worker-manager.h"
#include "../../net/rcv-message.h"

#include "abstract-rpc-client-sync.h"
#include "../exceptions.h"
#include "../common-def.h"

namespace ccraft {
namespace rpc {
ARpcClientSync::ARpcClientSync(uint16_t socketServiceThreadsCnt, common::cctime_t timeout, uint16_t logicPort) :
    m_iSSThreadsCnt(socketServiceThreadsCnt), m_timeout(timeout) {
    if (0 == m_iSSThreadsCnt) {
        m_iSSThreadsCnt = (uint16_t)(common::PHYSICAL_CPUS_CNT);
    }

    m_pMemPool = new common::MemPool();
    std::shared_ptr<net::net_addr_t> sspNat;
    std::shared_ptr<net::INetStackWorkerManager> sspMgr = std::shared_ptr<net::INetStackWorkerManager>(new net::UniqueWorkerManager());
    m_pSocketService = net::SocketServiceFactory::CreateService(net::SocketProtocal::Tcp,
                                                                sspNat, logicPort, m_pMemPool,
                                                                std::bind(&ARpcClientSync::recv_net_msg, this, std::placeholders::_1),
                                                                sspMgr);
}

ARpcClientSync::~ARpcClientSync() {
    DELETE_PTR(m_pSocketService);
    DELETE_PTR(m_pMemPool);
}

bool ARpcClientSync::Start() {
    if (!m_bStopped || !m_bRegistered) {
        return false;
    }

    m_bStopped = false;
    // TODO(sunchao): 等加了其他model，此处及类似的地方设置成可配置的。
    return m_pSocketService->Start(m_iSSThreadsCnt, net::NonBlockingEventModel::Posix);
}

bool ARpcClientSync::Stop() {
    if (m_bStopped) {
        return true;
    }

    m_bStopped = true;
    hw_rw_memory_barrier();

    return m_pSocketService->Stop();
}

bool ARpcClientSync::registerRpc(std::string &&rpcName, uint16_t id) {
    if (m_bRegistered) {
        return false;
    }

    m_hmapRpcs[std::move(rpcName)] = id;
    return true;
}

void ARpcClientSync::finishRegisterRpc() {
    m_bRegistered = true;
    hw_rw_memory_barrier();
}

ARpcClientSync::RpcCtx* ARpcClientSync::sendMessage(std::string &&rpcName, SP_PB_MSG msg,
                                     net::net_peer_info_t &&peer) {
    if (m_hmapRpcs.end() == m_hmapRpcs.find(rpcName)) {
        throw BadRpcException((uint16_t)RpcCode::ErrorNoRegisteredRpc, std::move(rpcName));
    }

    net::net_peer_info_t rcPeer = peer;
    auto handlerId = m_hmapRpcs[rpcName];
    auto rr = new RpcRequest(m_pMemPool, std::move(peer), handlerId, std::move(msg));
    if (UNLIKELY(!m_pSocketService->SendMessage(rr))) {
        DELETE_PTR(rr);
        return nullptr;
    }

    auto rc = m_rpcCtxPool.Get();
    rc->peer = std::move(rcPeer);
    rc->msgId = rr->GetId();
    rc->handlerId = handlerId;

    return rc;
}

std::shared_ptr<net::NotifyMessage> ARpcClientSync::recvMessage(RpcCtx* rc) {
    auto id = rc->msgId;
    {
        common::SpinLock l(&m_slRpcCtxs);
        m_hmapRpcCtxs[id] = rc;
        if (m_hmapPeerRpcs.end() == m_hmapPeerRpcs.find(rc->peer)) {
            std::set<RpcCtx*> rpcs { rc };
            m_hmapPeerRpcs[rc->peer] = std::move(rpcs);
        } else {
            m_hmapPeerRpcs[rc->peer].insert(rc);
        }
    }

    static common::Timer::TimerCallback s_cb = [id, this](void*) {
        RpcCtx *ctx = nullptr;
        {
            common::SpinLock l(&m_slRpcCtxs);
            if (m_hmapRpcCtxs.end() == m_hmapRpcCtxs.find(id)) {
                LOGIFUN << "recv is not in Id<->RpcCtx map message for id " << id;
                return;
            }

            ctx = m_hmapRpcCtxs[id];
            m_hmapRpcCtxs.erase(id);
            m_hmapPeerRpcs[ctx->peer].erase(ctx);
            if (m_hmapPeerRpcs[ctx->peer].empty()) {
                m_hmapPeerRpcs.erase(ctx->peer);
            }
        }
        {
            std::unique_lock<std::mutex> ll(*(ctx->mtx));
            ctx->complete = true;
            ctx->state = RpcCtx::State::Timeout;
        }

        ctx->cv->notify_one();
    };

    {
        std::unique_lock<std::mutex> ll(*(rc->mtx));
        common::Timer::Event ev(nullptr, &s_cb);
        auto eventId = common::g_pTimer->SubscribeEventAfter(m_timeout, ev);
        if (0 == eventId.when) {
            throw RpcClientInternalException();
        }

        while (!rc->complete) {
            rc->cv->wait(ll);
        }

        switch (rc->state) {
            case RpcCtx::State::Timeout: {
                throw RpcClientSendTimeoutException();
            }
            case RpcCtx::State::BrokenPipe: {
                throw RpcBrokenPipeException();
            }
            default: {
                break;
            }
        }
    }

    auto handlerId = rc->handlerId;
    auto sspNM = rc->ssp_nm;
    m_rpcCtxPool.Put(rc);
    auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());
    auto rm = mnm->GetContent();
    auto pos = rm->GetDataBuffer()->GetPos();
    auto serverCode = ByteOrderUtils::ReadUInt16(pos);
    switch ((RpcCode)serverCode) {
        case RpcCode::ErrorNoHandler: {
            throw BadRpcHandlerIdException(serverCode, handlerId);
        }
        case RpcCode::ErrorMsg: {
            throw RpcMessageCorruptException(handlerId);
        }
        case RpcCode::ErrorInternal: {
            throw RpcServerInternalException();
        }
        default:{
            break;
        }
    }

    return std::move(sspNM);
}

void ARpcClientSync::recv_net_msg(std::shared_ptr<net::NotifyMessage> sspNM) {
    switch (sspNM->GetType()) {
        case net::NotifyMessageType::Message: {
            auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());
            auto rm = mnm->GetContent();
            if (LIKELY(rm)) {
                auto id = rm->GetId();
                RpcCtx *rc = nullptr;
                {
                    common::SpinLock l(&m_slRpcCtxs);
                    if (m_hmapRpcCtxs.end() == m_hmapRpcCtxs.find(id)) {
                        LOGIFUN << "recv is not in Id<->RpcCtx map message for id " << id;
                        return;
                    }

                    rc = m_hmapRpcCtxs[id];
                    //common::g_pTimer->UnsubscribeEvent(rc->timer_ev);
                    m_hmapRpcCtxs.erase(id);
                    m_hmapPeerRpcs[rc->peer].erase(rc);
                    if (m_hmapPeerRpcs[rc->peer].empty()) {
                        m_hmapPeerRpcs.erase(rc->peer);
                    }
                }
                {
                    std::unique_lock<std::mutex> ll(*(rc->mtx));
                    rc->complete = true;
                    rc->ssp_nm = std::move(sspNM);
                }

                rc->cv->notify_one();
                LOGDFUN2("handled message for id ", id);
            } else {
                LOGWFUN << "recv message is empty!";
            }
            break;
        }
        case net::NotifyMessageType::Worker: {
            auto *wnm = dynamic_cast<net::WorkerNotifyMessage*>(sspNM.get());
            if (wnm) {
                LOGEFUN << "rc = " << (int)wnm->GetCode() << ", message = " << wnm->What();
                auto peer = wnm->GetPeer();
                {
                    common::SpinLock l(&m_slRpcCtxs);
                    if (m_hmapPeerRpcs.end() != m_hmapPeerRpcs.find(peer))
                    for (auto rc : m_hmapPeerRpcs[peer]) {
                        m_hmapRpcCtxs.erase(rc->msgId);
                        common::g_pTimer->UnsubscribeEvent(rc->timer_ev);
                        {
                            std::unique_lock<std::mutex> ll(*(rc->mtx));
                            rc->complete = true;
                            rc->state = RpcCtx::State::BrokenPipe;
                            rc->ssp_nm = std::move(sspNM);
                        }
                        rc->cv->notify_one();
                    }

                    m_hmapPeerRpcs[peer].clear();
                    m_hmapPeerRpcs.erase(peer);
                }
            }
            break;
        }
        case net::NotifyMessageType::Server: {
            LOGFFUN << "Rpc client shouldn't get NotifyMessageType::Server msg.";
        }
    }
}
} // namespace rpc
} // namespace ccraft
