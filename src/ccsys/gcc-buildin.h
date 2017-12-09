/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_CCSYS_GCC_BUILDIN_H
#define CCRAFT_CCSYS_GCC_BUILDIN_H

#define LIKELY(x)                         __builtin_expect(!!(x), 1)
#define UNLIKELY(x)                       __builtin_expect(!!(x), 0)

#define hw_sw_memory_barrier()            __sync_synchronize()
#define soft_yield_cpu()                  __asm__ ("pause")
#define hard_yield_cpu()                  sched_yield()
#define atomic_cas(lock, old, set)        __sync_bool_compare_and_swap(lock, old, set)
#define atomic_zero(lock)                 __sync_fetch_and_and(lock, 0)
#define atomic_fetch(lock)                __sync_fetch_and_and(lock, 1)
#define atomic_addone_and_fetch(lock)     __sync_add_and_fetch(lock, 1)
#define atomic_subone_and_fetch(lock)     __sync_sub_and_fetch(lock, 1)
#define atomic_fetch_and_set(lock, val)   __sync_lock_test_and_set(lock, val)

#endif //CCRAFT_CCSYS_GCC_BUILDIN_H
