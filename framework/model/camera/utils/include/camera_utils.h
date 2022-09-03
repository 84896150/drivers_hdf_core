/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef CAMERA_UTILS_H
#define CAMERA_UTILS_H

#include <utils/hdf_base.h>
#include "camera_device_manager.h"

#define MAX_ID_COUNT 10

int32_t HdfCameraAddIdMap(int32_t id);
int32_t HdfCameraClearIdMap(int32_t id);
struct CameraDevice *AllocPlatformCameraDevice(const char *driverName);
struct CameraDeviceDriver *GetDeviceDriver(const struct CameraDevice *cameraDev);
int32_t ReleasePlatformCameraDevice(struct CameraDevice *cameraDev);
char *HdfCameraGetIfName(void);
int32_t CheckPermission(int permissionId);
int32_t CheckCameraDevice(const char *ifName, int type);
int32_t GetDeviceNum(const char *driverName, int camId, int type);
const char *HdfCameraGetCameraId(char *ifName, const char *driverName, int type);
int32_t CheckFormatNum(int camId, const char *driverName, int type, struct CameraCtrlConfig *ctrlConfig);
int32_t CheckFrameRate(int camId, const char *driverName, int type, struct CameraCtrlConfig *ctrlConfig);
int32_t CheckFmtType(int camId, const char *driverName, int type, struct CameraCtrlConfig *ctrlConfig);

#endif  // CAMERA_UTILS_H