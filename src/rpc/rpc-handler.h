/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_HANDLER_H
#define CCRAFT_RPC_HANDLER_H

#include <memory>
#include <cassert>

#include "common-def.h"

namespace google {
namespace protobuf {
class Message;
}
}

namespace ccraft {
namespace common {
class Buffer;
}

namespace net {
class SndMessage;
}

namespace rpc {
class IRpcHandler {
public:
    virtual ~IRpcHandler() = default;
    /**
     * rpc事件的回调。
     * @param b 对端的消息内容buffer。
     * @param sm 本端的响应消息体。
     */
    virtual SP_PB_MSG Handle(SP_PB_MSG req) = 0;
    virtual SP_PB_MSG CreateRequest() = 0;
};

/**
 * 辅助的通用rpc handler。
 */
class TypicalRpcHandler : public IRpcHandler {
public:
    typedef std::function<SP_PB_MSG(SP_PB_MSG)> RpcHandle;
    typedef std::function<SP_PB_MSG(void)> RequestCreator;

public:
    TypicalRpcHandler(RpcHandle handle, RequestCreator requestCreator) :
        m_handle(std::move(handle)), m_requstCreator(std::move(requestCreator)) {
        assert((nullptr != handle) && (nullptr != requestCreator));
    }

    SP_PB_MSG Handle(SP_PB_MSG req) override;
    SP_PB_MSG CreateRequest() override;

private:
    RpcHandle             m_handle;
    RequestCreator        m_requstCreator;
};
}
}

#endif //CCRAFT_RPC_HANDLER_H
