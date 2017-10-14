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
namespace common {
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

        if (uint32_t(id) == myId) {
            if (m_selfServer.m_iId == uint32_t(id)) {
                LOGFFUN << "Server id " << id << " is duplicate!";
            }
            m_selfServer = RfServer(uint32_t(id), std::move(addr), portForServer, portForClient);
            continue;
        }

        if (m_mapOtherServers.end() != m_mapOtherServers.find(uint32_t(id))) {
            LOGFFUN << "Server id " << id << " is duplicate!";
        }

        m_mapOtherServers.insert(std::make_pair(uint32_t(id), RfServer(uint32_t(id), std::move(addr), portForServer, portForClient)));
    }

    return true;
}
} // namespace rfcommon
} // namespace ccraft
