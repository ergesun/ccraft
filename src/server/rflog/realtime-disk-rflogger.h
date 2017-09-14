/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_RFLOG_RFLOGGER_H
#define CCRAFT_SERVER_RFLOG_RFLOGGER_H

#include <string>

#include "irflogger.h"
#include "../../common/segmented-vector.h"

//         RFLOG_MAGIC_NO        r f l g
#define    RFLOG_MAGIC_NO      0x72666c67
#define    RFLOG_MAGIC_NO_LEN  4

namespace ccraft {
    namespace rflog {
        /**
         * log format on disk file(file header + log segments)：
         *                             |<-log file header(magic no)->|
         *                             |<-log content len(u32)->|<-log content->|<-log segment start pos(u32)->|
         *                              ...(log segments)...
         *                             |<-log content len(u32)->|<-log content->|<-log segment start pos(u32)->|
         */
        class RtDiskRfLogger : public IRfLogger {
        public:
            explicit RtDiskRfLogger(std::string &&logFilePath);
            ~RtDiskRfLogger() override;

            RfLogEntry *GetLastEntry() override;
            RfLogEntry *GetEntry(uint32_t index) override;
            bool AppendEntries(RepeatedPtrField<RfLogEntry> *entries) override;
            bool AppendEntry(RfLogEntry *entry) override;

        private:
            void initialize();

        private:
            std::string                           m_sLogFilePath;
            int                                   m_iFd = -1;
            common::SegmentedVector<RfLogEntry*>  m_vEntries;

        };
    } // namespace rflog
} // namespace ccraft

#endif //CCRAFT_SERVER_RFLOG_RFLOGGER_H
