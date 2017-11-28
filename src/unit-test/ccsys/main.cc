/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <string>

#include <gtest/gtest.h>
#include <atomic>

#include "../../ccsys/cctime.h"
#include "../../ccsys/thread-pool.h"
#include "../../ccsys/mem-pool.h"
#include "../../ccsys/spin-lock.h"
#include "../../ccsys/cctime.h"
#include "../../ccsys/timer.h"
#include "../../ccsys/random.h"
#include "../../ccsys/utils.h"

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

TEST(CCSysTest, CommonUtilsTest) {
    std::string localIp = "127.0.0.1";
    std::string host = "localhost";
    std::string ip;
    ccraft::ccsys::Utils::GetAddrInfo(host, ip);
    EXPECT_STREQ(localIp.c_str(), ip.c_str());
}

TEST(CCSysTest, MemPoolTest) {
    using std::cout;
    using std::endl;
    std::vector<MemPool::MemObject*> memObjects;
    MemPool mp;
    /*******************tiny objs**********************/
    for (int i = 0; i < 10000; ++i) {
        auto needSize = rand() % 17;
        needSize = needSize ? needSize : 1;
        auto memObject = mp.Get(needSize);
        memObjects.push_back(memObject);
        auto buf = memObject->Pointer();
        auto size = memObject->Size();
        const char *str = "hello world!";
        memcpy(buf, str, strlen(str) + 1);
        cout << "buf = " << buf << ", needSize = " << needSize <<  ", get size = " << size << endl;
    }

    for (auto p : memObjects) {
        mp.Put(p);
    }
    cout << mp.DumpDebugInfo() << endl;

    /*******************small objs**********************/
    memObjects.clear();
    for (int i = 0; i < 100000; ++i) {
        auto needSize = (32 + i) % 4097;
        needSize = needSize ? needSize : 32;
        auto memObject = mp.Get(needSize);
        memObjects.push_back(memObject);
        auto buf = memObject->Pointer();
        auto size = memObject->Size();
        const char *str = "hello world!";
        memcpy(buf, str, strlen(str) + 1);
        cout << "buf = " << buf << ", needSize = " << needSize <<  ", get size = " << size << endl;
    }

    for (auto p : memObjects) {
        mp.Put(p);
    }
    cout << mp.DumpDebugInfo() << endl;

    /*******************big objs**********************/
    memObjects.clear();
    for (int i = 0; i < 1000; ++i) {
        auto needSize = (4097 + i) % (32 * 4096 + 1);
        needSize = needSize ? needSize : 4097;
        auto memObject = mp.Get(needSize);
        memObjects.push_back(memObject);
        auto buf = memObject->Pointer();
        auto size = memObject->Size();
        const char *str = "hello world!";
        memcpy(buf, str, strlen(str) + 1);
        cout << "buf = " << buf << ", needSize = " << needSize <<  ", get size = " << size << endl;
    }

    for (auto p : memObjects) {
        mp.Put(p);
    }
    cout << mp.DumpDebugInfo() << endl;

    /*******************bulk objs**********************/
    memObjects.clear();
    for (int i = 0; i < 5; ++i) {
        auto needSize = 1024 * 1024 + i * 1024 * 1024;
        auto memObject = mp.Get(needSize);
        memObjects.push_back(memObject);
        auto buf = memObject->Pointer();
        auto size = memObject->Size();
        const char *str = "hello world!";
        memcpy(buf, str, strlen(str) + 1);
        cout << "buf = " << buf << ", needSize = " << needSize <<  ", get size = " << size << endl;
    }

    for (auto p : memObjects) {
        p->Put();
    }
    cout << mp.DumpDebugInfo() << endl;
}

TEST(CCSysTest, ThreadPoolTest) {
    static std::mutex s_mtx;
    ThreadPool<void*> tp;
    for (int i = 0; i < 100; ++i) {
        ThreadPool<void*>::Task t([i](void*){
            std::unique_lock<std::mutex> l(s_mtx);
            std::cout << "index = " << i << std::endl;
        });
        tp.AddTask(t);
    }

    tp.WaitAll();
}
