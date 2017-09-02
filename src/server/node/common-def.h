/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_NODE_COMMON_DEF_H
#define CCRAFT_SERVER_NODE_COMMON_DEF_H

namespace ccraft {
namespace server {
enum class NodeRoleType {
    Leader        = 0,
    Candidate     = 1,
    Follower      = 2
};
}
}

#endif //CCRAFT_SERVER_NODE_COMMON_DEF_H
