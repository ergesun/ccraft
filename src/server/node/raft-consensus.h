/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_RAFT_CONSENSUS_H
#define CCRAFT_SERVER_RAFT_CONSENSUS_H

#include <cstdint>

#include "common-def.h"
#include "iservice.h"
#include "../../common/timer.h"

#define RFLOG_DIR          "rflogs"
#define RFLOG_FILE_NAME    "rflog"
#define RFSTATE_FILE_NAME  "rfstate"

//         RFSTATE_MAGIC_NO        r f s t
#define    RFSTATE_MAGIC_NO      0x72667374
#define    RFSTATE_MAGIC_NO_LEN  4

namespace ccraft {
namespace rflog {
class IRfLogger;
}
namespace server {
class ElectorManager;
class RaftConsensus : public IService {
public:
    RaftConsensus();
    ~RaftConsensus() override;

    bool Start() override;
    bool Stop() override;

private:
    void initialize();
    void save_rf_state();
    /**
     * leader超时开始选举。
     */
    void on_leader_hb_timeout();

private:
    bool                        m_bStopped                = true;
    NodeRoleType                m_roleType                = NodeRoleType::Follower;
    uint32_t                    m_iCurrentTerm            = 0;
    uint32_t                    m_iVoteFor                = 0;
    std::string                 m_sRfstateFilePath;
    rflog::IRfLogger           *m_pRfLogger               = nullptr;
    int32_t                     m_iRfStateFd              = -1;
    common::Timer              *m_pTimer                  = nullptr;
    common::Timer::EventId      m_electionTimerEvent;
    ElectorManager             *m_pElectorManager         = nullptr;
}; // class RaftConsensus
} // namespace server
} // namespace ccraft

#endif //CCRAFT_SERVER_RAFT_CONSENSUS_H
