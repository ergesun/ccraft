/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <iostream>
#include <memory>
#include <gtest/gtest.h>

#include "../../common/global-vars.h"
#include "../../rpc/rpc-handler.h"

#include "client/test-rpc-client.h"
#include "server/test-rpc-server.h"

using namespace std;



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    ccraft::common::initialize();
    std::shared_ptr<ccraft::IService> pRpcServer(new ccraft::test::TestRpcServer());

    pRpcServer->Start();

    auto res = RUN_ALL_TESTS();
    pRpcServer->Stop();
}

TEST(RpcTest, ClientServerTest) {
    ccraft::common::cctime_t timeout(0, 1000000 * 200);
    std::shared_ptr<ccraft::test::TestRpcClientSync> pRpcClient(new ccraft::test::TestRpcClientSync(2, timeout, 42335));

    pRpcClient->Start();
    ccraft::net::net_peer_info_t peer = {
        .nat = {
            .addr = "localhost",
            .port = 42335
        },
        .sp = ccraft::net::SocketProtocal::Tcp
    };

    auto request = new ccraft::rpc::AppendOpLogRequest();
    request->set_term(1234);
    request->set_leaderid("test leader");
    request->set_prevlogindex(22);
    request->set_prevlogterm(1233);

    pRpcClient->AppendRfLog(ccraft::rpc::SP_PB_MSG(request), std::move(peer));

    pRpcClient->Stop();
}
