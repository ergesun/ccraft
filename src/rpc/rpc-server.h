/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_SERVER_H
#define CCRAFT_RPC_SERVER_H

#include <cstdint>
#include <memory>

#include "../iservice.h"

#include "../net/notify-message.h"
#include "../common/blocking-queue.h"
#include "../common/thread-pool.h"

// TODO(sunchao): fix rpc to use protobuf service. Now we just use protobuf data serialize feature.
namespace ccraft {
    namespace common {
        class MemPool;
    }
    namespace net {
        class ISocketService;
    }
    namespace rpc {
        class RpcServer : public IService {
        public:
            //typedef std::function<void()>

            /**
             *
             * @param workThreadsCnt default is [cpu Hyper-Threading threads number] * 2
             * @param netIOThreadsCnt
             * @param port
             * @param mp
             */
            RpcServer(uint16_t workThreadsCnt, uint16_t netIOThreadsCnt, int16_t port);
            ~RpcServer();

            void Start() override;
            void Stop() override;

            //void RegisterRpc(uint16_t id, )

        private:
            void recv_msg(std::shared_ptr<net::NotifyMessage> sspNM);
            void proc_msg(std::shared_ptr<net::NotifyMessage> sspNM);

        private:
            bool                                                            m_bStopped         = true;
            uint16_t                                                        m_iWorkThreadsCnt  = 0;
            uint16_t                                                        m_iNetIOThreadsCnt = 0;
            net::ISocketService                                            *m_pSocketService   = nullptr;
            common::MemPool                                                *m_pMemPool         = nullptr;
            common::ThreadPool<std::shared_ptr<net::NotifyMessage>>        *m_pWorkThreadPool  = nullptr;
            //std::vector<>
        };
    }
}

#endif //CCRAFT_RPC_SERVER_H
