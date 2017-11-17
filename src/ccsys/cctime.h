/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_CCSYS_CCTIME_H
#define CCRAFT_CCSYS_CCTIME_H

namespace ccraft {
namespace ccsys {
class cctime {
public:
    cctime() : sec(0), nsec(0) {}

    cctime(long s, long n) : sec(s), nsec(n) {}

    explicit cctime(const struct timespec ts) {
        sec = ts.tv_sec;
        nsec = ts.tv_nsec;
    }

    cctime(const cctime &ut) {
        this->sec = ut.sec;
        this->nsec = ut.nsec;
    }

    cctime &operator=(const cctime &ut) {
        this->sec = ut.sec;
        this->nsec = ut.nsec;
        return *this;
    }

    long sec;
    long nsec;

    long get_total_nsecs() const {
        return sec * 1000000000 + nsec;
    }

    /**
     * 获取当前系统时间(unix epoch到现在的秒+纳秒数)。
     * @return
     */
    static cctime GetCurrentTime() {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        return cctime(ts);
    }
};

// arithmetic operators
inline cctime &operator+=(cctime &l, const cctime &r) {
    l.sec += r.sec + (l.nsec + r.nsec) / 1000000000L;
    l.nsec += r.nsec;
    l.nsec %= 1000000000L;
    return l;
}

// comparators
inline bool operator>(const cctime &a, const cctime &b) {
    return (a.sec > b.sec) || (a.sec == b.sec && a.nsec > b.nsec);
}

inline bool operator<=(const cctime &a, const cctime &b) {
    return !(operator>(a, b));
}

inline bool operator<(const cctime &a, const cctime &b) {
    return (a.sec < b.sec) || (a.sec == b.sec && a.nsec < b.nsec);
}

inline bool operator>=(const cctime &a, const cctime &b) {
    return !(operator<(a, b));
}

inline bool operator==(const cctime &a, const cctime &b) {
    return a.sec == b.sec && a.nsec == b.nsec;
}

inline bool operator==(long tick, const cctime &b) {
    return tick == b.get_total_nsecs();
}

inline bool operator==(const cctime &b, long tick) {
    return tick == b.get_total_nsecs();
}

inline bool operator!=(const cctime &a, const cctime &b) {
    return a.sec != b.sec || a.nsec != b.nsec;
}
}
}

#endif //CCRAFT_CCSYS_CCTIME_H
