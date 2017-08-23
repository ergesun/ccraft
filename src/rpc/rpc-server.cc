/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <glog/logging.h>

#include "../net/socket-service-factory.h"
#include "../net/net-protocal-stacks/msg-worker-managers/unique-worker-manager.h"
#include "../net/rcv-message.h"
#include "../common/buffer.h"
#include "protobuf-utils.h"
#include "../common/codec-utils.h"

#include "framework-response.h"
#include "rpc-handler.h"

#include "rpc-server.h"



namespace ccraft {
    namespace rpc {
        RpcServer::RpcServer(uint16_t workThreadsCnt, uint16_t netIOThreadsCnt, uint16_t port)
                    : m_iWorkThreadsCnt(workThreadsCnt), m_iNetIOThreadsCnt(netIOThreadsCnt) {
            if (0 == workThreadsCnt) {
                m_iWorkThreadsCnt = (uint16_t)(common::PHYSICAL_CPUS_CNT * 2);
            }

            m_pRpcMemPool = new common::MemPool();
            auto nat = new net::net_addr_t("0.0.0.0", port);
            std::shared_ptr<net::net_addr_t> sspNat(nat);
            std::shared_ptr<net::INetStackWorkerManager> sspMgr = std::shared_ptr<net::INetStackWorkerManager>(new net::UniqueWorkerManager());
            m_pSocketService = net::SocketServiceFactory::CreateService(net::SocketProtocal::Tcp,
                                                                        sspNat,
                                                                        port,
                                                                        m_pRpcMemPool,
                                                                        std::bind(&RpcServer::recv_msg, this, std::placeholders::_1),
                                                                        sspMgr);
            m_hmHandlers.reserve(100);
        }

        RpcServer::~RpcServer() {
            DELETE_PTR(m_pRpcMemPool);
            DELETE_PTR(m_pSocketService);
        }

        void RpcServer::Start() {
            if (!m_bStopped) {
                return;
            }

            m_bStopped = false;
            if (!m_pSocketService->Start(m_iNetIOThreadsCnt, ccraft::net::NonBlockingEventModel::Posix)) {
                LOGFFUN << "start socket service failed!";
                throw std::runtime_error("start socket service failed!");
            }

            m_pWorkThreadPool = new common::ThreadPool<std::shared_ptr<net::NotifyMessage>>(m_iWorkThreadsCnt);
        }

        void RpcServer::Stop() {
            if (m_bStopped) {
                return;
            }

            m_bStopped = true;
            if (m_pSocketService->Stop()) {
                throw std::runtime_error("cannot stop RpcServer's SocketService");
            }

            DELETE_PTR(m_pWorkThreadPool);
        }

        void RpcServer::RegisterRpc(uint16_t id, IRpcHandler *handler) {
            assert(handler);
            m_hmHandlers[id] = handler;
        }

        void RpcServer::recv_msg(std::shared_ptr<net::NotifyMessage> sspNM) {
            m_pWorkThreadPool->AddTask(std::bind(&RpcServer::proc_msg, this, std::placeholders::_1), sspNM);
        }

        void RpcServer::proc_msg(std::shared_ptr<net::NotifyMessage> sspNM) {
            switch (sspNM->GetType()) {
                case net::NotifyMessageType::Message: {
                    auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());
                    auto rm = mnm->GetContent();
                    if (LIKELY(rm)) {
                        auto reqBuf = rm->GetDataBuffer();
                        auto handlerId = ByteOrderUtils::ReadUInt16(reqBuf->Pos);
                        reqBuf->MoveHeadBack(2);
                        auto handler = m_hmHandlers[handlerId];
                        if (!handler) {
                            LOGEFUN << "there is no handler for handler id " << handlerId;
                            BadRpcHandlerIdResponse(handlerId);
                            m_pSocketService->SendMessage(badRpcHandlerIdResponse);
                            return;
                        }

                        auto request = handler->CreateRequest();
                        if (!request.get()) {
                            LOGEFUN << "cannot create request for handler id " << handlerId;
                            RpcServiceInternalErrorResponse(handlerId);
                            m_pSocketService->SendMessage(rpcServiceInternalErrorResponse);
                            return;
                        }
                        if (!ProtoBufUtils::Parse(reqBuf, request.get())) {
                            LOGEFUN << "cannot parse request for handler id " << handlerId;
                            BadRequestMsgResponse(handlerId);
                            m_pSocketService->SendMessage(badRequestMsgResponse);
                            return;
                        }

                        auto res = handler->Handle(request);
                        auto respMessage = new RpcResponse(handlerId, res);
                        m_pSocketService->SendMessage(respMessage);
                        LOGDFUN2("handled message for handler id ", handlerId);
                    } else {
                        LOGWFUN << "recv message is empty!";
                    }
                    break;
                }
                case net::NotifyMessageType::Worker: {
                    auto *wnm = dynamic_cast<net::WorkerNotifyMessage*>(sspNM.get());
                    if (wnm) {
                        LOGEFUN << "rc = " << (int)wnm->GetCode() << ", message = " << wnm->What();
                    }
                    break;
                }
                case net::NotifyMessageType::Server: {
                    auto *snm = dynamic_cast<net::ServerNotifyMessage*>(sspNM.get());
                    if (snm) {
                        LOGEFUN << "rc = " << (int)snm->GetCode() << ", message = " << snm->What();
                    }

                    throw std::runtime_error("rpc server crushed!");
                }
            }
        }
    }
}
