/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <fstream>
#include <sstream>

#include "rfnode-configuration.h"
#include "common-utils.h"

namespace ccraft {
namespace common {
bool RfNodeConfiguration::Initialize(std::string &path) {
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
        uint32_t id;
        std::string addr;
        uint16_t portForServer, portForClient;
        if (!(iss >> id >> addr >> portForServer >> portForClient)) {
            return false;
        }

        if (0 == id) {
            LOGFFUN << "Server id must be larger than 0.";
        }

        if (m_mapServers.end() != m_mapServers.find(id)) {
            LOGFFUN << "Server id " << id << " is duplicate!";
        }

        m_mapServers.insert(std::make_pair(id, RfServer(id, std::move(addr), portForServer, portForClient)));
    }

    return true;
}

const std::map<uint32_t, RfServer>& RfNodeConfiguration::GetAllServers() const {
    return m_mapServers;
}

const RfServer& RfNodeConfiguration::operator[](uint32_t key) {
    return m_mapServers[key];
}
} // namespace common
} // namespace ccraft
