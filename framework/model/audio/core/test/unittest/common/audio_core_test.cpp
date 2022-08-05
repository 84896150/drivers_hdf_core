/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_common_test.h"
#include <gtest/gtest.h>
#include "hdf_uhdf_test.h"

using namespace testing::ext;
namespace {
class AudioCoreTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioCoreTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioCoreTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioCoreTest::SetUp()
{
}

void AudioCoreTest::TearDown()
{
}

HWTEST_F(AudioCoreTest, AudioDeviceReadReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIODEVICEREADREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioDeviceWriteReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIODEVICEWRITEREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioSocRegisterPlatform_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSOCREGISTERPLATFORM, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioSocRegisterDai_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSOCREGISTERDAI, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioRegisterDsp_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOREGISTERDSP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioRegisterCodec_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOREGISTERCODEC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioBindDaiLink_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOBINDDAILINK, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioUpdateCodecRegBits_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOUPDATECODECREGBITS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioUpdateDaiRegBits_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOUPDATEDAIREGBITS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioKcontrolGetCpuDai_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOKCONTROLGETCPUDAI, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioKcontrolGetCodec_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOKCONTROLGETCODEC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioAddControls_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOADDCONTROLS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioAddControl_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOADDCONTROL, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioGetCtrlOpsRReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOGETCTRLOPSRREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioGetCtrlOpsReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOGETCTRLOPSREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioSetCtrlOpsReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSETCTRLOPSREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioSetCtrlOpsRReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSETCTRLOPSRREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioDaiReadReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIODAIREADREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioDaiWriteReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIODAIWRITEREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCodecReadReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCODECREADREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCodecWriteReg_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCODECWRITEREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioInfoCtrlOps_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOINFOCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCodecGetCtrlOps_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCODECGETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCodecSetCtrlOps_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCODECSETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCpuDaiSetCtrlOps_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCPUDAISETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCpuDaiGetCtrlOps_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCPUDAIGETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
