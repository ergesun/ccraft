/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <cstring>
#include <vector>

#include "utils.h"
#include "random.h"

namespace ccraft {
namespace ccsys {
int Utils::SetNonBlocking(int fd) {
    int opts;
    int err;
    if ((opts = fcntl(fd, F_GETFL)) == -1) {
        err = errno;
        std::cerr << "get fd opts err = " << err << std::endl;

        return -1;
    }

    opts = opts | O_NONBLOCK;
    if ((err = fcntl(fd, F_SETFL, opts)) == -1) {
        err = errno;
        std::cerr << "set fd O_NONBLOCK err = " << err << std::endl;
        return -1;
    }

    return 0;
}

int Utils::SetBlocking(int fd) {
    int opts;
    int err;
    if ((opts = fcntl(fd, F_GETFL)) == -1) {
        err = errno;
        std::cerr << "get fd opts err = " << err << std::endl;

        return -1;
    }

    opts = opts & ~O_NONBLOCK;
    if (fcntl(fd, F_SETFL, opts) == -1) {
        err = errno;
        std::cerr << "set fd BLOCKing err = " << err << std::endl;
        return -1;
    }

    return 0;
}

void* Utils::PosixMemAlign(size_t align, size_t size) {
    void *pln;
    if (posix_memalign(&pln, align, size)) {
        return nullptr;
    }

    return pln;
}

bool Utils::GetAddrInfo(const std::string &host, std::string &ip) {
    ip.clear();
    bool ret = false;
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_NUMERICSERV | AI_V4MAPPED | AI_ADDRCONFIG;

    addrinfo* result = nullptr;
    int rc1 = getaddrinfo(host.c_str(), nullptr, &hints, &result);
    switch (rc1) {
        case 0: { // Success.
            // Look for IPv4 and IPv6 addresses.
            std::vector<addrinfo*> candidates;
            for (addrinfo* addr = result; addr != nullptr; addr = addr->ai_next) {
                // TODO(sunchao): support ipv6
                if (addr->ai_family == AF_INET /*|| addr->ai_family == AF_INET6*/) {
                    candidates.push_back(addr);
                }
            }
            if (!candidates.empty()) {
                // Select one randomly and hope it works.
                size_t idx = Random::GetNewWithSRand() % candidates.size();
                addrinfo* addr = candidates[idx];
                bool ok = false;
                for (int8_t i = 0; i < 10 && !ok; ++i) {
                    char tmpIp[256] = "";
                    auto rc2 = getnameinfo(addr->ai_addr, addr->ai_addrlen, tmpIp, sizeof(tmpIp), nullptr, 0, NI_NUMERICHOST);
                    switch (rc2) {
                        case 0: { // Success.
                            ip = tmpIp;
                            ok = true;
                            ret = true;
                            break;
                        }
                        case EAI_AGAIN: {
                            // retry.
                            std::cout << "retry getnameinfo for host " << host.c_str() << " times = " << i << "." << std::endl;
                            break;
                        }
                        default: {
                            std::cerr << "getnameinfo for host " << host.c_str() << " failed with errcode " << rc2 << ".";
                            ok = true;
                        }
                    }
                }
            } else {
                std::cerr << "Cannot find a valid ip on the host.";
            }
            break;
        }

        default:
            std::cerr << "Occur error when getaddrinfo for "
                    << host.c_str() << " with err code " << rc1 << "." << std::endl;
    }

    if (result) {
        freeaddrinfo(result);
    }

    return ret;
}
}
}
