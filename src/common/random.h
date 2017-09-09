/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_RANDOM_H
#define CCRAFT_COMMON_RANDOM_H

#include <sys/time.h>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

#include "errors.h"

namespace ccraft {
namespace common {
class Random {
public:
    struct Range {
        Range() {
            start = end = INVALID_RANGE;
        }
        Range(int32_t s, int32_t e) throw(std::invalid_argument) : start(s), end(e) {
            if (s > e || s < 0 || e < 0) {
                THROW_PARAM_OUT_RANGE_ERR();
            }
        }

    private:
        friend class Random;
        int32_t start, end;
        static const int32_t INVALID_RANGE;
    };

public:
    Random() {
        init_default_seed();
    }

    Random(uint32_t seed) : m_iSeed(seed) {
        srand(seed);
        m_r.start = m_r.end = Range::INVALID_RANGE;
    }
    Random(Range r) : m_r(r) {
        init_default_seed();
    }
    Random(uint32_t seed, Range r) : m_iSeed(seed), m_r(r) {
        srand(seed);
    }

    int32_t GetNew();

private:
    inline void init_default_seed() {
        timeval tv;
        gettimeofday(&tv, nullptr);
        srand(static_cast<uint32_t >(tv.tv_sec * 1000 + tv.tv_usec / 1000));
    }

    inline bool check_range() {
        return !(Range::INVALID_RANGE == m_r.start || Range::INVALID_RANGE == m_r.end);
    }

private:
    uint32_t m_iSeed;
    Range m_r;
};
}
}

#endif //CCRAFT_COMMON_RANDOM_H
