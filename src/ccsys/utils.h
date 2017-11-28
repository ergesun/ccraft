/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_CCSYS_UTILS_H
#define CCRAFT_CCSYS_UTILS_H

#include <cstdlib>
#include <string>

namespace ccraft {
namespace ccsys {
class Utils {
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
     * 解析host并随机选择一个候选sockaddr返回。
     * @param host
     * @param port
     * @param ss
     * @param len
     * @return 成功true，失败fasle。
     */
    static bool GetAddrInfo(const std::string &host, std::string &ip);
};
}
}

#endif //CCRAFT_CCSYS_UTILS_H
