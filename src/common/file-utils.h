/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_FILE_UTILS_H
#define CCRAFT_FILE_UTILS_H

#include <unistd.h>
#include <sys/stat.h>

#include <string>
#include <fcntl.h>

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
    static off_t GetFileSize(const char *path);

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

    static inline void GetStat(const string &path, struct stat *st) {
        stat(path.c_str(), st);
    }

    /**
     * 判断是否为文件夹
     * @param path
     * @return 是为true，否则false
     */
    static inline bool IsDir(const string &path) {
        struct stat st;
        return stat(path.c_str(), &st) >= 0 && S_ISDIR(st.st_mode);
    }

    /**
     * 判断是否为文件
     * @param path
     * @return 是为true，否则false
     */
    static inline bool IsFile(const string &path) {
        struct stat st;
        return stat(path.c_str(), &st) >= 0 && S_ISREG(st.st_mode);
    }

    /**
     * 检查文件是否存在
     * @param path
     * @return 存在为true，否则为false
     */
    static inline bool Exist(const string &path) {
        return access(path.c_str(), F_OK) != -1;
    }

    /**
     * 打开文件。参数参考man 2 open。
     * @param path
     * @param mode read、write
     * @param create 创建与否，与truncate组合与否
     * @param access 文件的访问权限
     * @return 成功返回fd，失败返回-1
     */
    static inline int Open(const string &path, int mode, int create, int access) {
        return open(path.c_str(), mode|create, access);
    }
};
}
}

#endif //CCRAFT_FILE_UTILS_H
