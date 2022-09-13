/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef CAMERA_EVENT_H
#define CAMERA_EVENT_H

#include <utils/hdf_base.h>
#include <utils/hdf_sbuf.h>
#include <core/hdf_device_desc.h>

typedef enum {
    CAMERA_OPEN_CAMERA_RESULT,
    CAMERA_CLOSE_CAMERA_RESULT,
    CAMERA_POWER_UP_RESULT,
    CAMERA_POWER_DOWN_RESULT,
    CAMERA_CTRL_CONFIG_RESULT,
    CAMERA_ENUM_DEVICES_RESULT,
    CAMERA_GET_STATUS_RESULT,
    CAMERA_GET_ABILITY_RESULT,
    CAMERA_REQ_MEMORY_RESULT,
    CAMERA_QUERT_MEMORY_RESULT,
    CAMERA_STREAM_QUEUE_RESULT,
    CAMERA_STREAM_DEQUEUE_RESULT,
    CAMERA_STREAM_ON_RESULT,
    CAMERA_STREAM_OFF_RESULT,
    CAMERA_TEST_RESULT
} CameraRepotEventResult;

int32_t HdfCameraSendEvent(uint32_t eventId, const struct HdfSBuf *data);
int32_t HdfCameraAddDevice(struct HdfDeviceObject *device);
void HdfCameraDelDevice(void);

#endif  // CAMERA_EVENT_H