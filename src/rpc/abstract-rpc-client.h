/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_ARPC_CLIENT_H
#define CCRAFT_RPC_ARPC_CLIENT_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>
#include <condition_variable>
#include <set>

#include "../common/blocking-queue.h"
#include "../net/common-def.h"
#include "../server/node/iservice.h"

#include "common-def.h"
#include "imessage-handler.h"

namespace google {
namespace protobuf {
class Message;
}
}

namespace ccraft {
namespace ccsys {
class MemPool;
}
namespace net {
class ISocketService;
class NotifyMessage;
}
namespace rpc {
class RpcRequest;
/**
 * 返回值SentRet的msgId为INVALID_MSG_ID(0)表示发送失败，原因为发送队列满了。
 */
#define DefineStandardAsyncRpcWithNoMsgId(RpcName)                                                                             \
    rpc::ARpcClient::SentRet RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer)

#define ImplStandardAsyncRpcWithNoMsgId(ClassName, RpcName)                                                                    \
    rpc::ARpcClient::SentRet ClassName::RpcName(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {                             \
        return rpc::ARpcClient::SendMessageAsync(#RpcName, std::move(req), std::move(peer));                                   \
    }

/**
 * 返回值SentRet的msgId为INVALID_MSG_ID(0)表示发送失败，原因为发送队列满了。
 */
#define DefineStandardAsyncRpcWithMsgId(RpcName)                                                                               \
    rpc::ARpcClient::SentRet RpcName(net::Message::Id id, rpc::SP_PB_MSG req, net::net_peer_info_t &&peer)

#define ImplStandardAsyncRpcWithMsgId(ClassName, RpcName)                                                                      \
    rpc::ARpcClient::SentRet ClassName::RpcName(net::Message::Id id, rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {        \
        return rpc::ARpcClient::SendMessageAsync(#RpcName, id, std::move(req), std::move(peer));                               \
    }

/**
 * [Message format]
 *    client -> server :  |net common header(Message::HeaderSize() bytes)|[handler id(2bytes)|protobuf msg(n bytes)]|
 *    server -> client :  |net common header(Message::HeaderSize() bytes)|[rpc code(2bytes)|[handler id(2bytes)|protobuf msg(n bytes or 0 if no return value)]|
 */
/**
* Rpc client base class.
*/
class ARpcClient : public IService, public IMessageHandler {
public:
    struct SentRet {
        SentRet() = default;
        SentRet(SentRet &&sr) noexcept {
            peer = std::move(sr.peer);
            handlerId = sr.handlerId;
            msgId = sr.msgId;
        }

        SentRet(net::net_peer_info_t &&p, uint16_t h, uint64_t m) : peer(std::move(p)), handlerId(h), msgId(m) {}

        net::net_peer_info_t                 peer;
        uint16_t                             handlerId = 0;
        uint64_t                             msgId     = 0;
    };

public:
    /**
     *
     * @param workThreadsCnt
     * @param timeout
     * @param memPool 如果为空，则内部自己构造。
     */
    ARpcClient(net::ISocketService *ss, ccsys::MemPool *memPool);
    ~ARpcClient() override;

    bool Start() override;
    bool Stop() override;

    /**
     * 自动生成消息id
     * @param rpcName
     * @param msg
     * @return SentRet::msgId如果是INVALID_MSG_ID(0)则失败，否则成功。
     */
    SentRet SendMessageAsync(std::string &&rpcName, SP_PB_MSG msg, net::net_peer_info_t &&peer);
    /**
     * 指定消息id
     * @param rpcName
     * @param msg
     * @return SentRet::msgId如果是INVALID_MSG_ID(0)则失败，否则成功。
     */
    SentRet SendMessageAsync(std::string &&rpcName, net::Message::Id id, SP_PB_MSG msg, net::net_peer_info_t &&peer);
    void HandleMessage(std::shared_ptr<net::NotifyMessage> sspNM) override final;

protected:
    bool registerRpc(std::string &&rpcName, uint16_t id);
    void finishRegisterRpc();

    virtual bool onStart() = 0;
    virtual bool onStop() = 0;
    virtual void onRecvMessage(std::shared_ptr<net::NotifyMessage> sspNM) = 0;

protected:
    ccsys::MemPool                                                *m_pMemPool;

private:
    bool                                                           m_bStopped          = true;
    // 关联关系，无需本类释放。
    net::ISocketService                                           *m_pSocketService    = nullptr;
    bool                                                           m_bRegistered       = false;
    std::unordered_map<std::string, uint16_t>                      m_hmapRpcs;
    bool                                                           m_bOwnPool = false;
}; // class ARpcClient
} // namespace rpc
} // namespace ccraft

#endif //CCRAFT_RPC_ARPC_CLIENT_H
