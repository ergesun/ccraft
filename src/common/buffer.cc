/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../ccsys/mem-pool.h"

#include "buffer.h"

namespace ccraft {
namespace common {
void Buffer::Refresh(uchar *pos, uchar *last, uchar *start, uchar *end, ccsys::MemPoolObject *mpo) {
    if (m_pMpObject) {
        m_pMpObject->Put();
    } else {
        DELETE_ARR_PTR(m_pStart);
    }

    m_pMpObject = mpo;
    m_pPos = pos;
    m_pLast = last;
    m_pStart = start;
    m_pEnd = end;
    check_available();
}
} // namespace common
} // namespace ccraft
