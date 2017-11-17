/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <unistd.h>

#include "../common/common-def.h"

#include "io-utils.h"

namespace ccraft {
namespace fsio {
ssize_t IOUtils::WriteFully(int fd, const char *buf, size_t size) {
    if (-1 == fd || !buf) {
        return -1;
    }

    if (0 == size) {
        return 0;
    }

    size_t left_size = size;
    ssize_t n;
    while (left_size) {
        n = write(fd, buf + size - left_size, left_size);
        if (-1 == n) {
            return -1;
        }

        left_size -= n;
    }

    return size;
}

ssize_t IOUtils::ReadFully(int fd, char **buf, size_t size) {
    if (-1 == fd || 0 == size) {
        return 0;
    }

    *buf = new char[size];
    bzero(*buf, size);
    size_t left_size = size;
    ssize_t n;
    while (left_size) {
        n = read(fd, *buf + size - left_size, left_size);
        if (-1 == n) {
            DELETE_ARR_PTR(*buf);
            return -1;
        }

        if (0 == n) {
            return size - left_size;
        }

        left_size -= n;
    }

    return size;
}

ssize_t IOUtils::ReadFully_V2(int fd, char **buf, size_t size) {
    if (-1 == fd || 0 == size) {
        return 0;
    }

    *buf = new char[size];
    bzero(*buf, size);
    size_t total = size, nread = 0;
    ssize_t n;

    while (0 < (n = pread(fd, *buf + nread, total - nread, nread))) {
        nread += n;
    }

    if (-1 == n) {
        DELETE_ARR_PTR(*buf);

        return -1;
    }

    return nread;
}
} // namespace fsio
} // namespace ccraft
