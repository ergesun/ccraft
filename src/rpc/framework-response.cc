/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <google/protobuf/message.h>

#include "framework-response.h"
#include "protobuf-utils.h"

namespace ccraft {
namespace rpc {
uint32_t RpcResponse::getDerivePayloadLength() {
    if (m_pMsg.get()) {
        return sizeof(CodeType) + sizeof(uint16_t) + m_pMsg->ByteSize();
    } else {
        return sizeof(CodeType) + sizeof(uint16_t);
    }
}

void RpcResponse::encodeDerive(common::Buffer *b) {
    ByteOrderUtils::WriteUInt16(b->GetPos(), (CodeType)m_code);
    b->MoveHeadBack(sizeof(CodeType));
    ByteOrderUtils::WriteUInt16(b->GetPos(), m_iHandlerId);
    b->MoveHeadBack(sizeof(uint16_t));
    if (m_pMsg.get()) {
        ProtoBufUtils::Serialize(m_pMsg.get(), b);
    }
}

uint32_t RpcErrorResponse::getDerivePayloadLength() {
    return sizeof(CodeType) + sizeof(uint16_t) + m_sContent.length();
}

void RpcErrorResponse::encodeDerive(common::Buffer *b) {
    ByteOrderUtils::WriteUInt16(b->GetPos(), (CodeType)m_code);
    b->MoveHeadBack(sizeof(CodeType));
    ByteOrderUtils::WriteUInt16(b->GetPos(), m_iHandlerId);
    b->MoveHeadBack(sizeof(uint16_t));
    auto contentLen = m_sContent.length();
    if (contentLen > 0) {
        memcpy(b->GetPos(), m_sContent.c_str(), contentLen);
    }
}
} // namespace rpc
} // namespace ccraft
