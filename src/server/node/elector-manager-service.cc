/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../common/rf-server-configuration.h"
#include "../../common/common-def.h"
#include "../../common/server-gflags-config.h"

#include "elector-manager-service.h"

namespace ccraft {
namespace server {
ElectorManagerService::ElectorManagerService() {
    m_pNodeConf = new common::RfServerConfiguration();
}

ElectorManagerService::~ElectorManagerService() {
    DELETE_PTR(m_pNodeConf);
}

bool ElectorManagerService::Start() {
    if (!initialize(m_iMyId, FLAGS_servers_conf_path.c_str())) {
        LOGFFUN << "ElectorManagerService initialize failed!";
    }

    return true;
}

bool ElectorManagerService::Stop() {
    return true;
}

bool ElectorManagerService::initialize(uint32_t myId, std::string &&serversConfPath) {
    if (0 == myId) {
        LOGFFUN << "server id must be larger than 0.";
    }

    m_iMyId = myId;

    return m_pNodeConf->Initialize(myId, serversConfPath);
}

const common::RfServer& ElectorManagerService::GetSelfServerConf() const {
    return m_pNodeConf->GetSelfServer();
}

const std::map<uint32_t, common::RfServer>& ElectorManagerService::GetOtherServersConf() const {
    return m_pNodeConf->GetOtherServers();
}
} // namespace server
} // namespace ccraft
