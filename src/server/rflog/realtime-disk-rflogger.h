/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_RFLOG_RFLOGGER_H
#define CCRAFT_SERVER_RFLOG_RFLOGGER_H

#include "irflogger.h"
#include "../../common/segmented-vector.h"

namespace ccraft {
    namespace rflog {
        class RtDiskRfLogger : public IRfLogger {
        public:
            ~RtDiskRfLogger() override;

            RfLogEntry *GetLastEntry() override;
            RfLogEntry *GetEntry(uint32_t index) override;
            bool AppendEntries(RepeatedPtrField<RfLogEntry> *entries) override;

        private:
            common::SegmentedVector<RfLogEntry*>  m_vEntries;
        };
    } // namespace rflog
} // namespace ccraft

#endif //CCRAFT_SERVER_RFLOG_RFLOGGER_H
