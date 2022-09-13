/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef CAMERA_CONFIG_H
#define CAMERA_CONFIG_H

#include <utils/device_resource_if.h>
#include <utils/hdf_base.h>

#define BUFFER_TYPE_COUNT 3
#define FORMAT_TYPE_COUNT 32
#define CAMERA_MAX_DEVICE_NUM 4

#define CHECK_PARSER_CONFIG_RET(ret,  str) do { \
    if ((ret) != HDF_SUCCESS) { \
        HDF_LOGE("%s: get %s failed, ret = %{public}d, line : %{public}d", __func__, str, ret, __LINE__); \
        return HDF_FAILURE; \
    } \
} while (0)

struct SensorDeviceConfig{
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

struct VcmDeviceConfig {
    const char *name;
    uint8_t id;
    uint8_t autoFocus;
    uint8_t zoom;
    uint8_t zoomMaxNum;
};

struct LensDeviceConfig {
    const char *name;
    uint8_t id;
    uint8_t aperture;
};

struct CameraFlashConfig {
    uint8_t mode;
    const char *name;
    uint8_t id;
    uint8_t flashMode;
    uint8_t flashIntensity;
};

struct CameraSensorConfig {
    uint8_t mode;
    uint16_t sensorNum;
    struct SensorDeviceConfig sensor[CAMERA_MAX_DEVICE_NUM];
};

struct CameraIspConfig {
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

struct CameraLensConfig {
    uint8_t mode;
    uint16_t lensNum;
    struct LensDeviceConfig lens[CAMERA_MAX_DEVICE_NUM];
};

struct CameraVcmConfig {
    uint8_t mode;
    uint16_t vcmNum;
    struct VcmDeviceConfig vcm[CAMERA_MAX_DEVICE_NUM];
};

struct CameraUvcConfig {
    uint8_t mode;
};

struct CameraDeviceConfig {
    const char *deviceName;
    struct CameraSensorConfig sensor;
    struct CameraIspConfig isp;
    struct CameraLensConfig lens;
    struct CameraVcmConfig vcm;
    struct CameraFlashConfig flash;
    struct CameraUvcConfig uvc;
};

struct CameraConfigRoot {
    uint16_t deviceNum;
    struct CameraDeviceConfig deviceConfig[CAMERA_MAX_DEVICE_NUM];
};

struct CameraConfigRoot *HdfCameraGetConfigRoot(void);
int32_t HdfParseCameraConfig(const struct DeviceResourceNode *node);

#endif  // CAMERA_CONFIG_H
