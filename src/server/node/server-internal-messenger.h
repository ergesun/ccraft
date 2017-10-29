/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NODE_INTERNAL_MESSENGER_H
#define CCRAFT_NODE_INTERNAL_MESSENGER_H

#include "iservice.h"
#include "../../common/cctime.h"
#include "../../common/blocking-queue.h"
#include "../../common/thread-pool.h"
#include "../../rpc/abstract-rpc-client.h"

#include "iserver-internal-rpc-handler.h"

namespace ccraft {
namespace common {
class MemPool;
}
namespace net {
class ISocketService;
class NotifyMessage;
}

namespace protocal {
class AppendRfLogResponse;
class RequestVoteResponse;
}

namespace server {
class RfSrvInternalRpcClientSync;
class RfSrvInternalRpcClientAsync;
class RfSrvInternalRpcServerSync;

struct CreateServerInternalMessengerParam {
    /**
     *
     * @param handler
     * @param cliRpcWorkThreadsCnt
     * @param cliWaitRespTimeout
     * @param srvRpcWorkThreadsCnt
     * @param mngerWorkThreadsCnt
     * @param inetIOThreadsCnt
     * @param iport
     * @param pMp 可以为nullptr
     * @param connTimeout
     */
    CreateServerInternalMessengerParam(INodeInternalRpcHandler *handler, uint16_t cliRpcWorkThreadsCnt, const common::cctime_t &cliWaitRespTimeout,
                                    uint16_t srvRpcWorkThreadsCnt, uint16_t mngerWorkThreadsCnt, uint16_t inetIOThreadsCnt,
                                    uint16_t iport, common::MemPool *pMp, int32_t connTimeout) :
        nodeInternalRpcHandler(handler), clientRpcWorkThreadsCnt(cliRpcWorkThreadsCnt), clientWaitResponseTimeout(cliWaitRespTimeout),
        serverRpcWorkThreadsCnt(srvRpcWorkThreadsCnt), mngerDispatchWorkThreadsCnt(mngerWorkThreadsCnt), netIOThreadsCnt(inetIOThreadsCnt),
        port(iport), memPool(pMp), connectTimeout(connTimeout) {}

    INodeInternalRpcHandler   *nodeInternalRpcHandler;
    uint16_t                   clientRpcWorkThreadsCnt;
    common::cctime_t           clientWaitResponseTimeout;
    uint16_t                   serverRpcWorkThreadsCnt;
    uint16_t                   mngerDispatchWorkThreadsCnt;
    uint16_t                   netIOThreadsCnt;
    uint16_t                   port;
    common::MemPool           *memPool;
    int32_t                    connectTimeout;
};

// TODO(sunchao): 考虑有必要再对messenger抽象不.
class ServerInternalMessenger : public IService, public INodeInternalRpcHandler {
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
    explicit ServerInternalMessenger(CreateServerInternalMessengerParam &createParam);
    ~ServerInternalMessenger() override;

    bool Start() override;
    bool Stop() override;

    std::shared_ptr<protocal::AppendRfLogResponse> AppendRfLogSync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::ARpcClient::SendRet AppendRfLogAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::SP_PB_MSG OnAppendRfLog(rpc::SP_PB_MSG sspMsg) override;

    std::shared_ptr<protocal::RequestVoteResponse> RequestVoteSync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::ARpcClient::SendRet RequestVoteAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::SP_PB_MSG OnRequestVote(rpc::SP_PB_MSG sspMsg) override;

    void OnRecvRpcCallbackMsg(std::shared_ptr<net::NotifyMessage> sspNM) override;

private:
    void dispatch_msg(std::shared_ptr<net::NotifyMessage> sspNM);

private:
    uint16_t                                                    m_iIOThreadsCnt  = 0;
    uint16_t                                                    m_iPort          = 0;
    bool                                                        m_bStopped       = true;
    /**
     * 关联关系，无需本类释放。
     */
    INodeInternalRpcHandler                                    *m_pNodeINRpcHandler  = nullptr;
    net::ISocketService                                        *m_pSocketService     = nullptr;
    RfSrvInternalRpcClientSync                                 *m_pSyncClient        = nullptr;
    RfSrvInternalRpcClientAsync                                *m_pAsyncClient       = nullptr;
    RfSrvInternalRpcServerSync                                 *m_pServer            = nullptr;
    bool                                                        m_bOwnMemPool        = false;
    common::MemPool                                            *m_pMemPool           = nullptr;
    uint16_t                                                    m_iDispatchTpCnt     = 0;
    common::ThreadPool<std::shared_ptr<net::NotifyMessage>>    *m_pDispatchTp        = nullptr;
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_NODE_INTERNAL_MESSENGER_H
