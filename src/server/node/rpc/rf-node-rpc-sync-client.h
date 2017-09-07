/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RF_NODE_RPC_SYNC_CLIENT_H
#define CCRAFT_RF_NODE_RPC_SYNC_CLIENT_H

#include "../../../rpc/client-bases/abstract-rpc-client-sync.h"
#include "../../../rpc/common-def.h"

#define RpcAppendRfLog "AppendRfLog"
#define RpcRequestVote "RequestVote"

#define DefineRfNodeRpcWithPeer(RpcName)                                                                               \
    std::shared_ptr<protocal::serverraft::RpcName##Response> RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer)

namespace ccraft {
namespace protocal {
namespace serverraft {
class AppendRfLogResponse;
class RequestVoteResponse;
}
}

namespace server {
class RfNodeInternalRpcClientSync : public rpc::ARpcClientSync {
public:
    RfNodeInternalRpcClientSync(net::ISocketService *ss, common::cctime_t timeout,
                                uint16_t workThreadsCnt, common::MemPool *memPool = nullptr) :
        rpc::ARpcClientSync(ss, timeout, workThreadsCnt, memPool) {}

    bool Start() override;

    // Define Rpc start
    DefineRfNodeRpcWithPeer(AppendRfLog);
    DefineRfNodeRpcWithPeer(RequestVote);
    // Define Rpc end

private:
    bool register_rpc_handlers();
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_RF_NODE_RPC_SYNC_CLIENT_H
