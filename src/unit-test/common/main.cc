/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <string>

#include <gtest/gtest.h>
#include <mutex>

#include "../../common/hash-algorithms.h"
#include "../../common/common-def.h"
#include "../../common/common-utils.h"
#include "../../common/rf-server-configuration.h"
#include "../../common/global-vars.h"

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
