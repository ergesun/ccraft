/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_POSIXTCPCONNECTION_H
#define CCRAFT_NET_CORE_POSIXTCPCONNECTION_H

#include "../../../../../../../common/common-def.h"
#include "../../../../../../common-def.h"
#include "../../abstract-file-event-handler.h"
#include "stack/server-socket.h"
#include "../../../event-drivers/ievent-driver.h"
#include "../../abstract-event-manager.h"
#include "../../../../../../notify-message.h"
#include "../../../../../../../ccsys/thread-pool.h"

namespace ccraft {
namespace ccsys {
class MemPool;
}

namespace net {
class PosixTcpServerEventHandler : public AFileEventHandler {
public:
    PosixTcpServerEventHandler(EventWorker *ew, net_addr_t *nat,
        ConnectHandler stackConnectHandler, ConnectFunc onLogicConnect,
        ccsys::MemPool *memPool, NotifyMessageCallbackHandler msgCallbackHandler);
    ~PosixTcpServerEventHandler() override;

    bool Initialize() override;
    bool HandleReadEvent() override;
    bool HandleWriteEvent() override;

    ANetStackMessageWorker *GetStackMsgWorker() override;

private:
    inline void handle_message(NotifyMessage* nm);

private:
    PosixTcpServerSocket             *m_pSrvSocket;
    ConnectHandler                    m_onStackConnect;
    ConnectFunc                       m_onLogicConnect;
    /**
     * 关联关系，外部创建者会释放，本类无需释放。
     */
    ccsys::MemPool                  *m_pMemPool;
    NotifyMessageCallbackHandler      m_msgCallbackHandler;
    ccsys::ThreadPool<void*>        *m_tp;
};
} // namespace net
} // namespace ccraft

#endif //CCRAFT_NET_CORE_POSIXTCPCONNECTION_H
