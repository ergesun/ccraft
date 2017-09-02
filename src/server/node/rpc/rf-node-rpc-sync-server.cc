/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../../common/server-gflags-config.h"
#include "../../../rpc/rpc-handler.h"
#include "common-def.h"
#include "../../../codegen/append-log.pb.h"

#include "../inode-internal-rpc-handler.h"
#include "rf-node-rpc-sync-server.h"

namespace ccraft {
namespace server {
RfNodeInternalRpcServerSync::RfNodeInternalRpcServerSync(INodeInternalRpcHandler *handler, uint16_t workThreadsCnt,
                                                         net::ISocketService *ss, common::MemPool *memPool) :
            m_pHandler(handler) {
    CHECK(handler);
    m_pRpcServer = new rpc::RpcServer(workThreadsCnt, ss, memPool);
}

RfNodeInternalRpcServerSync::~RfNodeInternalRpcServerSync() {
    DELETE_PTR(m_pRpcServer);
}

bool RfNodeInternalRpcServerSync::Start() {
    register_rpc_handlers();

    return m_pRpcServer->Start();
}

bool RfNodeInternalRpcServerSync::Stop() {
    return m_pRpcServer->Stop();
}

void RfNodeInternalRpcServerSync::HandleMessage(std::shared_ptr<net::NotifyMessage> sspNM) {
    m_pRpcServer->HandleMessage(sspNM);
}

void RfNodeInternalRpcServerSync::register_rpc_handlers() {
    // internal communication
    auto appendLogHandler = new rpc::TypicalRpcHandler(std::bind(&RfNodeInternalRpcServerSync::on_append_rflog, this, std::placeholders::_1),
                                              std::bind(&RfNodeInternalRpcServerSync::create_append_rflog_request, this));
    m_pRpcServer->RegisterRpc(APPEND_RFLOG_RPC_ID, appendLogHandler);
    m_pRpcServer->FinishRegisterRpc();
}

rpc::SP_PB_MSG RfNodeInternalRpcServerSync::on_append_rflog(rpc::SP_PB_MSG sspMsg) {
    return m_pHandler->OnAppendRfLog(sspMsg);
}

rpc::SP_PB_MSG RfNodeInternalRpcServerSync::create_append_rflog_request() {
    return rpc::SP_PB_MSG(new rpc::AppendOpLogRequest());
}
} // namespace server
} // namespace ccraft
