/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NODE_INTERNAL_MESSENGER_H
#define CCRAFT_NODE_INTERNAL_MESSENGER_H

#include "../../iservice.h"
#include "../../common/cctime.h"
#include "../../common/blocking-queue.h"
#include "../../common/thread-pool.h"

#include "inode-internal-rpc-handler.h"

namespace ccraft {
namespace common {
class MemPool;
}
namespace net {
class ISocketService;
class NotifyMessage;
}

namespace server {
class RfNode;
class RfNodeInternalRpcClientSync;
class RfNodeInternalRpcServerSync;

struct CreateNodeInternalMessengerParam {
    CreateNodeInternalMessengerParam(RfNode *prfNode, uint16_t cliRpcWorkThreadsCnt, common::cctime_t cliWaitRespTimeout,
                                    uint16_t srvRpcWorkThreadsCnt, uint16_t mngerWorkThreadsCnt, uint16_t inetIOThreadsCnt,
                                    uint16_t iport, common::MemPool *pMp = nullptr) :
        rfNode(prfNode), clientRpcWorkThreadsCnt(cliRpcWorkThreadsCnt), clientWaitResponseTimeout(cliWaitRespTimeout),
        serverRpcWorkThreadsCnt(srvRpcWorkThreadsCnt), mngerDispatchWorkThreadsCnt(mngerWorkThreadsCnt), netIOThreadsCnt(inetIOThreadsCnt),
        port(iport), memPool(pMp) {}

    RfNode             *rfNode;
    uint16_t            clientRpcWorkThreadsCnt;
    common::cctime_t    clientWaitResponseTimeout;
    uint16_t            serverRpcWorkThreadsCnt;
    uint16_t            mngerDispatchWorkThreadsCnt;
    uint16_t            netIOThreadsCnt;
    uint16_t            port;
    common::MemPool    *memPool;
};

// TODO(sunchao): 考虑有必要再对messenger进行抽象不.
class NodeInternalMessenger : public IService, public INodeInternalRpcHandler {
public:
    /**
     *
     * @param rfNode
     * @param workThreadsCnt
     * @param netIOThreadsCnt
     * @param port
     * @param timeout
     * @param memPool
     */
    explicit NodeInternalMessenger(CreateNodeInternalMessengerParam &createParam);
    ~NodeInternalMessenger() override;

    bool Start() override;
    bool Stop() override;

    std::shared_ptr<rpc::AppendOpLogResponse> AppendRfLogSync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::SP_PB_MSG OnAppendRfLog(rpc::SP_PB_MSG sspMsg) override;

private:
    void recv_msg(std::shared_ptr<net::NotifyMessage> sspNM);
    void dispatch_msg(std::shared_ptr<net::NotifyMessage> sspNM);

private:
    uint16_t                                                    m_iIOThreadsCnt  = 0;
    uint16_t                                                    m_iPort          = 0;
    bool                                                        m_bStopped       = true;
    /**
     * 关联关系，无需本类释放。
     */
    RfNode                                                     *m_pRfNode         = nullptr;
    net::ISocketService                                        *m_pSocketService  = nullptr;
    RfNodeInternalRpcClientSync                                *m_pClient         = nullptr;
    RfNodeInternalRpcServerSync                                *m_pServer         = nullptr;
    bool                                                        m_bOwnMemPool     = false;
    common::MemPool                                            *m_pMemPool        = nullptr;
    uint16_t                                                    m_iDispatchTpCnt  = 0;
    common::ThreadPool<std::shared_ptr<net::NotifyMessage>>    *m_pDispatchTp     = nullptr;
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_NODE_INTERNAL_MESSENGER_H
