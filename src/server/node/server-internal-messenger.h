/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NODE_INTERNAL_MESSENGER_H
#define CCRAFT_NODE_INTERNAL_MESSENGER_H

#include "iservice.h"
#include "../../ccsys/cctime.h"
#include "../../common/blocking-queue.h"
#include "../../ccsys/thread-pool.h"
#include "../../rpc/abstract-rpc-client.h"

#include "inode-internal-rpc-handler.h"

namespace ccraft {
namespace rpc {
class IMessageHandler;
}

namespace ccsys {
class MemPool;
}

namespace net {
class ISocketService;
class NotifyMessage;
}

namespace protocol {
class AppendEntriesResponse;
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
    CreateServerInternalMessengerParam(INodeInternalRpcHandler *handler, uint16_t cliRpcWorkThreadsCnt, const ccsys::cctime &cliWaitRespTimeout,
                                    uint16_t srvRpcWorkThreadsCnt, uint16_t mngerWorkThreadsCnt, uint16_t inetIOThreadsCnt,
                                    uint16_t iport, ccsys::MemPool *pMp, int32_t connTimeout) :
        nodeInternalRpcHandler(handler), clientRpcWorkThreadsCnt(cliRpcWorkThreadsCnt), clientWaitResponseTimeout(cliWaitRespTimeout),
        serverRpcWorkThreadsCnt(srvRpcWorkThreadsCnt), mngerDispatchWorkThreadsCnt(mngerWorkThreadsCnt), netIOThreadsCnt(inetIOThreadsCnt),
        port(iport), memPool(pMp), connectTimeout(connTimeout) {}

    INodeInternalRpcHandler   *nodeInternalRpcHandler;
    uint16_t                   clientRpcWorkThreadsCnt;
    ccsys::cctime              clientWaitResponseTimeout;
    uint16_t                   serverRpcWorkThreadsCnt;
    uint16_t                   mngerDispatchWorkThreadsCnt;
    uint16_t                   netIOThreadsCnt;
    uint16_t                   port;
    ccsys::MemPool            *memPool;
    int32_t                    connectTimeout;
};

// TODO(sunchao): 把messenger抽象一下，将AbstractRpcMessenger放到rpc模块，把dispatcher部分抽象出来。否则现在的缺点是rpc message type协议头的处理在应用层了。
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

    std::shared_ptr<protocol::AppendEntriesResponse> AppendEntriesSync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::ARpcClient::SentRet AppendEntriesAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::SP_PB_MSG OnAppendEntries(rpc::SP_PB_MSG sspMsg) override;

    std::shared_ptr<protocol::RequestVoteResponse> RequestVoteSync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::ARpcClient::SentRet RequestVoteAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::SP_PB_MSG OnRequestVote(rpc::SP_PB_MSG sspMsg) override;

    void OnRecvRpcReturnResult(std::shared_ptr<net::NotifyMessage> sspNM) override;

private:
    void add_msg_to_mapper(net::Message::Id id, rpc::IMessageHandler* handler, net::net_peer_info_t &peer);
    rpc::IMessageHandler* remove_msg_from_msg_handler_map(net::Message::Id id);
    rpc::IMessageHandler* remove_msg_from_map(net::Message::Id id, net::net_peer_info_t &peer);
    void clear_peer_msg_map(net::net_peer_info_t &peer);
    void dispatch_msg(std::shared_ptr<net::NotifyMessage> sspNM);

private:
    uint16_t                                                                             m_iIOThreadsCnt  = 0;
    uint16_t                                                                             m_iPort          = 0;
    bool                                                                                 m_bStopped       = true;
    /**
     * 关联关系，无需本类释放。
     */
    INodeInternalRpcHandler                                                             *m_pNodeINRpcHandler  = nullptr;
    net::ISocketService                                                                 *m_pSocketService     = nullptr;
    RfSrvInternalRpcClientSync                                                          *m_pSyncClient        = nullptr;
    RfSrvInternalRpcClientAsync                                                         *m_pAsyncClient       = nullptr;
    RfSrvInternalRpcServerSync                                                          *m_pServer            = nullptr;
    bool                                                                                 m_bOwnMemPool        = false;
    ccsys::MemPool                                                                      *m_pMemPool           = nullptr;
    uint16_t                                                                             m_iDispatchTpCnt     = 0;
    ccsys::ThreadPool<std::shared_ptr<net::NotifyMessage>>                              *m_pDispatchTp        = nullptr;
    std::unordered_map<net::Message::Id, rpc::IMessageHandler*>                          m_msgHandlerMapper;
    std::unordered_map<net::net_peer_info_t, std::unordered_set<net::Message::Id>>       m_addrMsgMapper;
    ccsys::spin_lock_t                                                                   m_slMsgMapper        = UNLOCKED;
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_NODE_INTERNAL_MESSENGER_H
