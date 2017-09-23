/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_EXCEPTIONS_H
#define CCRAFT_RPC_EXCEPTIONS_H

#include <exception>
#include <cstdint>
#include <string>
#include <sstream>

namespace ccraft {
namespace rpc {
class RpcException : public std::exception {
public:
    RpcException() = default;

protected:
    std::string m_sWhat;
};

class BadRpcException : public RpcException {
public:
    explicit BadRpcException(uint16_t code, std::string &&rpcName) : m_iCode(code), m_sRpcName(std::move(rpcName)) {
        std::stringstream ss;
        ss << "Cannot found registered rpc " << m_sRpcName << ".";
        m_sWhat = ss.str();
    }

    const char *what() const noexcept override {
        return m_sWhat.c_str();
    }

    uint16_t code() const {
        return m_iCode;
    }

private:
    uint16_t m_iCode;
    std::string m_sRpcName;
    std::string m_sWhat;
};
}
}

#endif //CCRAFT_RPC_EXCEPTIONS_H
