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
namespace rfcommon {
class RfServerConfiguration {
public:
    RfServerConfiguration() = default;
    ~RfServerConfiguration() = default;

    bool Initialize(std::string &path);
    const std::map<uint32_t, RfServer>& GetAllServers() const;
    const RfServer& operator[](uint32_t key);

private:
    std::map<uint32_t, RfServer>      m_mapServers;
};
} // namespace rfcommon
} // namespace ccraft

#endif //CCRAFT_RFCOMMON_SERVER_CONFIGURATION_H
