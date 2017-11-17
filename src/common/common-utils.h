/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_COMMON_UTILS_H
#define CCRAFT_COMMON_COMMON_UTILS_H

#include <sys/socket.h>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#include "../ccsys/cctime.h"

#include "common-def.h"
#include "mem-pool.h"

namespace ccraft {
namespace common {
class Buffer;
class CommonUtils {
public:
    /**
     * 设置fd为非阻塞。
     * @param fd
     * @return
     */
    static int SetNonBlocking(int fd);

    /**
     * 设置fd为阻塞。
     * @param fd
     * @return
     */
    static int SetBlocking(int fd);

    /**
     * posix_memalign的封装
     * @param align 对齐大小
     * @param size 申请的内存大小
     * @return
     */
    static void* PosixMemAlign(size_t align, size_t size);

    /**
     *
     * @param mpo
     * @param size
     * @return
     */
    static common::Buffer* GetNewBuffer(common::MemPoolObject *mpo, uint32_t totalBufferSize);

    /**
     * 判断入参是否为2的N次幂。
     * @param x
     * @return
     */
    static bool IsPowerOfTwo(int x) {
        return (x > 0) && ((x & (x - 1)) == 0);
    }

    /**
     * 解析host并随机选择一个候选sockaddr返回。
     * @param host
     * @param port
     * @param ss
     * @param len
     * @return 成功true，失败fasle。
     */
    static bool GetAddrInfo(const std::string &host, std::string &ip);

    // trim from start (in place)
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                        std::not1(std::ptr_fun<int, int>(std::isspace))));
    }

    // trim from end (in place)
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    static inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    static inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    static inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }
}; // class CommonUtils
}  // namespace common
}  // namespace ccraft
#endif //CCRAFT_COMMON_COMMON_UTILS_H
