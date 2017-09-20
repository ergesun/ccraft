/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_NODE_COMMON_DEF_H
#define CCRAFT_SERVER_NODE_COMMON_DEF_H

#include <vector>

using std::vector;

namespace ccraft {
namespace server {
enum class NodeRoleType {
    Leader        = 0,
    Candidate     = 1,
    Follower      = 2
};

enum class ServiceType {
    ElectorManager   = 0,
    ClientRpc        = 1,
    RaftConsensus    = 2,
    RaftStateMachine = 3
};

const vector<ServiceType> SERVICES_TYPES = {
    ServiceType::ElectorManager,
    ServiceType::ClientRpc,
    ServiceType::RaftConsensus,
    ServiceType::RaftStateMachine
};
} // namespace server
} // namespace ccraft

#endif //CCRAFT_SERVER_NODE_COMMON_DEF_H
