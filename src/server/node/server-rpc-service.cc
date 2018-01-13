/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../codegen/node-raft.pb.h"
#include "../../common/server-gflags-config.h"

#include "raft-consensus.h"
#include "server-internal-messenger.h"

#include "server-rpc-service.h"

namespace ccraft {
namespace server {
ServerRpcService::ServerRpcService(uint16_t port, uint16_t rpcThreadsCnt, RaftConsensus *pRaftConsensus) :
    m_pRaftConsensus(pRaftConsensus) {
    assert(m_pRaftConsensus);
    ccsys::cctime clientWaitTimeOut(FLAGS_internal_rpc_client_wait_timeout_secs,
                                    FLAGS_internal_rpc_client_wait_timeout_nsecs);
    CreateServerInternalMessengerParam cnimp(this, (uint16_t)FLAGS_internal_rpc_client_threads_cnt,
                                             clientWaitTimeOut, (uint16_t)FLAGS_internal_rpc_server_threads_cnt,
                                             (uint16_t)FLAGS_internal_rpc_messenger_threads_cnt,
                                             (uint16_t)FLAGS_internal_rpc_io_threads_cnt,
                                             port, nullptr, FLAGS_net_server_connect_timeout);
    m_pNodeInternalMessenger = new ServerInternalMessenger(cnimp);
    m_pExecRpcTp = new ccsys::ThreadPool<RpcTask>(rpcThreadsCnt);
}

ServerRpcService::~ServerRpcService() {
    DELETE_PTR(m_pNodeInternalMessenger);
    DELETE_PTR(m_pExecRpcTp);
}

bool ServerRpcService::Start() {
    INOUT_LOG;
    return m_pNodeInternalMessenger->Start();
}

bool ServerRpcService::Stop() {
    INOUT_LOG;
    return m_pNodeInternalMessenger->Stop();
}

std::shared_ptr<protocol::AppendEntriesResponse> ServerRpcService::AppendEntriesSync(rpc::SP_PB_MSG req,
                                                                                        net::net_peer_info_t &&peer) {
    return m_pNodeInternalMessenger->AppendEntriesSync(req, std::move(peer));
}

rpc::ARpcClient::SentRet ServerRpcService::AppendEntriesAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {
    return m_pNodeInternalMessenger->AppendEntriesAsync(req, std::move(peer));
}

rpc::SP_PB_MSG ServerRpcService::OnAppendEntries(rpc::SP_PB_MSG sspMsg) {
    return m_pNodeInternalMessenger->OnAppendEntries(sspMsg);
}

std::shared_ptr<protocol::RequestVoteResponse> ServerRpcService::RequestVoteSync(rpc::SP_PB_MSG req,
                                                                                net::net_peer_info_t &&peer) {
    return m_pNodeInternalMessenger->RequestVoteSync(req, std::move(peer));
}

void ServerRpcService::RequestVoteAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {
    static auto send_req_vote = [this](RpcTask rt) {
        auto sr = rt.pSim->RequestVoteAsync(rt.msg, std::move(rt.peer));
        m_pRaftConsensus->OnMessageSent(std::move(sr));
    };

    RpcTask rt(req, m_pNodeInternalMessenger, std::move(peer));
    m_pExecRpcTp->AddTask(send_req_vote, rt);
}

rpc::SP_PB_MSG ServerRpcService::OnRequestVote(rpc::SP_PB_MSG sspMsg) {
    return m_pNodeInternalMessenger->OnRequestVote(sspMsg);
}

void ServerRpcService::OnRecvRpcReturnResult(std::shared_ptr<net::NotifyMessage> sspNM) {
    if (UNLIKELY(!sspNM)) {
        return;
    }

    switch (sspNM->GetType()) {
        case net::NotifyMessageType::Message: {
            auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());
            auto rm = mnm->GetContent();
            if (LIKELY(rm)) {

            } else {
                LOGWFUN << "recv message is empty!";
            }
            break;
        }
        default: {
            // skip.
        }
    }
}
} // namespace server
} // namespace ccraft
