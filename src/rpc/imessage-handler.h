/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_MESSAGE_HANDLER_H
#define CCRAFT_RPC_MESSAGE_HANDLER_H

#include <memory>

namespace ccraft {
namespace net {
    class NotifyMessage;
}

namespace rpc {
class IMessageHandler {
public:
    virtual ~IMessageHandler() = default;

    virtual void HandleMessage(std::shared_ptr<net::NotifyMessage> sspNM) = 0;
}; // class IMessageHandler
} // namespace rpc
} // namespace ccraft

#endif //CCRAFT_RPC_MESSAGE_HANDLER_H
