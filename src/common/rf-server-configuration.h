/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RFCOMMON_SERVER_CONFIGURATION_H
#define CCRAFT_RFCOMMON_SERVER_CONFIGURATION_H

#include <string>
#include <map>

#include "rf-server.h"

namespace ccraft {
namespace common {
class RfServerConfiguration {
public:
    RfServerConfiguration() = default;
    ~RfServerConfiguration() = default;

    bool Initialize(uint32_t myId, std::string &path);
    inline const std::map<uint32_t, RfServer>& GetOtherServers() const;
    const RfServer& GetSelfServer() const;

private:
    std::map<uint32_t, RfServer>      m_mapOtherServers;
    RfServer                          m_selfServer;
};
} // namespace rfcommon
} // namespace ccraft

#endif //CCRAFT_RFCOMMON_SERVER_CONFIGURATION_H
