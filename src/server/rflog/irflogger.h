/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_RFLOG_IRFLOGGER_H
#define CCRAFT_SERVER_RFLOG_IRFLOGGER_H

#include "../../codegen/node-raft.pb.h"

using namespace google::protobuf;

using ccraft::protocal::serverraft::RfLogEntry;

namespace ccraft {
    namespace rflog {
        class IRfLogger {
        public:
            virtual ~IRfLogger() = default;
            virtual RfLogEntry* GetLastEntry() = 0;
            virtual RfLogEntry* GetEntry(uint32_t index) = 0;
            virtual bool AppendEntries(RepeatedPtrField<RfLogEntry>* entries) = 0;
            virtual bool AppendEntry(RfLogEntry* entry) = 0;
        };
    } // namespace rflog
} // namespace ccraft

#endif //CCRAFT_SERVER_RFLOG_IRFLOGGER_H
