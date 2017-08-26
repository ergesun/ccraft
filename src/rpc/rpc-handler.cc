/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "rpc-handler.h"

namespace ccraft {
namespace rpc {
std::shared_ptr<google::protobuf::Message>
TypicalRpcHandler::Handle(SP_PB_MSG req) {
    return m_handle(req);
}

SP_PB_MSG TypicalRpcHandler::CreateRequest() {
    return m_requstCreator();
}
} // namespace rpc
} // namespace ccraft
