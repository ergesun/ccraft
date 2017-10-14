/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RF_NODE_RPC_SYNC_CLIENT_H
#define CCRAFT_RF_NODE_RPC_SYNC_CLIENT_H

#include <unordered_map>
#include <set>

#include "../../../rpc/abstract-rpc-client.h"
#include "../../../rpc/common-def.h"
#include "../../../common/cctime.h"
#include "../../../common/resource-pool.h"
#include "../../../net/common-def.h"

#define RpcAppendRfLog "AppendRfLog"
#define RpcRequestVote "RequestVote"

#define DefineRfNodeRpcWithPeer(RpcName)                                                                               \
    std::shared_ptr<protocal::RpcName##Response> RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer)

namespace ccraft {
namespace protocal {
class AppendRfLogResponse;
class RequestVoteResponse;
}

namespace server {
class RfSrvInternalRpcClientSync : public rpc::ARpcClient {
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
        uint16_t                             handlerId = 0;
        uint64_t                             msgId     = 0;
        bool                                 complete  = false;
        State                                state     = State::Ok;
        common::Timer::EventId               timer_ev;
        std::shared_ptr<net::NotifyMessage>  ssp_nm;
    };

public:
    RfSrvInternalRpcClientSync(net::ISocketService *ss, common::cctime_t timeout,
                                uint16_t workThreadsCnt, common::MemPool *memPool = nullptr) :
        rpc::ARpcClient(ss, memPool), m_timeout(timeout) {}

    // Define Rpc start
    DefineRfNodeRpcWithPeer(AppendRfLog);
    DefineRfNodeRpcWithPeer(RequestVote);
    // Define Rpc end

protected:
    bool onStart() override;
    bool onStop() override;
    bool onRecvMessage(std::shared_ptr<net::NotifyMessage> sspNM) override;

private:
    bool register_rpc_handlers();
    std::shared_ptr<net::NotifyMessage> recv_message(RpcCtx *rc);

private:
    common::ResourcePool<RpcCtx>                                   m_rpcCtxPool = common::ResourcePool<RpcCtx>(200);
    std::unordered_map<uint64_t, RpcCtx*>                          m_hmapRpcCtxs;
    std::unordered_map<net::net_peer_info_t, std::set<RpcCtx*>>    m_hmapPeerRpcs;
    common::spin_lock_t                                            m_slRpcCtxs = UNLOCKED;
    common::cctime_t                                               m_timeout;
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_RF_NODE_RPC_SYNC_CLIENT_H
