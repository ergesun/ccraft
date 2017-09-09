/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "realtime-disk-rflogger.h"

namespace ccraft {
    namespace rflog {
        RtDiskRfLogger::~RtDiskRfLogger() {

        }

        RfLogEntry *RtDiskRfLogger::GetLastEntry() {
            return nullptr;
        }

        RfLogEntry *RtDiskRfLogger::GetEntry(uint32_t index) {
            return nullptr;
        }

        bool RtDiskRfLogger::AppendEntries(RepeatedPtrField<RfLogEntry> *entries) {
            return false;
        }
    } // namespace rflog
} // namespace ccraft
