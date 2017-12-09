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
        id(id), addr(std::move(addr)), portForServer(portForServer), portForClient(portForClient) {}

    RfServer(uint32_t id, std::string &addr, uint16_t portForServer, uint16_t portForClient) :
        id(id), addr(addr), portForServer(portForServer), portForClient(portForClient) {}

    RfServer(const RfServer &rs) {
        this->id            = rs.id;
        this->addr          = rs.addr;
        this->portForServer = rs.portForServer;
        this->portForClient = rs.portForClient;
    }

    RfServer(RfServer &&rs) noexcept {
        this->id            = rs.id;
        this->addr          = std::move(rs.addr);
        this->portForServer = rs.portForServer;
        this->portForClient = rs.portForClient;
    }

    RfServer& operator=(const RfServer &rs) = default;

    RfServer& operator=(RfServer &&rs) noexcept {
        this->id            = rs.id;
        this->addr          = std::move(rs.addr);
        this->portForServer = rs.portForServer;
        this->portForClient = rs.portForClient;

        return *this;
    }

    uint32_t              id              = 0;
    std::string           addr;
    uint16_t              portForServer   = 0;
    uint16_t              portForClient   = 0;
};
} // namespace rfcommon
} // namespace ccraft

#endif //CCRAFT_RFCOMMON_RF_SERVER_H
