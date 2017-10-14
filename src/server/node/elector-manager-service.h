/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_ELECTOR_MANAGER_H
#define CCRAFT_SERVER_ELECTOR_MANAGER_H

#include <string>
#include "iservice.h"

namespace ccraft {
namespace common {
class RfServer;
class RfServerConfiguration;
}
namespace server {
class ElectorManagerService : public IService {
public:
    ElectorManagerService();
    ~ElectorManagerService();

    bool Start() override;
    bool Stop() override;

    const common::RfServer& GetSelfServerConf() const;

    bool MarkVoted();
    bool MeetQuorum();

private:
    bool initialize(uint32_t myId, std::string &&serversConfPath);

private:
    common::RfServerConfiguration             *m_nodeConf = nullptr;
    uint32_t                                   m_iMyId = 0;
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_SERVER_ELECTOR_MANAGER_H
