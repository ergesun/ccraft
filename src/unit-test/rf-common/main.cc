/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <string>

#include <gtest/gtest.h>

#include "../../rf-common/rf-server-configuration.h"

using namespace ccraft::rfcommon;

const std::string RfCommonTestBinName = "rfcommon_test";

std::string g_sConfTestFileRootPath;

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    std::string exePath = argv[0];
    auto projectStartPos = exePath.find(RfCommonTestBinName);
    auto projectRootPath = exePath.substr(0, projectStartPos);
    g_sConfTestFileRootPath = projectRootPath + "../../../src/unit-test/rf-common/";

    return RUN_ALL_TESTS();
}

TEST(RfCommonTest, ConfigurationTest) {
    std::string TestFileOK = "test-servers-ok";
    std::string TestFileErr1 = "test-servers-err1";
    std::string TestFileErr2 = "test-servers-err2";
    std::string TestFileErr3 = "test-servers-err3";
    std::string TestFileErr4 = "test-servers-err4";
    ccraft::rfcommon::RfServerConfiguration rfc;
    auto path = g_sConfTestFileRootPath + TestFileOK;
    EXPECT_EQ(true, rfc.Initialize(path));
    path = g_sConfTestFileRootPath + TestFileErr1;
    EXPECT_EQ(false, rfc.Initialize(path));
    path = g_sConfTestFileRootPath + TestFileErr2;
    EXPECT_EQ(false, rfc.Initialize(path));
    path = g_sConfTestFileRootPath + TestFileErr3;
    EXPECT_EQ(false, rfc.Initialize(path));
    path = g_sConfTestFileRootPath + TestFileErr4;
    EXPECT_EQ(false, rfc.Initialize(path));
}
