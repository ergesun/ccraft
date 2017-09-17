/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_RAFT_CONSENSUS_H
#define CCRAFT_SERVER_RAFT_CONSENSUS_H

#include <cstdint>

#include "common-def.h"

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
class RaftConsensus {
public:
    RaftConsensus();
    ~RaftConsensus();

private:
    void initialize();
    void save_rf_state();

private:
    NodeRoleType                m_roleType        = NodeRoleType::Follower;
    uint32_t                    m_iCurrentTerm    = 0;
    uint32_t                    m_iVoteFor        = 0;
    rflog::IRfLogger           *m_pRfLogger       = nullptr;
    int32_t                     m_iRfStateFd      = -1;
}; // class RaftConsensus
} // namespace server
} // namespace ccraft

#endif //CCRAFT_SERVER_RAFT_CONSENSUS_H
