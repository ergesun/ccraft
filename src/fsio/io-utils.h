/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_FSIO_IO_UTILS_H
#define CCRAFT_FSIO_IO_UTILS_H

#include <cstdio>

#define WriteFileFullyWithFatalLOG(fd, buf, size, filePath)                                         \
        if (-1 == fsio::IOUtils::WriteFully((fd), (buf), (size))) {                                 \
            auto err = errno;                                                                       \
            LOGFFUN << "write file " << (filePath) << " failed with errmsg " << strerror(err);      \
        }

#define ReadFileFullyWithFatalLOG(fd, buf, size, filePath)                                          \
        if (-1 == fsio::IOUtils::ReadFully_V2((fd), (buf), (size))) {                               \
            auto err = errno;                                                                       \
            LOGFFUN << "read file " << (filePath) << " failed with errmsg " << strerror(err);       \
        }

#define LSeekFileWithFatalLOG(fd, len, where, filePath)                                             \
        if (-1 == lseek((fd), (len), (where))) {                                                    \
            auto err = errno;                                                                       \
            LOGFFUN << "lseek file " << (filePath) << " failed with errmsg " << strerror(err);      \
        }

#define FSyncFileWithFatalLOG(fd, filePath)                                                         \
        if (-1 == fsync((fd))) {                                                                    \
            auto err = errno;                                                                       \
            LOGFFUN << "fdatasync file " << (filePath) << " failed with errmsg " << strerror(err);  \
        }

#define FDataSyncFileWithFatalLOG(fd, filePath)                                                     \
        if (-1 == fdatasync((fd))) {                                                                \
            auto err = errno;                                                                       \
            LOGFFUN << "fdatasync file " << (filePath) << " failed with errmsg " << strerror(err);  \
        }

namespace ccraft {
namespace fsio {
class IOUtils {
public:
    /**
     * 将size大小的buf写入fd。
     * @param fd
     * @param buf
     * @param size
     * @return -1失败，否则为size
     */
    static ssize_t WriteFully(int fd, const char *buf, size_t size);

    /**
     * 从fd中读取size大小的buf。
     * ！注意：user负责使用完后释放*buf。
     * @param fd
     * @param buf 输出
     * @param size 想读取的大小
     * @return -1出错，否则为实际读取到的大小。
     */
    static ssize_t ReadFully(int fd, char **buf, size_t size);

    /**
     * 从fd中读取size大小的buf。
     * ！注意：user负责使用完后释放*buf。
     * @param fd
     * @param buf 输出
     * @param size 想读取的大小
     * @return -1出错，否则为实际读取到的大小。
     */
    static ssize_t ReadFully_V2(int fd, char **buf, size_t size);
};
}
}

#endif //CCRAFT_FSIO_IO_UTILS_H
