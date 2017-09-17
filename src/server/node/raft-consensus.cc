/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <bits/shared_ptr.h>
#include "../../common/common-def.h"
#include "../../common/server-gflags-config.h"
#include "../rflog/realtime-disk-rflogger.h"
#include "../../common/file-utils.h"
#include "../../common/codec-utils.h"

#include "raft-consensus.h"
#include "../../common/io-utils.h"
#include "../../common/buffer.h"
#include "../../codegen/raft-state.pb.h"
#include "../../rpc/protobuf-utils.h"

namespace ccraft {
namespace server {
RaftConsensus::RaftConsensus() {
    initialize();
}

RaftConsensus::~RaftConsensus() {
    if (-1 != m_iRfStateFd) {
        close(m_iRfStateFd);
    }

    if (m_pRfLogger) {
        DELETE_PTR(m_pRfLogger);
    }
}

void RaftConsensus::initialize() {
    // rf logger
    std::stringstream ss1;
    ss1 << FLAGS_data_dir.c_str() << '/' << RFLOG_DIR;
    auto dataDirPath = ss1.str();
    assert(-1 != common::FileUtils::CreateDirPath(dataDirPath.c_str(), 0755));
    ss1 << '/' << RFLOG_FILE_NAME;
    if ("realtime-disk" == FLAGS_rflogger_type) {
        m_pRfLogger = new rflog::RtDiskRfLogger(ss1.str(), true);
    } else {
        LOGFFUN << "We don't have a rf logger named " << FLAGS_rflogger_type.c_str();
    }

    // rf state
    std::stringstream ss2;
    ss2 << dataDirPath.c_str() << '/' << RFSTATE_FILE_NAME;
    auto rfstateFilePath = ss2.str();
    if (!common::FileUtils::Exist(rfstateFilePath)) {
        LOGDFUN2("create rfstate file ", m_sLogFilePath.c_str());
        if (-1 == (m_iRfStateFd = common::FileUtils::Open(rfstateFilePath, O_WRONLY, O_CREAT, 0644))) {
            auto err = errno;
            LOGFFUN << "create rfstate file " << rfstateFilePath.c_str() << " failed with errmsg " << strerror(err);
        }

        // write file header.
        uchar header[RFSTATE_MAGIC_NO_LEN];
        ByteOrderUtils::WriteUInt32(header, RFSTATE_MAGIC_NO);
        WriteFileFullyWithFatalLOG(m_iRfStateFd, (char*)header, RFSTATE_MAGIC_NO_LEN, rfstateFilePath.c_str());
    } else {
        if (-1 == (m_iRfStateFd = common::FileUtils::Open(rfstateFilePath, O_RDWR, 0, 0))) {
            auto err = errno;
            LOGFFUN << "open rfstate file " << rfstateFilePath.c_str() << " failed with errmsg " << strerror(err);
        }

        auto fileSize = common::FileUtils::GetFileSize(m_iRfStateFd);
        if (-1 == fileSize) {
            auto err = errno;
            LOGFFUN << "get file size for " << rfstateFilePath.c_str() << " failed with errmsg " << strerror(err);
        }
        if (0 == (uint32_t)fileSize) {
            LOGDFUN3("rfstate file ", m_sLogFilePath.c_str(), " is empty!");
            return;
        }

        if (fileSize < RFSTATE_MAGIC_NO_LEN) {
            LOGFFUN << "rfstate file " << rfstateFilePath.c_str() << " header is corrupt!";
        }

        char *buf = nullptr;
        ReadFileFullyWithFatalLOG(m_iRfStateFd, &buf, (size_t)fileSize, rfstateFilePath.c_str());
        std::shared_ptr<char> sspBuf(buf, common::ArrDeleter<char>());

        uint32_t magicNo = ByteOrderUtils::ReadUInt32((uchar*)buf);
        if (RFSTATE_MAGIC_NO != magicNo) {
            LOGFFUN << "rfstate " << rfstateFilePath.c_str() << " header is corrupt!";
        }

        auto startPtr = (uchar*)(buf + RFSTATE_MAGIC_NO_LEN);
        auto endPtr = (uchar*)(buf + fileSize - 1);
        common::Buffer b;

        b.Refresh(startPtr, endPtr, startPtr, endPtr, nullptr);
        auto rs = new protocal::RaftState();
        rpc::ProtoBufUtils::Deserialize(&b, rs);
        m_iCurrentTerm = rs->currentterm();
        m_iVoteFor = rs->votedfor();

        DELETE_PTR(rs);
    }
}

void RaftConsensus::save_rf_state() {

}
} // namespace server
} // namespace ccraft