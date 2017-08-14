/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RESOURCE_POOL_H
#define CCRAFT_RESOURCE_POOL_H

#include <cstdint>
#include <list>
#include <cassert>

#include "common-def.h"
#include "spin-lock.h"

namespace ccraft {
    namespace common {
        template <typename T>
        class ResourcePool {
        public:
            ResourcePool(uint32_t maxResCnt) : m_iMaxResCnt(maxResCnt) {}
            ~ResourcePool() {
                SpinLock l(&m_sl);
                if (0 != m_iCurResCnt) {
                    while (!m_freeRes.empty()) {
                        auto resIter = m_freeRes.begin();
                        auto res = *resIter;
                        m_freeRes.erase(resIter);
                        DELETE_PTR(res);
                    }
                }
            }

            T* Get() {
                SpinLock l(&m_sl);
                if (0 == m_iCurResCnt) {
                    return new T();
                } else {
                    auto resIter = m_freeRes.begin();
                    auto res = *resIter;
                    m_freeRes.erase(resIter);

                    return res;
                }
            }

            void Put(T *res) {
                assert(res);
                SpinLock l(&m_sl);
                if (m_iCurResCnt < m_iMaxResCnt) {
                    m_freeRes.push_back(res);
                } else {
                    DELETE_PTR(res);
                }
            }

            inline uint32_t Count() {
                SpinLock l(&m_sl);
                return m_iCurResCnt;
            }

            inline bool Empty() {
                SpinLock l(&m_sl);
                return 0 == m_iCurResCnt;
            }

        private:
            uint32_t                 m_iMaxResCnt = 0;
            uint32_t                 m_iCurResCnt = 0;
            std::list<T*>            m_freeRes;
            spin_lock_t              m_sl = UNLOCKED;
        };
    }
}

#endif //CCRAFT_RESOURCE_POOL_H
