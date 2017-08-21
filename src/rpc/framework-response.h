//
// Created by sunchao31 on 17-8-15.
//

#ifndef CCRAFT_FRAMEWORK_RESPONSE_H
#define CCRAFT_FRAMEWORK_RESPONSE_H

#include <sstream>

#include "../net/snd-message.h"
#include "../common/codec-utils.h"
#include "../common/buffer.h"

namespace ccraft {
    namespace rpc {
#define CodeType uint16_t
        class RpcResponse : public net::SndMessage {
        public:
            /**
             * 不要超过CodeType_MAX
             */
            enum class Code {
                BadHandlerId =   0
            };

        public:
            RpcResponse(Code code, std::string &&content) :
                m_code(code), m_sContent(std::move(content)) {}

        protected:
            uint32_t GetDerivePayloadLength() override {
                return sizeof(Code) + m_sContent.length();
            }

            void EncodeDerive(common::Buffer *b) override {
                ByteOrderUtils::WriteUInt16(b->Pos, (CodeType)m_code);
                b->Pos += sizeof(CodeType);
                memcpy(b->Pos, m_sContent.c_str(), m_sContent.length());
            }

        private:
            Code          m_code;
            std::string   m_sContent;
        };

#define BadRpcHandlerIdResponse(id)                                 \
        std::stringstream   ss;                                     \
        ss << "Rpc handler id " << id << " isn't existent!";        \
        auto badRpcHandlerIdResponse = RpcResponse(RpcResponse::Code::BadHandlerId, ss.str());
    }
}

#endif //CCRAFT_FRAMEWORK_RESPONSE_H
