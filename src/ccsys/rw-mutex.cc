/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <cstdio>
#include <cstring>
#include "rw-mutex.h"

namespace ccraft {
namespace ccsys {
RwMutex::RwMutex() {
    int err;
    if (UNLIKELY(0 != (err = pthread_rwlock_init(&m_rwLockMtx, nullptr)))) {
        fprintf(stderr, strerror(err), "\n");
        exit(err);
    }
}

RwMutex::~RwMutex() {
    int err;
    if (UNLIKELY(0 != (err = pthread_rwlock_destroy(&m_rwLockMtx)))) {
        fprintf(stderr, strerror(err), "\n");
        exit(err);
    }
}
}
}