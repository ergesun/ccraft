/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RF_NODE_RPC_ASYNC_CLIENT_H
#define CCRAFT_RF_NODE_RPC_ASYNC_CLIENT_H

#include "../../../rpc/common-def.h"
#include "../../../rpc/abstract-rpc-client.h"

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
    DefineStandardAsyncRpcWithNoMsgId(AppendEntries);
    DefineStandardAsyncRpcWithNoMsgId(RequestVote);
    DefineStandardAsyncRpcWithMsgId(AppendEntries);
    DefineStandardAsyncRpcWithMsgId(RequestVote);
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
