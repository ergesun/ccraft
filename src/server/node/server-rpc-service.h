/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RF_NODE_H
#define CCRAFT_RF_NODE_H

#include <unordered_map>

#include "iservice.h"
#include "../../common/thread-pool.h"
#include "../../net/common-def.h"
#include "../../net/notify-message.h"
#include "../../rpc/common-def.h"
#include "../../rpc/abstract-rpc-client.h"
#include "iserver-internal-rpc-handler.h"

#include "common-def.h"

namespace ccraft {
namespace protocal {
    class AppendRfLogResponse;
    class RequestVoteResponse;
}

namespace server {
class ServerInternalMessenger;
class ServerRpcService : public IService, public INodeInternalRpcHandler {
public:
    struct RpcTask {
        RpcTask() {}
        RpcTask(rpc::SP_PB_MSG m, ServerInternalMessenger *sim, net::net_peer_info_t &p) :
                msg(m), pSim(sim), peer(p) {}
        RpcTask(rpc::SP_PB_MSG m, ServerInternalMessenger *sim, net::net_peer_info_t &&p) :
                msg(m), pSim(sim), peer(std::move(p)) {}

        rpc::SP_PB_MSG           msg;
        ServerInternalMessenger *pSim = nullptr;
        net::net_peer_info_t     peer;
    };

public:
    ServerRpcService(uint16_t port, uint16_t rpcThreadsCnt);
    ~ServerRpcService() override;

    bool Start() override;
    bool Stop() override;

    std::shared_ptr<protocal::AppendRfLogResponse> AppendRfLogSync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::ARpcClient::SendRet AppendRfLogAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::SP_PB_MSG OnAppendRfLog(rpc::SP_PB_MSG sspMsg) override;

    std::shared_ptr<protocal::RequestVoteResponse> RequestVoteSync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    void RequestVoteAsync(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer);
    rpc::SP_PB_MSG OnRequestVote(rpc::SP_PB_MSG sspMsg) override;

    void OnRecvRpcCallbackMsg(std::shared_ptr<net::NotifyMessage> sspNM);

private:
    ServerInternalMessenger          *m_pNodeInternalMessenger = nullptr;
    common::ThreadPool<RpcTask>      *m_pExecRpcTp             = nullptr;
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_RF_NODE_H
