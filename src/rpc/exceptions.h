/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_EXCEPTIONS_H
#define CCRAFT_EXCEPTIONS_H

#include <exception>
#include <cstdint>
#include <string>

namespace ccraft {
    namespace rpc {
        class BadRpcHandlerIdException : public std::exception {
        public:
            BadRpcHandlerIdException(uint16_t code, std::string what) : m_iCode(code), m_sWhat(std::move(what)) {}
            BadRpcHandlerIdException(BadRpcHandlerIdException &r) {
                this->m_iCode = r.m_iCode;
                this->m_sWhat = r.m_sWhat;
            }

            BadRpcHandlerIdException(BadRpcHandlerIdException &&r) noexcept {
                this->m_iCode = r.m_iCode;
                this->m_sWhat = std::move(r.m_sWhat);
            }

            const char *what() const override {
                return m_sWhat.c_str();
            }

            uint16_t code() const {
                return m_iCode;
            }

        private:
            uint16_t    m_iCode;
            std::string m_sWhat;
        };
    }
}

#endif //CCRAFT_EXCEPTIONS_H
