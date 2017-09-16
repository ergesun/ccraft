/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <string>

#include <gtest/gtest.h>

#include "../../common/hash-algorithms.h"
#include "../../common/common-def.h"
#include "../../common/common-utils.h"

using namespace ccraft::common;

//const std::string UnitTestProjectName = "biz_conf_parser_test";
//const std::string TestConfFileName = "eg--acc-biz.json";
//
//std::string g_sConFilePath;
//
//int main(int argc, char **argv) {
//    testing::InitGoogleTest(&argc, argv);
//    std::string exePath = argv[0];
//    auto projectStartPos = exePath.find(UnitTestProjectName);
//    auto projectRootPath = exePath.substr(0, projectStartPos);
//    g_sConFilePath = projectRootPath + "../../../conf/" + TestConfFileName;
//
//    return RUN_ALL_TESTS();
//}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    ccraft::common::initialize();

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
    auto curTime = CommonUtils::GetCurrentTime();
    EXPECT_NE(curTime.sec, -1);
}

TEST(CommonTest, SpinLockTest) {
    spin_lock_t l = UNLOCKED;
    SpinLock sl(&l, false);
    EXPECT_EQ(sl.TryLock(), true);
    SpinLock sl2(&l, false);
    EXPECT_EQ(sl2.TryLock(), false);

    sl.Unlock();
    SpinLock sl3(&l, false);
    EXPECT_EQ(sl3.TryLock(), true);
}

TEST(CommonTest, CommonUtilsTest) {
    std::string localIp = "127.0.0.1";
    std::string host = "localhost";
    std::string ip;
    ccraft::common::CommonUtils::GetAddrInfo(host, ip);
    EXPECT_STREQ(localIp.c_str(), ip.c_str());
}
