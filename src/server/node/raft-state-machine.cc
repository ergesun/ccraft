/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../common/common-def.h"
#include "../../codegen/client-server.pb.h"

#include "raft-state-machine.h"

namespace ccraft {
namespace server {
RfStateMachine::RfStateMachine() {

}

RfStateMachine::~RfStateMachine() {

}

bool RfStateMachine::Start() {
    INOUT_LOG;
    if (!m_bStopped) {
        return true;
    }

    m_bStopped = false;
    return true;
}

bool RfStateMachine::Stop() {
    INOUT_LOG;
    if (m_bStopped) {
        return true;
    }

    m_bStopped = true;
    
    return true;
}

void RfStateMachine::Apply(std::vector<protocol::StateMachineCommand*> commands) {
    for (auto command : commands) {
        m_mapKVs[command->key()] = command->value();
    }
}

std::string RfStateMachine::GetValue(const std::string &key) {
    return this->m_mapKVs[key];
}
} // namespace server
} // namespace ccraft
