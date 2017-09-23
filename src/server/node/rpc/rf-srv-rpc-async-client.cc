/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../../net/notify-message.h"
#include "../../../net/rcv-message.h"
#include "../../../common/protobuf-utils.h"
#include "../../../common/buffer.h"
#include "../../../common/global-vars.h"
#include "../../../common/timer.h"
#include "../../../common/codec-utils.h"
#include "../../../codegen/node-raft.pb.h"
#include "../../../rpc/common-def.h"

#include "common-def.h"
#include "exceptions.h"

#include "rf-srv-rpc-async-client.h"

using ccraft::rpc::RpcCode;

#define ImplRfNodeRpcWithPeer(RpcName)                                                                  \
std::shared_ptr<protocal::RpcName##Response>                                                            \
    RfSrvInternalRpcClientAsync::RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {             \
        auto tmpPeer = peer;                                                                            \
        auto ctx = sendMessage(#RpcName, std::move(req), std::move(peer));                              \
        auto sspNM = recv_message(ctx);                                                                 \
        auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());                              \
        auto rm = mnm->GetContent();                                                                    \
        auto RpcName##Resp__Impl_DEF_TMP = new protocal::RpcName##Response();                           \
        auto buf = rm->GetDataBuffer();                                                                 \
        buf->MoveHeadBack(sizeof(uint16_t));                                                            \
        common::ProtoBufUtils::Deserialize(buf, RpcName##Resp__Impl_DEF_TMP);                           \
        return std::shared_ptr<protocal::RpcName##Response>(RpcName##Resp__Impl_DEF_TMP);               \
    }

namespace ccraft {
    namespace server {
        ImplRfNodeRpcWithPeer(AppendRfLog)
        ImplRfNodeRpcWithPeer(RequestVote)

        bool RfSrvInternalRpcClientAsync::register_rpc_handlers() {
            if (!registerRpc(RpcAppendRfLog, APPEND_RFLOG_RPC_ID)) {
                return false;
            }

            if (!registerRpc(RpcRequestVote, REQUEST_VOTE_RPC_ID)) {
                return false;
            }

            finishRegisterRpc();

            return true;
        }

        bool RfSrvInternalRpcClientAsync::onStart() {
            return register_rpc_handlers();
        }

        bool RfSrvInternalRpcClientAsync::onStop() {
            return true;
        }

        bool RfSrvInternalRpcClientAsync::onRecvMessage(std::shared_ptr<net::NotifyMessage> sspNM) {
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
                            common::SpinLock l(&m_slRpcCtxs);
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

            return true;
        }

        std::shared_ptr<net::NotifyMessage> RfSrvInternalRpcClientAsync::recv_message(RpcCtx *rc) {
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

            auto handlerId = rc->handlerId;
            auto sspNM = rc->ssp_nm;
            m_rpcCtxPool.Put(rc);
            auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());
            auto rm = mnm->GetContent();
            auto pos = rm->GetDataBuffer()->GetPos();
            auto serverCode = ByteOrderUtils::ReadUInt16(pos);
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
    } // namespace server
} // namespace ccraft
