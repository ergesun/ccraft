/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <memory>

#include "../../common/common-def.h"
#include "../../common/server-gflags-config.h"
#include "../../fsio/file-utils.h"
#include "../../common/codec-utils.h"
#include "../../fsio/io-utils.h"
#include "../../common/buffer.h"
#include "../../common/protobuf-utils.h"
#include "../../codegen/raft-state.pb.h"
#include "../rflog/realtime-disk-rflogger.h"
#include "elector-manager-service.h"
#include "server-rpc-service.h"
#include "../../codegen/node-raft.pb.h"

#include "raft-consensus.h"
#include "service-manager.h"

namespace ccraft {
namespace server {
RaftConsensus::RaftConsensus() : m_iMyId((uint32_t)FLAGS_server_id),
                                 m_random(ccsys::Random::Range(FLAGS_start_election_rand_latency_low,
                                                                FLAGS_start_election_rand_latency_high)) {
    m_iRequestVoteTimeoutInterval = uint32_t(FLAGS_raft_election_interval * FLAGS_raft_election_timeout_percent);
    m_pTimer = new ccsys::Timer();
    m_moniterLeaderHbTimerCb = std::bind(&RaftConsensus::on_leader_hb_timeout, this, std::placeholders::_1);
    m_monitorLeaderHbTimerEvent = {nullptr, &m_moniterLeaderHbTimerCb};
    m_moniterRVTimeoTimerCb = std::bind(&RaftConsensus::on_request_vote_timeout, this, std::placeholders::_1);
    m_monitorRVTimeoTimerEvent = {nullptr, &m_moniterRVTimeoTimerCb};

    m_pElectorManager = dynamic_cast<ElectorManagerService*>(ServiceManager::GetService(ServiceType::ElectorManager));
    assert(m_pElectorManager);

    auto selfConf = m_pElectorManager->GetSelfServerConf();
    m_pSrvRpcService = new ServerRpcService(selfConf.m_iPortForServer,
                                            (uint16_t)m_pElectorManager->GetOtherServersConf().size(),
                                            this);
    initialize();
}

RaftConsensus::~RaftConsensus() {
    if (-1 != m_iRfStateFd) {
        close(m_iRfStateFd);
    }

    DELETE_PTR(m_pTimer);
    DELETE_PTR(m_pRfLogger);
    DELETE_PTR(m_pSrvRpcService);
}

bool RaftConsensus::Start() {
    INOUT_LOG;
    if (!m_bStopped) {
        return true;
    }

    m_bStopped = false;
    if (!m_pSrvRpcService->Start()) {
        LOGEFUN << "Cannot start ServerRpcService.";
        return false;
    }

    m_pTimer->Start();
    subscribe_leader_hb_timer_tick();

    return true;
}

bool RaftConsensus::Stop() {
    INOUT_LOG;
    if (m_bStopped) {
        return true;
    }

    m_bStopped = true;
    m_pTimer->Stop();
    if (!m_pSrvRpcService->Stop()) {
        return false;
    }

    return true;
}

rpc::SP_PB_MSG RaftConsensus::OnAppendRfLog(rpc::SP_PB_MSG sspMsg) {
    auto appendRfLogRequest = dynamic_cast<protocol::AppendRfLogRequest*>(sspMsg.get());
    auto response = new protocol::AppendRfLogResponse();
    response->set_term(1111);
    response->set_success(true);

    return rpc::SP_PB_MSG(response);
}

rpc::SP_PB_MSG RaftConsensus::OnRequestVote(rpc::SP_PB_MSG sspMsg) {
    auto response = new protocol::RequestVoteResponse();
    response->set_term(1111);
    response->set_votegranted(true);

    return rpc::SP_PB_MSG(response);
}

void RaftConsensus::OnMessageSent(rpc::ARpcClient::SentRet &&sentRet) {
    if (INVALID_MSG_ID == sentRet.msgId) {
        LOGWFUN << "send rpc(id = " << sentRet.handlerId << ") to " << sentRet.peer << " failed because send msg queue is full!";
    }


}

void RaftConsensus::OnRecvRpcCallbackMsg(std::shared_ptr<net::NotifyMessage> sspNM) {

}

void RaftConsensus::initialize() {
    std::stringstream ss1;
    ss1 << FLAGS_data_dir.c_str() << '/' << RFLOG_DIR;
    auto dataDirPath = ss1.str();
    assert(-1 != fsio::FileUtils::CreateDirPath(dataDirPath.c_str(), 0755));
    ss1 << '/' << RFLOG_FILE_NAME;
    if ("realtime-disk" == FLAGS_rflogger_type) {
        m_pRfLogger = new rflog::RtDiskRfLogger(ss1.str(), true);
    } else {
        LOGFFUN << "We don't have a rf logger named " << FLAGS_rflogger_type.c_str();
    }

    // rf state
    std::stringstream ss2;
    ss2 << dataDirPath.c_str() << '/' << RFSTATE_FILE_NAME;
    m_sRfStateFilePath = ss2.str();
    if (!fsio::FileUtils::Exist(m_sRfStateFilePath)) {
        LOGDFUN2("create rfstate file ", m_sLogFilePath.c_str());
        if (-1 == (m_iRfStateFd = fsio::FileUtils::Open(m_sRfStateFilePath, O_WRONLY, O_CREAT, 0644))) {
            auto err = errno;
            LOGFFUN << "create rfstate file " << m_sRfStateFilePath.c_str() << " failed with errmsg " << strerror(err);
        }

        // write file header.
        uchar header[RFSTATE_MAGIC_NO_LEN];
        ByteOrderUtils::WriteUInt32(header, RFSTATE_MAGIC_NO);
        WriteFileFullyWithFatalLOG(m_iRfStateFd, (char*)header, RFSTATE_MAGIC_NO_LEN, m_sRfStateFilePath.c_str());
    } else {
        if (-1 == (m_iRfStateFd = fsio::FileUtils::Open(m_sRfStateFilePath, O_RDWR, 0, 0))) {
            auto err = errno;
            LOGFFUN << "open rfstate file " << m_sRfStateFilePath.c_str() << " failed with errmsg " << strerror(err);
        }

        auto fileSize = fsio::FileUtils::GetFileSize(m_iRfStateFd);
        if (-1 == fileSize) {
            auto err = errno;
            LOGFFUN << "get file size for " << m_sRfStateFilePath.c_str() << " failed with errmsg " << strerror(err);
        }
        if (0 == (uint32_t)fileSize) {
            LOGDFUN3("rfstate file ", m_sLogFilePath.c_str(), " is empty!");
            return;
        }

        if (fileSize < RFSTATE_MAGIC_NO_LEN) {
            LOGFFUN << "rfstate file " << m_sRfStateFilePath.c_str() << " header is corrupt!";
        }

        char *buf = nullptr;
        ReadFileFullyWithFatalLOG(m_iRfStateFd, &buf, (size_t)fileSize, m_sRfStateFilePath.c_str());
        std::shared_ptr<char> sspBuf(buf, common::ArrDeleter<char>());

        uint32_t magicNo = ByteOrderUtils::ReadUInt32((uchar*)buf);
        if (RFSTATE_MAGIC_NO != magicNo) {
            LOGFFUN << "rfstate " << m_sRfStateFilePath.c_str() << " header is corrupt!";
        }

        auto startPtr = (uchar*)(buf + RFSTATE_MAGIC_NO_LEN);
        auto endPtr = (uchar*)(buf + fileSize - 1);
        common::Buffer b;

        b.Refresh(startPtr, endPtr, startPtr, endPtr, nullptr);
        auto rs = new protocol::RaftState();
        common::ProtoBufUtils::Deserialize(&b, rs);
        m_iCurrentTerm = rs->currentterm();
        m_iVoteFor = rs->votedfor();

        DELETE_PTR(rs);
    }
}

void RaftConsensus::save_rf_state() {
    LOGDTAG;
    auto rfsfp = m_sRfStateFilePath.c_str();

    // write state.
    // mpo will be Putted in Buffer 'b'.
    auto rs = new protocol::RaftState();

    rs->set_currentterm(m_iCurrentTerm);
    rs->set_votedfor(m_iVoteFor);

    common::Buffer b;
    common::ProtoBufUtils::Serialize(rs, &b, common::g_pMemPool);
    LSeekFileWithFatalLOG(m_iRfStateFd, RFSTATE_MAGIC_NO_LEN, SEEK_SET, rfsfp);
    WriteFileFullyWithFatalLOG(m_iRfStateFd, (char*)(b.GetStart()), size_t(b.AvailableLength()), m_sRfStateFilePath.c_str());
    FDataSyncFileWithFatalLOG(m_iRfStateFd, rfsfp);
}

void RaftConsensus::start_new_election() {
    LOGITAG;
    m_roleType = NodeRoleType::Candidate;
    ++m_iCurrentTerm;
    if (m_iVoteFor != m_iMyId) {
        save_rf_state();
    }

    m_iVoteFor = m_iMyId;
    broadcast_request_vote(m_pElectorManager->GetOtherServersConf());
    subscribe_leader_hb_timer_tick();
    subscribe_request_vote_timeout_tick();
}

inline void RaftConsensus::subscribe_leader_hb_timer_tick() {
    LOGDTAG;
    auto latency = m_random.GetNew();
    ccsys::cctime interval(FLAGS_raft_election_interval, latency * 1000 * 1000);
    m_monitorLeaderHbTimerEventId = m_pTimer->SubscribeEventAfter(interval, m_monitorLeaderHbTimerEvent);
    if (0 == m_monitorLeaderHbTimerEventId.when) {
        LOGFFUN << "timer event is replicated!";
    }
}

inline void RaftConsensus::on_leader_hb_timeout(void *ctx) {
    start_new_election();
}

inline void RaftConsensus::subscribe_request_vote_timeout_tick() {
    ccsys::cctime interval(m_iRequestVoteTimeoutInterval, 0);
    m_monitorRVTimeoTimerEventId = m_pTimer->SubscribeEventAfter(interval, m_monitorRVTimeoTimerEvent);
    if (0 == m_monitorRVTimeoTimerEventId.when) {
        LOGFFUN << "timer event is replicated!";
    }
}

void RaftConsensus::on_request_vote_timeout(void *ctx) {

}

void RaftConsensus::broadcast_request_vote(const std::map<uint32_t, common::RfServer> &otherSrvs) {
    auto lastRfLogEntry = m_pRfLogger->GetLastEntry();
    auto *pq = new protocol::RequestVoteRequest();
    pq->set_term(m_iCurrentTerm);
    pq->set_candidateid(m_iMyId);
    pq->set_lastlogterm(lastRfLogEntry->term());
    pq->set_lastlogindex(lastRfLogEntry->index());

    rpc::SP_PB_MSG spPbMsg(pq);
    for (auto const &srv : otherSrvs) {
        m_pSrvRpcService->RequestVoteAsync(spPbMsg, srv.second.GetAddrForServer());
    }
}
} // namespace server
} // namespace ccraft
