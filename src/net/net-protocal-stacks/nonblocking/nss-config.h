/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_NSS_CONFIG_H
#define CCRAFT_NET_NSS_CONFIG_H

#include "../../notify-message.h"

namespace ccraft {
namespace common {
    class MemPool;
}

namespace net {
    class INetStackWorkerManager;
    /**
     * 无需move构造，无需copy构造。
     */
    struct NssConfig {
        NssConfig(SocketProtocal sp, std::shared_ptr<net_addr_t> sspNat, uint16_t logicPort,
                    std::shared_ptr<INetStackWorkerManager> sspMgr, common::MemPool *memPool,
                    NotifyMessageCallbackHandler msgCallbackHandler, timeval connectTimeout) {
            this->sp = sp;
            this->sspNat = sspNat;
            this->logicPort = logicPort;
            this->sspMgr = sspMgr;
            this->memPool = memPool;
            this->msgCallbackHandler = msgCallbackHandler;
            this->connectTimeout = connectTimeout;
        }

        SocketProtocal                          sp;
        std::shared_ptr<net_addr_t>             sspNat;
        uint16_t                                logicPort;
        std::shared_ptr<INetStackWorkerManager> sspMgr;
        /**
         * 关联关系，无需本类释放。
         */
        common::MemPool                        *memPool;
        NotifyMessageCallbackHandler            msgCallbackHandler;
        timeval                                 connectTimeout;
    };
}
}

#endif //CCRAFT_NET_NSS_CONFIG_H
