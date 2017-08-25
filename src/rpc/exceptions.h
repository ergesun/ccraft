/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_EXCEPTIONS_H
#define CCRAFT_EXCEPTIONS_H

#include <exception>
#include <cstdint>
#include <string>
#include <sstream>

namespace ccraft {
namespace rpc {
class BadRpcHandlerIdException : public std::exception {
public:
    explicit BadRpcHandlerIdException(uint16_t code, uint16_t handlerId) : m_iCode(code), m_iHandlerId(handlerId) {
        std::stringstream ss;
        ss << "Bad handler id " << m_iHandlerId << ".";
        m_sWhat = ss.str();
    }

    const char *what() const override {
        return m_sWhat.c_str();
    }

    uint16_t code() const {
        return m_iCode;
    }

    uint16_t handler_id() const {
        return m_iHandlerId;
    }

private:
    uint16_t    m_iCode;
    uint16_t    m_iHandlerId;
    std::string m_sWhat;
};

class BadRpcException : public std::exception {
public:
    explicit BadRpcException(uint16_t code, std::string &&rpcName) : m_iCode(code), m_sRpcName(std::move(rpcName)) {
        std::stringstream ss;
        ss << "Cannot found registered rpc " << m_sRpcName << ".";
        m_sWhat = ss.str();
    }

    const char *what() const override {
        return m_sWhat.c_str();
    }

    uint16_t code() const {
        return m_iCode;
    }

private:
    uint16_t    m_iCode;
    std::string m_sRpcName;
    std::string m_sWhat;
};
}
}

#endif //CCRAFT_EXCEPTIONS_H
