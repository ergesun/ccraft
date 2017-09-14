/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_IO_UTILS_H
#define CCRAFT_COMMON_IO_UTILS_H

#include <cstdio>

namespace ccraft {
namespace common {
class IOUtils {
public:
    /**
     * 将size大小的buf写入fd。
     * @param fd
     * @param buf
     * @param size
     * @return -1失败，否则为size
     */
    static ssize_t WriteFully(int fd, const void *buf, size_t size);

    /**
     * 从fd中读取size大小的buf。
     * ！注意：user负责使用完后释放*buf。
     * @param fd
     * @param buf 输出
     * @param size 想读取的大小
     * @return -1出错，否则为实际读取到的大小。
     */
    static ssize_t ReadFully(int fd, void **buf, size_t size);

    /**
     * 从fd中读取size大小的buf。
     * ！注意：user负责使用完后释放*buf。
     * @param fd
     * @param buf 输出
     * @param size 想读取的大小
     * @return -1出错，否则为实际读取到的大小。
     */
    static ssize_t ReadFully_V2(int fd, void **buf, size_t size);
};
}
}

#endif //CCRAFT_COMMON_IO_UTILS_H
