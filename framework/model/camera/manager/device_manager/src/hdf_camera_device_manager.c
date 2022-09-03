/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_camera_device_manager.h"
#include "gpio_if.h"
#include "osal_mem.h"
#include "hdf_log.h"
#include "securec.h"

#define HDF_LOG_TAG HDF_CAMERA_CORE

static struct HdfDeviceDriverFactory *g_cameraDeviceDriverManagerFactory[MAX_DEVICEDRIVER_COUNT] = {NULL};

static struct CameraDevice *g_cameraDeviceTable[MAX_CAMDEVICE_COUNT] = {NULL};

static void DeInitCameraDevice(struct CameraDevice *camDev)
{
    if (camDev == NULL) {
        HDF_LOGE("%s: success: already free!", __func__);
        return;
    } else {
        OsalMemFree(camDev);
        camDev = NULL;
    }
    HDF_LOGI("%s: success!", __func__);
    return;
}

static bool FindAvailableTable(uint32_t *index)
{
    int32_t i;

    if (index == NULL) {
        HDF_LOGE("%s: Find Available table index error!", __func__);
        return false;
    }
    for (i = 0; i < MAX_CAMDEVICE_COUNT; i++) {
        if (g_cameraDeviceTable[i] == NULL) {
            *index = i;
            return true;
        }
    }
    return false;
}

static bool AddCameraDeviceToTable(uint32_t index, struct CameraDevice *cameraDevice)
{
    if (index >= MAX_CAMDEVICE_COUNT) {
        HDF_LOGE("%s: error: because of not enough space!", __func__);
        return false;
    }
    g_cameraDeviceTable[index] = cameraDevice;
    return true;
}


static void DeleteCameraDeviceFromTable(const struct CameraDevice *cameraDevice)
{
    int32_t i;
    for (i = 0; i < MAX_CAMDEVICE_COUNT; i++) {
        if (g_cameraDeviceTable[i] == cameraDevice) {
            g_cameraDeviceTable[i] = NULL;
            return;
        }
    }
    return;
}

int32_t CameraDeviceDeInit(struct CameraDevice *camDev)
{
    if (camDev == NULL) {
        HDF_LOGI("%s: success: already deinit!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    DeleteCameraDeviceFromTable(camDev);
    return HDF_SUCCESS;
}

struct CameraDevice *CameraDeviceInit(const char *ifName, uint32_t len)
{
    struct CameraDevice *camDevice = NULL;
    int32_t index = 0;

    if ((ifName == NULL) || (strlen(ifName) != len) || (strlen(ifName) > IFNAMSIZ - 1)) {
        HDF_LOGE("%s: fail: ifName = null or len not right!", __func__);
        return NULL;
    }
    camDevice = (struct CameraDevice *)OsalMemCalloc(sizeof(struct CameraDevice));
    if (camDevice == NULL) {
        HDF_LOGE("%s: fail: OsalMemCalloc fail!", __func__);
        return NULL;
    }
    if (strcpy_s(camDevice->name, IFNAMSIZ, ifName) != EOK) {
        HDF_LOGE("%s: fail: strcpy_s fail!", __func__);
        OsalMemFree(camDevice);
        return NULL;
    }
    if (FindAvailableTable(&index)) {
        AddCameraDeviceToTable(index, camDevice);
    } else {
        DeInitCameraDevice(camDevice);
        HDF_LOGE("%s: fail: Not extra table.", __func__);
        return NULL;
    }
    HDF_LOGI("%s: Init Camera Device success!", __func__);

    return camDevice;
}

struct CameraDevice *CameraDeviceGetInstByName(const char *name)
{
    int32_t i;
    if (name == NULL) {
        HDF_LOGE("%s: fail: name = NULL.", __func__);
        return NULL;
    }
    for (i = 0; i < MAX_CAMDEVICE_COUNT; i++) {
        if (g_cameraDeviceTable[i] != NULL) {
            if (strcmp(g_cameraDeviceTable[i]->name, name) == 0) {
                return g_cameraDeviceTable[i];
            }
        }
    }
    return NULL;
}

struct HdfDeviceDriverFactory *HdfCameraGetDeviceDriverByName(const char *driverName)
{
    int32_t i;
    if (driverName == NULL) {
        HDF_LOGE("%s: fail: driverName is NULL", __func__);
        return NULL;
    }
    for (i = 0; i < MAX_DEVICEDRIVER_COUNT; i++) {
        if (g_cameraDeviceDriverManagerFactory[i] != NULL &&
            g_cameraDeviceDriverManagerFactory[i]->driverName != NULL) {
            struct HdfDeviceDriverFactory *factory = g_cameraDeviceDriverManagerFactory[i];
            if (strcmp(factory->driverName, driverName) == 0) {
                return factory;
            }
        }
    }
    return NULL;
}

int32_t HdfCameraRegDeviceDriver(struct HdfDeviceDriverFactory *obj)
{
    int32_t index;
    if (obj == NULL || obj->driverName == NULL) {
        HDF_LOGE("%s: HdfDeviceDriverFactory obj is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (HdfCameraGetDeviceDriverByName(obj->driverName) != NULL) {
        HDF_LOGW("%s: devicedriver factory is already registered. name=%s", __func__, obj->driverName);
        return HDF_SUCCESS;
    }
    for (index = 0; index < MAX_DEVICEDRIVER_COUNT; index++) {
        if (g_cameraDeviceDriverManagerFactory[index] == NULL) {
            g_cameraDeviceDriverManagerFactory[index] = obj;
            HDF_LOGI("%s: device driver %s registered.", __func__, obj->driverName);
            return HDF_SUCCESS;
        }
    }
    return HDF_FAILURE;
}

int32_t DeviceDriverMgrDeInit(void)
{
    int32_t cnt;
    for (cnt = 0; cnt < MAX_DEVICEDRIVER_COUNT; cnt++) {
        if (g_cameraDeviceDriverManagerFactory[cnt] != NULL) {
            if (g_cameraDeviceDriverManagerFactory[cnt]->ReleaseFactory != NULL) {
                g_cameraDeviceDriverManagerFactory[cnt]->ReleaseFactory(g_cameraDeviceDriverManagerFactory[cnt]);
            }
            g_cameraDeviceDriverManagerFactory[cnt] = NULL;
        }
    }
    return HDF_SUCCESS;
}

static struct HdfDeviceDriverManager g_deviceDriverManager = {
    .deviceFactoryInsts = g_cameraDeviceDriverManagerFactory,
    .RegDeviceDriver = HdfCameraRegDeviceDriver,
    .GetDeviceDriverByName = HdfCameraGetDeviceDriverByName,
};

struct HdfDeviceDriverManager *HdfCameraGetDeviceDriverMgr(void)
{
    return &g_deviceDriverManager;
}