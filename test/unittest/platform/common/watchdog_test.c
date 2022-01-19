/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "watchdog_test.h"
#include "hdf_base.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "watchdog_if.h"

#define HDF_LOG_TAG watchdog_test

static int32_t g_wdtState = 0;

struct WatchdogTestEntry {
    int cmd;
    int32_t (*func)(void);
};

static int32_t WatchdogTestGetTestConfig(struct WatchdogTestConfig *config)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    const void *buf = NULL;
    uint32_t len;

    HDF_LOGD("%s: enter", __func__);
    service = HdfIoServiceBind("WATCHDOG_TEST");
    if (service == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    reply = HdfSBufObtain(sizeof(*config) + sizeof(uint64_t));
    if (reply == NULL) {
        HDF_LOGE("%s: Failed to obtain reply", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, 0, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Remote dispatch failed", __func__);
        return ret;
    }

    if (!HdfSbufReadBuffer(reply, &buf, &len)) {
        HDF_LOGE("%s: Read buf failed", __func__);
        HdfSBufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (len != sizeof(*config)) {
        HDF_LOGE("%s: Config size:%zu, read size:%u", __func__, sizeof(*config), len);
        HdfSBufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (memcpy_s(config, sizeof(*config), buf, sizeof(*config)) != EOK) {
        HDF_LOGE("%s: Memcpy buf failed", __func__);
        HdfSBufRecycle(reply);
        return HDF_ERR_IO;
    }
    HdfSBufRecycle(reply);
    HDF_LOGD("%s: Done", __func__);
    return HDF_SUCCESS;
}

struct WatchdogTester *WatchdogTesterGet(void)
{
    if (g_wdtState == HDF_ERR_DEVICE_BUSY) {
        HDF_LOGE("%s: DEVICE IS BUSY", __func__);
        return NULL;
    }

    int32_t ret;
    static struct WatchdogTester tester;
    struct WatchdogTester *temp = NULL;
    static bool hasInit = false;

    if (hasInit) {
        return &tester;
    }
    HDF_LOGI("%s: enter", __func__);
    if (hasInit) {
        return &tester;
    }
    ret = WatchdogTestGetTestConfig(&tester.config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed:%d", __func__, ret);
        return NULL;
    }
    temp = &tester;
    ret = WatchdogOpen(tester.config.id, &temp->handle);
    if (ret != HDF_SUCCESS) {
        if (ret == HDF_ERR_DEVICE_BUSY) {
            g_wdtState = ret;
        }
        HDF_LOGE("%s: open watchdog_%d failed:%d", __func__, tester.config.id, ret);
        return NULL;
    }
    hasInit = true;
    HDF_LOGD("%s: Done", __func__);
    return &tester;
}

static int32_t TestCaseWatchdogSetGetTimeout(void)
{
    struct WatchdogTester *tester = NULL;
    tester = WatchdogTesterGet();
    if (tester == NULL) {
        if (g_wdtState == HDF_ERR_DEVICE_BUSY) {
            HDF_LOGE("%s: DEVICE IS BUSY", __func__);
            return HDF_SUCCESS;
        }
        HDF_LOGE("%s: Get tester failed!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t ret;
    uint32_t timeoutGet = 0;

    ret = WatchdogSetTimeout(tester->handle, tester->config.timeoutSet);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set timeout fail! ret:%d", __func__, ret);
        return ret;
    }
    ret = WatchdogGetTimeout(tester->handle, &timeoutGet);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get timeout fail! ret:%d", __func__, ret);
        return ret;
    }
    if (tester->config.timeoutSet != timeoutGet) {
        HDF_LOGE("%s: set:%u, but get:%u", __func__, tester->config.timeoutSet, timeoutGet);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t TestCaseWatchdogStartStop(void)
{
    struct WatchdogTester *tester = NULL;
    tester = WatchdogTesterGet();
    if (tester == NULL) {
        if (g_wdtState == HDF_ERR_DEVICE_BUSY) {
            HDF_LOGE("%s: DEVICE IS BUSY", __func__);
            return HDF_SUCCESS;
        }
        HDF_LOGE("%s: Get tester failed!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t ret;
    int32_t status;

    ret = WatchdogStart(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: satrt fail! ret:%d", __func__, ret);
        return ret;
    }

    status = WATCHDOG_STOP;
    ret = WatchdogGetStatus(tester->handle, &status);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get status fail! ret:%d", __func__, ret);
        return ret;
    }
    if (status != WATCHDOG_START) {
        HDF_LOGE("%s: status is:%d after start", __func__, status);
        return HDF_FAILURE;
    }

    ret = WatchdogStop(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: stop fail! ret:%d", __func__, ret);
        return ret;
    }
    status = WATCHDOG_START;
    ret = WatchdogGetStatus(tester->handle, &status);
    if (status != WATCHDOG_STOP) {
        HDF_LOGE("%s: status is:%d after stop", __func__, status);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t TestCaseWatchdogFeed(void)
{
    struct WatchdogTester *tester = NULL;
    tester = WatchdogTesterGet();
    if (tester == NULL) {
        if (g_wdtState == HDF_ERR_DEVICE_BUSY) {
            HDF_LOGE("%s: DEVICE IS BUSY", __func__);
            return HDF_SUCCESS;
        }
        HDF_LOGE("%s: Get tester failed!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t ret;
    uint32_t i;

    ret = WatchdogStart(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: satrt fail! ret:%d", __func__, ret);
        return ret;
    }

    for (i = 0; i < tester->config.feedTime; i++) {
        HDF_LOGE("%s: feeding watchdog %d times... ", __func__, i);
        ret = WatchdogFeed(tester->handle);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: feed dog fail! ret:%d", __func__, ret);
            return ret;
        }
        OsalSleep(1);
    }

    HDF_LOGE("%s: no reset ... feeding test OK!!!", __func__);
    return HDF_SUCCESS;
}

static int32_t TestCaseWatchdogBark(void)
{
    struct WatchdogTester *tester = NULL;
    tester = WatchdogTesterGet();
    if (tester == NULL) {
        if (g_wdtState == HDF_ERR_DEVICE_BUSY) {
            HDF_LOGE("%s: DEVICE IS BUSY", __func__);
            return HDF_SUCCESS;
        }
        HDF_LOGE("%s: Get tester failed!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

#ifdef WATCHDOG_TEST_BARK_RESET
    int32_t ret;
    int32_t i;

    ret = WatchdogStart(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: satrt fail! ret:%d", __func__, ret);
        return ret;
    }

    for (i = 0; i < tester->config.feedTime; i++) {
        HDF_LOGE("%s: watiting dog buck %d times... ", __func__, i);
        OsalSleep(1);
    }

    HDF_LOGE("%s: dog has't buck!!! ", __func__, i);
    return HDF_FAILURE;
#else
    (void)tester;
    return HDF_SUCCESS;
#endif
}

static int32_t TestCaseWatchdogReliability(void)
{
    struct WatchdogTester *tester = NULL;
    tester = WatchdogTesterGet();
    if (tester == NULL) {
        if (g_wdtState == HDF_ERR_DEVICE_BUSY) {
            HDF_LOGE("%s: DEVICE IS BUSY", __func__);
            return HDF_SUCCESS;
        }
        HDF_LOGE("%s: Get tester failed!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t status;
    uint32_t timeout;

    HDF_LOGE("%s: test dfr for WatchdogGetStatus ...", __func__);
    /* invalid device handle */
    (void)WatchdogGetStatus(NULL, &status);
    /* invalid status pointer */
    (void)WatchdogGetStatus(tester->handle, NULL);

    HDF_LOGE("%s: test dfr for WatchdogStart&Stop ...", __func__);
    /* invalid device handle */
    (void)WatchdogStart(NULL);
    /* invalid device handle */
    (void)WatchdogStop(NULL);

    HDF_LOGE("%s: test dfr for WatchdogSet&GetTimeout ...", __func__);
    /* invalid device handle */
    (void)WatchdogSetTimeout(NULL, tester->config.timeoutSet);
    /* invalid device handle */
    (void)WatchdogGetTimeout(NULL, &timeout);
    /* invalid timeout pointer */
    (void)WatchdogGetTimeout(tester->handle, NULL);

    HDF_LOGE("%s: test dfr for WatchdogFeed ...", __func__);
    /* invalid device handle */
    (void)WatchdogFeed(NULL);

    return HDF_SUCCESS;
}

static struct WatchdogTestEntry g_entry[] = {
    { WATCHDOG_TEST_SET_GET_TIMEOUT, TestCaseWatchdogSetGetTimeout},
    { WATCHDOG_TEST_START_STOP, TestCaseWatchdogStartStop},
    { WATCHDOG_TEST_FEED, TestCaseWatchdogFeed},
    { WATCHDOG_TEST_RELIABILITY, TestCaseWatchdogReliability},
    { WATCHDOG_TEST_BARK, TestCaseWatchdogBark},
};

int32_t WatchdogTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;
    
    if (cmd > WATCHDOG_TEST_MAX) {
        HDF_LOGE("%s: invalid cmd:%d", __func__, cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        goto __EXIT__;
    }

    for (i = 0; i < sizeof(g_entry) / sizeof(g_entry[0]); i++) {
        if (g_entry[i].cmd != cmd || g_entry[i].func == NULL) {
            continue;
        }
        ret = g_entry[i].func();
        break;
    }

__EXIT__:
    HDF_LOGE("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
    return ret;    
}
