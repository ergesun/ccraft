/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../../net/notify-message.h"
#include "../../../net/rcv-message.h"
#include "../../../rpc/protobuf-utils.h"
#include "../../../common/buffer.h"

#include "rf-node-rpc-sync-client.h"

namespace ccraft {
    namespace server {
        bool RfNodeRpcClientSync::Start() {
            register_rpc_handlers();

            return rpc::ARpcClientSync::Start();
        }

        std::shared_ptr<rpc::AppendOpLogResponse>
        RfNodeRpcClientSync::AppendLog(rpc::SP_PB_MSG req, net::net_peer_info_t &&peer) {
            auto tmpPeer = peer;
            auto ctx = sendMessage(RpcAppendLog, req, std::move(peer));
            if (UNLIKELY(!ctx)) {
                LOGFFUN << "send msg to " << tmpPeer.nat.addr << ":" << tmpPeer.nat.port << " failed!";
            }

            auto sspNM = recvMessage(ctx);
            auto *mnm = dynamic_cast<net::MessageNotifyMessage*>(sspNM.get());
            auto rm = mnm->GetContent();
            auto appendLogResp = new rpc::AppendOpLogResponse();
            auto buf = rm->GetDataBuffer();
            buf->MoveHeadBack(sizeof(uint16_t)); // server code
            rpc::ProtoBufUtils::Parse(buf, appendLogResp);

            return std::shared_ptr<rpc::AppendOpLogResponse>(appendLogResp);
        }

        bool RfNodeRpcClientSync::register_rpc_handlers() {
            if (!registerRpc(RpcAppendLog, 1)) {
                return false;
            }

            finishRegisterRpc();

            return true;
        }
    } // namespace server
} // namespace ccraft
