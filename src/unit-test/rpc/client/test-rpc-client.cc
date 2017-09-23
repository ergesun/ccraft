/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../../net/notify-message.h"
#include "../../../net/rcv-message.h"
#include "../../../common/protobuf-utils.h"
#include "../../../common/buffer.h"
#include "../../../codegen/node-raft.pb.h"

#include "test-rpc-client.h"

#define ImplTestRpcClientSyncWithPeer(RpcName)                                                          \
std::shared_ptr<protocal::RpcName##Response>                                                            \
    TestRpcClientSync::RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {                       \
        auto tmpPeer = peer;                                                                            \
        auto sspNM = sendMessage(#RpcName, std::move(req), std::move(peer));                            \
        auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());                              \
        auto rm = mnm->GetContent();                                                                    \
        auto RpcName##Resp__Impl_DEF_TMP = new protocal::RpcName##Response();                           \
        auto buf = rm->GetDataBuffer();                                                                 \
        buf->MoveHeadBack(sizeof(uint16_t));                                                            \
        common::ProtoBufUtils::Deserialize(buf, RpcName##Resp__Impl_DEF_TMP);                           \
        return std::shared_ptr<protocal::RpcName##Response>(RpcName##Resp__Impl_DEF_TMP);               \
    }

namespace ccraft {
namespace test {
bool TestRpcClientSync::Start() {
    register_rpc_handlers();

    return rpc::ARpcClientSync::Start();
}

ImplTestRpcClientSyncWithPeer(AppendRfLog)
ImplTestRpcClientSyncWithPeer(RequestVote)

bool TestRpcClientSync::register_rpc_handlers() {
    if (!registerRpc(RpcAppendRfLog, 1)) {
        return false;
    }

    if (!registerRpc(RpcRequestVote, 2)) {
        return false;
    }

    finishRegisterRpc();

    return true;
}
}
}
