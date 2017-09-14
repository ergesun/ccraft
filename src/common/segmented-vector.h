/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_SEGMENTED_VECTOR_H
#define CCRAFT_COMMON_SEGMENTED_VECTOR_H

#include <cstdint>
#include <vector>
#include "common-def.h"
#include "nocopyable.h"
#include "common-utils.h"

#define DEFAULT_SEGMENT_SIZE                       127
#define DEFAULT_SEGMENTS_PTR_CONTAINER_SIZE        500
#define DEFAULT_EXPEND_SEG_NUM                     1

namespace ccraft {
namespace common {
/**
 * Not thread-safe! 所有Api没有安全检查！
 * vector的优化实现。当前版本受到idx限制__Size_t类型宽度上限为uint32_t。
 * TODO(sunchao): 加上模板元编程实现释放功能就可以增加更多的功能，比如erase_front()。
 */
template <typename T, typename __Size_t = uint32_t>
class SegmentedVector : public common::Noncopyable {
public:
    SegmentedVector() {
        m_vSegments.reserve(DEFAULT_SEGMENTS_PTR_CONTAINER_SIZE);
        alloc_new_segments();
        init();
    }

    /**
     *
     * @param segmentSize 大小只能是2的N次幂。
     */
    explicit SegmentedVector(__Size_t segmentSize) {
        assert(common::CommonUtils::IsPowerOfTwo(segmentSize));
        m_iSegmentSize = segmentSize - 1; // 2^n - 1
        m_vSegments.reserve(DEFAULT_SEGMENTS_PTR_CONTAINER_SIZE);
        alloc_new_segments();
        init();
    }

    /**
     *
     * @param segmentSize 大小只能是2的N次幂。
     * @param reserveSegmentsCnt
     */
    SegmentedVector(__Size_t segmentSize, __Size_t reserveSegmentsCnt) {
        assert(common::CommonUtils::IsPowerOfTwo(segmentSize));
        m_iSegmentSize = segmentSize - 1; // 2^n - 1
        m_vSegments.reserve(DEFAULT_SEGMENTS_PTR_CONTAINER_SIZE);
        alloc_new_segments();
        init();
    }

    /**
     *
     * @param segmentSize 大小只能是2的N次幂。
     * @param reserveSegmentsCnt
     * @param expendSegNumber
     */
    SegmentedVector(__Size_t segmentSize, __Size_t reserveSegmentsCnt, __Size_t expendSegNumber)
        : m_iExpandSegmentNum(expendSegNumber) {
        assert(common::CommonUtils::IsPowerOfTwo(segmentSize));
        m_iSegmentSize = segmentSize - 1; // 2^n - 1
        m_vSegments.reserve(DEFAULT_SEGMENTS_PTR_CONTAINER_SIZE);
        alloc_new_segments();
        init();
    }

    ~SegmentedVector() {
        for (auto p : m_vSegments) {
            delete p;
        }

        m_vSegments.clear();
    }

    T& operator[](__Size_t index) {
        auto segIdx = index / m_iSegmentSize;
        auto inSegIdx = index & m_iSegmentSize;

        return (*(m_vSegments[segIdx]))[inSegIdx];
    }

    void push_back(T &element) {
        ++m_iSize;
        if (LIKELY(m_nextElementShouldUsedIdx.second > m_iSegmentSize)) {
            if (m_iSegmentsMaxIdx == m_nextElementShouldUsedIdx.first) {
                alloc_new_segments();
            }

            ++m_nextElementShouldUsedIdx.first;
            m_nextElementShouldUsedIdx.second = 1;
        } else {
            ++m_nextElementShouldUsedIdx.second;
        }

        m_vSegments[m_nextElementShouldUsedIdx.first]->push_back(element);
    }

    void push_back(T &&element) {
        ++m_iSize;
        if (LIKELY(m_nextElementShouldUsedIdx.second > m_iSegmentSize)) {
            if (m_iSegmentsMaxIdx == m_nextElementShouldUsedIdx.first) {
                alloc_new_segments();
            }

            ++m_nextElementShouldUsedIdx.first;
            m_nextElementShouldUsedIdx.second = 1;
        } else {
            ++m_nextElementShouldUsedIdx.second;
        }

        m_vSegments[m_nextElementShouldUsedIdx.first]->push_back(std::move(element));
    }

    /**
     * 删除index之后的所有元素，包含index。
     * @param index
     */
    void erase_tail(__Size_t index) {
        if (0 == index) {
            clear();
            return;
        }

        auto segIdx = index / m_iSegmentSize;
        auto inSegIdx = index % m_iSegmentSize;
        // 中间
        for (__Size_t i = segIdx + 1; segIdx < m_nextElementShouldUsedIdx.first; ++i) {
            m_vSegments[i]->clear();
            m_iSize -= m_iSegmentSize;
        }

        // 尾
        m_vSegments[m_nextElementShouldUsedIdx.first]->clear();
        m_iSize -= m_nextElementShouldUsedIdx.second;

        // 头
        m_vSegments[segIdx]->erase(inSegIdx, (__Size_t)(m_iSegmentSize - 1));
        m_iSize -= m_iSegmentSize - inSegIdx;

        m_nextElementShouldUsedIdx.first = index / m_iSegmentSize;
        m_nextElementShouldUsedIdx.second = index % m_iSegmentSize;
    }

    void clear() {
        init();
        for (auto p : m_vSegments) {
            p->clear();
        }
    }

    __Size_t size() const {
        return m_iSize;
    }

    bool empty() const {
        return !m_iSize;
    }

    /**
     * 函数语义类似于sqlite的vaccum。
     */
    void vacuum() {
        if (m_nextElementShouldUsedIdx.first < m_iSegmentsMaxIdx) {
            for (auto i = m_nextElementShouldUsedIdx.first + 1; i <= m_iSegmentsMaxIdx; ++i) {
                delete m_vSegments[i];
            }

            m_vSegments.erase(m_nextElementShouldUsedIdx.first + 1, __Size_t(m_iSegmentsMaxIdx));
            m_iSegmentsMaxIdx = m_nextElementShouldUsedIdx.first;
        }
    }

private:
    void alloc_new_segments() {
        for (__Size_t i = 0; i < m_iExpandSegmentNum; ++i) {
            auto pv = new std::vector<T>();
            pv->reserve(m_iSegmentSize);
            m_vSegments.push_back(pv);
        }

        m_iSegmentsMaxIdx += m_iExpandSegmentNum;
    }

    void init() {
        m_iSize = 0;
        m_nextElementShouldUsedIdx.first   = m_iSegmentsMaxIdx;
        m_nextElementShouldUsedIdx.second  = 0;
    }

private:
    __Size_t                       m_iSegmentSize      = DEFAULT_SEGMENT_SIZE;
    __Size_t                       m_iExpandSegmentNum = DEFAULT_EXPEND_SEG_NUM;
    std::vector<std::vector<T>*>   m_vSegments;
    /* +1之后为segments的个数 */
    int64_t                        m_iSegmentsMaxIdx      = -1;
    std::pair<__Size_t, __Size_t>  m_nextElementShouldUsedIdx;
    __Size_t                       m_iSize = 0;
};
}
}

#endif //CCRAFT_COMMON_SEGMENTED_VECTOR_H
