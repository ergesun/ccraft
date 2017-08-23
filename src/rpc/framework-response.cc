/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <google/protobuf/message.h>

#include "framework-response.h"
#include "protobuf-utils.h"

namespace ccraft {
namespace rpc {
uint32_t RpcResponse::GetDerivePayloadLength() {
    if (m_pMsg) {
        return sizeof(CodeType) + sizeof(uint16_t) + m_pMsg->ByteSize();
    } else {
        return sizeof(CodeType) + sizeof(uint16_t);
    }
}

void RpcResponse::EncodeDerive(common::Buffer *b) {
    ByteOrderUtils::WriteUInt16(b->Pos, (CodeType)m_code);
    b->Pos += sizeof(CodeType);
    ByteOrderUtils::WriteUInt16(b->Pos, m_iHandlerId);
    b->Pos += sizeof(uint16_t);
    ProtoBufUtils::Serialize(m_pMsg.get(), b);
}

uint32_t RpcErrorResponse::GetDerivePayloadLength() {
    return sizeof(CodeType) + sizeof(uint16_t) + m_sContent.length();
}

void RpcErrorResponse::EncodeDerive(common::Buffer *b) {
    ByteOrderUtils::WriteUInt16(b->Pos, (CodeType)m_code);
    b->Pos += sizeof(CodeType);
    ByteOrderUtils::WriteUInt16(b->Pos, m_iHandlerId);
    b->Pos += sizeof(uint16_t);
    memcpy(b->Pos, m_sContent.c_str(), m_sContent.length());
}
} // namespace rpc
} // namespace ccraft
