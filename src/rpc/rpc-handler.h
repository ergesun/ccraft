/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_HANDLER_H
#define CCRAFT_RPC_HANDLER_H

namespace ccraft {
    namespace common {
        class Buffer;
    }

    namespace net {
        class SndMessage;
    }

    namespace rpc {
        class IRpcHandler {
        public:
            ~IRpcHandler() {}
            /**
             * rpc事件的回调。
             * @param b 对端的消息内容buffer。
             * @param sm 本端的响应消息体。
             */
            virtual void handle(common::Buffer *b, net::SndMessage *sm) = 0;
        };
    }
}

#endif //CCRAFT_RPC_HANDLER_H
