/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_CLIENT_H
#define CCRAFT_RPC_CLIENT_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>
#include <condition_variable>

#include "../../common/cctime.h"

namespace google {
namespace protobuf {
class Message;
}
}

namespace ccraft {
    namespace net {
        class ISocketService;
        class NotifyMessage;
    }
    namespace rpc {
        /**
         * [Message format]
         *    client -> server :  |net common header(Message::HeaderSize() bytes)|[handler id(2bytes)|protobuf msg(n bytes)]|
         *    server -> client :  |net common header(Message::HeaderSize() bytes)|[rpc code(2bytes)|protobuf msg(n bytes or 0 bytes if no return value)]|
         */
        /**
         * Rpc client base class.
         */
        class ARpcClientSync {
        public:
            ARpcClientSync(uint16_t socketServiceThreadsCnt, common::cctime_t timeout);
            bool Start();
            bool Stop();
            bool RegisterRpc(std::string &&rpcName, uint16_t id);

        protected:
            struct RpcCtx {
                std::condition_variable             *cv;
                std::mutex                          *mtx;
                bool                                 ok;
                std::shared_ptr<net::NotifyMessage>  ssp_nm;
            };

        protected:
            bool sendMessage(std::string &&rpcName, std::shared_ptr<google::protobuf::Message> msg);
            std::shared_ptr<net::NotifyMessage> recvMessage();

        private:
            net::ISocketService                             *m_pSocketService = nullptr;
            bool                                             m_bRegistered = false;
            std::unordered_map<std::string, int16_t>         m_hmapRpcs;
            std::unordered_map<uint64_t, RpcCtx>             m_hmapRpcCtxs;
        };
    }
}

#endif //CCRAFT_RPC_CLIENT_H
