/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <climits>
#include <cstring>
#include <sys/stat.h>
#include <fstream>
#include <dirent.h>

#include "file-utils.h"
#include "common-def.h"

using std::ifstream;
using std::stringstream;

namespace ccraft {
namespace common {
int FileUtils::CreateDir(const char *dir, __mode_t mode) {
    int err;
    struct stat dir_stat;
    // 检查生成文件夹
    if (0 == stat(dir, &dir_stat)) { // 如果dir文件(夹)存在
        if (!S_ISDIR(dir_stat.st_mode)) { // 如果不是文件夹
            LOGEFUN << "CreateDir " << dir << " failed because there is a file named it.";
            return -1;
        }
    } else { // 如果dir文件(夹)不存在，则创建文件夹(0777权限)
        if (0 != mkdir(dir, mode)) {
            err = errno;
            if (EEXIST == err) {
                return 0;
            } else {
                LOGEFUN << "mkdir " << dir << " failed with errmsg = " << strerror(err);
                return -1;
            }
        }
    }

    return 0;
}

int FileUtils::CreateDirPath(const char *path, __mode_t access_mode) {
    char buffer[PATH_MAX + 1];
    bzero(buffer, sizeof(buffer));
    memcpy(buffer, path, PATH_MAX);
    char *p = buffer;
    if ('/' == *p) {
        ++p;
    }

    for (; *p; ++p) {
        if ('/' == *p) {
            *p = 0;
            if (-1 == FileUtils::CreateDir(buffer, access_mode)) {
                return -1;
            }
            *p = '/';
        }
    }

    if (-1 == FileUtils::CreateDir(buffer, access_mode)) {
        return -1;
    }

    return 0;
}

int FileUtils::RemoveDirectory(const char *path) {
    int rc = CleanDirectory(path);
    if (!rc) {
        rc = rmdir(path);
        if (0 != rc) {
            rc = errno;
        }
    }

    return rc;
}

int FileUtils::CleanDirectory(const char *path) {
    size_t path_len = strlen(path);
    int rc;
    DIR *d = opendir(path);
    if (d) {
        struct dirent *p;
        rc = 0;
        while (!rc && (p = readdir(d))) {
            int r2 = -1;
            size_t len;
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
                continue;
            }

            len = path_len + strlen(p->d_name) + 2;
            char *buf = new char[len];
            struct stat statbuf;
            snprintf(buf, len, "%s/%s", path, p->d_name);
            if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode)) {
                    r2 = RemoveDirectory(buf);
                }
                else {
                    r2 = unlink(buf);
                }
            }

            DELETE_ARR_PTR(buf);
            if (0 != r2) {
                r2 = errno;
            }
            rc = r2;
        }
        closedir(d);
    } else {
        rc = errno;
    }

    return rc;
}

ssize_t FileUtils::GetFileSize(const char *path) {
    struct stat s;
    bzero(&s, sizeof(struct stat));
    if (-1 == stat(path, &s)) {
        int err = errno;
        LOGEFUN << "stat path " << path << " err with errmsg = " << strerror(err);
        return -1;
    }

    return s.st_size;
}

ssize_t FileUtils::GetFileSize(int fd) {
    if (0 >= fd) {
        return -1;
    }

    auto cur_pos = lseek(fd, 0, SEEK_CUR);
    auto size = lseek(fd, 0, SEEK_END);
    lseek(fd, cur_pos, SEEK_SET);

    return size;
}

string FileUtils::ReadAllString(const string &file_path) {
    ifstream in_fs(file_path);

    if (!in_fs) {
        return "";
    }

    stringstream ss;
    ss << in_fs.rdbuf();
    return ss.str();
}
}
}
