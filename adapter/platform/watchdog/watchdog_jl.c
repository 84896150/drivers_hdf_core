/*
 * Copyright (c) 2022 Jiangsu Hoperun Software Co., Ltd.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include <stdio.h>
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "watchdog_if.h"
#include "watchdog_core.h"
#include "los_typedef.h"
#include "wdt.h"

#define NINE               9
#define OHOS_WDT_1MS       1
#define OHOS_WDT_2MS       2
#define OHOS_WDT_4MS       4
#define OHOS_WDT_8MS       8
#define OHOS_WDT_16MS      16
#define OHOS_WDT_32MS      32
#define OHOS_WDT_64MS      64
#define OHOS_WDT_128MS     128
#define OHOS_WDT_256MS     256
#define OHOS_WDT_512MS     512
#define OHOS_WDT_1S        1000
#define OHOS_WDT_2S        2000
#define OHOS_WDT_4S        4000
#define OHOS_WDT_8S        8000
#define OHOS_WDT_16S       16000
#define OHOS_WDT_32S       32000
#define JL_WDT_1S          1024
#define JL_WDT_2S          2048
#define JL_WDT_4S          4096
#define JL_WDT_8S          8192
#define JL_WDT_16S         16384
#define JL_WDT_32S         32768

struct WatchdogResource {
    int32_t watchdogId;
};

struct WatchdogDevice {
    struct WatchdogResource resource;
};

static int g_watchdogStart;
static int g_watchdogTimeout;

static int32_t WatchdogDevStart(struct WatchdogCntlr *watchdogCntlr);
static int32_t WatchdogDevSetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t seconds);
static int32_t WatchdogDevGetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t *seconds);
static int32_t WatchdogDevGetStatus(struct WatchdogCntlr *watchdogCntlr, uint32_t *status);
static int32_t WatchdogDevFeed(struct WatchdogCntlr *watchdogCntlr);

static struct WatchdogMethod g_WatchdogCntlrMethod = {
    .getStatus = WatchdogDevGetStatus,
    .setTimeout = WatchdogDevSetTimeout,
    .getTimeout = WatchdogDevGetTimeout,
    .start  = WatchdogDevStart,
    .stop   = NULL,  // WatchdogDevStop
    .feed   = WatchdogDevFeed,
    .getPriv = NULL, // WatchdogDevGetPriv
    .releasePriv = NULL, // WatchdogDevReleasePriv
};

static int32_t WatchdogDevStart(struct WatchdogCntlr *watchdogCntlr)
{
    (void)watchdogCntlr;
    g_watchdogStart = 1;
    return HDF_SUCCESS;
}

static int32_t WatchdogDevSetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t seconds)
{
    HDF_LOGD("%s: seconds %u\n", __func__, seconds);
    (void)watchdogCntlr;
    if(seconds >= OHOS_WDT_1MS && seconds <= OHOS_WDT_512MS) {
        for(int i = 0; i <= NINE; i++) {
            if(seconds == pow(OHOS_WDT_2MS, i)) {
                seconds = i;
            }
        }
    } else if(seconds >= OHOS_WDT_1S && seconds <= OHOS_WDT_32S) {
        seconds = seconds/OHOS_WDT_1S + NINE;
    } else {
    }

    wdt_init(seconds);
    if (g_watchdogStart == 1) {
        wdt_enable();
    }
    return HDF_SUCCESS;
}

static int32_t WatchdogDevGetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t *seconds)
{
    if (watchdogCntlr == NULL || seconds == NULL) {
        HDF_LOGE("%s: PARAM is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    g_watchdogTimeout = wdt_get_time();

    switch (g_watchdogTimeout) {
        case OHOS_WDT_1MS:
        case OHOS_WDT_2MS:
        case OHOS_WDT_4MS:
        case OHOS_WDT_8MS:
        case OHOS_WDT_16MS:
        case OHOS_WDT_32MS:
        case OHOS_WDT_64MS:
        case OHOS_WDT_128MS:
        case OHOS_WDT_256MS : /*OHOS_WDT_1MS ~ OHOS_WDT_256MS same as OHOS_WDT_512MS*/
        case OHOS_WDT_512MS :
            *seconds = g_watchdogTimeout;
            break;
        case JL_WDT_1S:
            *seconds = OHOS_WDT_1S;
            break;
        case JL_WDT_2S :
            *seconds = OHOS_WDT_2S;
            break;
        case JL_WDT_4S:
            *seconds = OHOS_WDT_4S;
            break;
        case JL_WDT_8S:
            *seconds = OHOS_WDT_8S;
            break;
        case JL_WDT_16S:
            *seconds = OHOS_WDT_16S;
            break;
        case JL_WDT_32S:
            *seconds = OHOS_WDT_32S;
            break;
        default:
            break;
    }
    return HDF_SUCCESS;
}

static int32_t WatchdogDevGetStatus(struct WatchdogCntlr *watchdogCntlr, uint32_t *status)
{
    if (watchdogCntlr == NULL || status == NULL) {
        HDF_LOGE("%s: PARAM is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (g_watchdogStart == 1) {
        *status = WATCHDOG_START;
    } else {
        *status = WATCHDOG_STOP;
    }
    return HDF_SUCCESS;
}

static int32_t WatchdogDevFeed(struct WatchdogCntlr *watchdogCntlr)
{
    (void)watchdogCntlr;
    if (g_watchdogStart == 1) {
        wdt_clr_pending();
    }
    return HDF_SUCCESS;
}

static uint32_t GetWatchdogDeviceResource(struct WatchdogDevice *device, const struct DeviceResourceNode *resourceNode)
{
    struct WatchdogResource *resource = NULL;
    struct DeviceResourceIface *dri = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: resource or device is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is NULL\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }

    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("%s: DeviceResourceIface is invalid\r\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (dri->GetUint32(resourceNode, "watchdogId", &resource->watchdogId, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: watchdog config read watchdogId fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t AttachWatchdogDevice(struct WatchdogCntlr *watchdogCntlr, struct HdfDeviceObject *device)
{
    int32_t ret;
    struct WatchdogDevice *watchdogDevice = NULL;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: param is NULL\r\n", __func__);
        return HDF_FAILURE;
    }

    watchdogDevice = (struct WatchdogDevice *)OsalMemAlloc(sizeof(struct WatchdogDevice));
    if (watchdogDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc watchdogDevice error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = GetWatchdogDeviceResource(watchdogDevice, device->property);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(watchdogDevice);
        return HDF_FAILURE;
    }

    watchdogCntlr->priv = watchdogDevice;

    return HDF_SUCCESS;
}

/* HdfDriverEntry method definitions */
static int32_t WatchdogDriverBind(struct HdfDeviceObject *device);
static int32_t WatchdogDriverInit(struct HdfDeviceObject *device);
static void WatchdogDriverRelease(struct HdfDeviceObject *device);

static int32_t WatchdogDriverBind(struct HdfDeviceObject *device)
{
    struct WatchdogCntlr *watchdogCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: hdfDevice object is null!\r\n", __func__);
        return HDF_FAILURE;
    }

    watchdogCntlr = (struct WatchdogCntlr *)OsalMemAlloc(sizeof(struct WatchdogCntlr));
    if (watchdogCntlr == NULL) {
        HDF_LOGE("%s: OsalMemAlloc watchdogCntlr error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    device->service = &watchdogCntlr->service;
    watchdogCntlr->device = device;
    watchdogCntlr->priv = NULL;

    return HDF_SUCCESS;
}

static int32_t WatchdogDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct WatchdogCntlr *watchdogCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    watchdogCntlr = WatchdogCntlrFromDevice(device);
    if (watchdogCntlr == NULL) {
        HDF_LOGE("%s: watchdogCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = AttachWatchdogDevice(watchdogCntlr, device);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(watchdogCntlr);
        HDF_LOGE("%s: attach error\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    watchdogCntlr->ops = &g_WatchdogCntlrMethod;

    HDF_LOGD("WatchdogDriverInit success!\r\n");
    return ret;
}

static void WatchdogDriverRelease(struct HdfDeviceObject *device)
{
    struct WatchdogCntlr *watchdogCntlr = NULL;
    struct WatchdogDevice *watchdogDevice = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null\r\n", __func__);
        return;
    }

    watchdogCntlr = WatchdogCntlrFromDevice(device);
    if (watchdogCntlr == NULL || watchdogCntlr->priv == NULL) {
        HDF_LOGE("%s: watchdogCntlr is NULL\r\n", __func__);
        return;
    }

    watchdogDevice = (struct WatchdogDevice *)watchdogCntlr->priv;
    OsalMemFree(watchdogDevice);
    return;
}

/* HdfDriverEntry definitions */
static struct HdfDriverEntry g_watchdogDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "JL_HDF_PLATFORM_WATCHDOG",
    .Bind = WatchdogDriverBind,
    .Init = WatchdogDriverInit,
    .Release = WatchdogDriverRelease,
};

// Initialize HdfDriverEntry
HDF_INIT(g_watchdogDriverEntry);
