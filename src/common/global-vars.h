/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_GLOBAL_VARS_H
#define CCRAFT_GLOBAL_VARS_H

namespace ccraft {
namespace common {
class Timer;
class MemPool;

extern int      LOGIC_CPUS_CNT;
extern int      PHYSICAL_CPUS_CNT;
extern long     CACHELINE_SIZE;
extern long     PAGE_SIZE;
extern Timer   *g_pTimer;
extern MemPool *g_pMemPool;

/**
 * 本文件内容的初始化。
 */
void initialize();
/**
 * 本文件内容的反初始化。
 */
void uninitialize();
} // namespace gv
} // namespace ccraft

#endif //CCRAFT_GLOBAL_VARS_H
