/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SRV_NODE_RPC_EXCEPTIONS_H
#define CCRAFT_SRV_NODE_RPC_EXCEPTIONS_H

#include <exception>
#include <cstdint>
#include <string>
#include <sstream>

#include "../../../rpc/exceptions.h"

namespace ccraft {
namespace server {
class RpcClientIsBusyException : public rpc::RpcException {
public:
    RpcClientIsBusyException() {
        m_sWhat = "Cannot push message into sent msg queue, the sent msg queue is full!";
    }

    const char *what() const noexcept override {
        return m_sWhat.c_str();
    }
};
}
}

#endif //CCRAFT_SRV_NODE_RPC_EXCEPTIONS_H
