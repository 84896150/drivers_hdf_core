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
class AudioDmaBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioDmaBaseTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioDmaBaseTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioDmaBaseTest::SetUp()
{
}

void AudioDmaBaseTest::TearDown()
{
}

HWTEST_F(AudioDmaBaseTest, DmaBufAlloc_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDMABUFALLOC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDmaBaseTest, DmaBufFree_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDMABUFFREE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDmaBaseTest, DmaRequestChannel_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDMAREQUESTCHANNEL, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDmaBaseTest, DmaConfigChannel_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDMACONFIGCHANNEL, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDmaBaseTest, DmaPrep_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDMAPREP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDmaBaseTest, DmaSubmit_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDMASUBMIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDmaBaseTest, DmaPending_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDMAPENDING, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDmaBaseTest, DmaPause_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDMAPAUSE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDmaBaseTest, DmaResume_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDMARESUME, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDmaBaseTest, DmaPointer_001, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDMAPOINTER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
