/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../ccsys/timer.h"
#include "../common/common-def.h"
#include "../common/global-vars.h"
#include "../common/codec-utils.h"
#include "../common/buffer.h"
#include "../net/socket-service-factory.h"
#include "../net/net-protocol-stacks/msg-worker-managers/unique-worker-manager.h"
#include "../net/rcv-message.h"

#include "request.h"
#include "exceptions.h"
#include "common-def.h"

#include "abstract-rpc-client.h"

namespace ccraft {
namespace rpc {
ARpcClient::ARpcClient(net::ISocketService *ss, ccsys::MemPool *memPool) :
    m_pMemPool(memPool), m_pSocketService(ss) {
    CHECK(ss);

    if (!memPool) {
        m_bOwnPool = true;
        m_pMemPool = new ccsys::MemPool();
    }
}

ARpcClient::~ARpcClient() {
    Stop();
    if (m_bOwnPool) {
        DELETE_PTR(m_pMemPool);
    }
}

bool ARpcClient::Start() {
    if (!m_bStopped) {
        return true;
    }

    if (!onStart()) {
        return false;
    }

    if (!m_bRegistered) {
        return false;
    }

    m_bStopped = false;
    hw_sw_memory_barrier();

    return true;
}

bool ARpcClient::Stop() {
    if (m_bStopped) {
        return true;
    }

    if (!onStop()) {
        return false;
    }

    m_bStopped = true;
    hw_sw_memory_barrier();

    return true;
}

void ARpcClient::HandleMessage(std::shared_ptr<net::NotifyMessage> sspNM) {
    onRecvMessage(sspNM);
}

bool ARpcClient::registerRpc(std::string &&rpcName, uint16_t id) {
    if (m_bRegistered) {
        return false;
    }

    m_hmapRpcs[std::move(rpcName)] = id;
    return true;
}

void ARpcClient::finishRegisterRpc() {
    m_bRegistered = true;
    hw_sw_memory_barrier();
}

ARpcClient::SentRet ARpcClient::sendMessage(std::string &&rpcName, SP_PB_MSG msg, net::net_peer_info_t &&peer) {
    if (m_hmapRpcs.end() == m_hmapRpcs.find(rpcName)) {
        throw BadRpcException((uint16_t)RpcCode::ErrorNoRegisteredRpc, std::move(rpcName));
    }

    SentRet sr;
    net::net_peer_info_t rcPeer = peer;
    auto handlerId = m_hmapRpcs[rpcName];
    auto rr = new RpcRequest(m_pMemPool, std::move(peer), handlerId, std::move(msg));
    auto id = rr->GetId();
    if (UNLIKELY(!m_pSocketService->SendMessage(rr))) {
        DELETE_PTR(rr);
        return sr;
    }

    sr.peer = std::move(rcPeer);
    sr.msgId = id;
    sr.handlerId = handlerId;

    return sr;
}
} // namespace rpc
} // namespace ccraft
