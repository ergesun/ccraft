/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../common/buffer.h"

#include "protobuf-utils.h"

namespace ccraft {
namespace rpc {
bool ProtoBufUtils::Deserialize(const common::Buffer *from, google::protobuf::Message *to) {
    if (UNLIKELY(!to->ParseFromArray(from->GetPos(), from->AvailableLength()))) {
        LOGWFUN << "Missing fields in protocol buffer of " << to->GetTypeName().c_str() << ": " <<
                to->InitializationErrorString().c_str();
        return false;
    }

    return true;
}

void ProtoBufUtils::Serialize(const google::protobuf::Message *from, common::Buffer *to, common::MemPool *mp) {
    // SerializeToArray seems to always return true, so we explicitly check
    // IsInitialized to make sure all required fields are set.
    if (UNLIKELY(!from->IsInitialized())) {
        LOGFFUN << "Missing fields in protocol buffer of type %s: %s (have %s)" << from->GetTypeName().c_str()
                << ": " << from->InitializationErrorString().c_str();
    }

    uint32_t len = static_cast<uint32_t>(from->ByteSize());

    auto mo = mp->Get(len);
    auto start = reinterpret_cast<uchar*>(mo->Pointer());
    from->SerializeToArray(start, len);
    to->Refresh(start, start + len - 1, start, start + mo->Size() - 1, mo);
}

void ProtoBufUtils::Serialize(const google::protobuf::Message *from, common::Buffer *to) {
    // SerializeToArray seems to always return true, so we explicitly check
    // IsInitialized to make sure all required fields are set.
    if (UNLIKELY(!from->IsInitialized())) {
        LOGFFUN << "Missing fields in protocol buffer of type %s: %s (have %s)" << from->GetTypeName().c_str()
                << ": " << from->InitializationErrorString().c_str();
    }

    from->SerializeToArray(to->GetPos(), from->ByteSize());
}
}
}
