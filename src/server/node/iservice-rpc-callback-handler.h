//
// Created by root on 17-12-10.
//

#ifndef CCRAFT_ISERVICE_RPC_CALLBACK_HANDLER_H
#define CCRAFT_ISERVICE_RPC_CALLBACK_HANDLER_H

#include "../../rpc/abstract-rpc-client.h"
#include "../../codegen/node-raft.pb.h"

namespace ccraft {
namespace server {
class IServiceRpcCallbackHandler {
public:
    virtual ~IServiceRpcCallbackHandler() = default;
    /**
     * 消息已发送。
     * @param sentRet
     */
    virtual void OnMessageSent(rpc::ARpcClient::SentRet &&sentRet) = 0;
    // Ret: return, Res: result
    virtual void OnRecvRequestVoteRetRes(protocol::RequestVoteResponse *resp) = 0;
    virtual void OnRecvAppendEntriesRetRes(protocol::AppendEntriesResponse *resp) = 0;
}; // class IServiceRpcCallbackHandler
} // namespace server
} // namespace ccraft

#endif //CCRAFT_ISERVICE_RPC_CALLBACK_HANDLER_H
