/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RFCOMMON_RF_SERVER_H
#define CCRAFT_RFCOMMON_RF_SERVER_H

#include <string>

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

    uint32_t              m_iId              = 0;
    std::string           m_sAddr;
    uint16_t              m_iPortForServer   = 0;
    uint16_t              m_iPortForClient   = 0;
};
} // namespace rfcommon
} // namespace ccraft

#endif //CCRAFT_RFCOMMON_RF_SERVER_H
