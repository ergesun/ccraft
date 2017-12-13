/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_CCSYS_RW_MUTEX_H
#define CCRAFT_CCSYS_RW_MUTEX_H

#include <pthread.h>
#include <cstdlib>
#include <cassert>
#include <stdexcept>

#include "gcc-buildin.h"

namespace ccraft {
namespace ccsys {
class RwMutex {
public:
    RwMutex();
    ~RwMutex();

    inline void RLock() {
        int err;
        if (UNLIKELY(0 != (err = pthread_rwlock_rdlock(&m_rwLockMtx)))) {
            fprintf(stderr, strerror(err), "\n");
            exit(err);
        }
    }

    inline bool TryRLock() {
        return 0 == pthread_rwlock_tryrdlock(&m_rwLockMtx);
    }

    inline void WLock() {
        int err;
        if (UNLIKELY(0 != (err = pthread_rwlock_wrlock(&m_rwLockMtx)))) {
            fprintf(stderr, strerror(err), "\n");
            exit(err);
        }
    }

    inline bool TryWLock() {
        return 0 == pthread_rwlock_trywrlock(&m_rwLockMtx);
    }

    inline void UnLock() {
        int err;
        if (UNLIKELY(0 != (err = pthread_rwlock_unlock(&m_rwLockMtx)))) {
            fprintf(stderr, strerror(err), "\n");
            exit(err);
        }
    }

private:
    pthread_rwlock_t  m_rwLockMtx = PTHREAD_RWLOCK_INITIALIZER;
};

class ReadLock {
public:
    ReadLock(RwMutex *const m) : m_pRwMutex(m) {
        assert(m_pRwMutex);
        m_pRwMutex->RLock();
        m_bOwnLock = true;
    }

    ReadLock(RwMutex *const m, bool defer_lock) : m_pRwMutex(m) {
        assert(m_pRwMutex);
    }

    ~ReadLock() {
        if (m_bOwnLock) Unlock();
    }

    inline bool TryLock() {
        if (m_bOwnLock) {
            throw std::runtime_error("ReadLock is locked, cannot lock it again.");
        }

        return m_bOwnLock = m_pRwMutex->TryRLock();
    }

    inline void Lock() {
        if (m_bOwnLock) {
            throw std::runtime_error("ReadLock is locked, cannot lock it again.");
        }

        m_pRwMutex->RLock();
    }

    inline void Unlock() {
        if (!m_bOwnLock) {
            throw std::runtime_error("ReadLock is unlocked, cannot unlock it again.");
        }

        m_pRwMutex->UnLock();
        m_bOwnLock = false;
    }

private:
    RwMutex   *const m_pRwMutex      = nullptr;
    volatile   bool  m_bOwnLock      = false;
};

class WriteLock {
public:
    WriteLock(RwMutex *const m) : m_pRwMutex(m) {
        assert(m_pRwMutex);
        m_pRwMutex->WLock();
        m_bOwnLock = true;
    }

    WriteLock(RwMutex *const m, bool defer_lock) : m_pRwMutex(m) {
        assert(m_pRwMutex);
    }

    ~WriteLock() {
        if (m_bOwnLock) Unlock();
    }

    inline bool TryLock() {
        if (m_bOwnLock) {
            throw std::runtime_error("WriteLock is locked, cannot lock it again.");
        }

        return m_bOwnLock = m_pRwMutex->TryWLock();
    }

    inline void Lock() {
        if (m_bOwnLock) {
            throw std::runtime_error("WriteLock is locked, cannot lock it again.");
        }

        m_pRwMutex->WLock();
    }

    inline void Unlock() {
        if (!m_bOwnLock) {
            throw std::runtime_error("WriteLock is unlocked, cannot unlock it again.");
        }

        m_pRwMutex->UnLock();
        m_bOwnLock = false;
    }

private:
    RwMutex   *const m_pRwMutex      = nullptr;
    volatile   bool  m_bOwnLock      = false;
};
}
}

#endif //CCRAFT_CCSYS_RW_MUTEX_H
