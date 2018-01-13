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
#include "../../rpc/exceptions.h"

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

    net::NssConfig nc(net::SocketProtocol::Tcp, sspNat, createParam.port, net::NetStackWorkerMgrType::Unique,
                      m_pMemPool, std::bind(&INodeInternalRpcHandler::OnRecvRpcReturnResult, this, std::placeholders::_1),
                      connTimeout);
    m_pSocketService = net::SocketServiceFactory::CreateService(nc);
    m_pSyncClient = new RfSrvInternalRpcClientSync(m_pSocketService, createParam.clientWaitResponseTimeout,
                                                createParam.clientRpcWorkThreadsCnt, m_pMemPool);
    m_pAsyncClient = new RfSrvInternalRpcClientAsync(m_pSocketService,
                                                     std::bind(&INodeInternalRpcHandler::OnRecvRpcReturnResult, createParam.nodeInternalRpcHandler, std::placeholders::_1),
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

    if (!m_pAsyncClient->Start()) {
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
    m_pAsyncClient->Stop();
    m_pServer->Stop();
    m_pSocketService->Stop();
    DELETE_PTR(m_pDispatchTp);
    return true;
}

std::shared_ptr<protocol::AppendEntriesResponse> ServerInternalMessenger::AppendEntriesSync(rpc::SP_PB_MSG req,
                                                                                 net::net_peer_info_t &&peer) {
    auto id = net::SndMessage::GetNewId();
    add_msg_to_mapper(id, m_pSyncClient, peer);
    return m_pSyncClient->AppendEntries(id, req, std::move(peer));
}

rpc::ARpcClient::SentRet ServerInternalMessenger::AppendEntriesAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {
    auto id = net::SndMessage::GetNewId();
    add_msg_to_mapper(id, m_pAsyncClient, peer);
    return m_pAsyncClient->AppendEntries(id, req, std::move(peer));
}

rpc::SP_PB_MSG ServerInternalMessenger::OnAppendEntries(rpc::SP_PB_MSG sspMsg) {
    return m_pNodeINRpcHandler->OnAppendEntries(sspMsg);
}

std::shared_ptr<protocol::RequestVoteResponse> ServerInternalMessenger::RequestVoteSync(rpc::SP_PB_MSG req,
                                                                                 net::net_peer_info_t &&peer) {
    auto id = net::SndMessage::GetNewId();
    add_msg_to_mapper(id, m_pSyncClient, peer);
    return m_pSyncClient->RequestVote(id, req, std::move(peer));
}

rpc::ARpcClient::SentRet ServerInternalMessenger::RequestVoteAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {
    auto id = net::SndMessage::GetNewId();
    add_msg_to_mapper(id, m_pAsyncClient, peer);
    return m_pAsyncClient->RequestVote(req, std::move(peer));
}

rpc::SP_PB_MSG ServerInternalMessenger::OnRequestVote(rpc::SP_PB_MSG sspMsg) {
    return m_pNodeINRpcHandler->OnRequestVote(sspMsg);
}

void ServerInternalMessenger::add_msg_to_mapper(net::Message::Id id, rpc::IMessageHandler *handler, net::net_peer_info_t &peer) {
    ccsys::SpinLock l(&m_slMsgMapper);
    if (m_msgHandlerMapper.end() != m_msgHandlerMapper.find(id)) {
        throw rpc::RpcException("Message id is conflict, if you send a high frequency of messages, you should use BIG_MSG_ID or BULK_MSG_ID in net module.");
    }

    m_msgHandlerMapper[id] = handler;
    auto iter = m_addrMsgMapper.find(peer);
    if (m_addrMsgMapper.end() != iter) {
        iter->second.insert(id);
    } else {
        auto s = std::unordered_set<net::Message::Id>();
        m_addrMsgMapper[peer] = s;
        s.insert(id);
    }
}

rpc::IMessageHandler* ServerInternalMessenger::remove_msg_from_msg_handler_map(net::Message::Id id) {
    auto iter = m_msgHandlerMapper.find(id);
    if (m_msgHandlerMapper.end() != iter) {
        return nullptr;
    }

    return iter->second;
}

rpc::IMessageHandler* ServerInternalMessenger::remove_msg_from_map(net::Message::Id id,
                                                                   net::net_peer_info_t &peer) {
    ccsys::SpinLock l(&m_slMsgMapper);
    rpc::IMessageHandler *handler = nullptr;
    if (!(handler = remove_msg_from_msg_handler_map(id))) {
        return nullptr;
    }

    auto iter = m_addrMsgMapper.find(peer);
    if (m_addrMsgMapper.end() == iter) {
        return nullptr;
    }

    iter->second.erase(id);
    return handler;
}

void ServerInternalMessenger::clear_peer_msg_map(net::net_peer_info_t &peer) {
    ccsys::SpinLock l(&m_slMsgMapper);
    auto iter = m_addrMsgMapper.find(peer);
    if (m_addrMsgMapper.end() == iter || 0 == iter->second.size()) {
        return;
    }

    for (auto id : iter->second) {
        remove_msg_from_msg_handler_map(id);
    }

    iter->second.clear();
    m_addrMsgMapper.erase(peer);
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
                auto id = rm->GetId();
                auto peer = rm->GetPeerInfo();
                auto clientHandler = remove_msg_from_map(id, peer);
                if (clientHandler) {
                    clientHandler->HandleMessage(sspNM);
                    LOGDFUN1("dispatch message type = Response.");
                } else {
                    m_pServer->HandleMessage(sspNM);
                    LOGDFUN1("dispatch message type = Request.");
                }
            } else {
                LOGWFUN << "recv message is empty!";
            }
            break;
        }
        case net::NotifyMessageType::Worker: {
            auto *wnm = dynamic_cast<ccraft::net::WorkerNotifyMessage*>(sspNM.get());
            if (wnm) {
                auto peer = wnm->GetPeer();
                clear_peer_msg_map(peer);
                LOGIFUN << "worker notify message , rc = " << (int)wnm->GetCode() << ", message = " << wnm->What() << std::endl;
            }
            break;
        }
        case net::NotifyMessageType::Server: {
            auto *snm = dynamic_cast<ccraft::net::ServerNotifyMessage*>(sspNM.get());
            if (snm) {
                std::cout << "server notify message , rc = " << (int)snm->GetCode() << ", message = " << snm->What() << std::endl;
            }
            break;
        }
    }
}

void ServerInternalMessenger::OnRecvRpcReturnResult(std::shared_ptr<net::NotifyMessage> sspNM) {
    if (UNLIKELY(m_bStopped)) {
        return;
    }

    m_pDispatchTp->AddTask(std::bind(&ServerInternalMessenger::dispatch_msg, this, std::placeholders::_1), sspNM);
}


} // namespace server
} // namespace ccraft
