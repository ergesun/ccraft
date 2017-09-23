/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../common/common-def.h"
#include "../../common/global-vars.h"
#include "../../common/timer.h"
#include "../../common/codec-utils.h"
#include "../../common/buffer.h"

#include "../request.h"
#include "../../net/socket-service-factory.h"
#include "../../net/net-protocal-stacks/msg-worker-managers/unique-worker-manager.h"
#include "../../net/rcv-message.h"

#include "abstract-rpc-client.h"
#include "../exceptions.h"
#include "../common-def.h"

namespace ccraft {
namespace rpc {
ARpcClient::ARpcClient(net::ISocketService *ss, common::MemPool *memPool) :
    m_pSocketService(ss), m_pMemPool(memPool) {
    CHECK(ss);

    if (!memPool) {
        m_bOwnPool = true;
        m_pMemPool = new common::MemPool();
    }
}

ARpcClient::~ARpcClient() {
    Stop();
    if (m_bOwnPool) {
        DELETE_PTR(m_pMemPool);
    }
}

bool ARpcClient::Start() {
    if (!m_bStopped || !m_bRegistered) {
        return false;
    }

    m_bStopped = false;
    hw_rw_memory_barrier();

    return onStart();
}

bool ARpcClient::Stop() {
    if (m_bStopped) {
        return true;
    }

    m_bStopped = true;
    return onStop();
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
    hw_rw_memory_barrier();
}

ARpcClient::RpcCtx* ARpcClient::sendMessage(std::string &&rpcName, SP_PB_MSG msg, net::net_peer_info_t &&peer) {
    if (m_hmapRpcs.end() == m_hmapRpcs.find(rpcName)) {
        throw BadRpcException((uint16_t)RpcCode::ErrorNoRegisteredRpc, std::move(rpcName));
    }

    net::net_peer_info_t rcPeer = peer;
    auto handlerId = m_hmapRpcs[rpcName];
    auto rr = new RpcRequest(m_pMemPool, std::move(peer), handlerId, std::move(msg));
    auto id = rr->GetId();
    if (UNLIKELY(!m_pSocketService->SendMessage(rr))) {
        DELETE_PTR(rr);
        return nullptr;
    }

    auto rc = m_rpcCtxPool.Get();
    rc->peer = std::move(rcPeer);
    rc->msgId = id;
    rc->handlerId = handlerId;

    return rc;
}
} // namespace rpc
} // namespace ccraft
