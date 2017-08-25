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
typedef uint16_t CodeType;
class RpcResponse : public net::SndMessage {
public:
    RpcResponse(std::shared_ptr<google::protobuf::Message> msg) :
        m_code(RpcCode::OK), m_pMsg(msg) {}

protected:
    uint32_t getDerivePayloadLength() override;
    void encodeDerive(common::Buffer *b) override;

private:
    RpcCode                                         m_code;
    std::shared_ptr<google::protobuf::Message>      m_pMsg = nullptr;
};

class RpcErrorResponse : public net::SndMessage {
public:
    RpcErrorResponse(RpcCode code, std::string &&content) :
        m_code(code), m_sContent(std::move(content)) {}

protected:
    uint32_t getDerivePayloadLength() override;
    void encodeDerive(common::Buffer *b) override;

private:
    RpcCode          m_code;
    std::string      m_sContent;
};

#define BadRpcHandlerIdResponse(id)                                                                 \
        std::stringstream   ss;                                                                     \
        ss << "Rpc handler id " << id << " isn't existent!";                                        \
        auto badRpcHandlerIdResponse = new RpcErrorResponse(RpcCode::ErrorNoHandler, ss.str());

#define RpcServiceInternalErrorResponse(id)                                                                 \
        std::stringstream   ss;                                                                             \
        ss << "Rpc service handler id " << id << " internal error.";                                        \
        auto rpcServiceInternalErrorResponse = new RpcErrorResponse(RpcCode::ErrorInternal, ss.str());

#define BadRequestMsgResponse(id)                                                               \
        std::stringstream   ss;                                                                 \
        ss << "Rpc handler id " << id << " Cannot parse request message!";                      \
        auto badRequestMsgResponse = new RpcErrorResponse(RpcCode::ErrorMsg, ss.str());
} // namespace rpc
} // namespace ccraft

#endif //CCRAFT_RPC_RESPONSE_H
