/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_CAMERA_DEVICE_MANAGER_H
#define HDF_CAMERA_DEVICE_MANAGER_H

#include "device_resource_if.h"
#include "hdf_camera_dispatch.h"

#define IFNAMSIZ 20
#define MAX_DEVICEDRIVER_COUNT 20
#define MAX_CAMDEVICE_COUNT 20
#define MAX_CAMERA_COMPONENT_NAME_LEN 20

#define CHECK_RETURN_RET(ret) do { \
    if ((ret) != 0) { \
        HDF_LOGE("%s: failed, ret = %d, line: %d", __func__, ret, __LINE__); \
        return 1; \
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

struct HdfDeviceDriverFactory {
    const char *driverName;
    void (*ReleaseFactory)(struct HdfDeviceDriverFactory *factory);
    struct HdfDeviceDriver *(*Build)(const char *driverName);
    void (*Release)(struct HdfDeviceDriver *deviceDriver);
};

struct CameraDevice {
    char name[IFNAMSIZ];
    const char* classDriverName;
    void *classDriverPriv;
    void *mlPriv;
    struct HdfDeviceDriver *deviceDriver;
};

struct SensorOps {
    int32_t (*SensorPowerUp)(struct CameraDevice *camDev, const char *driverName);
    int32_t (*SensorPowerDown)(struct CameraDevice *camDev, const char *driverName);
    int32_t (*SensorSetConfig)(struct CameraCtrlConfig *config, const char *driverName, uint32_t cmd);
    int32_t (*SensorGetConfig)(struct CameraCtrlConfig *config, const char *driverName, uint32_t cmd);
};

struct SensorDevice {
    struct SensorOps devOps;
};

struct HdfDeviceDriverManager {
    struct HdfDeviceDriverFactory **deviceFactoryInsts;
    int32_t (*RegDeviceDriver)(struct HdfDeviceDriverFactory *factoryInst);
    struct HdfDeviceDriverFactory *(*GetDeviceDriverByName)(const char *name);
};

struct BaseOps {
    int32_t (*OpenCamera)(struct CameraDevice *camDev);
    int32_t (*CloseCamera)(struct CameraDevice *camDev);
};

struct HdfDeviceDriver {
    char name[MAX_CAMERA_COMPONENT_NAME_LEN];
    struct BaseOps ops;
    struct SensorDevice sensor;
    int32_t (*init)(struct HdfDeviceDriver *deviceDriver, struct CameraDevice *camDev);
    int32_t (*deinit)(struct HdfDeviceDriver *deviceDriver, struct CameraDevice *camDev);
};

struct CameraDevice *CameraDeviceInit(const char *ifName, uint32_t len);
int32_t CameraDeviceDeInit(struct CameraDevice *camDev);
struct CameraDevice *CameraDeviceGetInstByName(const char *name);
struct HdfDeviceDriverFactory *HdfDeviceGetDeviceDriverByName(const char *driverName);
int32_t HdfCameraRegDeviceDriver(struct HdfDeviceDriverFactory *obj);
int32_t DeviceDriverMgrDeInit(void);
struct CameraDevice *CameraDeviceGetInstByName(const char *name);
struct HdfDeviceDriverManager *HdfCameraGetDeviceDriverMgr(void);

#endif  // HDF_CAMERA_DEVICE_MANAGER_H