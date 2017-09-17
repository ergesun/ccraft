/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <memory>

#include "../../common/common-def.h"
#include "../../common/server-gflags-config.h"
#include "../../common/timer.h"
#include "../../common/file-utils.h"
#include "../../common/codec-utils.h"
#include "../../common/io-utils.h"
#include "../../common/buffer.h"
#include "../../codegen/raft-state.pb.h"
#include "../../common/protobuf-utils.h"

#include "../rflog/realtime-disk-rflogger.h"
#include "elector-manager.h"

#include "raft-consensus.h"

namespace ccraft {
namespace server {
RaftConsensus::RaftConsensus() {
    initialize();
}

RaftConsensus::~RaftConsensus() {
    if (-1 != m_iRfStateFd) {
        close(m_iRfStateFd);
    }

    DELETE_PTR(m_pTimer);
    DELETE_PTR(m_pRfLogger);
    DELETE_PTR(m_pElectorManager);
}

bool RaftConsensus::Start() {
    if (!m_bStopped) {
        return true;
    }

    m_bStopped = false;
    m_pTimer->Start();

    return true;
}

bool RaftConsensus::Stop() {
    if (m_bStopped) {
        return true;
    }

    m_bStopped = true;
    m_pTimer->Stop();

    return true;
}

void RaftConsensus::initialize() {
    m_pTimer = new common::Timer();
    m_pElectorManager = new ElectorManager();
    if (!m_pElectorManager->Initialize((uint32_t)FLAGS_server_id, FLAGS_servers_conf_path.c_str())) {
        LOGFFUN << "ElectorManager initialize failed!";
    }

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
    m_sRfstateFilePath = ss2.str();
    if (!common::FileUtils::Exist(m_sRfstateFilePath)) {
        LOGDFUN2("create rfstate file ", m_sLogFilePath.c_str());
        if (-1 == (m_iRfStateFd = common::FileUtils::Open(m_sRfstateFilePath, O_WRONLY, O_CREAT, 0644))) {
            auto err = errno;
            LOGFFUN << "create rfstate file " << m_sRfstateFilePath.c_str() << " failed with errmsg " << strerror(err);
        }

        // write file header.
        uchar header[RFSTATE_MAGIC_NO_LEN];
        ByteOrderUtils::WriteUInt32(header, RFSTATE_MAGIC_NO);
        WriteFileFullyWithFatalLOG(m_iRfStateFd, (char*)header, RFSTATE_MAGIC_NO_LEN, m_sRfstateFilePath.c_str());
    } else {
        if (-1 == (m_iRfStateFd = common::FileUtils::Open(m_sRfstateFilePath, O_RDWR, 0, 0))) {
            auto err = errno;
            LOGFFUN << "open rfstate file " << m_sRfstateFilePath.c_str() << " failed with errmsg " << strerror(err);
        }

        auto fileSize = common::FileUtils::GetFileSize(m_iRfStateFd);
        if (-1 == fileSize) {
            auto err = errno;
            LOGFFUN << "get file size for " << m_sRfstateFilePath.c_str() << " failed with errmsg " << strerror(err);
        }
        if (0 == (uint32_t)fileSize) {
            LOGDFUN3("rfstate file ", m_sLogFilePath.c_str(), " is empty!");
            return;
        }

        if (fileSize < RFSTATE_MAGIC_NO_LEN) {
            LOGFFUN << "rfstate file " << m_sRfstateFilePath.c_str() << " header is corrupt!";
        }

        char *buf = nullptr;
        ReadFileFullyWithFatalLOG(m_iRfStateFd, &buf, (size_t)fileSize, m_sRfstateFilePath.c_str());
        std::shared_ptr<char> sspBuf(buf, common::ArrDeleter<char>());

        uint32_t magicNo = ByteOrderUtils::ReadUInt32((uchar*)buf);
        if (RFSTATE_MAGIC_NO != magicNo) {
            LOGFFUN << "rfstate " << m_sRfstateFilePath.c_str() << " header is corrupt!";
        }

        auto startPtr = (uchar*)(buf + RFSTATE_MAGIC_NO_LEN);
        auto endPtr = (uchar*)(buf + fileSize - 1);
        common::Buffer b;

        b.Refresh(startPtr, endPtr, startPtr, endPtr, nullptr);
        auto rs = new protocal::RaftState();
        common::ProtoBufUtils::Deserialize(&b, rs);
        m_iCurrentTerm = rs->currentterm();
        m_iVoteFor = rs->votedfor();

        DELETE_PTR(rs);
    }
}

void RaftConsensus::save_rf_state() {
    auto rfsfp = m_sRfstateFilePath.c_str();
    LSeekFileWithFatalLOG(m_iRfStateFd, 0, SEEK_SET, rfsfp);
    if (-1 == ftruncate(m_iRfStateFd, 0)) {
        auto err = errno;
        LOGFFUN << "ftruncate file " << rfsfp << " failed with errmsg " << strerror(err) << ".";
    }

    // write file header.
    uchar header[RFSTATE_MAGIC_NO_LEN];
    ByteOrderUtils::WriteUInt32(header, RFSTATE_MAGIC_NO);
    WriteFileFullyWithFatalLOG(m_iRfStateFd, (char*)header, RFSTATE_MAGIC_NO_LEN, rfsfp);

    // write state.
    // mpo will be Putted in Buffer 'b'.
    auto rs = new protocal::RaftState();

    rs->set_currentterm(m_iCurrentTerm);
    rs->set_votedfor(m_iVoteFor);

    common::Buffer b;
    common::ProtoBufUtils::Serialize(rs, &b, common::g_pMemPool);
    WriteFileFullyWithFatalLOG(m_iRfStateFd, (char*)(b.GetStart()), size_t(b.AvailableLength()), m_sRfstateFilePath.c_str());
    FDataSyncFileWithFatalLOG(m_iRfStateFd, rfsfp);
}

void RaftConsensus::on_leader_hb_timeout() {

}
} // namespace server
} // namespace ccraft