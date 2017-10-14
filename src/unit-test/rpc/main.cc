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
#include "../../net/net-protocal-stacks/inet-stack-worker-manager.h"
#include "../../net/socket-service-factory.h"
#include "../../net/net-protocal-stacks/msg-worker-managers/unique-worker-manager.h"
#include "../../codegen/node-raft.pb.h"
#include "../../server/node/rpc/rf-srv-rpc-sync-client.h"

#include "client/test-rpc-client.h"
#include "server/test-rpc-server.h"

using namespace std;

#define TEST_PORT            43225

ccraft::net::ISocketService                 *g_pSS     = nullptr;
ccraft::common::MemPool                     *g_mp      = nullptr;
ccraft::test::TestRpcServer                 *g_pServer = nullptr;
ccraft::server::RfSrvInternalRpcClientSync  *g_pClient = nullptr;
bool                                         g_bStopped = false;

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
    g_pClient = new ccraft::server::RfSrvInternalRpcClientSync(g_pSS, timeout, 1, g_mp);

    EXPECT_EQ(g_pClient->Start(), true);
    ccraft::net::net_peer_info_t peer = {
        .nat = {
            .addr = "localhost",
            .port = TEST_PORT
        },
        .sp = ccraft::net::SocketProtocal::Tcp
    };

    auto appendRfLogRequest = new ccraft::protocal::AppendRfLogRequest();
    appendRfLogRequest->set_term(1234);
    appendRfLogRequest->set_leaderid(1);
    appendRfLogRequest->set_prevlogindex(22);
    appendRfLogRequest->set_prevlogterm(1233);
    appendRfLogRequest->add_entries();
    auto entry = appendRfLogRequest->mutable_entries(0);
    entry->set_term(1233);
    entry->set_index(25);
    entry->set_type(ccraft::protocal::RfLogEntryType::CONFIGURATION);
    entry->set_data("test entry data!");

    auto tmpPeer = peer;
    std::shared_ptr<ccraft::protocal::AppendRfLogResponse> appendRfLogSspResp;
    EXPECT_NO_THROW(appendRfLogSspResp = g_pClient->AppendRfLog(ccraft::rpc::SP_PB_MSG(appendRfLogRequest), std::move(tmpPeer)));
    EXPECT_EQ(appendRfLogSspResp->term(), 1111);
    EXPECT_EQ(appendRfLogSspResp->success(), true);
    std::cout << "server resp: term = " << appendRfLogSspResp->term() << ", ok = " << appendRfLogSspResp->success() << std::endl;

    auto reqVoteRequest = new ccraft::protocal::RequestVoteRequest();
    reqVoteRequest->set_term(1234);
    reqVoteRequest->set_leaderid(1);
    reqVoteRequest->set_lastlogindex(22);
    reqVoteRequest->set_lastlogterm(1233);

    std::shared_ptr<ccraft::protocal::RequestVoteResponse> reqVoteSspResp;
    EXPECT_NO_THROW(reqVoteSspResp = g_pClient->RequestVote(ccraft::rpc::SP_PB_MSG(reqVoteRequest), std::move(tmpPeer = peer)));
    EXPECT_EQ(reqVoteSspResp->term(), 1111);
    EXPECT_EQ(reqVoteSspResp->success(), true);
    std::cout << "server resp: term = " << reqVoteSspResp->term() << ", ok = " << reqVoteSspResp->success() << std::endl;

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
