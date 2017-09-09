/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_SEGMENTED_VECTOR_H
#define CCRAFT_COMMON_SEGMENTED_VECTOR_H

#include <cstdint>
#include <vector>
#include "common-def.h"

#define DEFAULT_SEGMENT_SIZE                       100
#define DEFAULT_SEGMENTS_PTR_CONTAINER_SIZE        500
#define DEFAULT_EXPEND_SEG_NUM                     1

namespace ccraft {
    namespace common {
        /**
         * Not thread-safe!
         * vector的优化实现。
         * TODO(sunchao): 完善功能，比如增加迭代器。
         */
        template <typename T, typename __Size_t = uint32_t>
        class SegmentedVector {
        public:
            SegmentedVector() {
                m_vSegments.reserve(DEFAULT_SEGMENTS_PTR_CONTAINER_SIZE);
                alloc_new_segments();
            }

            explicit SegmentedVector(__Size_t segmentSize) : m_iSegmentSize(segmentSize) {
                m_vSegments.reserve(DEFAULT_SEGMENTS_PTR_CONTAINER_SIZE);
                alloc_new_segments();
            }

            SegmentedVector(__Size_t segmentSize, __Size_t reserveSegmentsCnt) : m_iSegmentSize(segmentSize) {
                m_vSegments.reserve(DEFAULT_SEGMENTS_PTR_CONTAINER_SIZE);
                alloc_new_segments();
            }

            SegmentedVector(__Size_t segmentSize, __Size_t reserveSegmentsCnt, __Size_t expendSegNumber)
                : m_iSegmentSize(segmentSize), m_iExpandSegmentNum(expendSegNumber) {
                m_vSegments.reserve(DEFAULT_SEGMENTS_PTR_CONTAINER_SIZE);
                alloc_new_segments();
            }

            T& operator[](__Size_t index) {
                auto segIdx = index / m_iSegmentSize;
                auto inSegIdx = index % m_iSegmentSize;

                return m_vSegments[segIdx][inSegIdx];
            }

            void push_back(T &element) {
                if (UNLIKELY(m_iSize == m_iCapacity)) {
                    alloc_new_segments();
                }

                (m_vSegments[m_iSegmentsCnt])->push_back(element);
                ++m_iSize;
            }

            void push_back(T &&element) {
                if (UNLIKELY(m_iSize == m_iCapacity)) {
                    alloc_new_segments();
                }

                (m_vSegments[m_iSegmentsCnt])->push_back(std::move(element));
                ++m_iSize;
            }

            /**
             * 删除index之后的所有元素，包含index。
             * TODO(sunchao): 完成这个函数。
             * @param index
             */
            void erase(__Size_t index) {
                auto segIdx = index / m_iSegmentSize;
                auto inSegIdx = index % m_iSegmentSize;
                auto eraseSegCnt = m_iSegmentsCnt - segIdx;
                eraseSegCnt += inSegIdx ? 0 : 1;

                m_vSegments.erase(eraseSegCnt, (__Size_t)(m_iSegmentsCnt - 1));
                if (inSegIdx) {
                    m_vSegments[segIdx].erase(inSegIdx, m_iSegmentSize - 1);
                }

                m_iSegmentsCnt -= eraseSegCnt;
            }

            __Size_t size() const {
                return m_iSize;
            }

            bool empty() const {
                return !m_iSize;
            }

        private:
            void alloc_new_segments() {
                for (auto i = 0; i < m_iExpandSegmentNum; ++i) {
                    auto pv = new std::vector<T>();
                    pv->reserve(m_iSegmentSize);
                    m_vSegments.push_back(pv);
                }

                m_iSegmentsCnt += m_iExpandSegmentNum;
                m_iCapacity += m_iSegmentSize * m_iExpandSegmentNum;
            }

        private:
            __Size_t                       m_iSegmentSize      = DEFAULT_SEGMENT_SIZE;
            __Size_t                       m_iExpandSegmentNum = DEFAULT_EXPEND_SEG_NUM;
            std::vector<std::vector<T>*>   m_vSegments;
            __Size_t                       m_iSize             = 0;
            __Size_t                       m_iCapacity         = 0;
            /* +1之后为segments的个数 */
            int64_t                        m_iSegmentsCnt      = -1;
        };
    }
}

#endif //CCRAFT_COMMON_SEGMENTED_VECTOR_H
