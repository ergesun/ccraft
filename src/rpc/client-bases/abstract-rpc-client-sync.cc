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
            m_pSocketService->Start(m_iSSThreadsCnt, net::NonBlockingEventModel::Posix);
        }

        bool ARpcClientSync::Stop() {
            if (m_bStopped) {
                return true;
            }

            m_bStopped = true;
            hw_rw_memory_barrier();
            m_pSocketService->Stop();
        }

        bool ARpcClientSync::RegisterRpc(std::string &&rpcName, uint16_t id) {
            if (m_bRegistered) {
                return false;
            }

            m_hmapRpcs[std::move(rpcName)] = id;
            return true;
        }

        void ARpcClientSync::FinishRegisterRpc() {
            m_bRegistered = true;
            hw_rw_memory_barrier();
        }

        RpcCtx* ARpcClientSync::sendMessage(std::string &&rpcName, std::shared_ptr<google::protobuf::Message> msg,
                                             net::net_peer_info_t &&peer) {
            if (m_hmapRpcs.end() == m_hmapRpcs.find(rpcName)) {
                throw BadRpcException((uint16_t)RpcCode::ErrorNoRegisteredRpc, std::move(rpcName));
            }

            auto rcPeer = peer;
            auto rr = new RpcRequest(m_pMemPool, std::move(peer), m_hmapRpcs[rpcName], std::move(msg));
            if (UNLIKELY(!m_pSocketService->SendMessage(rr))) {
                DELETE_PTR(rr);
                return nullptr;
            }

            auto rc = m_rpcCtxPool.Get();
            rc->peer = std::move(rcPeer);
            rc->id = rr->GetId();

            return rc;
        }

        std::shared_ptr<net::NotifyMessage> ARpcClientSync::recvMessage(RpcCtx* rc) {
            common::SpinLock l(&m_slRpcCtxs);
            {
                m_hmapRpcCtxs[rc->id] = rc;
                if (m_hmapPeerRpcs.end() == m_hmapPeerRpcs.find(rc->peer)) {
                    std::set<RpcCtx*> rpcs { rc };
                    m_hmapPeerRpcs[rc->peer] = std::move(rpcs);
                } else {
                    m_hmapPeerRpcs[rc->peer].insert(rc);
                }
            }

            std::unique_lock<std::mutex> ll(*(rc->mtx));
            //common::g_pTimer->SubscribeEventAfter()
            while (!rc->complete) {
                rc->cv->wait(ll);
            }

            auto nm = rc->ssp_nm;
            m_rpcCtxPool.Put(rc);

            return std::move(nm);
        }

        void ARpcClientSync::recv_net_msg(std::shared_ptr<net::NotifyMessage> sspNM) {
            switch (sspNM->GetType()) {
                case net::NotifyMessageType::Message: {
                    auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());
                    auto rm = mnm->GetContent();
                    if (LIKELY(rm)) {
                        auto id = rm->GetId();
                        RpcCtx *ctx = nullptr;
                        common::SpinLock l(&m_slRpcCtxs);
                        {
                            if (m_hmapRpcCtxs.end() == m_hmapRpcCtxs.find(id)) {
                                LOGWFUN << "recv is not in Id<->RpcCtx map message for id " << id;
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
                            ctx->ssp_nm = std::move(sspNM);
                        }

                        ctx->cv->notify_one();
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
                        common::SpinLock l(&m_slRpcCtxs);
                        {
                            //for ()
                        }
                    }
                    break;
                }
                default: {

                }
            }
        }
    }
}
