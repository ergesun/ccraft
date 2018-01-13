/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_RF_NODE_H
#define CCRAFT_SERVER_RF_NODE_H

#include "../../../rpc/rpc-server.h"
#include "../../../rpc/common-def.h"

namespace ccraft {
namespace server {
class INodeInternalRpcHandler;
class RfSrvInternalRpcServerSync : public IService, public rpc::IMessageHandler {
public:
    RfSrvInternalRpcServerSync(INodeInternalRpcHandler *handler, uint16_t workThreadsCnt,
                                net::ISocketService *ss, ccsys::MemPool *memPool = nullptr);
    ~RfSrvInternalRpcServerSync() override;

    bool Start() override;
    bool Stop() override;

    void HandleMessage(std::shared_ptr<net::NotifyMessage> sspNM) override;

private:
    void register_rpc_handlers();
    rpc::SP_PB_MSG on_append_entries(rpc::SP_PB_MSG sspMsg);
    /**
     * for rpc lib to deserialize client request.
     * @return
     */
    rpc::SP_PB_MSG create_append_entries_request();
    rpc::SP_PB_MSG on_request_vote(rpc::SP_PB_MSG sspMsg);
    /**
     * for rpc lib to deserialize client request.
     * @return
     */
    rpc::SP_PB_MSG create_request_vote_request();

private:
    rpc::RpcServer           *m_pRpcServer = nullptr;
    INodeInternalRpcHandler  *m_pHandler   = nullptr;
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_SERVER_RF_NODE_H
