/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "securec.h"
#include "utils/hdf_log.h"
#include "hdf_camera_dispatch.h"
#include "hdf_camera_sensor.h"
#include "hdf_camera_device_manager.h"
#include "hdf_camera_utils.h"
#include "osal_mem.h"

#define HDF_LOG_TAG HDF_CAMERA_CORE

static int32_t CameraOpenCamera(struct HdfDeviceIoClient *client, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t permissionId, ret;

    if (client == NULL || reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(reqData, &permissionId)) {
        HDF_LOGE("%s: Read request data failed! permissionId = %d", __func__, permissionId);
        return HDF_FAILURE;
    }
    const char *ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: fail to get ifname!", __func__);
        return HDF_FAILURE;
    } else {
        HDF_LOGE("%s: get ifname = %s", __func__, ifName);
    }
    ret = HdfCameraAddIdMap(permissionId);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdfCameraAddIdMap failed!", __func__);
        return ret;
    }

    return ret;
}

static int32_t CameraCloseCamera(struct HdfDeviceIoClient *client, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t permissionId, ret;

    if (client == NULL || reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(reqData, &permissionId)) {
        HDF_LOGE("%s: Read request data failed! permissionId = %d", __func__, permissionId);
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(JudgePermission(permissionId));
    const char *ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: fail to get ifname!", __func__);
        return HDF_FAILURE;
    } else {
        HDF_LOGE("%s: get ifname = %s", __func__, ifName);
    }
    ret = HdfCameraClearIdMap(permissionId);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HdfCameraClearIdMap failed!", __func__);
        return ret;
    }

    return ret;
}

static int32_t CameraCmdPowerUp(struct HdfDeviceIoClient *client, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t type, permissionId, ret;

    if (client == NULL || reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(reqData, &type)) {
        HDF_LOGE("%s: Read request data failed! type = %d", __func__, type);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &permissionId)) {
        HDF_LOGE("%s: Read request data failed! permissionId = %d", __func__, permissionId);
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(JudgePermission(permissionId));
    switch (type) {
        case SENSOR_TYPE:
            ret = CameraCmdSensorPowerUp(type, reqData, rspData);
            break;
        case ISP_TYPE:
            HDF_LOGE("%s: type: %d isp is not support.", __func__, type);
            break;
        case VCM_TYPE:
            HDF_LOGE("%s: type: %d vcm is not support.", __func__, type);
            break;
        case LENS_TYPE:
            HDF_LOGE("%s: type: %d lens is not support.", __func__, type);
            break;
        case FLASH_TYPE:
            HDF_LOGE("%s: type: %d flash is not support.", __func__, type);
            break;
        case UVC_TYPE:
            HDF_LOGE("%s: type: %d uvc is not support.", __func__, type);
            break;
        default:
            HDF_LOGE("%s: wrong type: %d", __func__, type);
            return HDF_ERR_NOT_SUPPORT;
    }

    return ret;
}

static int32_t CameraCmdPowerDown(struct HdfDeviceIoClient *client, struct HdfSBuf *reqData,
                                struct HdfSBuf *rspData)
{
    int32_t type, permissionId, ret;

    if (client == NULL || reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(reqData, &type)) {
        HDF_LOGE("%s: Read request data failed! type = %d", __func__, type);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &permissionId)) {
        HDF_LOGE("%s: Read request data failed! permissionId = %d", __func__, permissionId);
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(JudgePermission(permissionId));
    switch (type) {
        case SENSOR_TYPE:
            ret = CameraCmdSensorPowerDown(type, reqData, rspData);
            break;
        case ISP_TYPE:
            HDF_LOGE("%s: type: %d isp is not support.", __func__, type);
            break;
        case VCM_TYPE:
            HDF_LOGE("%s: type: %d vcm is not support.", __func__, type);
            break;
        case LENS_TYPE:
            HDF_LOGE("%s: type: %d lens is not support.", __func__, type);
            break;
        case FLASH_TYPE:
            HDF_LOGE("%s: type: %d flash is not support.", __func__, type);
            break;
        case UVC_TYPE:
            HDF_LOGE("%s: type: %d uvc is not support.", __func__, type);
            break;
        default:
            HDF_LOGE("%s: wrong type: %d", __func__, type);
            return HDF_ERR_NOT_SUPPORT;
    }

    return ret;
}

static int32_t CameraCmdGetConfig(struct HdfDeviceIoClient *client, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t type, permissionId, ret;

    if (client == NULL || reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(reqData, &type)) {
        HDF_LOGE("%s: Read request data failed! type = %d", __func__, type);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &permissionId)) {
        HDF_LOGE("%s: Read request data failed! permissionId = %d", __func__, permissionId);
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(JudgePermission(permissionId));
    switch (type) {
        case SENSOR_TYPE:
            ret = CameraCmdSensorGetConfig(type, reqData, rspData);
            break;
        case ISP_TYPE:
            HDF_LOGE("%s: type: %d isp is not support.", __func__, type);
            break;
        case VCM_TYPE:
            HDF_LOGE("%s: type: %d vcm is not support.", __func__, type);
            break;
        case LENS_TYPE:
            HDF_LOGE("%s: type: %d lens is not support.", __func__, type);
            break;
        case FLASH_TYPE:
            HDF_LOGE("%s: type: %d flash is not support.", __func__, type);
            break;
        case UVC_TYPE:
            HDF_LOGE("%s: type: %d uvc is not support.", __func__, type);
            break;
        default:
            HDF_LOGE("%s: wrong type: %d", __func__, type);
            return HDF_ERR_NOT_SUPPORT;
    }

    return ret;
}

static int32_t CameraCmdSetConfig(struct HdfDeviceIoClient *client, struct HdfSBuf *reqData,
                                struct HdfSBuf *rspData)
{
    int32_t type, permissionId, ret;

    if (client == NULL || reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(reqData, &type)) {
        HDF_LOGE("%s: Read request data failed! type = %d", __func__, type);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &permissionId)) {
        HDF_LOGE("%s: Read request data failed! permissionId = %d", __func__, permissionId);
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(JudgePermission(permissionId));
    switch (type) {
        case SENSOR_TYPE:
            ret = CameraCmdSensorSetConfig(type, reqData, rspData);
            break;
        case ISP_TYPE:
            HDF_LOGE("%s: type: %d isp is not support.", __func__, type);
            break;
        case VCM_TYPE:
            HDF_LOGE("%s: type: %d vcm is not support.", __func__, type);
            break;
        case LENS_TYPE:
            HDF_LOGE("%s: type: %d lens is not support.", __func__, type);
            break;
        case FLASH_TYPE:
            HDF_LOGE("%s: type: %d flash is not support.", __func__, type);
            break;
        case UVC_TYPE:
            HDF_LOGE("%s: type: %d uvc is not support.", __func__, type);;
            break;
        default:
            HDF_LOGE("%s: wrong type: %d", __func__, type);
            return HDF_ERR_NOT_SUPPORT;
    }

    return ret;
}

static struct CameraCmdHandleList g_cameraCmdHandle[] = {
    {CMD_OPEN_CAMERA, CameraOpenCamera},
    {CMD_CLOSE_CAMERA, CameraCloseCamera},
    {CMD_POWER_UP, CameraCmdPowerUp},
    {CMD_POWER_DOWN, CameraCmdPowerDown},
    {CMD_GET_CONFIG, CameraCmdGetConfig},
    {CMD_SET_CONFIG, CameraCmdSetConfig},
};

int32_t HdfCameraDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    uint32_t i;
    HDF_LOGE("%s: [cmdId = %d].", __func__, cmdId);
    if ((client == NULL) || (reqData == NULL)) {
        HDF_LOGE("%s: Input params check error: client=%p, data=%p.", __func__, client, reqData);
        return HDF_FAILURE;
    }

    if ((cmdId >= CAMERA_MAX_CMD_ID) || (cmdId < 0)) {
        HDF_LOGE("%s: Invalid [cmdId=%d].", __func__, cmdId);
        return HDF_FAILURE;
    }

    for (i = 0; i < HDF_ARRAY_SIZE(g_cameraCmdHandle); ++i) {
        if ((cmdId == (int)(g_cameraCmdHandle[i].cmd)) && (g_cameraCmdHandle[i].func != NULL)) {
            return g_cameraCmdHandle[i].func(client, reqData, rspData);
        }
    }
    return HDF_FAILURE;
}