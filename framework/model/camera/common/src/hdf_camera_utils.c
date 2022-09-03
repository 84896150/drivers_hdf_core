/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_camera_utils.h"
#include <securec.h>
#include "hdf_log.h"
#include "hdf_camera_device_manager.h"
#include "hdf_camera_config_parser.h"

#define HDF_LOG_TAG HDF_CAMERA_CORE

const char * const HDF_CAMERA_PLATFORM_DRIVER_NAME = "HDF_CAMERA_PLATFORM_DRIVER";
static int32_t *g_cameraIdMap[MAX_ID_COUNT] = {NULL};
char g_ifName[IFNAMSIZ] = {0};
static uint32_t g_allocatedDevMap = 0;

int32_t HdfCameraAddIdMap(int32_t id)
{
    int32_t i;
    for (i = 0; i < MAX_ID_COUNT; i++) {
        if (g_cameraIdMap[i] != NULL) {
            if (g_cameraIdMap[i] == id) {
                return HDF_ERR_DEVICE_BUSY;
            }
        } else {
            g_cameraIdMap[i] = id;
            return HDF_SUCCESS;
        }
    }
    return HDF_FAILURE;
}

int32_t HdfCameraClearIdMap(int32_t id)
{
    int32_t i;
    for (i = 0; i < MAX_ID_COUNT; i++) {
        if (g_cameraIdMap[i] != NULL) {
            if (g_cameraIdMap[i] == id) {
                g_cameraIdMap[i] = NULL;
                return HDF_SUCCESS;
            }
        }
    }
    return HDF_FAILURE;
}

struct CameraDevice *AllocPlatformCameraDevice(const char *driverName)
{
    struct CameraDevice *result = NULL;

    if (driverName == NULL) {
        HDF_LOGE("%s: NULL ptr!", __func__);
        return NULL;
    }

    do {
        if (g_allocatedDevMap >= MAX_CAMDEVICE_COUNT) {
            HDF_LOGE("%s: not enough Dev! map=%u", __func__, g_allocatedDevMap);
            break;
        }

        if (snprintf_s(g_ifName, IFNAMSIZ, IFNAMSIZ - 1, "camera%d", g_allocatedDevMap) < 0) {
            HDF_LOGE("%s: format g_ifName failed! cameraDevice id=%d", __func__, g_allocatedDevMap);
            return NULL;
        }

        result = CameraDeviceInit(g_ifName, strlen(g_ifName));
    } while (false);

    if (result == NULL) {
        HDF_LOGE("%s: alloc CameraDevice return NULL!", __func__);
        return result;
    }
    result->classDriverName = HDF_CAMERA_PLATFORM_DRIVER_NAME;
    g_allocatedDevMap++;

    return result;
}

struct HdfDeviceDriver *GetDeviceDriver(const struct CameraDevice *cameraDev)
{
    if (cameraDev == NULL) {
        HDF_LOGE("%s: NULL ptr!", __func__);
        return NULL;
    }

    if (cameraDev->classDriverName != HDF_CAMERA_PLATFORM_DRIVER_NAME) {
        return NULL;
    }

    return cameraDev->deviceDriver;
}

int32_t ReleasePlatformCameraDevice(struct CameraDevice *camDev)
{
    int32_t ret;

    if (camDev == NULL) {
        HDF_LOGE("%s: NULL ptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (camDev->classDriverName != HDF_CAMERA_PLATFORM_DRIVER_NAME) {
        HDF_LOGE("%s: Wrong camera device!", __func__);
        return HDF_FAILURE;
    }
    ret = CameraDeviceDeInit(camDev);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    return HDF_SUCCESS;
}

char *HdfCameraGetIfNames(void)
{
    return g_ifName;
}

int32_t JudgePermission(int permissionId)
{
    if (permissionId != CAMERA_MASTER) {
        HDF_LOGE("%s: No permissions set ctrl!", __func__);
        return HDF_ERR_NOPERM;
    }
    return HDF_SUCCESS;
}

int32_t JudgeCameraDevice(const char *ifName, int type)
{
    int32_t camId;
    struct HdfConfigCameraRoot *rootConfig = NULL;
    rootConfig = HdfCameraGetConfigRoot();
    sscanf(ifName, "%*s%d", camId);
    switch (type) {
        case SENSOR_TYPE:
            if (rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].sensor.mode == 0) {
                HDF_LOGE("%s: not support sensor!", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;
        case ISP_TYPE:
            if (rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].isp.mode == 0) {
                HDF_LOGE("%s: not support isp!", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;
        case VCM_TYPE:
            if (rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].vcm.mode == 0) {
                HDF_LOGE("%s: not support vcm!", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;
        case LENS_TYPE:
            if (rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].lens.mode == 0) {
                HDF_LOGE("%s: not support lens!", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;
        case FLASH_TYPE:
            if (rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].flash.mode == 0) {
                HDF_LOGE("%s: not support flash!", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;
        case UVC_TYPE:
            if (rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].uvc.mode == 0) {
                HDF_LOGE("%s: not support uvc!", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;
        default:
            HDF_LOGE("%s: wrong type: %d", __func__, type);
            return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t JudgeSensorNum(const char *driverName, int camId, struct HdfConfigCameraRoot *rootConfig)
{
    int32_t i, j;
    for (i = 0; i < rootConfig->cameraConfig.abilityConfig.deviceNum; i++) {
        for (j = 0; j < rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].sensor.sensorNum; j++) {
            if (strcmp(rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].
                        sensor.sensor[j].name, driverName) == 0) {
                return j;
            }
        }
    }
    return HDF_FAILURE;
}

static int32_t JudgeVcmNum(const char *driverName, int camId, struct HdfConfigCameraRoot *rootConfig)
{
    int32_t i, j;
    for (i = 0; i < rootConfig->cameraConfig.abilityConfig.deviceNum; i++) {
        for (j = 0; j < rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].vcm.vcmNum; j++) {
            if (strcmp(rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].
                        vcm.vcm[j].name, driverName) == 0) {
                return j;
            }
        }
    }
    return HDF_FAILURE;
}

static int32_t JudgeLensNum(const char *driverName, int camId, struct HdfConfigCameraRoot *rootConfig)
{
    int32_t i, j;
    for (i = 0; i < rootConfig->cameraConfig.abilityConfig.deviceNum; i++) {
        for (j = 0; j < rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].lens.lensNum; j++) {
            if (strcmp(rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].
                        lens.lens[j].name, driverName) == 0) {
                return j;
            }
        }
    }
    return HDF_FAILURE;
}

static int32_t JudgeUvcNum(const char *driverName, int camId, struct HdfConfigCameraRoot *rootConfig)
{
    int32_t i, j;
    for (i = 0; i < rootConfig->cameraConfig.abilityConfig.deviceNum; i++) {
        for (j = 0; j < rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].uvc.uvcNum; j++) {
            if (strcmp(rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].
                        uvc.uvc[j].name, driverName) == 0) {
                return j;
            }
        }
    }
    return HDF_FAILURE;
}

int32_t JudgeDeviceNum(const char *driverName, int camId, int type)
{
    int32_t ret;
    struct HdfConfigCameraRoot *rootConfig = NULL;
    rootConfig = HdfCameraGetConfigRoot();
    switch (type) {
        case SENSOR_TYPE:
            ret = JudgeSensorNum(driverName, camId, rootConfig);
            break;
        case ISP_TYPE:
            return HDF_SUCCESS;
        case VCM_TYPE:
            ret = JudgeVcmNum(driverName, camId, rootConfig);
            break;
        case LENS_TYPE:
            ret = JudgeLensNum(driverName, camId, rootConfig);
            break;
        case FLASH_TYPE:
            return HDF_SUCCESS;
        case UVC_TYPE:
            ret = JudgeUvcNum(driverName, camId, rootConfig);
            break;
        default:
            HDF_LOGE("%s: wrong type: %d", __func__, type);
            return HDF_FAILURE;
    }
    return ret;
}

const char *HdfCameraGetCameraId(char *ifName, const char *driverName, int type)
{
    int32_t camId, deviceNum;
    struct HdfConfigCameraRoot *rootConfig = NULL;
    rootConfig = HdfCameraGetConfigRoot();

    sscanf(ifName, "%*s%d", camId);
    HDF_LOGE("%s: camId = %d", __func__, camId);

    deviceNum = JudgeDeviceNum(driverName, camId, type);
    HDF_LOGE("%s: deviceNum = %d", __func__, deviceNum);

    switch (type) {
        case SENSOR_TYPE:
            return rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].sensor.sensor[deviceNum].name;
        case ISP_TYPE:
            return rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].isp.name;
        case VCM_TYPE:
            return rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].vcm.vcm[deviceNum].name;
        case LENS_TYPE:
            return rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].lens.lens[deviceNum].name;
        case FLASH_TYPE:
            return rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].flash.name;
        case UVC_TYPE:
            return rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].uvc.uvc[deviceNum].name;
        default:
            HDF_LOGE("%s: wrong type: %d", __func__, type);
            return NULL;
    }
    HDF_LOGE("%s: wrong ifName!", __func__);
    return NULL;
}

int32_t JudgeFormatNum(int camId, const char *driverName, int type, struct CameraCtrlConfig *ctrlConfig)
{
    int32_t deviceNum;
    struct HdfConfigCameraRoot *rootConfig = NULL;
    rootConfig = HdfCameraGetConfigRoot();

    deviceNum = JudgeDeviceNum(driverName, camId, type);
    HDF_LOGE("%s: deviceNum = %d", __func__, deviceNum);

    switch (type) {
        case SENSOR_TYPE:
            if (ctrlConfig->pixelFormat.pixel.width > rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].
                                                        sensor.sensor[deviceNum].widthMaxNum) {
                HDF_LOGE("%s: sensor wrong width = %d", __func__, ctrlConfig->pixelFormat.pixel.width);
                return HDF_FAILURE;
            } else if (ctrlConfig->pixelFormat.pixel.height > rootConfig->cameraConfig.abilityConfig.
                                                        deviceConfig[camId].sensor.sensor[deviceNum].heightMaxNum) {
                HDF_LOGE("%s: sensor wrong height = %d", __func__, ctrlConfig->pixelFormat.pixel.height);
                return HDF_FAILURE;
            }
            break;
        case UVC_TYPE:
            if (ctrlConfig->pixelFormat.pixel.width > rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].
                                                        uvc.uvc[deviceNum].widthMaxNum) {
                HDF_LOGE("%s: uvc wrong width = %d", __func__, ctrlConfig->pixelFormat.pixel.width);
                return HDF_FAILURE;
            } else if (ctrlConfig->pixelFormat.pixel.height > rootConfig->cameraConfig.abilityConfig.
                                                        deviceConfig[camId].uvc.uvc[deviceNum].heightMaxNum) {
                HDF_LOGE("%s: uvc wrong height = %d", __func__, ctrlConfig->pixelFormat.pixel.height);
                return HDF_FAILURE;
            }
            break;
        default:
            HDF_LOGE("%s: wrong type: %d", __func__, type);
            return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t JudgeFrameRate(int camId, const char *driverName, int type, struct CameraCtrlConfig *ctrlConfig)
{
    int32_t fps, deviceNum;
    struct HdfConfigCameraRoot *rootConfig = NULL;
    rootConfig = HdfCameraGetConfigRoot();

    deviceNum = JudgeDeviceNum(driverName, camId, type);
    HDF_LOGE("%s: deviceNum = %d", __func__, deviceNum);
    fps = 1000*(ctrlConfig->pixelFormat.fps.denominator)/ctrlConfig->pixelFormat.fps.numerator;
    HDF_LOGE("%s: fps = %d", __func__, fps);
    switch (type) {
        case SENSOR_TYPE:
            if (fps > 1000*(rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].
                                                        sensor.sensor[deviceNum].frameRateMaxNum)) {
                HDF_LOGE("%s: sensor wrong fps = %d", __func__, fps);
                return HDF_FAILURE;
            }
            break;
        case UVC_TYPE:
            if (fps > 1000*(rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].
                                                        uvc.uvc[deviceNum].frameRateMaxNum)) {
                HDF_LOGE("%s: uvc wrong fps = %d", __func__, fps);
                return HDF_FAILURE;
            }
            break;
        default:
            HDF_LOGE("%s: wrong type: %d", __func__, type);
            return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t JudgeFmtType(int camId, const char *driverName, int type, struct CameraCtrlConfig *ctrlConfig)
{
    uint32_t format_size, i, num;
    struct HdfConfigCameraRoot *rootConfig = NULL;

    rootConfig = HdfCameraGetConfigRoot();
    switch (type) {
        case SENSOR_TYPE:
            num = JudgeDeviceNum(driverName, camId, SENSOR_TYPE);
            format_size = rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].sensor.sensor[num].formatTypeNum;
            for (i = 0; i < format_size; i++) {
                if (ctrlConfig->pixelFormat.pixel.format ==
                    rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].sensor.sensor[num].formatType[i]) {
                    HDF_LOGE("%s: formatType: %d", __func__, ctrlConfig->pixelFormat.pixel.format);
                    return HDF_SUCCESS;
                }
            }
            HDF_LOGE("%s: not support formatType: %d", __func__, ctrlConfig->pixelFormat.pixel.format);
            break;
        case UVC_TYPE:
            num = JudgeDeviceNum(driverName, camId, UVC_TYPE);
            format_size = rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].uvc.uvc[num].formatTypeNum;
            for (i = 0; i < format_size; i++) {
                if (ctrlConfig->pixelFormat.pixel.format ==
                    rootConfig->cameraConfig.abilityConfig.deviceConfig[camId].uvc.uvc[num].formatType[i]) {
                    HDF_LOGE("%s: formatType: %d", __func__, ctrlConfig->pixelFormat.pixel.format);
                    return HDF_SUCCESS;
                }
            }
            HDF_LOGE("%s: not support formatType: %d", __func__, ctrlConfig->pixelFormat.pixel.format);
            break;
    }

    return HDF_FAILURE;
}