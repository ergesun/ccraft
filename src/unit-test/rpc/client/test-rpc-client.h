///**
// * This work copyright Chao Sun(qq:296449610) and licensed under
// * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
// */
//
//#ifndef CCRAFT_TEST_RPC_CLIENT_H
//#define CCRAFT_TEST_RPC_CLIENT_H
//
//#include "../../../rpc/abstract-rpc-client.h"
//#include "../../../rpc/common-def.h"
//#include "../../../server/node/rpc/rf-srv-rpc-sync-client.h"
//
//#define RpcAppendRfLog "AppendRfLog"
//#define RpcRequestVote "RequestVote"
//
//#define DefineTestRpcClientSyncWithPeer(RpcName)                                                                      \
//    std::shared_ptr<protocal::RpcName##Response> RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer)
//
//namespace ccraft {
//namespace protocal {
//class AppendRfLogResponse;
//class RequestVoteResponse;
//}
//namespace test {
//class TestRpcClientSync : public server::RfSrvInternalRpcClientSync {
//public:
//    explicit TestRpcClientSync(net::ISocketService *ss, common::MemPool *memPool = nullptr) :
//        rpc::ARpcClient(ss, memPool) {}
//
//    bool Start() override;
//    DefineTestRpcClientSyncWithPeer(AppendRfLog);
//    DefineTestRpcClientSyncWithPeer(RequestVote);
//
//private:
//    bool register_rpc_handlers();
//};
//} // namespace test
//} // namespace ccraft
//
//#endif //CCRAFT_TEST_RPC_CLIENT_H
