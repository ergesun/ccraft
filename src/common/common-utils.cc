/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <time.h>
#include <fcntl.h>
#include <iostream>

#include "buffer.h"
#include "common-utils.h"

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

int CommonUtils::GetPhysicalCoresNumber() {
    FILE * fp = nullptr;
    char buffer[80] = "\0";
    if (!(fp = popen("cat /proc/cpuinfo |grep \"core id\"|sort|uniq|wc -l", "r"))) {
        auto err = errno;
        throw std::runtime_error(strerror(err));
    }

    while (fread(buffer, 1, 80, fp), !feof(fp) && !ferror(fp)) {}
    if (ferror(fp)) {
        throw std::runtime_error("fread err occur!");
    }

    auto socketCnt = atoi(buffer);
    pclose(fp);

    return socketCnt;
}
} // namespace common
} // namespace ccraft
