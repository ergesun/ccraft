/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RF_NODE_RPC_SYNC_CLIENT_H
#define CCRAFT_RF_NODE_RPC_SYNC_CLIENT_H

#include "../../../rpc/abstract-rpc-sync-client.h"
#include "../../../rpc/common-def.h"

namespace ccraft {
namespace protocol {
class AppendEntriesResponse;
class RequestVoteResponse;
}

namespace server {
class RfSrvInternalRpcClientSync : public rpc::ARpcSyncClient {
public:
    RfSrvInternalRpcClientSync(net::ISocketService *ss, const ccsys::cctime &timeout,
                                uint16_t workThreadsCnt, ccsys::MemPool *memPool = nullptr) :
        rpc::ARpcSyncClient(ss, timeout, workThreadsCnt, memPool) {}

    // Define Rpc start
    DefineStandardSyncRpc(AppendEntries);
    DefineStandardSyncRpc(RequestVote);
    // Define Rpc end

protected:
    bool onStart() override;
    bool onStop() override;

private:
    bool register_rpc_handlers();
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_RF_NODE_RPC_SYNC_CLIENT_H
