/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../ccsys/gcc-buildin.h"
#include "../common/global-vars.h"
#include "../net/notify-message.h"
#include "../net/rcv-message.h"

#include "common-def.h"
#include "response.h"
#include "exceptions.h"

#include "abstract-rpc-sync-client.h"

namespace ccraft {
namespace rpc {
std::shared_ptr<net::NotifyMessage> ARpcSyncClient::sendMessage(std::string &&rpcName,
                                                     std::shared_ptr<google::protobuf::Message> msg,
                                                     net::net_peer_info_t &&peer) {
    auto sr = ARpcClient::SendMessageAsync(std::move(rpcName), std::move(msg), std::move(peer));
    if (0 == sr.msgId) {
        return std::shared_ptr<net::NotifyMessage>(nullptr);
    }

    auto ctx = m_rpcCtxPool.Get();
    ctx->peer = std::move(sr.peer);
    ctx->msgId = sr.msgId;

    return recvMessage(ctx);
}

void ARpcSyncClient::onRecvMessage(std::shared_ptr<net::NotifyMessage> sspNM) {
    switch (sspNM->GetType()) {
        case net::NotifyMessageType::Message: {
            auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());
            auto rm = mnm->GetContent();
            if (LIKELY(rm)) {
                auto id = rm->GetId();
                RpcCtx *rc = nullptr;
                {
                    ccsys::SpinLock l(&m_slRpcCtxs);
                    if (m_hmapRpcCtxs.end() == m_hmapRpcCtxs.find(id)) {
                        LOGIFUN << "recv is not in Id<->RpcCtx map message for id " << id;
                        break;
                    }

                    rc = m_hmapRpcCtxs[id];
                    common::g_pTimer->UnsubscribeEvent(rc->timer_ev);
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
                    ccsys::SpinLock l(&m_slRpcCtxs);
                    if (m_hmapPeerRpcs.end() != m_hmapPeerRpcs.find(peer)) {
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
            }
            break;
        }
        case net::NotifyMessageType::Server: {
            LOGFFUN << "Rpc client shouldn't get NotifyMessageType::Server msg.";
        }
    }
}

std::shared_ptr<net::NotifyMessage> ARpcSyncClient::recvMessage(RpcCtx *rc) {
    auto id = rc->msgId;
    {
        ccsys::SpinLock l(&m_slRpcCtxs);
        m_hmapRpcCtxs[id] = rc;
        if (m_hmapPeerRpcs.end() == m_hmapPeerRpcs.find(rc->peer)) {
            std::set<RpcCtx*> rpcs { rc };
            m_hmapPeerRpcs[rc->peer] = std::move(rpcs);
        } else {
            m_hmapPeerRpcs[rc->peer].insert(rc);
        }
    }

    static ccsys::Timer::TimerCallback s_cb = [id, this](void*) {
        RpcCtx *ctx = nullptr;
        {
            ccsys::SpinLock l(&m_slRpcCtxs);
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
        ccsys::Timer::Event ev(nullptr, &s_cb);
        auto eventId = common::g_pTimer->SubscribeEventAfter(m_timeout, ev);
        if (0 == eventId.when) {
            throw RpcClientInternalException();
        }

        rc->timer_ev = eventId;
        while (!rc->complete) {
            rc->cv->wait(ll);
        }

        switch (rc->state) {
            case RpcCtx::State::Timeout: {
                m_rpcCtxPool.Put(rc);
                throw RpcClientSendTimeoutException();
            }
            case RpcCtx::State::BrokenPipe: {
                m_rpcCtxPool.Put(rc);
                throw RpcBrokenPipeException();
            }
            default: {
                break;
            }
        }
    }

    auto sspNM = rc->ssp_nm;
    m_rpcCtxPool.Put(rc);
    auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());
    auto rm = mnm->GetContent();
    auto buffer = rm->GetDataBuffer();
    auto serverCode = ByteOrderUtils::ReadUInt16(buffer->GetPos());
    buffer->MoveHeadBack(sizeof(rpc::RpcCodeType));
    auto handlerId = ByteOrderUtils::ReadUInt16(buffer->GetPos());
    switch ((RpcCode)serverCode) {
        case RpcCode::ErrorNoHandler:{
            throw BadRpcHandlerIdException(serverCode, handlerId);
        }
        case RpcCode::ErrorMsg:{
            throw RpcMessageCorruptException(handlerId);
        }
        case RpcCode::ErrorInternal:{
            throw RpcServerInternalException();
        }
        default:{
            break;
        }
    }

    return std::move(sspNM);
}
}
}