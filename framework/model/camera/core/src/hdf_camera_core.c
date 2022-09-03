/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_camera_core.h"
#include "securec.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "hdf_camera_event.h"
#include "hdf_camera_device_manager.h"
#include "hdf_camera_config_parser.h"
#include "hdf_camera_utils.h"
#include "hdf_camera_dispatch.h"

#define HDF_LOG_TAG HDF_CAMERA_CORE

static int32_t HdfCameraGetConfig(const struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: input pointer is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (HdfParseCameraConfig(device->property) != HDF_SUCCESS) {
        HDF_LOGE("%s: HdfParseCameraConfig failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static struct HdfDeviceDriverFactory *HdfCameraGetDriverFactory(const char *driverName)
{
    struct HdfDeviceDriverManager *initMgr = NULL;

    if (driverName == NULL) {
        HDF_LOGE("%s: deviceName is NULL", __func__);
        return NULL;
    }
    initMgr = HdfCameraGetDeviceDriverMgr();
    if (initMgr == NULL) {
        HDF_LOGE("%s: initMgr is NULL", __func__);
        return NULL;
    }
    return initMgr->GetDeviceDriverByName(driverName);
}

static int32_t HdfCameraDeinitInterfaces(const char *driverName, const char *ifName,
                                        struct HdfDeviceDriverFactory *factory)
{
    int32_t ret;
    struct CameraDevice *camDev = NULL;
    struct HdfDeviceDriver *deviceDriver = NULL;

    camDev = CameraDeviceGetInstByName(ifName);
    if (camDev == NULL) {
        HDF_LOGW("%s: CameraDevice already released! ifName=%s", __func__, ifName);
        return HDF_SUCCESS;
    }
    deviceDriver = GetDeviceDriver(camDev);
    if (deviceDriver == NULL) {
        HDF_LOGE("%s: bad camDev!deviceDriver is NULL. ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    ret = deviceDriver->deinit(deviceDriver, camDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: device driver deinit failed! ifName=%s", __func__, ifName);
        return ret;
    }

    if (CameraDeviceGetInstByName(ifName) != NULL) {
        ret = ReleasePlatformCameraDevice(camDev);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: release cameradevice failed! ifName=%s", __func__, ifName);
            return ret;
        }
    } else {
        HDF_LOGW("%s: device driver release platform cameradevice detected! ifName=%s", __func__, ifName);
    }

    if (factory->Release != NULL) {
        factory->Release(deviceDriver);
    }
    deviceDriver = NULL;
    HDF_LOGI("%s: HdfCameraDeinitInterfaces successful", __func__);

    return HDF_SUCCESS;
}

static int32_t HdfCameraInitInterfaces(const char *driverName, struct HdfDeviceDriverFactory *factory)
{
    int32_t ret;
    struct CameraDevice *camDev = NULL;
    struct HdfDeviceDriver *deviceDriver = NULL;
    do {
        deviceDriver = factory->Build(driverName);
        if (deviceDriver == NULL) {
            HDF_LOGE("%s: device driver %s build fail!", __func__, factory->driverName);
            ret = HDF_FAILURE;
            break;
        }
        if (deviceDriver->init == NULL) {
            HDF_LOGI("%s: device driver %s not 'init' api", __func__, factory->driverName);
            ret = HDF_DEV_ERR_OP;
            break;
        }
        camDev = AllocPlatformCameraDevice(driverName);
        if (camDev == NULL) {
            HDF_LOGE("%s: allocate camera device failed!", __func__);
            ret = HDF_FAILURE;
            break;
        }
        camDev->deviceDriver = deviceDriver;
        ret = deviceDriver->init(deviceDriver, camDev);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: init device %s failed! ret=%d", __func__, factory->driverName, ret);
            break;
        }
        deviceDriver = NULL;
    } while (false);

    if (ret != HDF_SUCCESS) {
        if (deviceDriver != NULL && factory->Release != NULL) {
            factory->Release(deviceDriver);
        }
        if (camDev != NULL) {
            ReleasePlatformCameraDevice(camDev);
        }
    }

    return ret;
}

static int32_t HdfCameraDeinitInterface(const char *driverName, struct HdfDeviceDriverFactory *factory)
{
    int32_t ret = HDF_SUCCESS;
    char *ifNames = NULL;

    ifNames = HdfCameraGetIfNames();
    if (ifNames == NULL) {
        HDF_LOGE("%s: HdfCameraGetIfNames failed!", __func__);
        return HDF_FAILURE;
    }

    ret = HdfCameraDeinitInterfaces(driverName, ifNames, factory);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Deinit camif failed! ret=%d", __func__,  ret);
    }
    OsalMemFree(ifNames);
    ifNames = NULL;
    HDF_LOGI("%s: camera deinit interface finished!", __func__);

    return ret;
}

int32_t HdfCameraDeinitDevice(const char *driverName)
{
    struct HdfDeviceDriverFactory *factory = NULL;
    int32_t ret;

    if (driverName == NULL) {
        HDF_LOGE("%s: Input is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    factory = HdfCameraGetDriverFactory(driverName);
    if (factory == NULL) {
        HDF_LOGE("%s: get factory failed! driverName=%s", __func__, driverName);
        return HDF_FAILURE;
    }
    ret = HdfCameraDeinitInterface(driverName, factory);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Deinit interface failed! ret=%d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t HdfCameraInitDevice(const char *driverName)
{
    int32_t ret;
    struct HdfDeviceDriverFactory *factory = NULL;

    factory = HdfCameraGetDriverFactory(driverName);
    if (factory == NULL) {
        HDF_LOGE("%s: get factory failed! driverName=%s", __func__, driverName);
        return HDF_FAILURE;
    }

    ret = HdfCameraInitInterfaces(driverName, factory);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: init interfaces failed! ret=%d!", __func__, ret);
    } else {
        HDF_LOGD("%s: init interfaces success! ret=%d!", __func__, ret);
    }

    return ret;
}

static int32_t HdfCameraDriverInit(struct HdfDeviceObject *device)
{
    struct HdfConfigCameraRoot *rootConfig = NULL;
    uint16_t i;
    HDF_LOGI("%s: HdfCameraDriverInit start...", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device == NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (HdfCameraGetConfig(device) != HDF_SUCCESS) {
        HDF_LOGE("%s: HdfCameraGetConfig get camera config failed!", __func__);
        return HDF_FAILURE;
    }
    rootConfig = HdfCameraGetConfigRoot();
    if (HdfCameraAddDevice(device) != HDF_SUCCESS) {
        HDF_LOGE("%s: HdfCameraAddDevice failed!", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < rootConfig->cameraConfig.abilityConfig.deviceNum; i++) {
        if (HdfCameraInitDevice(rootConfig->cameraConfig.abilityConfig.deviceConfig[i].driverName) != HDF_SUCCESS) {
            HDF_LOGE("%s: HdfCameraInitDevice failed!", __func__);
            return HDF_FAILURE;
        }
    }
    HDF_LOGV("%s: HdfCameraDriverInit finished.", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfCameraDriverBind(struct HdfDeviceObject *device)
{
    static struct IDeviceIoService cameraService = {
        .object.objectId = 1,
        .Dispatch = HdfCameraDispatch,
    };

    if (device == NULL) {
        HDF_LOGE("%s: Input param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    device->service = &cameraService;
    HDF_LOGI("%s: camera driver bind successed!", __func__);

    return HDF_SUCCESS;
}

static void HdfCameraDriverRelease(struct HdfDeviceObject *device)
{
    HDF_LOGW("%s: Driver HdfCamera deiniting...", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
    struct IDeviceIoService *drvData = device->service;
    if (drvData == NULL) {
        HDF_LOGE("%s: drvData is null", __func__);
        return;
    }
    OsalMemFree(drvData);
    HDF_LOGW("%s: Driver HdfCamera deinited.", __func__);
}

struct HdfDriverEntry g_hdfCameraEntry = {
    .moduleVersion = 1,
    .Bind = HdfCameraDriverBind,
    .Init = HdfCameraDriverInit,
    .Release = HdfCameraDriverRelease,
    .moduleName = "HDF_CAMERA"
};

HDF_INIT(g_hdfCameraEntry);