//
// Created by sunchao31 on 17-9-2.
//

#ifndef CCRAFT_INODE_INTERNAL_COMMUNICATOR_H
#define CCRAFT_INODE_INTERNAL_COMMUNICATOR_H

#include "../../rpc/common-def.h"

namespace ccraft {
namespace server {
class INodeInternalRpcHandler {
public:
    virtual ~INodeInternalRpcHandler() = default;
    virtual rpc::SP_PB_MSG OnAppendRfLog(rpc::SP_PB_MSG sspMsg) = 0;
}; // class IMessageHandler
} // namespace rpc
} // namespace ccraft

#endif //CCRAFT_INODE_INTERNAL_COMMUNICATOR_H
