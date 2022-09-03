/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_CAMERA_CONFIG_H
#define HDF_CAMERA_CONFIG_H

#include "device_resource_if.h"
#include "hdf_base.h"

#define BUFFER_TYPE_COUNT 3
#define FORMAT_TYPE_COUNT 32
#define CAMERA_MAX_DEVICE_NUM 4

#define CHECK_PARSER_CONFIG_RET(ret,  str) do { \
    if ((ret) != HDF_SUCCESS) { \
        HDF_LOGE("%s: get %s failed, ret = %d, line : %d", __func__, str, ret, __LINE__); \
        return HDF_FAILURE; \
    } \
} while (0)

struct HdfConfigCameraSensorDevice {
    const char *name;
    uint8_t id;
    uint8_t exposure;
    uint8_t mirror;
    uint32_t heightMaxNum;
    uint32_t widthMaxNum;
    uint32_t frameRateMaxNum;
    uint8_t bufferCount;
    uint32_t bufferTypeNum;
    uint32_t bufferType[BUFFER_TYPE_COUNT];
    uint32_t formatTypeNum;
    uint32_t formatType[FORMAT_TYPE_COUNT];
};

struct HdfConfigCameraVcmDevice {
    const char *name;
    uint8_t id;
    uint8_t autoFocus;
    uint8_t zoom;
    uint8_t zoomMaxNum;
};

struct HdfConfigCameraLensDevice {
    const char *name;
    uint8_t id;
    uint8_t aperture;
};

struct HdfConfigCameraFlashConfig {
    uint8_t mode;
    const char *name;
    uint8_t id;
    uint8_t flashMode;
    uint8_t flashIntensity;
};

struct HdfConfigCameraUvcDevice {
    const char *name;
    uint8_t id;
    uint8_t exposure;
    uint8_t mirror;
    uint32_t heightMaxNum;
    uint32_t widthMaxNum;
    uint32_t frameRateMaxNum;
    uint8_t bufferCount;
    uint32_t bufferTypeNum;
    uint32_t bufferType[BUFFER_TYPE_COUNT];
    uint8_t brightness;
    uint8_t contrast;
    uint8_t saturation;
    uint8_t sharpness;
    uint8_t gain;
    uint8_t gamma;
    uint8_t whiteBalance;
    uint32_t formatTypeNum;
    uint32_t formatType[FORMAT_TYPE_COUNT];
};

struct HdfConfigCameraSensorConfig {
    uint8_t mode;
    uint16_t sensorNum;
    struct HdfConfigCameraSensorDevice sensor[CAMERA_MAX_DEVICE_NUM];
};

struct HdfConfigCameraIspConfig {
    uint8_t mode;
    const char *name;
    uint8_t id;
    uint8_t brightness;
    uint8_t contrast;
    uint8_t saturation;
    uint8_t sharpness;
    uint8_t gain;
    uint8_t gamma;
    uint8_t whiteBalance;
};

struct HdfConfigCameraLensConfig {
    uint8_t mode;
    uint16_t lensNum;
    struct HdfConfigCameraLensDevice lens[CAMERA_MAX_DEVICE_NUM];
};

struct HdfConfigCameraVcmConfig {
    uint8_t mode;
    uint16_t vcmNum;
    struct HdfConfigCameraVcmDevice vcm[CAMERA_MAX_DEVICE_NUM];
};

struct HdfConfigCameraUvcConfig {
    uint8_t mode;
    uint16_t uvcNum;
    struct HdfConfigCameraUvcDevice uvc[CAMERA_MAX_DEVICE_NUM];
};

struct HdfConfigDeviceConfig {
    const char *driverName;
    struct HdfConfigCameraSensorConfig sensor;
    struct HdfConfigCameraIspConfig isp;
    struct HdfConfigCameraLensConfig lens;
    struct HdfConfigCameraVcmConfig vcm;
    struct HdfConfigCameraFlashConfig flash;
    struct HdfConfigCameraUvcConfig uvc;
};

struct HdfConfigCameraAbilityConfig {
    uint16_t deviceNum;
    struct HdfConfigDeviceConfig deviceConfig[CAMERA_MAX_DEVICE_NUM];
};

struct HdfConfigCameraConfig {
    struct HdfConfigCameraAbilityConfig abilityConfig;
};

struct HdfConfigCameraRoot {
    struct HdfConfigCameraConfig cameraConfig;
};

struct HdfConfigCameraRoot *HdfCameraGetConfigRoot(void);
int32_t HdfParseCameraConfig(const struct DeviceResourceNode *node);

#endif  // HDF_CAMERA_CONFIG_H
