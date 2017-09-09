/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_COMMON_SEGMENTED_VECTOR_H
#define CCRAFT_COMMON_SEGMENTED_VECTOR_H

#include <cstdint>
#include <vector>

#define DEFAULT_SEGMENT_SIZE                       100
#define DEFAULT_SEGMENTS_PTR_CONTAINER_SIZE        500
#define DEFAULT_EXPEND_SEG_NUM                     1

namespace ccraft {
    namespace common {
        /**
         * vector的优化实现。
         */
        template <typename T>
        class SegmentedVector {
        public:
            SegmentedVector() {

            }

            explicit SegmentedVector(uint32_t segmentSize) : m_iSegmentSize(segmentSize) {

            }

            SegmentedVector(uint32_t segmentSize, uint32_t reserveSegmentsCnt) : m_iSegmentSize(segmentSize) {

            }

            SegmentedVector(uint32_t segmentSize, uint32_t reserveSegmentsCnt, uint8_t expendSegNumber)
                : m_iSegmentSize(segmentSize), m_iExpandSegmentNum(expendSegNumber) {

            }

        private:
            uint32_t                            m_iSegmentSize      = DEFAULT_SEGMENT_SIZE;
            uint8_t                             m_iExpandSegmentNum = DEFAULT_EXPEND_SEG_NUM;
            std::vector<std::vector<T>*>        m_vSegments;
        };
    }
}

#endif //CCRAFT_COMMON_SEGMENTED_VECTOR_H
