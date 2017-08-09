/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_FILE_UTILS_H
#define CCRAFT_FILE_UTILS_H

#include <unistd.h>
#include <string>

using std::string;

namespace ccraft {
namespace common {
class FileUtils {
public:
    /**
     * 创建目录
     * @param dir
     * @return 成功返回0,失败返回-1
     */
    static int CreateDir(const char *dir, __mode_t mode);

    /**
     * 递归创建目录路径
     * @param path
     * @param access_mode 访问权限 eg:0755
     * @return 成功返回0,失败返回-1
     */
    static int CreateDirPath(const char *path, __mode_t access_mode);

    /**
     * 删除一个目录
     * @param path
     * @return 成功返回0,否则为errno
     */
    static int RemoveDirectory(const char *path);

    /**
     * 清空一个目录
     * @param path
     * @return 成功返回0,否则为errno
     */
    static int CleanDirectory(const char *path);

    /**
     * 获取文件的大小(bytes)
     * @param path
     * @return 成功返回自然数(>=0)，失败-1。
     */
    static ssize_t GetFileSize(const char *path);

    /**
     * 获取文件的大小(bytes)
     * @param path
     * @return 成功返回自然数(>=0)，失败-1。
     */
    static ssize_t GetFileSize(int fd);

    /**
     * 读取文件的所有字符。
     * @param file_path
     * @return
     */
    static string ReadAllString(const string &file_path);
};
}
}

#endif //CCRAFT_FILE_UTILS_H
