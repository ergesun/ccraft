/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SRV_NODE_RAFT_STATE_MACHINE_H
#define CCRAFT_SRV_NODE_RAFT_STATE_MACHINE_H

#include <string>
#include <map>
#include <vector>

#include "iservice.h"

namespace ccraft {
namespace protocal {
    class StateMachineCommand;
}
namespace server {
/**
 * 先实现一版KV的状态机。
 * TODO(sunchao): 考虑写一个tree的目录结构的状态机
 */
class RfStateMachine : public IService {
public:
    RfStateMachine();
    ~RfStateMachine() override;

    bool Start() override;
    bool Stop() override;

    void Apply(std::vector<protocal::StateMachineCommand*> commands);
    std::string GetValue(const std::string &key);

private:
    bool                                         m_bStopped = false;
    std::map<std::string, std::string>           m_mapKVs;
}; // class RfStateMachine
} // namespace server
} // namespace ccraft

#endif //CCRAFT_SRV_NODE_RAFT_STATE_MACHINE_H
