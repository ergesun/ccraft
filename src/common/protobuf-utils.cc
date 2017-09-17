/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "buffer.h"

#include "protobuf-utils.h"

namespace ccraft {
namespace common {
bool ProtoBufUtils::Deserialize(const Buffer *from, google::protobuf::Message *to) {
    if (UNLIKELY(!to->ParseFromArray(from->GetPos(), from->AvailableLength()))) {
        LOGWFUN << "Missing fields in protocol buffer of " << to->GetTypeName().c_str() << ": " <<
                to->InitializationErrorString().c_str();
        return false;
    }

    return true;
}

void ProtoBufUtils::Serialize(const google::protobuf::Message *from, Buffer *to, MemPool *mp) {
    auto len = static_cast<uint32_t>(from->ByteSize());

    auto mo = mp->Get(len);
    auto start = reinterpret_cast<uchar*>(mo->Pointer());
    from->SerializeToArray(start, len);
    to->Refresh(start, start + len - 1, start, start + mo->Size() - 1, mo);
}

void ProtoBufUtils::Serialize(const google::protobuf::Message *from, Buffer *to) {
    from->SerializeToArray(to->GetPos(), from->ByteSize());
}
}
}
