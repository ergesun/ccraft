/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_TEST_RPC_CLIENT_H
#define CCRAFT_TEST_RPC_CLIENT_H

#include "../../../rpc/client-bases/abstract-rpc-client-sync.h"
#include "../../../rpc/common-def.h"

#include "../../../codegen/append-log.pb.h"

#define RpcAppendRfLog "AppendRfLog"

namespace ccraft {
namespace test {
class TestRpcClientSync : public rpc::ARpcClientSync {
public:
    TestRpcClientSync(uint16_t socketServiceThreadsCnt, common::cctime_t timeout, uint16_t logicPort) :
        rpc::ARpcClientSync(socketServiceThreadsCnt, timeout, logicPort) {}

    bool Start() override;
    std::shared_ptr<rpc::AppendOpLogResponse> AppendRfLog(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);

private:
    bool register_rpc_handlers();
};
} // namespace test
} // namespace ccraft

#endif //CCRAFT_TEST_RPC_CLIENT_H
