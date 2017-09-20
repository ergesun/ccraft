/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <fstream>
#include <sstream>

#include "rf-server-configuration.h"
#include "../common/common-def.h"
#include "../common/common-utils.h"

namespace ccraft {
namespace rfcommon {
bool RfServerConfiguration::Initialize(uint32_t myId, std::string &path) {
    std::ifstream fs(path.c_str());
    if (!fs) {
        return false;
    }

    std::string line;
    while (std::getline(fs, line)) {
        common::CommonUtils::ltrim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        int32_t id;
        std::string addr;
        uint16_t portForServer, portForClient;
        if (!(iss >> id >> addr >> portForServer >> portForClient)) {
            return false;
        }

        if (0 >= id) {
            LOGFFUN << "Server id must be larger than 0.";
        }

        if (id == myId) {
            if (m_selfServer.m_iId == id) {
                LOGFFUN << "Server id " << id << " is duplicate!";
            }
            m_selfServer = RfServer(id, std::move(addr), portForServer, portForClient);
            continue;
        }

        if (m_mapOtherServers.end() != m_mapOtherServers.find(id)) {
            LOGFFUN << "Server id " << id << " is duplicate!";
        }

        m_mapOtherServers.insert(std::make_pair(id, RfServer(id, std::move(addr), portForServer, portForClient)));
    }

    return true;
}

inline const std::map<uint32_t, RfServer>& RfServerConfiguration::GetOtherServers() const {
    return m_mapOtherServers;
}

const RfServer& RfServerConfiguration::GetSelfServer() const {
    return m_selfServer;
}
} // namespace rfcommon
} // namespace ccraft
