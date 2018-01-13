/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../../net/notify-message.h"
#include "../../../net/rcv-message.h"
#include "../../../common/protobuf-utils.h"
#include "../../../common/buffer.h"
#include "../../../codegen/node-raft.pb.h"
#include "../../../rpc/response.h"

#include "common-def.h"
#include "exceptions.h"

#include "rf-srv-rpc-sync-client.h"

namespace ccraft {
namespace server {
ImplStandardSyncRpc(RfSrvInternalRpcClientSync, AppendEntries)
ImplStandardSyncRpc(RfSrvInternalRpcClientSync, RequestVote)

bool RfSrvInternalRpcClientSync::register_rpc_handlers() {
    if (!registerRpc(RpcAppendEntries, APPEND_ENTRIES_RPC_ID)) {
        return false;
    }

    if (!registerRpc(RpcRequestVote, REQUEST_VOTE_RPC_ID)) {
        return false;
    }

    finishRegisterRpc();

    return true;
}

bool RfSrvInternalRpcClientSync::onStart() {
    return register_rpc_handlers();
}

bool RfSrvInternalRpcClientSync::onStop() {
    return true;
}
} // namespace server
} // namespace ccraft
