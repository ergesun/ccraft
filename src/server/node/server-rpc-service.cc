/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../codegen/node-raft.pb.h"
#include "../../common/server-gflags-config.h"

#include "server-internal-messenger.h"

#include "server-rpc-service.h"

namespace ccraft {
namespace server {
ServerRpcService::ServerRpcService(uint16_t port) {
    common::cctime_t clientWaitTimeOut = {
        .sec = FLAGS_internal_rpc_client_wait_timeout_secs,
        .nsec = FLAGS_internal_rpc_client_wait_timeout_nsecs
    };

    CreateServerInternalMessengerParam cnimp = {
        .rfNode = this,
        .clientRpcWorkThreadsCnt = (uint16_t)FLAGS_internal_rpc_client_threads_cnt,
        .clientWaitResponseTimeout = clientWaitTimeOut,
        .serverRpcWorkThreadsCnt = (uint16_t)FLAGS_internal_rpc_server_threads_cnt,
        .mngerDispatchWorkThreadsCnt = (uint16_t)FLAGS_internal_rpc_messenger_threads_cnt,
        .netIOThreadsCnt = (uint16_t)FLAGS_internal_rpc_io_threads_cnt,
        .port = port,
        .memPool = nullptr
    };

    m_pNodeInternalMessenger = new ServerInternalMessenger(cnimp);
}

ServerRpcService::~ServerRpcService() {
    DELETE_PTR(m_pNodeInternalMessenger);
}

bool ServerRpcService::Start() {
    INOUT_LOG;
    return m_pNodeInternalMessenger->Start();
}

bool ServerRpcService::Stop() {
    INOUT_LOG;
    return m_pNodeInternalMessenger->Stop();
}

rpc::SP_PB_MSG ServerRpcService::OnAppendRfLog(rpc::SP_PB_MSG sspMsg) {
    auto appendRfLogRequest = dynamic_cast<protocal::AppendRfLogRequest*>(sspMsg.get());
    auto response = new protocal::AppendRfLogResponse();
    response->set_term(1111);
    response->set_success(true);

    return rpc::SP_PB_MSG(response);
}

rpc::SP_PB_MSG ServerRpcService::OnRequestVote(rpc::SP_PB_MSG sspMsg) {
    auto response = new protocal::RequestVoteResponse();
    response->set_term(1111);
    response->set_success(true);

    return rpc::SP_PB_MSG(response);
}
} // namespace server
} // namespace ccraft
