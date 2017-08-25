/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <sys/sysinfo.h>
#include <unistd.h>
#include <cstdio>
#include <stdexcept>
#include <cstring>

#include "timer.h"
#include "mem-pool.h"
#include "global-vars.h"

namespace ccraft {
namespace common {
int      LOGIC_CPUS_CNT;
int      PHYSICAL_CPUS_CNT;
long     CACHELINE_SIZE;
long     PAGE_SIZE;
Timer   *g_pTimer = nullptr;
MemPool *g_pMemPool;

#ifdef __linux__
void initialize() {
    CACHELINE_SIZE = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    PAGE_SIZE = sysconf(_SC_PAGESIZE);
    LOGIC_CPUS_CNT = get_nprocs();
    FILE * fp = nullptr;
    char buffer[80] = "\0";
    if (!(fp = popen("lscpu |grep \"Thread\"|awk -F \':\' \'{print $2}\'| awk \'{print $1}\'", "r"))) {
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

    PHYSICAL_CPUS_CNT = LOGIC_CPUS_CNT / atoi(buffer);
    if (0 == PHYSICAL_CPUS_CNT) {
        throw std::runtime_error("atoi physical cpus cnt failed!");
    }

    g_pTimer = new Timer();
    g_pTimer->Start();
    g_pMemPool = new MemPool();
}

void uninitialize() {
    g_pTimer->Stop();
    DELETE_PTR(g_pTimer);
    DELETE_PTR(g_pMemPool);
}
#endif
} // namespace common
} // namespace ccraft
