/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <iostream>
#include <gtest/gtest.h>

#include "../../../common/server-gflags-config.h"
#include "../../../rpc/rpc-handler.h"
#include "../../../codegen/node-raft.pb.h"

#include "test-rpc-server.h"

namespace ccraft {
namespace test {
TestRpcServer::TestRpcServer(uint16_t workThreadsCnt, net::ISocketService *ss, ccsys::MemPool *memPool) {
    m_pRpcServer = new rpc::RpcServer(workThreadsCnt, ss, memPool);
}

TestRpcServer::~TestRpcServer() {
    DELETE_PTR(m_pRpcServer);
}

bool TestRpcServer::Start() {
    register_rpc_handlers();

    return m_pRpcServer->Start();
}

bool TestRpcServer::Stop() {
    return m_pRpcServer->Stop();
}

void TestRpcServer::HandleMessage(std::shared_ptr<net::NotifyMessage> sspNM) {
    m_pRpcServer->HandleMessage(sspNM);
}

void TestRpcServer::register_rpc_handlers() {
    // internal communication
    auto appendLogHandler = new rpc::TypicalRpcHandler(std::bind(&TestRpcServer::on_append_entries, this, std::placeholders::_1),
                                                       std::bind(&TestRpcServer::create_append_entries_request, this));
    m_pRpcServer->RegisterRpc(1, appendLogHandler);
    auto requestVoteHandler = new rpc::TypicalRpcHandler(std::bind(&TestRpcServer::on_request_vote, this, std::placeholders::_1),
                                                       std::bind(&TestRpcServer::create_request_vote_request, this));
    m_pRpcServer->RegisterRpc(2, requestVoteHandler);
    m_pRpcServer->FinishRegisterRpc();
}

rpc::SP_PB_MSG TestRpcServer::on_append_entries(rpc::SP_PB_MSG sspMsg) {
    auto appendEntriesRequest = dynamic_cast<protocol::AppendEntriesRequest*>(sspMsg.get());
    EXPECT_EQ(1234, appendEntriesRequest->term());
    EXPECT_EQ(1, appendEntriesRequest->leaderid());
    EXPECT_EQ(22, appendEntriesRequest->prevlogindex());
    EXPECT_EQ(1233, appendEntriesRequest->prevlogterm());
    std::cout << "client req: term = " << appendEntriesRequest->term()
              << ", leader id = " << appendEntriesRequest->leaderid()
              << ", prev log idx = " << appendEntriesRequest->prevlogindex()
              << ", prev log term = " << appendEntriesRequest->prevlogterm() << std::endl;
    auto entry = appendEntriesRequest->mutable_entries(0);
    EXPECT_EQ(1233, entry->term());
    EXPECT_EQ(25, entry->index());
    EXPECT_EQ(1, entry->type());

    EXPECT_STREQ("test entry data!", entry->data().data());
    std::string entryData = std::move(*(entry->release_data()));
    std::cout << "entry 0 data = " << entryData.c_str() << std::endl;

    auto response = new protocol::AppendEntriesResponse();
    response->set_term(1111);
    response->set_success(true);

    return rpc::SP_PB_MSG(response);
}

rpc::SP_PB_MSG TestRpcServer::create_append_entries_request() {
    return rpc::SP_PB_MSG(new protocol::AppendEntriesRequest());
}

rpc::SP_PB_MSG TestRpcServer::on_request_vote(rpc::SP_PB_MSG sspMsg) {
    auto requestVoteRequest = dynamic_cast<protocol::RequestVoteRequest*>(sspMsg.get());
    EXPECT_EQ(1234, requestVoteRequest->term());
    EXPECT_EQ(1, requestVoteRequest->candidateid());
    EXPECT_EQ(22, requestVoteRequest->lastlogindex());
    EXPECT_EQ(1233, requestVoteRequest->lastlogterm());
    std::cout << "client req: term = " << requestVoteRequest->term()
              << ", leader id = " << requestVoteRequest->candidateid()
              << ", prev log idx = " << requestVoteRequest->lastlogindex()
              << ", prev log term = " << requestVoteRequest->lastlogterm() << std::endl;
    auto response = new protocol::RequestVoteResponse();
    response->set_term(1111);
    response->set_votegranted(true);

    return rpc::SP_PB_MSG(response);
}

rpc::SP_PB_MSG TestRpcServer::create_request_vote_request() {
    return rpc::SP_PB_MSG(new protocol::RequestVoteRequest());
}
}
}
