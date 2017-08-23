/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_PROTOBUF_UTILS_H
#define CCRAFT_PROTOBUF_UTILS_H

#include <google/protobuf/message.h>

namespace ccraft {
namespace common {
class MemPool;
class Buffer;
}

namespace rpc {
class ProtoBufUtils {
public:
    static bool Parse(const common::Buffer *from, google::protobuf::Message *to);
    static void Serialize(const google::protobuf::Message *from, common::Buffer *to, common::MemPool *mp);
    static void Serialize(const google::protobuf::Message *from, common::Buffer *to);
};
}
}

#endif //CCRAFT_PROTOBUF_UTILS_H
