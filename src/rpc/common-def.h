/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_DEF_H
#define CCRAFT_COMMON_DEF_H

#include "../net/snd-message.h"

namespace ccraft {
namespace rpc {
enum class RpcCode {
    OK              = 0,
    ErrorNoHandler  = 1,
    ErrorMsg        = 2,
    ErrorInternal   = 3,
    ErrorUnknown    = 4
};
}
}

#endif //CCRAFT_COMMON_DEF_H
