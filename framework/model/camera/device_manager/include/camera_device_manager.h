/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef CAMERA_DEVICE_MANAGER_H
#define CAMERA_DEVICE_MANAGER_H

#include <utils/device_resource_if.h>
#include <camera/camera_product.h>

#define DEVICE_NAME_SIZE 20
#define MAX_DEVICEDRIVER_COUNT 20
#define MAX_CAMDEVICE_COUNT 20
#define MAX_CAMERA_COMPONENT_NAME_LEN 20
#define DRIVER_NAME_SIZE 20

#define CHECK_RETURN_RET(ret) do { \
    if ((ret) != 0) { \
        HDF_LOGE("%s: failed, ret = %d, line: %d", __func__, ret, __LINE__); \
        return HDF_FAILURE; \
    } \
} while (0)

enum CameraPixFmt {
    CAMERA_PIX_FMT_YUV420 = 1,
    CAMERA_PIX_FMT_YUV422,
    CAMERA_PIX_FMT_YUV444,

    CAMERA_PIX_FMT_RGB565 = 100,
    CAMERA_PIX_FMT_RGB444,
    CAMERA_PIX_FMT_RGB555,
    CAMERA_PIX_FMT_RGB32,

    CAMERA_PIX_FMT_SRGGB8 = 200,
    CAMERA_PIX_FMT_SRGGB10,
    CAMERA_PIX_FMT_SRGGB12,
    CAMERA_PIX_FMT_SBGGR8,
    CAMERA_PIX_FMT_SBGGR10,
    CAMERA_PIX_FMT_SBGGR12,
};

struct Pixel {
    char* description;
    uint32_t format;
    uint32_t width;
    uint32_t height;
    uint32_t sizeImage;
};

struct FPS {
    uint32_t numerator;
    uint32_t denominator;
};

struct CameraControl {
    uint32_t id;
    uint32_t value;
};

struct Rect {
    int32_t left;
    int32_t top;
    uint32_t width;
    uint32_t height;
};

struct Capability {
    char driver[16];
    char card[32];
    char busInfo[32];
    uint32_t capabilities;
};

struct CropCap {
    struct Rect bounds;
    struct Rect defrect;
    struct FPS pixelaspect;
};

struct PixelFormat {
    struct FPS fps;
    struct Pixel pixel;
    struct Rect cop;
    struct CropCap cropCap;
};

struct CameraCtrlConfig {
    struct PixelFormat pixelFormat;
    struct CameraControl ctrl;
};

struct CameraDeviceDriverFactory {
    const char *deviceName;
    void (*ReleaseFactory)(struct CameraDeviceDriverFactory *factory);
    struct CameraDeviceDriver *(*Build)(const char *deviceName);
    void (*Release)(struct CameraDeviceDriver *deviceDriver);
};

struct CameraDevice {
    const char name[DEVICE_NAME_SIZE];
    char driverName[DRIVER_NAME_SIZE];
    const char* classDriverName;
    struct CameraDeviceDriver *deviceDriver;
};

struct SensorOps {
    int32_t (*SensorPowerUp)(struct CameraDevice *camDev);
    int32_t (*SensorPowerDown)(struct CameraDevice *camDev);
    int32_t (*SensorSetConfig)(struct CameraCtrlConfig *config, const char *driverName, uint32_t cmd);
    int32_t (*SensorGetConfig)(struct CameraCtrlConfig *config, const char *driverName, uint32_t cmd);
};

struct SensorDevice {
    struct SensorOps devOps;
};

struct CameraDeviceDriverManager {
    struct CameraDeviceDriverFactory **deviceFactoryInsts;
    int32_t (*RegDeviceDriverFactory)(struct CameraDeviceDriverFactory *factoryInst);
    struct CameraDeviceDriverFactory *(*GetDeviceDriverFactoryByName)(const char *name);
};

struct BaseOps {
    int32_t (*OpenCamera)(struct CameraDevice *camDev);
    int32_t (*CloseCamera)(struct CameraDevice *camDev);
};

struct CameraDeviceDriver {
    char name[DEVICE_NAME_SIZE];
    struct BaseOps ops;
    struct SensorDevice sensor;
    int32_t (*init)(struct CameraDeviceDriver *deviceDriver, struct CameraDevice *camDev);
    int32_t (*deinit)(struct CameraDeviceDriver *deviceDriver, struct CameraDevice *camDev);
};

struct CameraDevice *CameraDeviceInit(const char *ifName, uint32_t len);
int32_t CameraDeviceDeInit(struct CameraDevice *camDev);
struct CameraDeviceDriverFactory *CameraGetDeviceDriverFactoryByName(const char *deviceName);
int32_t CameraRegDeviceDriverFactory(struct CameraDeviceDriverFactory *obj);
int32_t DeviceDriverMgrDeInit(void);
struct CameraDevice *CameraDeviceGetInstByName(const char *name);
struct CameraDeviceDriverManager *CameraGetDeviceDriverMgr(void);

#endif  // CAMERA_DEVICE_MANAGER_H