/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_RESPONSE_H
#define CCRAFT_RPC_RESPONSE_H

#include <sstream>

#include "../net/snd-message.h"
#include "../common/codec-utils.h"
#include "../common/buffer.h"
#include "common-def.h"

namespace ccraft {
namespace rpc {
typedef uint16_t RpcCodeType;
class RpcResponse : public net::SndMessage {
public:
    RpcResponse(SP_PB_MSG msg) :
        m_code(RpcCode::OK), m_pMsg(msg) {}

protected:
    uint32_t getDerivePayloadLength() override;
    void encodeDerive(common::Buffer *b) override;

private:
    RpcCode        m_code;
    SP_PB_MSG      m_pMsg = nullptr;
};

class RpcErrorResponse : public net::SndMessage {
public:
    RpcErrorResponse(RpcCode code) :
        m_code(code) {}

protected:
    uint32_t getDerivePayloadLength() override;
    void encodeDerive(common::Buffer *b) override;

private:
    RpcCode          m_code;
};
} // namespace rpc
} // namespace ccraft

#endif //CCRAFT_RPC_RESPONSE_H
