/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <iostream>
#include "../test-snd-message.h"

#include "../../../../net/socket-service-factory.h"
#include "../../../../net/rcv-message.h"
#include "../../../../common/buffer.h"
#include "../../../../common/mem-pool.h"

#include "tcp-server-test-case.h"
#include "../../../../net/net-protocal-stacks/msg-worker-managers/unique-worker-manager.h"

namespace ccraft {
namespace test {
net::ISocketService     *TcpServerTest::s_ss = nullptr;
common::MemPool         *TcpServerTest::m_mp = nullptr;

void TcpServerTest::Run() {
    auto nat = new ccraft::net::net_addr_t("0.0.0.0", 2210);
    std::shared_ptr<ccraft::net::net_addr_t> ssp_npt(nat);
    m_mp = new ccraft::common::MemPool();
    std::shared_ptr<ccraft::net::INetStackWorkerManager> sspMgr =
        std::shared_ptr<ccraft::net::INetStackWorkerManager>(new ccraft::net::UniqueWorkerManager());
    s_ss = net::SocketServiceFactory::CreateService(ccraft::net::SocketProtocal::Tcp, ssp_npt, 2210, m_mp,
                                                    std::bind(&TcpServerTest::recv_msg,
                                                              std::placeholders::_1),
                                                    sspMgr);
    if (!s_ss->Start(2, ccraft::net::NonBlockingEventModel::Posix)) {
        throw std::runtime_error("cannot start SocketService");
    }
}

void TcpServerTest::recv_msg(std::shared_ptr<ccraft::net::NotifyMessage> sspNM) {
    switch (sspNM->GetType()) {
        case ccraft::net::NotifyMessageType::Message: {
            ccraft::net::MessageNotifyMessage *mnm = dynamic_cast<ccraft::net::MessageNotifyMessage*>(sspNM.get());
            auto rm = mnm->GetContent();
            if (rm) {
                auto respBuf = rm->GetDataBuffer();
#ifdef WITH_MSG_ID
                #ifdef BULK_MSG_ID
                std::cout << "request = "  << respBuf->GetPos() << ", " << "message id is { ts = " << rm->GetId().ts
                          << ", seq = " << rm->GetId().seq << "}" << std::endl;
#else
                std::cout << "request = "  << respBuf->GetPos() << ", " << "message id is " << rm->GetId() << "." << std::endl;
#endif
                TestSndMessage *tsm = new TestSndMessage(m_mp, rm->GetPeerInfo(),  rm->GetId(), "server response: hello client!");
#else
                std::cout << "request = "  << respBuf->GetPos() << "." << std::endl;
                std::stringstream ss;
                ss << "Server response: hello client! Req idx = " << atomic_addone_and_fetch(&s_idx) << ".";
                TestSndMessage *tsm = new TestSndMessage(m_mp, rm->GetPeerInfo(), ss.str());
#endif
                bool rc = s_ss->SendMessage(tsm);
                if (rc) {
                }
            }
            break;
        }
        case ccraft::net::NotifyMessageType::Worker : {
            auto *wnm = dynamic_cast<ccraft::net::WorkerNotifyMessage*>(sspNM.get());
            if (wnm) {
                std::cout << "worker notify message , rc = " << (int)wnm->GetCode() << ", message = " << wnm->What() << std::endl;
            }
            break;
        }
        case ccraft::net::NotifyMessageType::Server: {
            auto *snm = dynamic_cast<ccraft::net::ServerNotifyMessage*>(sspNM.get());
            if (snm) {
                std::cout << "server notify message , rc = " << (int)snm->GetCode() << ", message = " << snm->What() << std::endl;
            }
            break;
        }
    }
}
}
}
