/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "camera_event.h"
#include <utils/hdf_log.h>
#include <osal/osal_mem.h>
#include "camera_device_manager.h"

#define HDF_LOG_TAG HDF_CAMERA_CORE

static struct HdfDeviceObject *g_hdfDevice = NULL;

int32_t HdfCameraAddDevice(struct HdfDeviceObject *device)
{
    if (g_hdfDevice != NULL) {
        HDF_LOGE("%s: already inited!", __func__);
        return HDF_FAILURE;
    }
    g_hdfDevice = OsalMemCalloc(sizeof(struct HdfDeviceObject));
    if (g_hdfDevice == NULL) {
        HDF_LOGE("%s: OsalMemCalloc g_hdfDevice failed!", __func__);
        return HDF_FAILURE;
    }
    g_hdfDevice = device;
    return HDF_SUCCESS;
}

void HdfCameraDelDevice(void)
{
    if (g_hdfDevice == NULL) {
        HDF_LOGE("%s: already deinited!", __func__);
        return;
    }
    OsalMemFree(g_hdfDevice);
    g_hdfDevice = NULL;
    return;
}

int32_t HdfCameraSendEvent(uint32_t eventId, const struct HdfSBuf *data)
{
    int32_t ret;
    if (g_hdfDevice == NULL) {
        HDF_LOGE("%s: g_hdfDevice == NULL", __func__);
        return HDF_FAILURE;
    }

    ret = HdfDeviceSendEvent(g_hdfDevice, eventId, data);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdfDeviceSendEvent failed", __func__);
        OsalMemFree(g_hdfDevice);
        g_hdfDevice = NULL;
        return ret;
    }

    return ret;
}