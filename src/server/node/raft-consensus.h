/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_RAFT_CONSENSUS_H
#define CCRAFT_SERVER_RAFT_CONSENSUS_H

#include <cstdint>

#include "common-def.h"
#include "iservice.h"
#include "../../ccsys/timer.h"
#include "../../ccsys/cctime.h"
#include "../../ccsys/random.h"
#include "../../common/rf-server.h"

#include "../../rpc/abstract-rpc-client.h"
#include "iserver-internal-rpc-handler.h"

#define RFLOG_DIR          "rflogs"
#define RFLOG_FILE_NAME    "rflog"
#define RFSTATE_FILE_NAME  "rfstate"

//         RFSTATE_MAGIC_NO        r f s t
#define    RFSTATE_MAGIC_NO      0x72667374
#define    RFSTATE_MAGIC_NO_LEN  4

/**
 * TODO(sunchao): 考虑有没有必要摒弃流水线模式，改成一个peer独占一个线程玩心跳以及选举。
 */
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
    void start_new_election();
    inline void subscribe_leader_hb_timer_tick();
    /**
     * leader心跳超时。
     */
    inline void on_leader_hb_timeout(void *ctx);
    inline void subscribe_request_vote_timeout_tick();
    /**
     * 广播选举请求到收到major响应超时。
     * @param ctx
     */
    void on_request_vote_timeout(void *ctx);
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
    ccsys::Timer                  *m_pTimer                      = nullptr;
    ccsys::Timer::Event            m_monitorLeaderHbTimerEvent;
    ccsys::Timer::EventId          m_monitorLeaderHbTimerEventId;
    ccsys::Timer::TimerCallback    m_moniterLeaderHbTimerCb;
    ccsys::Timer::Event            m_monitorRVTimeoTimerEvent;
    ccsys::Timer::EventId          m_monitorRVTimeoTimerEventId;
    ccsys::Timer::TimerCallback    m_moniterRVTimeoTimerCb;
    ccsys::Random                  m_random;
    uint32_t                       m_iRequestVoteTimeoutInterval = 0;
    /**
     * 关联关系，无需本类释放。
     */
    ElectorManagerService         *m_pElectorManager             = nullptr;
    ServerRpcService              *m_pSrvRpcService              = nullptr;
}; // class RaftConsensus
} // namespace server
} // namespace ccraft

#endif //CCRAFT_SERVER_RAFT_CONSENSUS_H
