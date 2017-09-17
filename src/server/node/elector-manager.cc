/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../common/rfnode-configuration.h"
#include "../../common/common-def.h"

#include "elector-manager.h"

namespace ccraft {
namespace server {
ElectorManager::ElectorManager() {
    m_nodeConf = new common::RfNodeConfiguration();
}

ElectorManager::~ElectorManager() {
    DELETE_PTR(m_nodeConf);
}

bool ElectorManager::Initialzie(uint32_t myId, std::string &&serversConfPath) {
    if (0 == myId) {
        LOGFFUN << "server id must be larger than 0.";
    }

    m_iMyId = myId;

    return m_nodeConf->Initialize(serversConfPath);
}

const common::RfServer& ElectorManager::GetSelfServerConf() const {
    return (*m_nodeConf)[m_iMyId];
}
} // namespace server
} // namespace ccraft
