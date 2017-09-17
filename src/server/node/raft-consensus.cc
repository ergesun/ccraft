/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../common/common-def.h"
#include "../../common/server-gflags-config.h"
#include "../rflog/realtime-disk-rflogger.h"

#include "raft-consensus.h"
#include "../../common/file-utils.h"

namespace ccraft {
    namespace server {
        RaftConsensus::RaftConsensus() {
            std::stringstream ss;
            ss << FLAGS_data_dir.c_str() << '/' << RFLOG_DIR;
            auto dataDirPath = ss.str();
            assert(-1 != common::FileUtils::CreateDirPath(dataDirPath.c_str(), 0755));
            ss << '/' << RFLOG_FILE_NAME;
            if ("realtime-disk" == FLAGS_rflogger_type) {
                m_pRfLogger = new rflog::RtDiskRfLogger(ss.str(), true);
            } else {
                LOGFFUN << "We don't have a rf logger named " << FLAGS_rflogger_type.c_str();
            }
        }

        RaftConsensus::~RaftConsensus() {

        }
    } // namespace server
} // namespace ccraft