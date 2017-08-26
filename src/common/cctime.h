/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_CCTIME_H
#define CCRAFT_CCTIME_H

namespace ccraft {
namespace common {
typedef struct cctime_s {
    cctime_s() : sec(0), nsec(0) {}

    cctime_s(long s, long n) : sec(s), nsec(n) {}

    explicit cctime_s(const struct timespec ts) {
        sec = ts.tv_sec;
        nsec = ts.tv_nsec;
    }

    cctime_s(const cctime_s &ut) {
        this->sec = ut.sec;
        this->nsec = ut.nsec;
    }

    cctime_s &operator=(const cctime_s &ut) {
        this->sec = ut.sec;
        this->nsec = ut.nsec;
        return *this;
    }

    long sec;
    long nsec;

    long get_total_nsecs() const {
        return sec * 1000000000 + nsec;
    }
} cctime_t;

// arithmetic operators
inline cctime_t &operator+=(cctime_t &l, const cctime_t &r) {
    l.sec += r.sec + (l.nsec + r.nsec) / 1000000000L;
    l.nsec += r.nsec;
    l.nsec %= 1000000000L;
    return l;
}

// comparators
inline bool operator>(const cctime_t &a, const cctime_t &b) {
    return (a.sec > b.sec) || (a.sec == b.sec && a.nsec > b.nsec);
}

inline bool operator<=(const cctime_t &a, const cctime_t &b) {
    return !(operator>(a, b));
}

inline bool operator<(const cctime_t &a, const cctime_t &b) {
    return (a.sec < b.sec) || (a.sec == b.sec && a.nsec < b.nsec);
}

inline bool operator>=(const cctime_t &a, const cctime_t &b) {
    return !(operator<(a, b));
}

inline bool operator==(const cctime_t &a, const cctime_t &b) {
    return a.sec == b.sec && a.nsec == b.nsec;
}

inline bool operator==(long tick, const cctime_t &b) {
    return tick == b.get_total_nsecs();
}

inline bool operator==(const cctime_t &b, long tick) {
    return tick == b.get_total_nsecs();
}

inline bool operator!=(const cctime_t &a, const cctime_t &b) {
    return a.sec != b.sec || a.nsec != b.nsec;
}
}
}

#endif //CCRAFT_CCTIME_H
