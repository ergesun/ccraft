//
// Created by sunchao31 on 17-8-10.
//

#include "common-def.h"
#include "buffer.h"

namespace ccraft {
namespace common {
int LOGIC_CPUS_CNT;
int PHYSICAL_CPUS_CNT;
void initialize() {
    LOGIC_CPUS_CNT = get_nprocs();
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

    if (-1 == pclose(fp)) {
        auto err = errno;
        throw std::runtime_error(strerror(err));
    }

    PHYSICAL_CPUS_CNT = atoi(buffer);
    if (0 == PHYSICAL_CPUS_CNT) {
        throw std::runtime_error("atoi physical cpus cnt failed!");
    }
}
} // namespace common
} // namespace ccraft
