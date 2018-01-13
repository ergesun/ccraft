/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <glog/logging.h>

#include "../net/socket-service-factory.h"
#include "../net/net-protocol-stacks/msg-worker-managers/unique-worker-manager.h"
#include "../net/rcv-message.h"
#include "../common/buffer.h"
#include "../common/protobuf-utils.h"
#include "../common/codec-utils.h"
#include "../common/global-vars.h"

#include "response.h"
#include "rpc-handler.h"
#include "rpc-server.h"

namespace ccraft {
namespace rpc {
RpcServer::RpcServer(uint16_t workThreadsCnt, net::ISocketService *ss, ccsys::MemPool *memPool) :
    m_pSocketService(ss), m_pRpcMemPool(memPool) {
    CHECK(ss);
    if (0 == workThreadsCnt) {
        m_iWorkThreadsCnt = (uint16_t)(common::LOGIC_CPUS_CNT * 2);
    }

    if (!memPool) {
        m_bOwnMemPool = true;
        m_pRpcMemPool = new ccsys::MemPool();
    }

    m_hmHandlers.reserve(100);
}

RpcServer::~RpcServer() {
    Stop();
    if (m_bOwnMemPool) {
        DELETE_PTR(m_pRpcMemPool);
    }

    for (auto p : m_hmHandlers) {
        DELETE_PTR(p.second);
    }
}

bool RpcServer::Start() {
    if (!m_bStopped || !m_bRegistered) {
        return false;
    }

    m_bStopped = false;
    hw_sw_memory_barrier();
    m_pWorkThreadPool = new ccsys::ThreadPool<std::shared_ptr<net::NotifyMessage>>(m_iWorkThreadsCnt);

    return true;
}

bool RpcServer::Stop() {
    if (m_bStopped) {
        return true;
    }

    m_bStopped = true;
    hw_sw_memory_barrier();
    DELETE_PTR(m_pWorkThreadPool);
    return true;
}

bool RpcServer::RegisterRpc(uint16_t id, IRpcHandler *handler) {
    if (m_bRegistered) {
        return false;
    }

    assert(handler);
    m_hmHandlers[id] = handler;

    return true;
}

void RpcServer::FinishRegisterRpc() {
    m_bRegistered = true;
    hw_sw_memory_barrier();
}

void RpcServer::HandleMessage(std::shared_ptr<net::NotifyMessage> sspNM) {
    m_pWorkThreadPool->AddTask(std::bind(&RpcServer::proc_msg, this, std::placeholders::_1), sspNM);
}

void RpcServer::proc_msg(std::shared_ptr<net::NotifyMessage> sspNM) {
    switch (sspNM->GetType()) {
        case net::NotifyMessageType::Message: {
            auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());
            auto rm = mnm->GetContent();
            if (LIKELY(rm)) {
                auto reqBuf = rm->GetDataBuffer();
                auto handlerId = ByteOrderUtils::ReadUInt16(reqBuf->GetPos());
                reqBuf->MoveHeadBack(sizeof(uint16_t));
                auto handler = m_hmHandlers[handlerId];
                LOGDFUN2("handle message for handler id ", handlerId);
                if (!handler) {
                    LOGEFUN << "there is no handler for handler id " << handlerId;
                    m_pSocketService->SendMessage(new RpcResponseBase(RpcCode::ErrorNoHandler, handlerId));
                    return;
                }

                auto request = handler->CreateMessage();
                if (!request) {
                    LOGEFUN << "cannot create request for handler id " << handlerId;
                    m_pSocketService->SendMessage(new RpcResponseBase(RpcCode::ErrorInternal, handlerId));
                    return;
                }
                if (!common::ProtoBufUtils::Deserialize(reqBuf, request.get())) {
                    LOGEFUN << "cannot parse request for handler id " << handlerId;
                    m_pSocketService->SendMessage(new RpcResponseBase(RpcCode::ErrorMsg, handlerId));
                    return;
                }

                auto res = handler->Handle(request);
                auto respMessage = new RpcResponse(res, handlerId);
                respMessage->SetId(rm->GetId());
                respMessage->SetMemPool(m_pRpcMemPool);
                respMessage->SetPeerInfo(rm->GetPeerInfo());
                if (!m_pSocketService->SendMessage(respMessage)) {
                    LOGWFUN << "SendMessage failed for handler id " << handlerId << ", peer = {"
                            << respMessage->GetPeerInfo().nat.addr << ":" << respMessage->GetPeerInfo().nat.port << "}.";
                }
            } else {
                LOGEFUN << "recv message is empty!";
            }
            break;
        }
        case net::NotifyMessageType::Worker: {
            auto *wnm = dynamic_cast<net::WorkerNotifyMessage*>(sspNM.get());
            if (wnm) {
                LOGWFUN << "rc = " << (int)wnm->GetCode() << ", message = " << wnm->What();
            }
            break;
        }
        case net::NotifyMessageType::Server: {
            auto *snm = dynamic_cast<net::ServerNotifyMessage*>(sspNM.get());
            if (snm) {
                LOGFFUN << "rc = " << (int)snm->GetCode() << ", message = " << snm->What()
                        << ". rpc server port = " << m_iPort << " crushed!";
            }

            throw std::runtime_error("rpc server crushed!");
        }
    }
}
} // namespace rpc
} // namespace ccraft
