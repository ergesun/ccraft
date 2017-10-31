/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_COMMON_DEF_H
#define CCRAFT_COMMON_COMMON_DEF_H

#include <string>
#include <functional>

#include <glog/logging.h>

/**
 * 控制目标不导出，即仅库内部可见。
 */

#define LIKELY(x)                         __builtin_expect(!!(x), 1)
#define UNLIKELY(x)                       __builtin_expect(!!(x), 0)

#define hw_rw_memory_barrier()            __sync_synchronize()
#define soft_yield_cpu()                  __asm__ ("pause")
#define hard_yield_cpu()                  sched_yield()
#define atomic_cas(lock, old, set)        __sync_bool_compare_and_swap(lock, old, set)
#define atomic_zero(lock)                 __sync_fetch_and_and(lock, 0)
#define atomic_fetch(lock)                __sync_fetch_and_and(lock, 1)
#define atomic_addone_and_fetch(lock)     __sync_add_and_fetch(lock, 1)
#define atomic_subone_and_fetch(lock)     __sync_sub_and_fetch(lock, 1)
#define atomic_fetch_and_set(lock, val)   __sync_lock_test_and_set(lock, val)

#define DELETE_PTR(p) if (p) {delete (p); (p) = nullptr;}
#define DELETE_ARR_PTR(p) if (p) {delete [](p); (p) = nullptr;}
#define FREE_PTR(p) if (p) {free (p); (p) = NULL;}

// TODO(sunchao): 实现一个打log的库，glog性能不行。
// 不要使用此宏打debug log，而要使用LOGDFUNx系列。
#define LOGDFUN LOG(INFO) << " [DEBUG] " << __FUNCTION__ << ": "

#ifdef DEBUG_LOG

#define LOGDFUN0() LOGDFUN
#define LOGDFUN1(arg) LOGDFUN << arg
#define LOGDFUN2(arg0, arg1) LOGDFUN << arg0 << arg1
#define LOGDFUN3(arg0, arg1, arg2) LOGDFUN << arg0 << arg1 << arg2
#define LOGDFUN4(arg0, arg1, arg2, arg3) LOGDFUN << arg0 << arg1 << arg2 << arg3
#define LOGDFUN5(arg0, arg1, arg2, arg3, arg4) LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4
#define LOGDFUN6(arg0, arg1, arg2, arg3, arg4, arg5) LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4 << arg5
#define LOGDFUN7(arg0, arg1, arg2, arg3, arg4, arg5, arg6) LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4 << arg5 << arg6
#define LOGDFUN8(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7) LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7
#define LOGDFUN9(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)                                   \
        LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7 << arg8
#define LOGDFUN10(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)                            \
        LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7 << arg8 << arg9
#define LOGDFUN11(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10)                     \
        LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7 << arg8 << arg9 << arg10
#define LOGDFUN12(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11)                  \
        LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7 << arg8 << arg9 << arg10 << arg11
#define LOGDFUN13(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12)                 \
        LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7 << arg8 << arg9 << arg10 << arg11 << arg12
#define LOGDFUN14(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13)                 \
        LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7 << arg8 << arg9 << arg10 << arg11 << arg12 << arg13
#define LOGDFUN15(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14)                 \
        LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7 << arg8 << arg9 << arg10 << arg11 << arg12 << arg13 << arg14
#define LOGDFUN16(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15)                 \
        LOGDFUN << arg0 << arg1 << arg2 << arg3 << arg4 << arg5 << arg6 << arg7 << arg8 << arg9 << arg10 << arg11 << arg12 << arg13 << arg14 << arg15


#define LOGDTAG LOG(INFO) << "[DEBUG] " << __FUNCTION__ << "."
#define INOUT_DLOG stringstream ss;                                                                       \
                   ss << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__;                              \
                   FuncInOutLogger __in__out_log__(ss.str());

#else

#define LOGDFUN0()
#define LOGDFUN1(arg)
#define LOGDFUN2(arg0, arg1)
#define LOGDFUN3(arg0, arg1, arg2)
#define LOGDFUN4(arg0, arg1, arg2, arg3)
#define LOGDFUN5(arg0, arg1, arg2, arg3, arg4)
#define LOGDFUN6(arg0, arg1, arg2, arg3, arg4, arg5)
#define LOGDFUN7(arg0, arg1, arg2, arg3, arg4, arg5, arg6)
#define LOGDFUN8(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
#define LOGDFUN9(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
#define LOGDFUN10(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
#define LOGDFUN11(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10)
#define LOGDFUN12(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11)
#define LOGDFUN13(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12)
#define LOGDFUN14(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13)
#define LOGDFUN15(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14)
#define LOGDFUN16(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15)
#define INOUT_DLOG

#define LOGDTAG

#endif

#define LOGIFUN LOG(INFO) << __FUNCTION__ << ": "
#define LOGWFUN LOG(WARNING) << __FUNCTION__ << ": "
#define LOGEFUN LOG(ERROR) << __FUNCTION__ << ": "
#define LOGFFUN LOG(FATAL) << __FUNCTION__ << ": "


#define LOGITAG LOG(INFO) << __FUNCTION__ << "."
#define LOGWTAG LOG(WARNING) << __FUNCTION__ << "."
#define LOGETAG LOG(ERROR) << __FUNCTION__ << "."
#define LOGFTAG LOG(FATAL) << __FUNCTION__ << "."

#define INOUT_LOG ccraft::common::FuncInOutLogger __in__out_log__(__FUNCTION__);

typedef unsigned char uchar;

namespace ccraft {
namespace common {
class FuncInOutLogger {
public:
    FuncInOutLogger() = delete;
    FuncInOutLogger(const std::string &&content) {
        this->content = std::move(content);
        LOG(INFO) << this->content << " [In].";
    }

    ~FuncInOutLogger() {
        LOG(INFO) << this->content << " [Out].";
    }

private:
    std::string content;
};

template <typename T>
class ObjReleaseHandler {
public:
    ObjReleaseHandler(T *p, std::function<void(T *arg)> func) : __p(p), __func(func) {}
    ~ObjReleaseHandler() {
        if (__func) {
            __func(__p);
        }
    }

private:
    T *__p;
    std::function<void(T *arg)> __func;
};

template <typename T>
class ObjFreer {
public:
    void operator() (T* p) {
        FREE_PTR(p);
    }
};

template <typename T>
class ObjDeleter {
public:
    void operator() (T* p) {
        DELETE_PTR(p);
    }
};

template <typename T>
class ArrDeleter {
public:
    void operator() (T* p) {
        DELETE_ARR_PTR(p);
    }
};

} // namespace common
} // namespace ccraft
#endif //CCRAFT_COMMON_COMMON_DEF_H
