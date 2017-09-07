/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_TEST_RPC_CLIENT_H
#define CCRAFT_TEST_RPC_CLIENT_H

#include "../../../rpc/client-bases/abstract-rpc-client-sync.h"
#include "../../../rpc/common-def.h"

#define RpcAppendRfLog "AppendRfLog"
#define RpcRequestVote "RequestVote"

#define DefineTestRpcClientSyncWithPeer(RpcName)                                                                      \
    std::shared_ptr<protocal::serverraft::RpcName##Response> RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer)

namespace ccraft {
namespace protocal {
namespace serverraft {
class AppendRfLogResponse;
class RequestVoteResponse;
}
}
namespace test {
class TestRpcClientSync : public rpc::ARpcClientSync {
public:
    TestRpcClientSync(net::ISocketService *ss, common::cctime_t timeout, uint16_t workThreadsCnt, common::MemPool *memPool = nullptr) :
        rpc::ARpcClientSync(ss, timeout, workThreadsCnt, memPool) {}

    bool Start() override;
    DefineTestRpcClientSyncWithPeer(AppendRfLog);
    DefineTestRpcClientSyncWithPeer(RequestVote);

private:
    bool register_rpc_handlers();
};
} // namespace test
} // namespace ccraft

#endif //CCRAFT_TEST_RPC_CLIENT_H
