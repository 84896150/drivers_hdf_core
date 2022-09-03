/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_CAMERA_SENSOR_H
#define HDF_CAMERA_SENSOR_H

#include "hdf_device_desc.h"
#include "hdf_base.h"
#include "hdf_sbuf.h"
#include "hdf_types.h"

int32_t CameraCmdSensorPowerUp(int type, struct HdfSBuf *reqData, struct HdfSBuf *rspData);
int32_t CameraCmdSensorPowerDown(int type, struct HdfSBuf *reqData, struct HdfSBuf *rspData);
int32_t CameraCmdSensorGetConfig(int type, struct HdfSBuf *reqData, struct HdfSBuf *rspData);
int32_t CameraCmdSensorSetConfig(int type, struct HdfSBuf *reqData, struct HdfSBuf *rspData);

#endif  // HDF_CAMERA_SENSOR_H