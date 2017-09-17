/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../../net/notify-message.h"
#include "../../../net/rcv-message.h"
#include "../../../common/protobuf-utils.h"
#include "../../../common/buffer.h"
#include "../../../codegen/node-raft.pb.h"

#include "common-def.h"

#include "rf-node-rpc-sync-client.h"

#define ImplRfNodeRpcWithPeer(RpcName)                                                                  \
std::shared_ptr<protocal::RpcName##Response>                                                            \
    RfNodeInternalRpcClientSync::RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {             \
        auto tmpPeer = peer;                                                                            \
        auto ctx = sendMessage(#RpcName, std::move(req), std::move(peer));                              \
        if (UNLIKELY(!ctx)) {                                                                           \
            LOGFFUN << "send msg to " << tmpPeer.nat.addr << ":" << tmpPeer.nat.port << " failed!";     \
        }                                                                                               \
        auto sspNM = recvMessage(ctx);                                                                  \
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
bool RfNodeInternalRpcClientSync::Start() {
    register_rpc_handlers();

    return rpc::ARpcClientSync::Start();
}

ImplRfNodeRpcWithPeer(AppendRfLog)
ImplRfNodeRpcWithPeer(RequestVote)

bool RfNodeInternalRpcClientSync::register_rpc_handlers() {
    if (!registerRpc(RpcAppendRfLog, APPEND_RFLOG_RPC_ID)) {
        return false;
    }

    if (!registerRpc(RpcRequestVote, REQUEST_VOTE_RPC_ID)) {
        return false;
    }

    finishRegisterRpc();

    return true;
}
} // namespace server
} // namespace ccraft
