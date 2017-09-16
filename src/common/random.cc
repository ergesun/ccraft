/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "random.h"

namespace ccraft {
namespace common {
const int32_t Random::Range::INVALID_RANGE = -1;

int32_t Random::GetNew() {
    if (check_range()) {
        return rand() % (m_r.end - m_r.start) + m_r.start;
    } else {
        return rand();
    }
}

int32_t Random::GetNewWithSRand() {
    init_ts_seed();
    return rand();
}
}
}
