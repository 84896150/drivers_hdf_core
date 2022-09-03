/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_camera_event.h"
#include "hdf_camera_device_manager.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG HDF_CAMERA_CORE

static struct HdfDeviceObject *g_hdfCameraDevice = NULL;

int32_t HdfCameraAddDevice(struct HdfDeviceObject *device)
{
    if (g_hdfCameraDevice != NULL) {
        HDF_LOGE("%s: already inited!", __func__);
        return HDF_FAILURE;
    }
    g_hdfCameraDevice = OsalMemCalloc(sizeof(struct HdfDeviceObject));
    if (g_hdfCameraDevice == NULL) {
        HDF_LOGE("%s: OsalMemCalloc g_hdfCameraDevice failed!", __func__);
        return HDF_FAILURE;
    }
    g_hdfCameraDevice = device;
    return HDF_SUCCESS;
}

int32_t HdfCameraSendEvent(uint32_t eventId, const struct HdfSBuf *data)
{
    int32_t ret;
    if (g_hdfCameraDevice == NULL) {
        HDF_LOGE("%s: g_hdfCameraDevice == NULL", __func__);
        return HDF_FAILURE;
    }

    ret = HdfDeviceSendEvent(g_hdfCameraDevice, eventId, data);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdfDeviceSendEvent failed", __func__);
        OsalMemFree(g_hdfCameraDevice);
        g_hdfCameraDevice = NULL;
        return ret;
    }

    return ret;
}