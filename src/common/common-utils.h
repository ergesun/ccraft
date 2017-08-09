/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_COMMON_UTILS_H
#define CCRAFT_COMMON_COMMON_UTILS_H

#include "common-def.h"
#include "cctime.h"
#include "mem-pool.h"

namespace ccraft {
namespace common {
class Buffer;
class CommonUtils {
public:
    /**
     * 获取当前系统时间(unix epoch到现在的秒+纳秒数)。
     * @return
     */
    static cctime_t GetCurrentTime();

    /**
     * 设置fd为非阻塞。
     * @param fd
     * @return
     */
    static int SetNonBlocking(int fd);

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
}; // class CommonUtils
}  // namespace common
}  // namespace ccraft
#endif //CCRAFT_COMMON_COMMON_UTILS_H
