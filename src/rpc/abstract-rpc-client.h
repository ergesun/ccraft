/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_CLIENT_H
#define CCRAFT_RPC_CLIENT_H

/**
 * [Message format]
 *    client -> server :  |net common header(Message::HeaderSize() bytes)|[handler id(2bytes)|protobuf msg(n bytes)]|
 *    server -> client :  |net common header(Message::HeaderSize() bytes)|[rpc 'OK' code(2bytes)|handler id(2bytes)|protobuf msg(n bytes, maybe 0 if no return value or rpc code is not 'OK')]|
 */
namespace ccraft {
    namespace net {
        class ISocketService;
    }
    namespace rpc {
        class ARpcClient {
        public:
            ARpcClient(net::ISocketService *ss);

        protected:
            bool sendMessage();
            bool recvMessage();

        private:
            net::ISocketService                      *m_pSocketService = nullptr;

        };
    }
}

#endif //CCRAFT_RPC_CLIENT_H
