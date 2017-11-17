/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <string>

#include <gtest/gtest.h>
#include <mutex>

#include "../../ccsys/cctime.h"
#include "../../common/hash-algorithms.h"
#include "../../common/common-def.h"
#include "../../common/common-utils.h"
#include "../../common/rf-server-configuration.h"
#include "../../common/thread-pool.h"

using namespace ccraft::ccsys;
using namespace ccraft::common;

const std::string CommonTestBinName = "common_test";

std::string g_sConfTestFileRootPath;

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    ccraft::common::initialize();

    std::string exePath = argv[0];
    auto projectStartPos = exePath.find(CommonTestBinName);
    auto projectRootPath = exePath.substr(0, projectStartPos);
    g_sConfTestFileRootPath = projectRootPath + "../../../src/unit-test/common/";


    return RUN_ALL_TESTS();
}

TEST(CommonTest, HashTest) {
    std::string key = "hashtest";
    uint32_t hashcode1;

    MurmurHash3_x86_32(key.c_str(), (int) (key.length()), 22, &hashcode1);
    EXPECT_GT(hashcode1, uint32_t(0));
    uint32_t hashcode2;
    MurmurHash3_x86_32(key.c_str(), (int) (key.length()), 22, &hashcode2);
    EXPECT_EQ(hashcode1, hashcode2);
}

TEST(CommonTest, ClockTest) {
    auto curTime = cctime::GetCurrentTime();
    EXPECT_NE(curTime.sec, -1);
}

TEST(CommonTest, CommonUtilsTest) {
    std::string localIp = "127.0.0.1";
    std::string host = "localhost";
    std::string ip;
    ccraft::common::CommonUtils::GetAddrInfo(host, ip);
    EXPECT_STREQ(localIp.c_str(), ip.c_str());
}

TEST(CommonTest, ConfigurationTest) {
    std::string TestFileOK = "test-servers-ok";
    std::string TestFileErr1 = "test-servers-err1";
    std::string TestFileErr2 = "test-servers-err2";
    std::string TestFileErr3 = "test-servers-err3";
    std::string TestFileErr4 = "test-servers-err4";
    ccraft::common::RfServerConfiguration rfc;
    auto path = g_sConfTestFileRootPath + TestFileOK;
    EXPECT_EQ(true, rfc.Initialize(1, path));
    path = g_sConfTestFileRootPath + TestFileErr1;
    EXPECT_EQ(false, rfc.Initialize(1, path));
    path = g_sConfTestFileRootPath + TestFileErr2;
    EXPECT_EQ(false, rfc.Initialize(1, path));
    path = g_sConfTestFileRootPath + TestFileErr3;
    EXPECT_EQ(false, rfc.Initialize(1, path));
    path = g_sConfTestFileRootPath + TestFileErr4;
    EXPECT_EQ(false, rfc.Initialize(1, path));
}

TEST(CommonTest, MemPoolTest) {
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

TEST(CommonTest, ThreadPoolTest) {
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
