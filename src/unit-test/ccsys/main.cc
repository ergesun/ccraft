/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <string>

#include <gtest/gtest.h>
#include <atomic>

#include "../../ccsys/spin-lock.h"
#include "../../ccsys/cctime.h"
#include "../../ccsys/timer.h"
#include "../../ccsys/random.h"

using namespace ccraft::ccsys;

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(CCSysTest, ClockTest) {
    auto curTime = cctime::GetCurrentTime();
    EXPECT_NE(curTime.sec, -1);
}

TEST(CCSysTest, SpinLockTest) {
    spin_lock_t l = UNLOCKED;
    SpinLock sl(&l, false);
    EXPECT_EQ(sl.TryLock(), true);
    SpinLock sl2(&l, false);
    EXPECT_EQ(sl2.TryLock(), false);

    sl.Unlock();
    SpinLock sl3(&l, false);
    EXPECT_EQ(sl3.TryLock(), true);
}

TEST(CCSysTest, TimerTest) {
    Timer timer;
    timer.Start();
    int test_cnt = 5;
    std::atomic<int> backs_cnt{0};
    Timer::TimerCallback cb = [&backs_cnt](void *ctx) {
        std::cout << "timer callback!" << cctime::GetCurrentTime().sec << std::endl;
        backs_cnt++;
    };

    for (int i = 0; i < test_cnt; ++i) {
        Timer::Event ev(nullptr, &cb);
        /*auto eventId = */timer.SubscribeEventAfter(cctime(1 * (i + 1), 0), ev);
    }

    while (test_cnt != backs_cnt.load()) {
        sleep(3);
    }
}

TEST(CCSysTest, RandomTest) {
    Random r1(5);
    auto rs1 = r1.GetNew();
    Random r2(5);
    auto rs2 = r2.GetNew();

    EXPECT_EQ(rs1, rs2);

    Random r3(Random::Range(1, 5));
    EXPECT_LE(r3.GetNew(), 5);
    EXPECT_GE(r3.GetNew(), 1);
}
