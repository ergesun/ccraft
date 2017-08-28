/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../common/server-gflags-config.h"
#include "../../rpc/rpc-handler.h"
#include "common-def.h"
#include "../../codegen/append-log.pb.h"

#include "rf-node.h"

namespace ccraft {
    namespace server {
        RfNode::RfNode() {
            m_pInternalRpcServer = new rpc::RpcServer(uint16_t(FLAGS_internal_rpc_server_threads_cnt),
                                              uint16_t(FLAGS_internal_rpc_io_threads_cnt),
                                              uint16_t(FLAGS_internal_rpc_logic_port));
        }

        RfNode::~RfNode() {
            DELETE_PTR(m_pInternalRpcServer);
        }

        bool RfNode::Start() {
            register_rpc_handlers();

            return m_pInternalRpcServer->Start();
        }

        bool RfNode::Stop() {
            return m_pInternalRpcServer->Stop();
        }

        void RfNode::register_rpc_handlers() {
            // internal communication
            auto appendLogHandler = new rpc::TypicalRpcHandler(std::bind(&RfNode::append_rflog, this, std::placeholders::_1),
                                                      std::bind(&RfNode::create_append_rflog_request, this));
            m_pInternalRpcServer->RegisterRpc(APPEND_RFLOG_RPC_ID, appendLogHandler);
            m_pInternalRpcServer->FinishRegisterRpc();
        }

        rpc::SP_PB_MSG RfNode::append_rflog(rpc::SP_PB_MSG sspMsg) {
            auto response = new rpc::AppendOpLogResponse();
            response->set_term(1111);
            response->set_success(true);

            return rpc::SP_PB_MSG(response);
        }

        rpc::SP_PB_MSG RfNode::create_append_rflog_request() {
            return rpc::SP_PB_MSG(new rpc::AppendOpLogRequest());
        }
    } // namespace server
} // namespace ccraft
