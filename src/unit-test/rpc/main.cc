/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <iostream>
#include <memory>
#include <gtest/gtest.h>

#include "../../net/isocket-service.h"
#include "../../common/global-vars.h"
#include "../../rpc/rpc-handler.h"
#include "../../rpc/common-def.h"
#include "../../net/rcv-message.h"
#include "../../common/buffer.h"

#include "client/test-rpc-client.h"
#include "server/test-rpc-server.h"
#include "../../net/net-protocal-stacks/inet-stack-worker-manager.h"
#include "../../net/socket-service-factory.h"
#include "../../net/net-protocal-stacks/msg-worker-managers/unique-worker-manager.h"

using namespace std;

#define TEST_PORT            43225

ccraft::net::ISocketService      *g_pSS     = nullptr;
ccraft::common::MemPool          *g_mp      = nullptr;
ccraft::test::TestRpcServer      *g_pServer = nullptr;
ccraft::test::TestRpcClientSync  *g_pClient = nullptr;
bool                              g_bStopped = false;

void dispatch_msg(std::shared_ptr<ccraft::net::NotifyMessage> sspNM);

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    ccraft::common::initialize();

    g_mp = new ccraft::common::MemPool();
    auto nat = new ccraft::net::net_addr_t("0.0.0.0", TEST_PORT);
    std::shared_ptr<ccraft::net::net_addr_t> sspNat(nat);
    g_pSS = ccraft::net::SocketServiceFactory::CreateService(ccraft::net::SocketProtocal::Tcp,
                                                             sspNat,
                                                             TEST_PORT,
                                                             g_mp,
                                                             std::bind(&dispatch_msg, std::placeholders::_1),
                                                             std::shared_ptr<ccraft::net::INetStackWorkerManager>(new ccraft::net::UniqueWorkerManager()));

    g_pSS->Start(2, ccraft::net::NonBlockingEventModel::Posix);
    g_pServer = new ccraft::test::TestRpcServer(1, g_pSS, g_mp);

    EXPECT_EQ(g_pServer->Start(), true);

    auto res = RUN_ALL_TESTS();
    getchar();
    g_bStopped = true;
    EXPECT_EQ(g_pServer->Stop(), true);
    DELETE_PTR(g_pServer);
    DELETE_PTR(g_pClient);
    EXPECT_EQ(g_pSS->Stop(), true);
    DELETE_PTR(g_pSS);
    DELETE_PTR(g_mp);

    return res;
}

TEST(RpcTest, ClientServerTest) {
    ccraft::common::cctime_t timeout(100, 1000000 * 200);
    g_pClient = new ccraft::test::TestRpcClientSync(g_pSS, timeout, 1, g_mp);

    EXPECT_EQ(g_pClient->Start(), true);
    ccraft::net::net_peer_info_t peer = {
        .nat = {
            .addr = "localhost",
            .port = TEST_PORT
        },
        .sp = ccraft::net::SocketProtocal::Tcp
    };

    auto request = new ccraft::rpc::AppendOpLogRequest();
    request->set_term(1234);
    request->set_leaderid("test leader");
    request->set_prevlogindex(22);
    request->set_prevlogterm(1233);

    std::shared_ptr<ccraft::rpc::AppendOpLogResponse> sspResp;
    EXPECT_NO_THROW(sspResp = g_pClient->AppendRfLog(ccraft::rpc::SP_PB_MSG(request), std::move(peer)));
    EXPECT_EQ(sspResp->term(), 1111);
    EXPECT_EQ(sspResp->success(), true);
    std::cout << "server resp: term = " << sspResp->term() << ", ok = " << sspResp->success() << std::endl;

    EXPECT_EQ(g_pClient->Stop(), true);
}

void dispatch_msg(std::shared_ptr<ccraft::net::NotifyMessage> sspNM) {
    if (g_bStopped) {
        return;
    }

    switch (sspNM->GetType()) {
        case ccraft::net::NotifyMessageType::Message: {
            auto *mnm = dynamic_cast<ccraft::net::MessageNotifyMessage*>(sspNM.get());
            auto rm = mnm->GetContent();
            if (LIKELY(rm)) {
                auto buf = rm->GetDataBuffer();
                auto messageType = (ccraft::rpc::MessageType)(*(buf->GetPos()));
                buf->MoveHeadBack(1);
                if (ccraft::rpc::MessageType::Request == messageType) {
                    g_pServer->HandleMessage(sspNM);
                    std::cout << "dispatch message type = Request." << std::endl;
                } else {
                    g_pClient->HandleMessage(sspNM);
                    std::cout << "dispatch message type = Response." << std::endl;
                }
            } else {
                LOGWFUN << "recv message is empty!";
            }
            break;
        }
        case ccraft::net::NotifyMessageType::Worker: {
            g_pClient->HandleMessage(sspNM);
            g_pServer->HandleMessage(sspNM);
            break;
        }
        case ccraft::net::NotifyMessageType::Server: {
            std::cerr << "Messenger port = " << TEST_PORT << " cannot start to work." << std::endl;
        }
    }
}
