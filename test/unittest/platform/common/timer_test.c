/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "timer_test.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "osal_thread.h"
#include "osal_time.h"
#include "timer_if.h"

#define HDF_LOG_TAG timer_test_c

#ifdef HDF_LOGD
#undef HDF_LOGD
#define HDF_LOGD(fmt, args...) dprintf("[HDF]-[TIMER]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##args)
#endif

#ifdef HDF_LOGI
#undef HDF_LOGI
#define HDF_LOGI(fmt, args...) dprintf("[HDF]-[TIMER]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##args)
#endif

#ifdef HDF_LOGE
#undef HDF_LOGE
#define HDF_LOGE(fmt, args...) dprintf("[HDF]-[TIMER]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##args)
#endif

static bool g_theard1Flag;
static bool g_theard2Flag;

struct TimerTestFunc {
    enum TimerTestCmd type;
    int32_t (*Func)(struct TimerTest *test);
};

int32_t TimerTestcaseCb(void)
{
    static uint16_t num = 0;
    num++;
    HDF_LOGD("->>>>>>>>>>>%s:[%d]", __func__, num);
    if (num == 4) {
        HDF_LOGD("->>>>>>>>>>>%s:num exceed max", __func__);
        g_theard2Flag = true;
    }
    return HDF_SUCCESS;
}

int32_t TimerTestcaseOnceCb(void)
{
    HDF_LOGD("->>>>>>>>>>>%s:", __func__);
    g_theard1Flag = true;
    return HDF_SUCCESS;
}

static int32_t TimerSetTest(struct TimerTest *test)
{
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    TimerSet(test->handle, test->uSecond, TimerTestcaseCb);
    return HDF_SUCCESS;
}

static int32_t TimerSetOnceTest(struct TimerTest *test)
{
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    TimerSetOnce(test->handle, test->uSecond, TimerTestcaseOnceCb);
    return HDF_SUCCESS;
}

static int32_t TimerGetTest(struct TimerTest *test)
{
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    uint32_t uSecond;
    bool isPeriod;

    TimerGet(test->handle, &uSecond, &isPeriod);
    HDF_LOGD("%s:[%d][%d]", __func__, uSecond, isPeriod);
    return HDF_SUCCESS;
}

static int32_t TimerStartTest(struct TimerTest *test)
{
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    TimerStart(test->handle);
    return HDF_SUCCESS;
}

static int32_t TimerStopTest(struct TimerTest *test)
{
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    TimerStop(test->handle);
    return HDF_SUCCESS;
}

static int TimerOnceTestThreadFunc(void *param)
{
    DevHandle handle = (DevHandle)param;
    if (handle == NULL) {
        HDF_LOGE("%s: timer test get handle fail", __func__);
        g_theard1Flag = true;
        return HDF_FAILURE;
    }

    if(TimerSetOnce(handle, TIMER_TEST_TIME_USECONDS, TimerTestcaseOnceCb) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerSetOnce fail", __func__);
        g_theard1Flag = true;
        return HDF_FAILURE;
    }
    if (TimerStart(handle) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerStart fail", __func__);
        g_theard1Flag = true;
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int TimerPeriodTestThreadFunc(void *param)
{
    DevHandle handle = (DevHandle)param;
    if (handle == NULL) {
        HDF_LOGE("%s: timer test get handle fail", __func__);
        g_theard2Flag = true;
        return HDF_FAILURE;
    }

    if(TimerSet(handle, TIMER_TEST_TIME_USECONDS, TimerTestcaseCb) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerSet fail", __func__);
        g_theard2Flag = true;
        return HDF_FAILURE;
    }
    if (TimerStart(handle) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerStart fail", __func__);
        g_theard2Flag = true;
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t TimerTestMultiThread(struct TimerTest *test)
{
    int32_t ret;
    uint32_t time = 0;
    struct OsalThread thread1, thread2;
    struct OsalThreadParam cfg1, cfg2;
    if (test == NULL) {
        HDF_LOGE("%s: timer test NULL", __func__);
        return HDF_FAILURE;
    }

    DevHandle handle1 = TimerOpen(TIMER_TEST_TIME_ID_THREAD1);
    if (handle1 == NULL) {
        HDF_LOGE("%s: timer test get handle1 fail", __func__);
        return HDF_FAILURE;
    }
    DevHandle handle2 = TimerOpen(TIMER_TEST_TIME_ID_THREAD2);
    if (handle1 == NULL) {
        TimerClose(handle1);
        HDF_LOGE("%s: timer test get handle2 fail", __func__);
        return HDF_FAILURE;
    }

    ret = OsalThreadCreate(&thread1, (OsalThreadEntry)TimerOnceTestThreadFunc, (void *)handle1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("create test once timer fail:%d", ret);
        TimerClose(handle1);
        TimerClose(handle2);
        return HDF_FAILURE;
    }

    ret = OsalThreadCreate(&thread2, (OsalThreadEntry)TimerPeriodTestThreadFunc, (void *)handle2);
    if (ret != HDF_SUCCESS) {
        TimerClose(handle1);
        TimerClose(handle2);
        (void)OsalThreadDestroy(&thread1);
        HDF_LOGE("create test period timer fail:%d", ret);
        return HDF_FAILURE;
    }

    cfg1.name = "TimerTestThread-once";
    cfg2.name = "TimerTestThread-period";
    cfg1.priority = cfg2.priority = OSAL_THREAD_PRI_DEFAULT;
    cfg1.stackSize = cfg2.stackSize = TIMER_TEST_STACK_SIZE;

    ret = OsalThreadStart(&thread1, &cfg1);
    if (ret != HDF_SUCCESS) {
        (void)OsalThreadDestroy(&thread1);
        (void)OsalThreadDestroy(&thread2);
        TimerClose(handle1);
        TimerClose(handle2);
        HDF_LOGE("start test thread1 fail:%d", ret);
        return HDF_FAILURE;
    }

    ret = OsalThreadStart(&thread2, &cfg2);
    if (ret != HDF_SUCCESS) {
        TimerClose(handle1);
        TimerClose(handle2);
        (void)OsalThreadDestroy(&thread1);
        (void)OsalThreadDestroy(&thread2);
        HDF_LOGE("start test thread2 fail:%d", ret);
        return HDF_FAILURE;
    }

    while (g_theard1Flag == false || g_theard2Flag == false) {
        HDF_LOGE("[%d]waitting testing timer thread finish...", time);
        OsalSleep(TIMER_TEST_WAIT_TIMEOUT);
        time++;
        if (time > TIMER_TEST_WAIT_TIMES) {
            break;
        }
    }

    (void)OsalThreadDestroy(&thread1);
    (void)OsalThreadDestroy(&thread2);
    TimerClose(handle1);
    TimerClose(handle2);
    return HDF_SUCCESS;
}

int32_t TimerTestReliability(struct TimerTest *test)
{
    HDF_LOGD("TimerTestReliability: test for timer ...");
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    TimerSet(test->handle, test->uSecond, NULL);
    TimerSetOnce(test->handle, test->uSecond, NULL);
    TimerStart(NULL);
    return HDF_SUCCESS;
}


static struct TimerTestFunc g_timerTestFunc[] = {
    {TIMER_TEST_SET, TimerSetTest},
    {TIMER_TEST_SETONCE, TimerSetOnceTest},
    {TIMER_TEST_GET, TimerGetTest},
    {TIMER_TEST_START, TimerStartTest},
    {TIMER_TEST_STOP, TimerStopTest},
    {TIMER_RELIABILITY_TEST, TimerTestMultiThread},
    {TIMER_MULTI_THREAD_TEST, TimerTestReliability},
};

static int32_t TimerTestEntry(struct TimerTest *test, int32_t cmd)
{
    int32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;

    HDF_LOGD("test-- -- -- -- -- -->%s: enter cmd %d", __func__, cmd);

    if (test == NULL) {
        HDF_LOGE("%s: test null cmd %d", __func__, cmd);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cmd != TIMER_MULTI_THREAD_TEST) {
         test->handle = TimerOpen(test->id);
        if (test->handle == NULL) {
            HDF_LOGE("%s: timer test get handle fail", __func__);
            return HDF_FAILURE;
        }
    }

    for (i = 0; i < sizeof(g_timerTestFunc) / sizeof(g_timerTestFunc[0]); i++) {
        if (cmd == g_timerTestFunc[i].type && g_timerTestFunc[i].Func != NULL) {
            ret = g_timerTestFunc[i].Func(test);
            HDF_LOGD("%s: cmd %d ret %d", __func__, cmd, ret);
            break;
        }
    }

    if (cmd != TIMER_MULTI_THREAD_TEST) {
        TimerClose(test->handle);
    }

    return ret;
}

static int32_t TimerTestBind(struct HdfDeviceObject *device)
{
    static struct TimerTest test;
    HDF_LOGD("%s: enter", __func__);

    if (device != NULL) {
        device->service = &test.service;
    } else {
        HDF_LOGE("%s: device is NULL", __func__);
    }

    HDF_LOGD("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t TimerTestInitFromHcs(struct TimerTest *test, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *face = NULL;

    face = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (face == NULL) {
        HDF_LOGE("%s: face is null", __func__);
        return HDF_FAILURE;
    }
    if (face->GetUint32 == NULL) {
        HDF_LOGE("%s: GetUint32 not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = face->GetUint32(node, "id", &test->id, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read id fail!", __func__);
        return HDF_FAILURE;
    }

    ret = face->GetUint32(node, "useconds", &test->uSecond, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read useconds fail!", __func__);
        return HDF_FAILURE;
    }

    ret = face->GetUint32(node, "isPeriod", &test->isPeriod, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read isPeriod fail!", __func__);
        return HDF_FAILURE;
    }

    HDF_LOGD("timer test init:id[%d][%d][%d]", test->id, test->uSecond, test->isPeriod);
    return HDF_SUCCESS;
}

static int32_t TimerTestInit(struct HdfDeviceObject *device)
{
    struct TimerTest *test = NULL;

    HDF_LOGD("%s: in", __func__);
    if (device == NULL || device->service == NULL || device->property == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    test = (struct TimerTest *)device->service;

    if (TimerTestInitFromHcs(test, device->property) != HDF_SUCCESS) {
        HDF_LOGE("%s: RegulatorTestInitFromHcs failed", __func__);
        return HDF_FAILURE;
    }

    test->TestEntry = TimerTestEntry;
    g_theard1Flag = false;
    g_theard2Flag = false;
    HDF_LOGD("%s: success", __func__);

    return HDF_SUCCESS;
}

static void TimerTestRelease(struct HdfDeviceObject *device)
{
    (void)device;
}

struct HdfDriverEntry g_timerTestEntry = {
    .moduleVersion = 1,
    .Bind = TimerTestBind,
    .Init = TimerTestInit,
    .Release = TimerTestRelease,
    .moduleName = "PLATFORM_TIMER_TEST",
};
HDF_INIT(g_timerTestEntry);
