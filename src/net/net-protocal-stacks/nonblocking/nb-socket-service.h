/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_NBSOCKETSERVICE_H
#define CCRAFT_NET_CORE_NBSOCKETSERVICE_H

#include <memory>
#include <cassert>

#include "../inet-stack-worker-manager.h"
#include "../../abstract-socket-service.h"
#include "socket/event-drivers/ievent-driver.h"
#include "../../message.h"
#include "../../notify-message.h"
#include "nss-config.h"

// TODO(sunchao): 可配值
#define MAX_EVENTS  256

namespace ccraft {
namespace net {
class AEventManager;

/**
 * 支持Tcp/Udp(暂未实现)协议的收发server。
 * TODO(sunchao): 扩展为可listen多个端口？
 */
class NBSocketService : public ASocketService {
public:
    /**
     * TODO(sunchao): 入参改为右值，必须！否则会有易用性问题：内存控制，可以参考rpc单元测试main.cc的代码就清楚了。
     * @param nlt 如果为空，则是为仅仅一个服务于client的服务，否则为server信息，会开启server的服务。
     * @param sspMgr  worker的管理策略。
     * @param memPool 内存池。
     * @param sspMgr 所有权归本类所有，用户不可以保留。
     */
    explicit NBSocketService(NssConfig nssConfig);

    ~NBSocketService() override;

    /**
     * 开启服务。
     * @param m 模式。
     * @return 成功true,失败false.
     */
    bool Start(uint16_t ioThreadsCnt, NonBlockingEventModel m) override;

    /**
     * 一旦stop，就不能再用了(不可以重新start再用)。
     * @return
     */
    bool Stop() override;

    /**
     * 一旦发送成功，则m的所有权便属于了框架，user无需也不可以再管理此SndMessage，m生命周期由框架控制。
     * 如果发送失败，则m的生命周期由调用者控制。
     * @param m
     * @return
     */
    bool SendMessage(SndMessage *m) override;

    /**
     * 断开一个TCP连接。
     * @param peer
     * @return
     */
    bool Disconnect(net_peer_info_t peer) override;

private:
    bool connect(net_peer_info_t &npt);
    void on_stack_connect(AFileEventHandler *handler);
    bool on_logic_connect(AFileEventHandler *handler);
    void on_finish(AFileEventHandler *handler);

private:
    NssConfig           m_conf;
    AEventManager      *m_pEventManager = nullptr;
    bool                m_bStopped = true;
}; // class NBSocketService
}  // namespace net
} // namespace ccraft

#endif //CCRAFT_NET_CORE_NBSOCKETSERVICE_H
