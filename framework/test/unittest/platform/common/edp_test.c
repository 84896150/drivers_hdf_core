/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "edp_test.h"
#include "edp_if.h"
#include "hdf_base.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "osal_thread.h"
#include "osal_time.h"

#define DEFAULT_RATE                 10000
#define EDP_TEST_STACK_SIZE        (1024 * 64)
#define EDP_TEST_SLEEP_TIME        2000
#define EDP_TEST_WAIT_TIMEOUT      20
#define HDF_LOG_TAG edp_test_c

static DevHandle g_edpTesthdl = NULL;

static DevHandle EdpTesterGet(void)
{
    HDF_LOGI("EdpTesterGet: enter!");
    if (g_edpTesthdl) {
        return g_edpTesthdl;
    }
    g_edpTesthdl = EdpOpen(0);
    if (g_edpTesthdl == NULL) {
        HDF_LOGE("EdpTesterGet: open edp device:%u fail!", 0);
        return NULL;
    }
    HDF_LOGI("EdpTesterGet: done!");
    return g_edpTesthdl;
}

static int32_t EdpTestDeepColorSet(void)
{
    DevHandle edpHdl = NULL;
    int32_t ret;

    HDF_LOGI("EdpTestEnable: enter!");
    edpHdl = EdpTesterGet();
    if (edpHdl == NULL) {
        HDF_LOGE("EdpTestEnable: get edpHdl fail!");
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = EdpDeepColorSet(edpHdl, EDP_COLOR_DEPTH_10BIT);

    HDF_LOGI("EdpTestEnable: edp device num is %u!", edpHdl);
    HDF_LOGI("EdpTestEnable: done!");
    return ret;
}

static int32_t EdpTestDeepColorGet(void)
{
    DevHandle edpHdl = NULL;
    int32_t ret;
    enum EdpColorDepth color;

    HDF_LOGI("EdpTestDeepColorGet: enter!");
    edpHdl = EdpTesterGet();
    if (edpHdl == NULL) {
        HDF_LOGE("EdpTestDeepColorGet: get edpHdl fail!");
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = EdpDeepColorGet(edpHdl, &color);

    HDF_LOGI("EdpTestDeepColorGet: done: %d!", color);
    return ret;
}

static int32_t EdpTestSetVideoAttribute(void)
{
    int32_t ret = HDF_SUCCESS;
    DevHandle edpHdl = NULL;
    struct EdpVideoAttr attr = {
        .tmdsClock = 0,
        .pixelClock = 0,
        .pixelRepeat = 0,
        .colorSpace = EDP_COLOR_SPACE_YCBCR422,
        .colorDepth = EDP_COLOR_DEPTH_10BIT,
        .colorCoeff = EDP_COLOR_COEFF_YCBCR709,
        .colorimetry = EDP_COLORIMETRY_DEFAULT,
        .timing = EDP_VIDEO_TIMING_NONE,
    };

    HDF_LOGI("%s: enter!", __func__);
    edpHdl = EdpTesterGet();
    if (edpHdl == NULL) {
        HDF_LOGE("%s: get edpHdl fail!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = EdpSetVideoAttribute(edpHdl, &attr);

    HDF_LOGI("%s: done!", __func__);
    return HDF_SUCCESS;
}

static int32_t EdpTestSetAudioAttribute(void)
{
    DevHandle edpHdl = NULL;
    int32_t ret;
    struct EdpAudioAttr attr = {
        .codingType = EDP_AUDIO_CODING_TYPE_STREAM,
        .ifType = EDP_AUDIO_IF_TYPE_SPDIF,
        .bitDepth = EDP_AUDIO_BIT_DEPTH_16,
    };

    HDF_LOGI("%s: enter!", __func__);
    edpHdl = EdpTesterGet();
    if (edpHdl == NULL) {
        HDF_LOGE("%s: get edpHdl fail!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = EdpSetAudioAttribute(edpHdl, &attr);

    HDF_LOGI("%s: done!", __func__);
    return ret;
}

static int32_t EdpTestEnable(void)
{
    DevHandle edpHdl = NULL;
    int32_t ret = HDF_FAILURE;

    HDF_LOGI("%s: enter!", __func__);
    edpHdl = EdpTesterGet();
    if (edpHdl == NULL || edpHdl == NULL) {
        HDF_LOGE("%s: get edpHdl fail!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = EdpEnable(edpHdl);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s EdpEnable failed", __func__);
    }
    HDF_LOGI("%s: done!", __func__);
    return HDF_SUCCESS;
}

static int32_t EdpTestDisable(void)
{
    DevHandle edpHdl = NULL;
    int32_t ret = HDF_FAILURE;

    HDF_LOGI("%s: enter!", __func__);
    edpHdl = EdpTesterGet();
    if (edpHdl == NULL || edpHdl == NULL) {
        HDF_LOGE("%s: get edpHdl fail!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = EdpDisable(edpHdl);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s EdpEnable failed", __func__);
    }
    HDF_LOGI("%s: done!", __func__);
    OsalMSleep(EDP_TEST_SLEEP_TIME);
    return HDF_SUCCESS;
}

static void EdpTestHpdCallback(void *data, bool hpd)
{
    if (!hpd) {
        return;
    }

    HDF_LOGI("%s: eDP device hot-pluged detected with handle %p", __func__, data);
}

static int32_t EdpTestRegisterHpdCallbackFunc(void)
{
    uint32_t rate;
    HDF_LOGI("%s: enter!", __func__);
    DevHandle edpHdl = NULL;
    int32_t ret;

    edpHdl = EdpTesterGet();
    if (edpHdl == NULL) {
        HDF_LOGE("%s: get edpHdl fail!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    struct EdpHpdCallbackInfo cbInfo = {
        .data = edpHdl,
        .callbackFunc = &EdpTestHpdCallback,
    };
    ret = EdpRegisterHpdCallbackFunc(edpHdl, &cbInfo);
    HDF_LOGI("%s: done!", __func__);
    return HDF_SUCCESS;
}

static int32_t EdpTestUnRegisterHpdCallbackFunc(void)
{
    DevHandle edpHdl = NULL;
    int32_t ret = HDF_FAILURE;

    HDF_LOGI("%s: enter!", __func__);
    edpHdl = EdpTesterGet();
    if (edpHdl == NULL || edpHdl == NULL) {
        HDF_LOGE("%s: get edpHdl fail!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = EdpUnregisterHpdCallbackFunc(edpHdl);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s get edp failed", __func__);
    }
    HDF_LOGI("%s: done!", __func__);
    return HDF_SUCCESS;
}

static int32_t EdpTestGetConnector(void)
{
    DevHandle edpHdl = NULL;
    DevHandle connector = NULL;
    int32_t ret = HDF_FAILURE;

    HDF_LOGI("%s: enter!", __func__);
    edpHdl = EdpTesterGet();
    if (edpHdl == NULL || edpHdl == NULL) {
        HDF_LOGE("%s: get edpHdl fail!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = EdpGetConnector(edpHdl, &connector);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s get edp failed", __func__);
    }
    HDF_LOGI("%s: done!", __func__);
    return HDF_SUCCESS;
}

struct EdpTestEntry {
    int cmd;
    int32_t (*func)(void);
    const char *name;
};

static struct EdpTestEntry g_entry[] = {
    {EDP_TEST_CMD_DEEP_COLOR_SET, EdpTestDeepColorSet, "EdpTestDeepColorSet"},
    {EDP_TEST_CMD_DEEP_COLOR_GET, EdpTestDeepColorGet, "EdpTestDeepColorGet"},
    {EDP_TEST_CMD_SET_VIDEO_ATTR, EdpTestSetVideoAttribute, "EdpTestSetVideoAttribute"},
    {EDP_TEST_CMD_SET_AUDIO_ATTR, EdpTestSetAudioAttribute, "EdpTestSetAudioAttribute"},
    {EDP_TEST_CMD_DISABLE, EdpTestDisable, "EdpTestDisable"},
    {EDP_TEST_CMD_ENABLE, EdpTestEnable, "EdpTestEnable"},
    {EDP_TEST_CMD_REGISTER_HPD_CB, EdpTestRegisterHpdCallbackFunc, "EdpTestRegisterHpdCallbackFunc"},
    {EDP_TEST_CMD_UNREGISTER_HPD_CB, EdpTestUnRegisterHpdCallbackFunc, "EdpTestUnRegisterHpdCallbackFunc"},
    {EDP_TEST_CMD_GET_CONNECTER, EdpTestGetConnector, "EdpTestGetConnector"},
};

int32_t EdpTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;

    if (cmd > EDP_TEST_CMD_MAX) {
        HDF_LOGE("EdpTestExecute: invalid cmd:%d!", cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        HDF_LOGE("[EdpTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
        return ret;
    }

    for (i = 0; i < sizeof(g_entry) / sizeof(g_entry[0]); i++) {
        if (g_entry[i].cmd != cmd || g_entry[i].func == NULL) {
            continue;
        }
        ret = g_entry[i].func();
        break;
    }

    HDF_LOGE("[EdpTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
    return ret;
}

static int32_t EdpTestDoTest(struct EdpTest *test, int32_t cmd)
{
    (void)test;
    int32_t ret = HDF_FAILURE;

    ret = EdpTestExecute(cmd);
    return ret;
}

static int32_t EdpTestBind(struct HdfDeviceObject *device)
{
    static struct EdpTest test;

    test.total = EDP_TEST_CMD_MAX;
    test.doTest = &EdpTestDoTest;
    test.fails = 0;
    test.handle = EdpTesterGet();
    test.devNo = 0;
    test.device = device;
    device->service = &test.service;
    return HDF_SUCCESS;
}

static int32_t EdpTestInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_edpTestEntry = {
    .moduleVersion = 1,
    .Bind = EdpTestBind,
    .Init = EdpTestInit,
    .moduleName = "PLATFORM_EDP_TEST",
};
HDF_INIT(g_edpTestEntry);