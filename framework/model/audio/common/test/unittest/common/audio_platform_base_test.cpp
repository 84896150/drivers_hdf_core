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
class AudioPlatformBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioPlatformBaseTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioPlatformBaseTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioPlatformBaseTest::SetUp()
{
}

void AudioPlatformBaseTest::TearDown()
{
}

HWTEST_F(AudioPlatformBaseTest, PlatformDataFromCardTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDATAFROMCARD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioBytesToFramesTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTBYTESTOFRAMES, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioDataBigEndianChangeTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDATABIGENDIANCHANGE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioFramatToBitWidthTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTFRAMATTOBITWIDTH, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioSetPcmInfoTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTSETPCMINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioSetRenderBufInfoTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTSETRENDERBUFINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioSetCaptureBufInfoTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTSETCAPTUREBUFINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPcmWriteTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTPCMWRITE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPcmReadTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTPCMREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPcmMmapWriteTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTPCMMMAPWRITE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPcmMmapReadTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTPCMMMAPREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioRenderOpenTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTRENDEROPEN, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioCaptureOpenTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCAPTUREOPEN, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioRenderCloseTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTRENDERCLOSE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPcmPointerTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTPCMPOINTER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioCaptureCloseTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCAPTURECLOSE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioHwParamsTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTHWPARAMS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioRenderPrepareTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTRENDERPREPARE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioCapturePrepareTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCAPTUREPREPARE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioRenderTriggerTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTRENDERTRIGGER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioCaptureTriggerTest_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCAPTURETRIGGER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
