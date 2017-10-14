/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../../net/notify-message.h"
#include "../../../net/rcv-message.h"
#include "../../../rpc/common-def.h"

#include "common-def.h"
#include "exceptions.h"

#include "rf-srv-rpc-async-client.h"

using ccraft::rpc::RpcCode;

#define ImplRfNodeRpcWithPeer(RpcName)                                                                                   \
    rpc::ARpcClient::SendRet RfSrvInternalRpcClientAsync::RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {     \
        return sendMessage(#RpcName, std::move(req), std::move(peer));                                                   \
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
    return true;
}
} // namespace server
} // namespace ccraft
