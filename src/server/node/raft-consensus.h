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
#include "../../common/cctime.h"
#include "../../common/random.h"
#include "../../common/rf-server.h"

#include "../../rpc/abstract-rpc-client.h"
#include "iserver-internal-rpc-handler.h"

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
class ServerRpcService;
class ElectorManagerService;
class RaftConsensus : public IService, public INodeInternalRpcHandler {
public:
    RaftConsensus();
    ~RaftConsensus() override;

    bool Start() override;
    bool Stop() override;

    // INodeInternalRpcHandler IF
    rpc::SP_PB_MSG OnAppendRfLog(rpc::SP_PB_MSG sspMsg) override;
    rpc::SP_PB_MSG OnRequestVote(rpc::SP_PB_MSG sspMsg) override;
    void OnMessageSent(rpc::ARpcClient::SentRet &&sentRet);
    void OnRecvRpcCallbackMsg(std::shared_ptr<net::NotifyMessage> sspNM) override;

private:
    void initialize();
    void save_rf_state();
    /**
     * leader超时开始选举。
     */
    void on_leader_hb_timeout(void *ctx);
    void start_new_election();
    void subscribe_leader_hb_timer_tick();
    void broadcast_request_vote(const std::map<uint32_t, common::RfServer> &otherSrvs);

private:
    bool                           m_bStopped                    = true;
    NodeRoleType                   m_roleType                    = NodeRoleType::Follower;
    uint32_t                       m_iCurrentTerm                = 0;
    uint32_t                       m_iMyId                       = 0;
    /**
     * zero is no server.
     */
    uint32_t                       m_iVoteFor                    = 0;
    std::string                    m_sRfStateFilePath;
    rflog::IRfLogger              *m_pRfLogger                   = nullptr;
    int32_t                        m_iRfStateFd                  = -1;
    common::Timer                 *m_pTimer                      = nullptr;
    common::Timer::Event           m_monitorLeaderHbTimerEvent;
    common::Timer::EventId         m_monitorLeaderHbTimerEventId;
    common::Timer::TimerCallback   m_moniterLeaderHbTimerCb;
    common::Random                 m_random;
    /**
     * 关联关系，无需本类释放。
     */
    ElectorManagerService         *m_pElectorManager             = nullptr;
    ServerRpcService              *m_pSrvRpcService              = nullptr;
}; // class RaftConsensus
} // namespace server
} // namespace ccraft

#endif //CCRAFT_SERVER_RAFT_CONSENSUS_H
