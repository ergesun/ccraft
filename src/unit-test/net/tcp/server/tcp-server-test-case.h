/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_TCP_SERVER_TEST_CASE_H
#define CCRAFT_TCP_SERVER_TEST_CASE_H

#include <memory>

#include "../../../../net/notify-message.h"

namespace ccraft {
namespace ccsys {
    class MemPool;
}
namespace net {
    class ISocketService;
}

namespace test {
class TcpServerTest {
public:
    static void Run();

private:
    static void recv_msg(std::shared_ptr<ccraft::net::NotifyMessage> sspNM);
    static ccraft::net::ISocketService     *s_ss;
    static ccraft::ccsys::MemPool          *m_mp;
};
}
}

#endif //CCRAFT_TCP_SERVER_TEST_CASE_H
