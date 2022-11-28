/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <linux/slab.h>

#include "audio_core.h"
#include "audio_dma_base.h"
#include "audio_driver_log.h"
#include "audio_platform_base.h"
#include "audio_usb_dma_ops.h"
#include "audio_usb_linux.h"
#include "gpio_if.h"
#include "osal_io.h"
#include "osal_mem.h"

#define HDF_LOG_TAG HDF_AUDIO_USB

struct AudioDmaOps g_usbDmaDeviceOps = {
    .DmaBufAlloc = AudioUsbDmaBufAlloc,
    .DmaBufFree = AudioUsbDmaBufFree,
    .DmaRequestChannel = AudioUsbDmaRequestChannel,
    .DmaConfigChannel = AudioUsbDmaConfigChannel,
    .DmaPrep = AudioUsbDmaPrep,
    .DmaSubmit = AudioUsbDmaSubmit,
    .DmaPending = AudioUsbDmaPending,
    .DmaPause = AudioUsbDmaPause,
    .DmaResume = AudioUsbDmaResume,
    .DmaPointer = AudioUsbPcmPointer,
};

/* HdfDriverEntry implementations */
static int32_t PlatformDriverBind(struct HdfDeviceObject *device)
{
    struct PlatformHost *platformHost = NULL;

    if (device == NULL) {
        AUDIO_DEVICE_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    platformHost = (struct PlatformHost *)OsalMemCalloc(sizeof(*platformHost));
    if (platformHost == NULL) {
        AUDIO_DEVICE_LOG_ERR("malloc host fail!");
        return HDF_FAILURE;
    }

    platformHost->device = device;
    device->service = &platformHost->service;

    AUDIO_DEVICE_LOG_DEBUG("success!");
    return HDF_SUCCESS;
}

static int32_t PlatformGetServiceName(const struct HdfDeviceObject *device, struct PlatformData *platformData)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (device == NULL || platformData == NULL) {
        AUDIO_DEVICE_LOG_ERR("para is NULL.");
        return HDF_FAILURE;
    }

    node = device->property;
    if (node == NULL) {
        AUDIO_DEVICE_LOG_ERR("node is NULL.");
        return HDF_FAILURE;
    }

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL) {
        AUDIO_DEVICE_LOG_ERR("get drsops object instance fail!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetString(node, "serviceName", &platformData->drvPlatformName, 0);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("read serviceName fail!");
        return ret;
    }
    AUDIO_DEVICE_LOG_INFO("success!");

    return HDF_SUCCESS;
}

static int32_t PlatformDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct PlatformData *platformData;
    struct PlatformHost *platformHost;

    if (device == NULL) {
        AUDIO_DEVICE_LOG_ERR("device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    platformHost = (struct PlatformHost *)device->service;
    if (platformHost == NULL) {
        AUDIO_DEVICE_LOG_ERR("platformHost is NULL");
        return HDF_FAILURE;
    }

    platformData = (struct PlatformData *)OsalMemCalloc(sizeof(*platformData));
    if (platformData == NULL) {
        AUDIO_DEVICE_LOG_ERR("malloc PlatformData fail!");
        return HDF_FAILURE;
    }
    (void)memset_s(platformData, sizeof(struct PlatformData), 0, sizeof(struct PlatformData));

    ret = PlatformGetServiceName(device, platformData);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(platformHost);
        return ret;
    }

    platformData->PlatformInit = AudioUsbDmaDeviceInit;
    platformData->ops = &g_usbDmaDeviceOps;

    OsalMutexInit(&platformData->renderBufInfo.buffMutex);
    OsalMutexInit(&platformData->captureBufInfo.buffMutex);
    ret = AudioSocRegisterPlatform(device, platformData);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(platformHost);
        return ret;
    }

    platformHost->priv = platformData;
    AUDIO_DEVICE_LOG_INFO("success.\n");
    return HDF_SUCCESS;
}

static void PlatformDriverRelease(struct HdfDeviceObject *device)
{
    struct PlatformData *platformData;
    struct PlatformHost *platformHost;
    if (device == NULL) {
        AUDIO_DEVICE_LOG_ERR("device is NULL");
        return;
    }

    platformHost = (struct PlatformHost *)device->service;
    if (platformHost == NULL) {
        AUDIO_DEVICE_LOG_ERR("platformHost is NULL");
        return;
    }

    platformData = (struct PlatformData *)platformHost->priv;
    if (platformData != NULL) {
        AudioUsbDmaDeviceRelease(platformData);
        OsalMutexDestroy(&platformData->renderBufInfo.buffMutex);
        OsalMutexDestroy(&platformData->captureBufInfo.buffMutex);
        OsalMemFree(platformData);
    }

    OsalMemFree(platformHost);
    AUDIO_DEVICE_LOG_INFO("success.\n");
    return;
}

struct HdfDriverEntry g_platformUsbDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "AUDIO_USB_DMA",
    .Bind = PlatformDriverBind,
    .Init = PlatformDriverInit,
    .Release = PlatformDriverRelease,
};
HDF_INIT(g_platformUsbDriverEntry);
