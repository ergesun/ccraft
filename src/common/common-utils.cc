/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <time.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>

#include "buffer.h"
#include "common-utils.h"
#include "random.h"

namespace ccraft {
namespace common {
cctime_t CommonUtils::GetCurrentTime() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    return cctime_t(ts);
}

int CommonUtils::SetNonBlocking(int fd) {
    int opts;
    int err;
    if ((opts = fcntl(fd, F_GETFL)) == -1) {
        err = errno;
        LOGEFUN << "get fd opts err = " << err;

        return -1;
    }

    opts = opts | O_NONBLOCK;
    if ((err = fcntl(fd, F_SETFL, opts)) == -1) {
        err = errno;
        LOGEFUN << "set fd O_NONBLOCK err = " << err;
        return -1;
    }

    return 0;
}

int CommonUtils::SetBlocking(int fd) {
    int opts;
    int err;
    if ((opts = fcntl(fd, F_GETFL)) == -1) {
        err = errno;
        LOGEFUN << "get fd opts err = " << err;

        return -1;
    }

    opts = opts & ~O_NONBLOCK;
    if (fcntl(fd, F_SETFL, opts) == -1) {
        err = errno;
        LOGEFUN << "set fd BLOCKing err = " << err;
        return -1;
    }

    return 0;
}

void* CommonUtils::PosixMemAlign(size_t align, size_t size) {
    void *pln;
    if (posix_memalign(&pln, align, size)) {
        return nullptr;
    }

    return pln;
}

common::Buffer* CommonUtils::GetNewBuffer(common::MemPoolObject *mpo, uint32_t totalBufferSize) {
    auto bufferStart = (uchar*)(mpo->Pointer());
    auto bufferEnd = bufferStart + totalBufferSize - 1;
    return new common::Buffer(nullptr, nullptr, bufferStart, bufferEnd, mpo);
}

bool CommonUtils::GetAddrInfo(const std::string &host, std::string &ip) {
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
                            LOGDFUN5("retry getnameinfo for host ", host.c_str(), " times = ", i, ".");
                            break;
                        }
                        default: {
                            LOGEFUN << "getnameinfo for host " << host.c_str() << " failed with errcode " << rc2 << ".";
                            ok = true;
                        }
                    }
                }
            } else {
                LOGWFUN << "Cannot find a valid ip on the host.";
            }
            break;
        }

        default:
            LOGWFUN << "Occur error when getaddrinfo for "
                    << host.c_str() << " with err code " << rc1 << ".";
    }

    if (result) {
        freeaddrinfo(result);
    }

    return ret;
}
} // namespace common
} // namespace ccraft
