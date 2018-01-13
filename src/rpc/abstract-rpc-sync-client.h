/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_ABSTRACT_RPC_SYNC_CLIENT_H
#define CCRAFT_RPC_ABSTRACT_RPC_SYNC_CLIENT_H

#include <memory>

#include "../ccsys/timer.h"
#include "../ccsys/spin-lock.h"
#include "../common/common-def.h"
#include "../common/resource-pool.h"

#include "abstract-rpc-client.h"

#define DefineStandardSyncRpcWithNoMsgId(RpcName)                                                                   \
    std::shared_ptr<protocol::RpcName##Response> RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer)

#define ImplStandardSyncRpcWithNoMsgId(ClassName, RpcName)                                                         \
std::shared_ptr<protocol::RpcName##Response>                                                                       \
    ClassName::RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {                                          \
        auto sspNM = sendMessage(#RpcName, std::move(req), std::move(peer));                                       \
        if (!sspNM) {                                                                                              \
            throw RpcClientIsBusyException();                                                                      \
        }                                                                                                          \
                                                                                                                   \
        auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());                                         \
        auto rm = mnm->GetContent();                                                                               \
        auto RpcName##Resp__Impl_DEF_TMP = new protocol::RpcName##Response();                                      \
        auto buf = rm->GetDataBuffer();                                                                            \
        buf->MoveHeadBack(sizeof(rpc::HandlerType));                                                               \
        common::ProtoBufUtils::Deserialize(buf, RpcName##Resp__Impl_DEF_TMP);                                      \
        return std::shared_ptr<protocol::RpcName##Response>(RpcName##Resp__Impl_DEF_TMP);                          \
    }

#define DefineStandardSyncRpcWithMsgId(RpcName)                                                                                 \
    std::shared_ptr<protocol::RpcName##Response> RpcName(net::Message::Id id, rpc::SP_PB_MSG req, net::net_peer_info_t &&peer)

#define ImplStandardSyncRpcWithMsgId(ClassName, RpcName)                                                           \
std::shared_ptr<protocol::RpcName##Response>                                                                       \
    ClassName::RpcName(net::Message::Id id, rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {                     \
        auto sspNM = sendMessage(#RpcName, id, std::move(req), std::move(peer));                                   \
        if (!sspNM) {                                                                                              \
            throw RpcClientIsBusyException();                                                                      \
        }                                                                                                          \
                                                                                                                   \
        auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());                                         \
        auto rm = mnm->GetContent();                                                                               \
        auto RpcName##Resp__Impl_DEF_TMP = new protocol::RpcName##Response();                                      \
        auto buf = rm->GetDataBuffer();                                                                            \
        buf->MoveHeadBack(sizeof(rpc::HandlerType));                                                               \
        common::ProtoBufUtils::Deserialize(buf, RpcName##Resp__Impl_DEF_TMP);                                      \
        return std::shared_ptr<protocol::RpcName##Response>(RpcName##Resp__Impl_DEF_TMP);                          \
    }


namespace ccraft {
namespace net {
class NotifyMessage;
}
namespace rpc {
class ARpcSyncClient : public ARpcClient {
private:
    class RpcCtx {
    public:
        enum class State {
            Ok          = 0,
            Timeout     = 1,
            BrokenPipe  = 2
        };
    public:
        RpcCtx() {
            cv = new std::condition_variable();
            mtx = new std::mutex();
        }

        ~RpcCtx() {
            DELETE_PTR(mtx);
            DELETE_PTR(cv);
        }

        void Release() {
            ssp_nm.reset();
        }

    public:
        std::condition_variable             *cv        = nullptr;
        std::mutex                          *mtx       = nullptr;
        net::net_peer_info_t                 peer;
        uint64_t                             msgId     = 0;
        bool                                 complete  = false;
        State                                state     = State::Ok;
        ccsys::Timer::EventId                timer_ev;
        std::shared_ptr<net::NotifyMessage>  ssp_nm;
    };

public:
    ARpcSyncClient(net::ISocketService *ss, const ccsys::cctime &timeout,
            uint16_t workThreadsCnt, ccsys::MemPool *memPool = nullptr) :
    rpc::ARpcClient(ss, memPool), m_timeout(timeout) {}

protected:
    std::shared_ptr<net::NotifyMessage> recvMessage(RpcCtx *rc);
    void onRecvMessage(std::shared_ptr<net::NotifyMessage> sspNM) override;
    /**
     * @param rpcName
     * @param msg
     * @return 如果get()为null则失败，否则成功。
     */
    std::shared_ptr<net::NotifyMessage> sendMessage(std::string &&rpcName, SP_PB_MSG msg, net::net_peer_info_t &&peer);

    /**
     * @param rpcName
     * @param msg
     * @return 如果get()为null则失败，否则成功。
     */
    std::shared_ptr<net::NotifyMessage> sendMessage(std::string &&rpcName, net::Message::Id id, SP_PB_MSG msg, net::net_peer_info_t &&peer);

private:
    common::ResourcePool<RpcCtx>                                   m_rpcCtxPool = common::ResourcePool<RpcCtx>(200);
    std::unordered_map<uint64_t, RpcCtx*>                          m_hmapRpcCtxs;
    std::unordered_map<net::net_peer_info_t, std::set<RpcCtx*>>    m_hmapPeerRpcs;
    ccsys::spin_lock_t                                             m_slRpcCtxs = UNLOCKED;
    ccsys::cctime                                                  m_timeout;
};
}
}

#endif //CCRAFT_RPC_ABSTRACT_RPC_SYNC_CLIENT_H
