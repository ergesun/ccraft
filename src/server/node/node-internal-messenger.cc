/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <glog/logging.h>

#include "../../common/buffer.h"
#include "../../net/socket-service-factory.h"
#include "../../net/net-protocal-stacks/msg-worker-managers/unique-worker-manager.h"
#include "../../net/rcv-message.h"
#include "../../rpc/common-def.h"

#include "./rpc/rf-node-rpc-sync-client.h"
#include "./rpc/rf-node-rpc-sync-server.h"
#include "rf-node.h"

#include "node-internal-messenger.h"

namespace ccraft {
namespace server {
NodeInternalMessenger::NodeInternalMessenger(CreateNodeInternalMessengerParam &createParam) :
        m_pRfNode(createParam.rfNode), m_iDispatchTpCnt(createParam.mngerDispatchWorkThreadsCnt) {
    CHECK(createParam.rfNode);
    if (!createParam.memPool) {
        m_bOwnMemPool = true;
        // TODO(sunchao): 优化此内存池参数。
        m_pMemPool = new common::MemPool();
    }

    m_iIOThreadsCnt = createParam.netIOThreadsCnt;
    m_iPort = createParam.port;
    auto nat = new net::net_addr_t("0.0.0.0", createParam.port);
    std::shared_ptr<net::net_addr_t> sspNat(nat);
    m_pSocketService = net::SocketServiceFactory::CreateService(net::SocketProtocal::Tcp,
                                                                sspNat,
                                                                createParam.port,
                                                                m_pMemPool,
                                                                std::bind(&NodeInternalMessenger::recv_msg, this, std::placeholders::_1),
                                                                std::shared_ptr<net::INetStackWorkerManager>(new net::UniqueWorkerManager()));
    m_pClient = new RfNodeInternalRpcClientSync(m_pSocketService, createParam.clientWaitResponseTimeout,
                                                createParam.clientRpcWorkThreadsCnt, m_pMemPool);
    m_pServer = new RfNodeInternalRpcServerSync(this, createParam.serverRpcWorkThreadsCnt, m_pSocketService, m_pMemPool);
}

NodeInternalMessenger::~NodeInternalMessenger() {
    Stop();
    if (m_bOwnMemPool) {
        DELETE_PTR(m_pMemPool);
    }

    DELETE_PTR(m_pClient);
    DELETE_PTR(m_pServer);
    DELETE_PTR(m_pSocketService);
}

bool NodeInternalMessenger::Start() {
    if (!m_bStopped) {
        return true;
    }

    m_bStopped = false;
    hw_rw_memory_barrier();
    m_pSocketService->Start(m_iIOThreadsCnt, net::NonBlockingEventModel::Posix);
    m_pDispatchTp = new common::ThreadPool<std::shared_ptr<net::NotifyMessage>>(m_iDispatchTpCnt);
    m_pClient->Start();
    m_pServer->Start();

    return true;
}

bool NodeInternalMessenger::Stop() {
    if (m_bStopped) {
        return true;
    }

    m_bStopped = true;
    hw_rw_memory_barrier();
    m_pClient->Stop();
    m_pServer->Stop();
    m_pSocketService->Stop();
    DELETE_PTR(m_pDispatchTp);
    return true;
}

std::shared_ptr<protocal::serverraft::AppendRfLogResponse> NodeInternalMessenger::AppendRfLogSync(rpc::SP_PB_MSG req,
                                                                                 net::net_peer_info_t &&peer) {
    return m_pClient->AppendRfLog(req, std::move(peer));
}

rpc::SP_PB_MSG NodeInternalMessenger::OnAppendRfLog(rpc::SP_PB_MSG sspMsg) {
    return m_pRfNode->OnAppendRfLog(sspMsg);
}

std::shared_ptr<protocal::serverraft::RequestVoteResponse> NodeInternalMessenger::RequestVoteSync(rpc::SP_PB_MSG req,
                                                                                 net::net_peer_info_t &&peer) {
    return m_pClient->RequestVote(req, std::move(peer));
}

rpc::SP_PB_MSG NodeInternalMessenger::OnRequestVote(rpc::SP_PB_MSG sspMsg) {
    return m_pRfNode->OnRequestVote(sspMsg);
}

void NodeInternalMessenger::dispatch_msg(std::shared_ptr<net::NotifyMessage> sspNM) {
    if (UNLIKELY(m_bStopped)) {
        return;
    }

    switch (sspNM->GetType()) {
        case net::NotifyMessageType::Message: {
            auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());
            auto rm = mnm->GetContent();
            if (LIKELY(rm)) {
                auto buf = rm->GetDataBuffer();
                auto messageType = (rpc::MessageType)(*(buf->GetPos()));
                buf->MoveHeadBack(1);
                if (rpc::MessageType::Request == messageType) {
                    m_pServer->HandleMessage(sspNM);
                    LOGDFUN1("dispatch message type = Request.");
                } else {
                    m_pClient->HandleMessage(sspNM);
                    LOGDFUN1("dispatch message type = Response.");
                }
            } else {
                LOGWFUN << "recv message is empty!";
            }
            break;
        }
        case net::NotifyMessageType::Worker: {
            m_pClient->HandleMessage(sspNM);
            m_pServer->HandleMessage(sspNM);
            break;
        }
        case net::NotifyMessageType::Server: {
            LOGFFUN << "Messenger port = " << m_iPort << " cannot start to work.";
        }
    }
}

void NodeInternalMessenger::recv_msg(std::shared_ptr<net::NotifyMessage> sspNM) {
    if (UNLIKELY(m_bStopped)) {
        return;
    }

    m_pDispatchTp->AddTask(std::bind(&NodeInternalMessenger::dispatch_msg, this, std::placeholders::_1), sspNM);
}
} // namespace server
} // namespace ccraft
