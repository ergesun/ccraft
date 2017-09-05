/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../codegen/requst-vote.pb.h"
#include "../../codegen/append-log.pb.h"
#include "../../common/server-gflags-config.h"

#include "node-internal-messenger.h"

#include "rf-node.h"

namespace ccraft {
    namespace server {
        RfNode::RfNode() {
            common::cctime_t clientWaitTimeOut = {
                .sec = FLAGS_internal_rpc_client_wait_timeout_secs,
                .nsec = FLAGS_internal_rpc_client_wait_timeout_nsecs
            };

            CreateNodeInternalMessengerParam cnimp = {
                .rfNode = this,
                .clientRpcWorkThreadsCnt = (uint16_t)FLAGS_internal_rpc_client_threads_cnt,
                .clientWaitResponseTimeout = clientWaitTimeOut,
                .serverRpcWorkThreadsCnt = (uint16_t)FLAGS_internal_rpc_server_threads_cnt,
                .mngerDispatchWorkThreadsCnt = (uint16_t)FLAGS_internal_rpc_messenger_threads_cnt,
                .netIOThreadsCnt = (uint16_t)FLAGS_internal_rpc_io_threads_cnt,
                .port = (uint16_t)FLAGS_internal_rpc_logic_port,
                .memPool = nullptr
            };

            m_pNodeInternalMessenger = new NodeInternalMessenger(cnimp);
        }

        RfNode::~RfNode() {
            DELETE_PTR(m_pNodeInternalMessenger);
        }

        bool RfNode::Start() {
            return m_pNodeInternalMessenger->Start();
        }

        bool RfNode::Stop() {
            return m_pNodeInternalMessenger->Stop();
        }

        rpc::SP_PB_MSG RfNode::OnAppendRfLog(rpc::SP_PB_MSG sspMsg) {
            auto response = new rpc::AppendRfLogResponse();
            response->set_term(1111);
            response->set_success(true);

            return rpc::SP_PB_MSG(response);
        }

        rpc::SP_PB_MSG RfNode::OnRequestVote(rpc::SP_PB_MSG sspMsg) {
            auto response = new rpc::RequestVoteResponse();
            response->set_term(1111);
            response->set_success(true);

            return rpc::SP_PB_MSG(response);
        }
    } // namespace server
} // namespace ccraft
