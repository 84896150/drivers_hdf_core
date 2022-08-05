/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <gtest/gtest.h>
#include "audio_common_test.h"
#include "hdf_uhdf_test.h"

using namespace testing::ext;

namespace {
class AudioCodecBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioCodecBaseTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioCodecBaseTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioCodecBaseTest::SetUp()
{
}

void AudioCodecBaseTest::TearDown()
{
}

HWTEST_F(AudioCodecBaseTest, GetServiceName_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTGETSERVICENAME, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, GetDaiName_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTGETDAINAME, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, GetConfigInfo_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTGETCONFIGINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, SetConfigInfo_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTSETCONFIGINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, SetCtlFunc_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTSETCTLFUNC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, DeviceReadReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCODECDEVREADREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, DeviceWriteReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCODECDEVWRITEREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, DaiRegI2cRead_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCODECDAIREGI2CREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, DaiRegI2cWrite_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCODECDAIREGI2CWRITE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, DeviceRegI2cRead_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCODECDEVREGI2CREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, DeviceRegI2cWrite_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCODECDEVREGI2CWRITE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, DeviceInitRegConfig_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCODECDEVINITREGCONFIG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, DaiDeviceStartupRegConfig_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCODECDAIDEVSTARTUPREGCONFIG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
