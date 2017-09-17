/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SRV_NODE_CLIENT_RPC_SERVICE_H
#define CCRAFT_SRV_NODE_CLIENT_RPC_SERVICE_H

#include "iservice.h"

namespace ccraft {
namespace server {
class ClientRpcService : public IService {
public:
    ClientRpcService() = default;
    ~ClientRpcService() override = default;

    bool Start() override;
    bool Stop() override;
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_SRV_NODE_CLIENT_RPC_SERVICE_H
