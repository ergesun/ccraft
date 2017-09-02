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

#include "imessage-handler.h"

// TODO(sunchao): fix rpc to use protobuf service. Now we just use protobuf data serialize feature.
namespace ccraft {
namespace common {
class MemPool;
}
namespace net {
class ISocketService;
}
namespace rpc {
class IRpcHandler;
/**
 * [Message format]
 *    client -> server :  |net common header(Message::HeaderSize() bytes)|msg type|[handler id(2bytes)|protobuf msg(n bytes)]|
 *    server -> client :  |net common header(Message::HeaderSize() bytes)|msg type|[rpc code(2bytes)|protobuf msg(n bytes or 0 bytes if no return value)]|
 */
class RpcServer : public IService, public IMessageHandler {
public:
    /**
     *
     * @param workThreadsCnt
     * @param ss
     * @param memPool 如果为空，则内部自己构造。
     */
    RpcServer(uint16_t workThreadsCnt, net::ISocketService *ss, common::MemPool *memPool = nullptr);
    ~RpcServer() override;

    bool Start() override;
    bool Stop() override;

    bool RegisterRpc(uint16_t id, IRpcHandler *handler);
    void FinishRegisterRpc();

    void HandleMessage(std::shared_ptr<net::NotifyMessage> sspNM) override;

private:
    void proc_msg(std::shared_ptr<net::NotifyMessage> sspNM);

private:
    bool                                                        m_bStopped              = true;
    uint16_t                                                    m_iWorkThreadsCnt       = 0;
    // 关联指针，无需本类释放。
    net::ISocketService                                        *m_pSocketService        = nullptr;
    bool                                                        m_bOwnMemPool           = false;
    common::MemPool                                            *m_pRpcMemPool           = nullptr;
    common::ThreadPool<std::shared_ptr<net::NotifyMessage>>    *m_pWorkThreadPool       = nullptr;
    std::unordered_map<uint16_t, IRpcHandler*>                  m_hmHandlers;
    uint16_t                                                    m_iPort;
    bool                                                        m_bRegistered           = false;
};
}
}

#endif //CCRAFT_RPC_SERVER_H
