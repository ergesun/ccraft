/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RF_NODE_RPC_ASYNC_CLIENT_H
#define CCRAFT_RF_NODE_RPC_ASYNC_CLIENT_H

#include "../../../rpc/abstract-rpc-client.h"

#define RpcAppendRfLog "AppendRfLog"
#define RpcRequestVote "RequestVote"

#define DefineRfNodeRpcWithPeer(RpcName)                                                                               \
    rpc::ARpcClient::SendRet RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer)

namespace ccraft {
namespace protocal {
class AppendRfLogResponse;
class RequestVoteResponse;
}

namespace server {
class RfSrvInternalRpcClientAsync : public rpc::ARpcClient {
public:
    RfSrvInternalRpcClientAsync(net::ISocketService *ss, uint16_t workThreadsCnt, common::MemPool *memPool = nullptr) :
                                rpc::ARpcClient(ss, memPool) {}

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
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_RF_NODE_RPC_ASYNC_CLIENT_H
