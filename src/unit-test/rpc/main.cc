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

    EXPECT_EQ(pRpcServer->Start(), true);

    auto res = RUN_ALL_TESTS();
    getchar();
    EXPECT_EQ(pRpcServer->Stop(), true);

    return res;
}

TEST(RpcTest, ClientServerTest) {
    ccraft::common::cctime_t timeout(10, 1000000 * 200);
    std::shared_ptr<ccraft::test::TestRpcClientSync> pRpcClient(new ccraft::test::TestRpcClientSync(2, timeout, 42335));

    EXPECT_EQ(pRpcClient->Start(), true);
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

    std::shared_ptr<ccraft::rpc::AppendOpLogResponse> sspResp;
    EXPECT_NO_THROW(sspResp = pRpcClient->AppendRfLog(ccraft::rpc::SP_PB_MSG(request), std::move(peer)));
    EXPECT_EQ(sspResp->term(), 1111);
    EXPECT_EQ(sspResp->success(), true);
    std::cout << "server resp: term = " << sspResp->term() << ", ok = " << sspResp->success() << std::endl;

    EXPECT_EQ(pRpcClient->Stop(), true);
}
