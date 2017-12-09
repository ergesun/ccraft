/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <glog/logging.h>

#include "../../common/buffer.h"
#include "../../net/socket-service-factory.h"
#include "../../net/net-protocol-stacks/msg-worker-managers/unique-worker-manager.h"
#include "../../net/rcv-message.h"
#include "../../rpc/common-def.h"

#include "rpc/rf-srv-rpc-sync-client.h"
#include "rpc/rf-srv-rpc-async-client.h"
#include "rpc/rf-srv-rpc-sync-server.h"
#include "server-rpc-service.h"

#include "server-internal-messenger.h"

namespace ccraft {
namespace server {
ServerInternalMessenger::ServerInternalMessenger(CreateServerInternalMessengerParam &createParam) :
        m_pNodeINRpcHandler(createParam.nodeInternalRpcHandler), m_iDispatchTpCnt(createParam.mngerDispatchWorkThreadsCnt) {
    CHECK(createParam.nodeInternalRpcHandler);
    m_pMemPool = createParam.memPool;
    if (!createParam.memPool) {
        m_bOwnMemPool = true;
        // TODO(sunchao): 优化此内存池参数。
        m_pMemPool = new ccsys::MemPool();
    }

    m_iIOThreadsCnt = createParam.netIOThreadsCnt;
    m_iPort = createParam.port;
    auto nat = new net::net_addr_t("0.0.0.0", createParam.port);
    std::shared_ptr<net::net_addr_t> sspNat(nat);
    timeval connTimeout = {
        .tv_sec = createParam.connectTimeout / 1000,
        .tv_usec = (createParam.connectTimeout % 1000) * 1000
    };
    net::NssConfig nc = {
        .sp = net::SocketProtocol::Tcp,
        .sspNat = sspNat,
        .logicPort = createParam.port,
        .netMgrType = net::NetStackWorkerMgrType::Unique,
        .memPool = m_pMemPool,
        .msgCallbackHandler = std::bind(&INodeInternalRpcHandler::OnRecvRpcResult, this, std::placeholders::_1),
        .connectTimeout = connTimeout
    };
    m_pSocketService = net::SocketServiceFactory::CreateService(nc);
    m_pSyncClient = new RfSrvInternalRpcClientSync(m_pSocketService, createParam.clientWaitResponseTimeout,
                                                createParam.clientRpcWorkThreadsCnt, m_pMemPool);
    m_pAsyncClient = new RfSrvInternalRpcClientAsync(m_pSocketService,
                                                     std::bind(&INodeInternalRpcHandler::OnRecvRpcResult, createParam.nodeInternalRpcHandler, std::placeholders::_1),
                                                     m_pMemPool);
    m_pServer = new RfSrvInternalRpcServerSync(this, createParam.serverRpcWorkThreadsCnt, m_pSocketService, m_pMemPool);
}

ServerInternalMessenger::~ServerInternalMessenger() {
    Stop();
    if (m_bOwnMemPool) {
        DELETE_PTR(m_pMemPool);
    }

    DELETE_PTR(m_pSyncClient);
    DELETE_PTR(m_pAsyncClient);
    DELETE_PTR(m_pServer);
    DELETE_PTR(m_pSocketService);
}

bool ServerInternalMessenger::Start() {
    if (!m_bStopped) {
        return true;
    }

    m_bStopped = false;
    hw_sw_memory_barrier();
    m_pSocketService->Start(m_iIOThreadsCnt, net::NonBlockingEventModel::Posix);
    m_pDispatchTp = new ccsys::ThreadPool<std::shared_ptr<net::NotifyMessage>>(m_iDispatchTpCnt);
    if (!m_pSyncClient->Start()) {
        return false;
    }

    return m_pServer->Start();
}

bool ServerInternalMessenger::Stop() {
    if (m_bStopped) {
        return true;
    }

    m_bStopped = true;
    hw_sw_memory_barrier();
    m_pSyncClient->Stop();
    m_pServer->Stop();
    m_pSocketService->Stop();
    DELETE_PTR(m_pDispatchTp);
    return true;
}

std::shared_ptr<protocol::AppendRfLogResponse> ServerInternalMessenger::AppendRfLogSync(rpc::SP_PB_MSG req,
                                                                                 net::net_peer_info_t &&peer) {
    return m_pSyncClient->AppendRfLog(req, std::move(peer));
}

rpc::ARpcClient::SentRet ServerInternalMessenger::AppendRfLogAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {
    return m_pAsyncClient->AppendRfLog(req, std::move(peer));
}

rpc::SP_PB_MSG ServerInternalMessenger::OnAppendRfLog(rpc::SP_PB_MSG sspMsg) {
    return m_pNodeINRpcHandler->OnAppendRfLog(sspMsg);
}

std::shared_ptr<protocol::RequestVoteResponse> ServerInternalMessenger::RequestVoteSync(rpc::SP_PB_MSG req,
                                                                                 net::net_peer_info_t &&peer) {
    return m_pSyncClient->RequestVote(req, std::move(peer));
}

rpc::ARpcClient::SentRet ServerInternalMessenger::RequestVoteAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {
    return m_pAsyncClient->RequestVote(req, std::move(peer));
}

rpc::SP_PB_MSG ServerInternalMessenger::OnRequestVote(rpc::SP_PB_MSG sspMsg) {
    return m_pNodeINRpcHandler->OnRequestVote(sspMsg);
}

void ServerInternalMessenger::dispatch_msg(std::shared_ptr<net::NotifyMessage> sspNM) {
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
                    m_pSyncClient->HandleMessage(sspNM);
                    LOGDFUN1("dispatch message type = Response.");
                }
            } else {
                LOGWFUN << "recv message is empty!";
            }
            break;
        }
        case net::NotifyMessageType::Worker: {
            m_pSyncClient->HandleMessage(sspNM);
            m_pServer->HandleMessage(sspNM);
            break;
        }
        case net::NotifyMessageType::Server: {
            LOGFFUN << "Messenger port = " << m_iPort << " cannot start to work.";
        }
    }
}

void ServerInternalMessenger::OnRecvRpcResult(std::shared_ptr<net::NotifyMessage> sspNM) {
    if (UNLIKELY(m_bStopped)) {
        return;
    }

    m_pDispatchTp->AddTask(std::bind(&ServerInternalMessenger::dispatch_msg, this, std::placeholders::_1), sspNM);
}
} // namespace server
} // namespace ccraft
