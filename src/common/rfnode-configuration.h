/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_CONFIGURATION_H
#define CCRAFT_SERVER_CONFIGURATION_H

#include <string>
#include <map>

namespace ccraft {
namespace common {
struct RfServer {
    RfServer() = default;
    RfServer(uint32_t id, std::string &&addr, uint16_t portForServer, uint16_t portForClient) :
        m_iId(id), m_sAddr(std::move(addr)), m_iPortForServer(portForServer), m_iPortForClient(portForClient) {}

    RfServer(uint32_t id, std::string &addr, uint16_t portForServer, uint16_t portForClient) :
        m_iId(id), m_sAddr(addr), m_iPortForServer(portForServer), m_iPortForClient(portForClient) {}

    RfServer(const RfServer &rs) {
        this->m_iId            = rs.m_iId;
        this->m_sAddr          = rs.m_sAddr;
        this->m_iPortForServer = rs.m_iPortForServer;
        this->m_iPortForClient = rs.m_iPortForClient;
    }

    RfServer(RfServer &&rs) noexcept {
        this->m_iId            = rs.m_iId;
        this->m_sAddr          = std::move(rs.m_sAddr);
        this->m_iPortForServer = rs.m_iPortForServer;
        this->m_iPortForClient = rs.m_iPortForClient;
    }

    RfServer& operator=(const RfServer &rs) = default;

    RfServer& operator=(RfServer &&rs) noexcept {
        this->m_iId            = rs.m_iId;
        this->m_sAddr          = std::move(rs.m_sAddr);
        this->m_iPortForServer = rs.m_iPortForServer;
        this->m_iPortForClient = rs.m_iPortForClient;

        return *this;
    }

    uint32_t              m_iId;
    std::string           m_sAddr;
    uint16_t              m_iPortForServer;
    uint16_t              m_iPortForClient;
};

class RfNodeConfiguration {
public:
    RfNodeConfiguration() = default;
    ~RfNodeConfiguration() = default;

    bool Initialize(std::string &path);
    const std::map<uint32_t, RfServer>& GetAllServers() const;
    const RfServer& operator[](uint32_t key);

private:
    std::map<uint32_t, RfServer>      m_mapServers;
};
} // namespace common
} // namespace ccraft

#endif //CCRAFT_SERVER_CONFIGURATION_H
