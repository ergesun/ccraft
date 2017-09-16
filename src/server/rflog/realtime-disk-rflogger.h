/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_RFLOG_RFLOGGER_H
#define CCRAFT_SERVER_RFLOG_RFLOGGER_H

#include <string>
#include <vector>

#include "irflogger.h"

//         RFLOG_MAGIC_NO        r f l g
#define    RFLOG_MAGIC_NO      0x72666c67
#define    RFLOG_MAGIC_NO_LEN  4

namespace ccraft {
    namespace rflog {
        /**
         * Not thread-safe!
         * log format on disk file(file header + log segments)：
         *                             |<-log file header(magic no)->|
         *                             |<-log content len(u32)->|<-log content->|<-log segment start pos(u32)->|
         *                              ...(log segments)...
         *                             |<-log content len(u32)->|<-log content->|<-log segment start pos(u32)->|
         */
        class RtDiskRfLogger : public IRfLogger {
        public:
            explicit RtDiskRfLogger(std::string &&logFilePath, bool autoSync = false);
            ~RtDiskRfLogger() override;

            RfLogEntry *GetLastEntry() override;
            RfLogEntry *GetEntry(uint32_t index) override;
            void AppendEntries(RepeatedPtrField<RfLogEntry> *entries) override;
            void AppendEntry(RfLogEntry *entry) override;
            void Sync() override;

        private:
            void initialize();

        private:
            std::string                           m_sLogFilePath;
            bool                                  m_bAutoSync = false;
            int                                   m_iFd = -1;
            std::vector<RfLogEntry*>              m_vEntries;
            uint32_t                              m_iCurIdxToSync = 0;
            uint32_t                              m_iFileSize = 0;
        };
    } // namespace rflog
} // namespace ccraft

#endif //CCRAFT_SERVER_RFLOG_RFLOGGER_H
