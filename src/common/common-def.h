/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_COMMON_DEF_H
#define CCRAFT_COMMON_COMMON_DEF_H

#include <time.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <cstring>

/**
 * 控制目标不导出，即仅库内部可见。
 */

#define LIKELY(x)                      __builtin_expect(!!(x), 1)
#define UNLIKELY(x)                    __builtin_expect(!!(x), 0)

#define hw_rw_memory_barrier()         __sync_synchronize()
#define soft_yield_cpu()               __asm__ ("pause")
#define hard_yield_cpu()               sched_yield()
#define atomic_cas(lock, old, set)     __sync_bool_compare_and_swap(lock, old, set)
#define atomic_zero(lock)              __sync_fetch_and_and(lock, 0)
#define atomic_addone_and_fetch(lock)  __sync_add_and_fetch(lock, 1)

#define DELETE_PTR(p) if (p) {delete (p); (p) = nullptr;}
#define DELETE_ARR_PTR(p) if (p) {delete [](p); (p) = nullptr;}
#define FREE_PTR(p) if (p) {free (p); (p) = NULL;}

typedef unsigned char uchar;

namespace ccraft {
namespace common {

const int  CPUS_CNT = get_nprocs();
const long CACHELINE_SIZE = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
const long PAGE_SIZE = sysconf(_SC_PAGESIZE);

} // namespace common
} // namespace ccraft
#endif //CCRAFT_COMMON_COMMON_DEF_H
