/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_COMMON_DEF_H
#define CCRAFT_RPC_COMMON_DEF_H

#include "../net/snd-message.h"

namespace google {
namespace protobuf {
class Message;
}
}

namespace ccraft {
namespace rpc {
typedef std::shared_ptr<google::protobuf::Message> SP_PB_MSG;

enum class RpcCode {
    // server side
    OK                   = 0,
    ErrorNoHandler       = 1,
    ErrorMsg             = 2,
    ErrorInternal        = 3,

    // client side
    ErrorNoRegisteredRpc = 4,

    // common
    ErrorUnknown         = 5
};
} // namespace rpc
} // namespace ccraft

#endif //CCRAFT_RPC_COMMON_DEF_H
