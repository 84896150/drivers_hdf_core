/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_CAMERA_UTILS_H
#define HDF_CAMERA_UTILS_H

#include "hdf_base.h"
#include "hdf_camera_device_manager.h"

#define MAX_ID_COUNT 10

int32_t HdfCameraAddIdMap(int32_t id);
int32_t HdfCameraClearIdMap(int32_t id);
struct CameraDevice *AllocPlatformCameraDevice(const char *driverName);
struct HdfDeviceDriver *GetDeviceDriver(const struct CameraDevice *cameraDev);
int32_t ReleasePlatformCameraDevice(struct CameraDevice *camDev);
char *HdfCameraGetIfNames(void);
int32_t JudgePermission(int permissionId);
int32_t JudgeCameraDevice(const char *ifName, int type);
int32_t JudgeDeviceNum(const char *driverName, int camId, int type);
const char *HdfCameraGetCameraId(char *ifName, const char *driverName, int type);
int32_t JudgeFormatNum(int camId, const char *driverName, int type, struct CameraCtrlConfig *ctrlConfig);
int32_t JudgeFrameRate(int camId, const char *driverName, int type, struct CameraCtrlConfig *ctrlConfig);
int32_t JudgeFmtType(int camId, const char *driverName, int type, struct CameraCtrlConfig *ctrlConfig);

#endif  // HDF_CAMERA_UTILS_H