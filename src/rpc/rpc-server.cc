/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <glog/logging.h>

#include "../net/socket-service-factory.h"
#include "../net/net-protocal-stacks/msg-worker-managers/unique-worker-manager.h"
#include "../net/rcv-message.h"

#include "rpc-server.h"

namespace ccraft {
    namespace rpc {
        RpcServer::RpcServer(uint16_t workThreadsCnt, uint16_t netIOThreadsCnt, int16_t port, common::MemPool *mp)
                    : m_iWorkThreadsCnt(workThreadsCnt), m_iNetIOThreadsCnt(netIOThreadsCnt) {
            CHECK(mp);
            m_pMemPool = mp;
            auto nat = new net::net_addr_t("0.0.0.0", port);
            std::shared_ptr<net::net_addr_t> sspNat(nat);
            std::shared_ptr<net::INetStackWorkerManager> sspMgr = std::shared_ptr<net::INetStackWorkerManager>(new net::UniqueWorkerManager());
            m_pSocketService = net::SocketServiceFactory::CreateService(net::SocketProtocal::Tcp,
                                                                        sspNat,
                                                                        m_pMemPool,
                                                                        std::bind(&RpcServer::recv_msg, this, std::placeholders::_1),
                                                                        sspMgr);
        }

        void RpcServer::Start() {
            m_bStopped = false;
            if (!m_pSocketService->Start(m_iNetIOThreadsCnt)) {
                LOGFFUN << "start socket service failed!";
                throw std::runtime_error("start socket service failed!");
            }

            m_pWorkThreadPool = new common::ThreadPool<std::shared_ptr<net::NotifyMessage>>(m_iWorkThreadsCnt);
        }

        void RpcServer::Stop() {
            m_bStopped = true;
            if (m_pSocketService->Stop()) {
                throw std::runtime_error("cannot stop RpcServer's SocketService");
            }

            DELETE_PTR(m_pWorkThreadPool);
        }

        void RpcServer::recv_msg(std::shared_ptr<net::NotifyMessage> sspNM) {
            switch (sspNM->GetType()) {
                case net::NotifyMessageType::Message: {
                    m_pWorkThreadPool->AddTask(std::bind(&RpcServer::proc_msg, this, std::placeholders::_1), sspNM);
                    break;
                }
                case net::NotifyMessageType::Worker: {
                    net::WorkerNotifyMessage *wnm = dynamic_cast<net::WorkerNotifyMessage*>(sspNM.get());
                    if (wnm) {
                    }
                    break;
                }
                case net::NotifyMessageType::Server: {
                    net::ServerNotifyMessage *snm = dynamic_cast<net::ServerNotifyMessage*>(sspNM.get());
                    if (snm) {
                    }
                    break;
                }
            }
        }

        void RpcServer::proc_msg(std::shared_ptr<net::NotifyMessage> sspNM) {
            while (!m_bStopped) {
                // 这里Pop有锁具有屏障作用就不需要担心m_bStopped不更新了。
                //auto msg = m_bqMessagePtrs.Pop();

            }
        }
    }
}
