/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_CCSYS_TIMER_H
#define CCRAFT_CCSYS_TIMER_H

#include <functional>
#include <map>
#include <condition_variable>
#include <thread>

#include "cctime.h"

#include "spin-lock.h"

namespace ccraft {
namespace ccsys {
/**
 * 一个拥有一个检测、触发线程的操作安全的定时器。
 * 使用此定时器回调函数一定要是瞬时的。
 * TODO(sunchao)：添加一个线程池、一个到期任务队列用来执行到期的作业？还是觉得这项任务交给用户比较合适。
 */
class Timer {
public:
    typedef std::function<void(void *)> TimerCallback, *TimerCallbackPointer;
    // TODO(sunchao): 优化EventId。
    struct EventId {
        EventId() = default;
        EventId(cctime w, TimerCallbackPointer h) : when(w), how(h) {}
        cctime             when;
        TimerCallbackPointer how = nullptr;

        bool operator<(const EventId &another) const {
            return (this->when < another.when) || (this->how < another.how);
        }
    };

    struct Event {
        /**
         * 事件的一个上下文，可作用户传递数据使用。
         */
        void *ctx;
        /**
         * 每个不同的事件订阅一定要不同的callback(即新的function的地址)用以作为唯一区分。
         */
        TimerCallbackPointer callback;

        Event() : ctx(nullptr), callback(nullptr) {}

        Event(void *c, TimerCallbackPointer cb) : ctx(c), callback(cb) {}

        Event(const Event &ev) {
            this->ctx = ev.ctx;
            this->callback = ev.callback;
        }

        Event &operator=(const Event &ev) {
            this->ctx = ev.ctx;
            this->callback = ev.callback;
            return *this;
        }
    };

    typedef std::multimap<cctime, Event> TimerEvents;
    typedef std::map<EventId, TimerEvents::iterator> EventsTable;

    Timer() = default;

    ~Timer();

    /**
     * 启动timer。
     */
    void Start();

    /**
     * 停止timer。
     */
    void Stop();

    /**
     * 订阅事件：在指定的时间点触发。同一时间点同一handler不可以重复订阅。
     * @param when 从epoch到触发的时间。
     * @return 返回订阅事件的id，可用于取消。如果id的when为0,则表示订阅失败。
     */
    EventId SubscribeEventAt(cctime when, Event &ev);

    /**
     * 订阅事件：从现在开始指定的时间后触发。同一时间点同一handler不可以重复订阅。
     * @param duration 等待触发的时间。
     * @return 返回订阅事件的id，可用于取消。如果id的when为0,则表示订阅失败。
     */
    EventId SubscribeEventAfter(cctime duration, Event &ev);

    /**
     * 取消指定事件的订阅。
     * @param eventId 取消的事件的唯一id。
     */
    bool UnsubscribeEvent(EventId eventId);

    /**
     * 取消所有事件的订阅。
     */
    void UnsubscribeAllEvent();

private:
    /**
     * 事件处理线程。
     */
    void process();

private:
    bool m_stop = true;
    TimerEvents m_mapSubscribedEvents;
    EventsTable m_mapEventsEntry;
    std::mutex m_evs_mtx;
    std::condition_variable m_cv;
    std::thread *m_pWorkThread = nullptr;
    spin_lock_t m_thread_safe_sl = UNLOCKED;
}; // class Timer
}  // namespace ccsys
}  // namespace ccraft
#endif //CCRAFT_CCSYS_TIMER_H
