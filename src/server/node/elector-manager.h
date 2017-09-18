/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_ELECTOR_MANAGER_H
#define CCRAFT_SERVER_ELECTOR_MANAGER_H

#include <string>

namespace ccraft {
namespace rfcommon {
class RfServer;
class RfServerConfiguration;
}
namespace server {
class ElectorManager {
public:
    ElectorManager();
    ~ElectorManager();

    bool Initialize(uint32_t myId, std::string &&serversConfPath);
    const rfcommon::RfServer& GetSelfServerConf() const;

    bool MarkVoted();
    bool MeetQuorum();

private:
    rfcommon::RfServerConfiguration           *m_nodeConf = nullptr;
    uint32_t                                   m_iMyId = 0;
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_SERVER_ELECTOR_MANAGER_H
