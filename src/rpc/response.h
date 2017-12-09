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
class RpcResponseBase : public net::SndMessage {
public:
    RpcResponseBase(RpcCode code, HandlerType ht) :
            m_code(code), m_ht(ht) {}

protected:
    uint32_t getDerivePayloadLength() override;
    void encodeDerive(common::Buffer *b) override;

protected:
    RpcCode          m_code;
    HandlerType      m_ht;
};

class RpcResponse : public RpcResponseBase {
public:
    RpcResponse(SP_PB_MSG msg, HandlerType ht) :
            RpcResponseBase(RpcCode::OK, ht), m_pMsg(msg) {}

protected:
    uint32_t getDerivePayloadLength() override;
    void encodeDerive(common::Buffer *b) override;

private:
    SP_PB_MSG      m_pMsg = nullptr;
};

typedef RpcResponseBase RpcErrorResponse;
} // namespace rpc
} // namespace ccraft

#endif //CCRAFT_RPC_RESPONSE_H
