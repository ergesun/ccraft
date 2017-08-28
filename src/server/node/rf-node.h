/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_RF_NODE_H
#define CCRAFT_SERVER_RF_NODE_H

#include "../../rpc/rpc-server.h"
#include "../../rpc/common-def.h"

namespace ccraft {
    namespace server {
        class RfNode : public IService {
        public:
            RfNode();
            ~RfNode() override;

            bool Start() override;
            bool Stop() override;

        private:
            void register_rpc_handlers();
            rpc::SP_PB_MSG append_rflog(rpc::SP_PB_MSG sspMsg);
            /**
             * for rpc lib to deserialize client request.
             * @return
             */
            rpc::SP_PB_MSG create_append_rflog_request();

        private:
            rpc::RpcServer        *m_pInternalRpcServer = nullptr;
        };
    } // namespace server
} // namespace ccraft

#endif //CCRAFT_SERVER_RF_NODE_H
