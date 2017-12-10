/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RF_NODE_RPC_ASYNC_CLIENT_H
#define CCRAFT_RF_NODE_RPC_ASYNC_CLIENT_H

#include "../../../rpc/abstract-rpc-client.h"

/**
 * 返回值SentRet的msgId为INVALID_MSG_ID(0)表示发送失败，原因为发送队列满了。
 */
#define DefineRfNodeAsyncRpcWithPeer(RpcName)                                                        \
    rpc::ARpcClient::SentRet RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer)

namespace ccraft {
namespace protocol {
class AppendEntriesResponse;
class RequestVoteResponse;
}

namespace server {
class RfSrvInternalRpcClientAsync : public rpc::ARpcClient {
public:
    RfSrvInternalRpcClientAsync(net::ISocketService *ss, net::NotifyMessageCallbackHandler cb,
                                ccsys::MemPool *memPool = nullptr) : rpc::ARpcClient(ss, memPool) {
        m_fCallback = cb;
    }

    // Define Rpc start
    DefineRfNodeAsyncRpcWithPeer(AppendEntries);
    DefineRfNodeAsyncRpcWithPeer(RequestVote);
    // Define Rpc end

protected:
    bool onStart() override;
    bool onStop() override;
    void onRecvMessage(std::shared_ptr<net::NotifyMessage> sspNM) override;

private:
    bool register_rpc_handlers();

private:
    net::NotifyMessageCallbackHandler        m_fCallback;
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_RF_NODE_RPC_ASYNC_CLIENT_H
