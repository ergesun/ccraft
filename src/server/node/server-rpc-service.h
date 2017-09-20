/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RF_NODE_H
#define CCRAFT_RF_NODE_H

#include <unordered_map>

#include "iservice.h"
#include "../../net/common-def.h"
#include "../../rpc/common-def.h"
#include "iserver-internal-rpc-handler.h"

#include "common-def.h"

namespace ccraft {
namespace server {
class ServerInternalMessenger;
class ServerRpcService : public IService, public INodeInternalRpcHandler {
public:
    explicit ServerRpcService(uint16_t port);
    ~ServerRpcService() override;

    bool Start() override;
    bool Stop() override;

    rpc::SP_PB_MSG OnAppendRfLog(rpc::SP_PB_MSG sspMsg) override;
    rpc::SP_PB_MSG OnRequestVote(rpc::SP_PB_MSG sspMsg) override;

private:
    ServerInternalMessenger          *m_pNodeInternalMessenger = nullptr;
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_RF_NODE_H
