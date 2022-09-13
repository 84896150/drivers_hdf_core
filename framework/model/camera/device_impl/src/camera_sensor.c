/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "camera_sensor.h"
#include "securec.h"
#include <utils/hdf_log.h>
#include "camera_config_parser.h"
#include "camera_device_manager.h"
#include "camera_event.h"
#include "camera_utils.h"

#define HDF_LOG_TAG HDF_CAMERA_CORE

static int32_t CameraSensorPowerUp(struct CameraDevice *camDev)
{
    struct CameraDeviceDriver *deviceDriver = GetDeviceDriver(camDev);
    if (deviceDriver == NULL) {
        HDF_LOGE("%s: deviceDriver ptr is null!", __func__);
        return HDF_FAILURE;
    }
    if (deviceDriver->sensor.devOps.SensorPowerUp == NULL) {
        HDF_LOGE("%s: SensorPowerUp ptr is null!", __func__);
        return HDF_FAILURE;
    }

    return deviceDriver->sensor.devOps.SensorPowerUp(camDev);
}

static int32_t CameraSensorPowerDown(struct CameraDevice *camDev)
{
    struct CameraDeviceDriver *deviceDriver = GetDeviceDriver(camDev);
    if (deviceDriver == NULL) {
        HDF_LOGE("%s: deviceDriver ptr is null!", __func__);
        return HDF_FAILURE;
    }
    if (deviceDriver->sensor.devOps.SensorPowerDown == NULL) {
        HDF_LOGE("%s: SensorPowerDown ptr is null!", __func__);
        return HDF_FAILURE;
    }
    return deviceDriver->sensor.devOps.SensorPowerDown(camDev);
}

static int32_t CameraSensorGetConfig(struct CameraDevice *camDev, uint8_t cmdId, struct CameraCtrlConfig *ctrlConfig)
{
    int32_t ret;
    struct CameraDeviceDriver *deviceDriver = GetDeviceDriver(camDev);
    if (deviceDriver == NULL) {
        HDF_LOGE("%s: deviceDriver ptr is null!", __func__);
        return HDF_FAILURE;
    }
    if (deviceDriver->sensor.devOps.SensorGetConfig == NULL) {
        HDF_LOGE("%s: SensorGetConfig ptr is null!", __func__);
        return HDF_FAILURE;
    }
    switch (cmdId) {
        case CAMERA_CMD_GET_CONFIG:
            ret = deviceDriver->sensor.devOps.SensorGetConfig(ctrlConfig, camDev->driverName,
                            CAMERA_CMD_GET_CONFIG);
            break;
        case CAMERA_CMD_GET_FMT:
            ret = deviceDriver->sensor.devOps.SensorGetConfig(ctrlConfig, camDev->driverName,
                            CAMERA_CMD_GET_FMT);
            break;
        case CAMERA_CMD_GET_CROP:
            ret = deviceDriver->sensor.devOps.SensorGetConfig(ctrlConfig, camDev->driverName,
                            CAMERA_CMD_GET_CROP);
            break;
        case CAMERA_CMD_GET_PARM:
            ret = deviceDriver->sensor.devOps.SensorGetConfig(ctrlConfig, camDev->driverName,
                            CAMERA_CMD_GET_PARM);
            break;
        case CAMERA_CMD_GET_CROPCAP:
            ret = deviceDriver->sensor.devOps.SensorGetConfig(ctrlConfig, camDev->driverName,
                            CAMERA_CMD_GET_CROPCAP);
            break;
        default:
            HDF_LOGE("%s: unknow command", __func__);
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }

    return ret;
}

static int32_t CameraGetCtrlData(struct HdfSBuf *rspData, struct CameraCtrlConfig ctrlConfig)
{
    bool isFailed = false;
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.ctrl.value);
    CHECK_RETURN_RET(isFailed);
    return HDF_SUCCESS;
}

static int32_t CameraGetFmtData(struct HdfSBuf *rspData, struct CameraCtrlConfig ctrlConfig)
{
    bool isFailed = false;
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.pixel.format);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.pixel.width);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.pixel.height);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.pixel.sizeImage);
    CHECK_RETURN_RET(isFailed);
    return HDF_SUCCESS;
}

static int32_t CameraGetCropData(struct HdfSBuf *rspData, struct CameraCtrlConfig ctrlConfig)
{
    bool isFailed = false;
    isFailed = isFailed || !HdfSbufWriteInt32(rspData, ctrlConfig.pixelFormat.cop.left);
    isFailed = isFailed || !HdfSbufWriteInt32(rspData, ctrlConfig.pixelFormat.cop.top);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cop.width);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cop.height);
    CHECK_RETURN_RET(isFailed);
    return HDF_SUCCESS;
}

static int32_t CameraGetParmData(struct HdfSBuf *rspData, struct CameraCtrlConfig ctrlConfig)
{
    bool isFailed = false;
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.fps.numerator);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.fps.denominator);
    CHECK_RETURN_RET(isFailed);
    return HDF_SUCCESS;
}

static int32_t CameraGetCropCapData(struct HdfSBuf *rspData, struct CameraCtrlConfig ctrlConfig)
{
    bool isFailed = false;
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.bounds.height);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.bounds.top);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.bounds.width);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.defrect.height);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.defrect.left);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.defrect.top);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.defrect.width);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.pixelaspect.denominator);
    isFailed = isFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.pixelaspect.numerator);
    CHECK_RETURN_RET(isFailed);
    return HDF_SUCCESS;
}

static int32_t CameraGetConfigSwitchCtrlId(const char *ifName, const char *driverName,
    struct HdfSBuf *reqData, struct CameraCtrlConfig *ctrlConfig)
{
    uint32_t camId = 0;
    uint32_t num;
    struct CameraConfigRoot *SensorConfigRoot = NULL;
    if (ifName == NULL) {
        HDF_LOGE("%s: get ifName failed!", __func__);
        return HDF_FAILURE;
    }
    sscanf(ifName, "%d*%s", camId);
    SensorConfigRoot = HdfCameraGetConfigRoot();
    if (SensorConfigRoot == NULL) {
        HDF_LOGE("%s: get SensorConfigRoot failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &ctrlConfig->ctrl.id)) {
        HDF_LOGE("%s: Read ctrlConfig.ctrl.id failed!", __func__);
        return HDF_FAILURE;
    }
    num = GetDeviceNum(driverName, camId, SENSOR_TYPE);
    HDF_LOGD("%s: deviceNum = %{public}d", __func__, num);
    switch (ctrlConfig->ctrl.id) {
        case CAMERA_CMD_GET_EXPOSURE_MODE:
            if ((SensorConfigRoot->deviceConfig[camId].sensor.sensor[num].exposure) == 0) {
                HDF_LOGD("%s: senor not support set exposure", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;

        case CAMERA_CMD_GET_HFLIP:
            if ((SensorConfigRoot->deviceConfig[camId].sensor.sensor[num].mirror) == 0) {
                HDF_LOGD("%s: senor not support set mirror", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;

        default:
            HDF_LOGE("%s: unknow command", __func__);
            return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t CameraGetConfigSwitchId(int id, const char *ifName, const char *driverName,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret = 0;
    struct CameraDevice *camDev = NULL;
    struct CameraCtrlConfig ctrlConfig = {};

    camDev = CameraDeviceGetInstByName(ifName);
    if (camDev == NULL) {
        HDF_LOGE("%s: camDev not found! ifName=%{public}s", __func__, ifName);
        return HDF_FAILURE;
    }
    if (strcpy_s(camDev->driverName, DRIVER_NAME_SIZE, driverName) != EOK) {
        HDF_LOGE("%s: fail: strcpy_s driverName fail!", __func__);
        return HDF_FAILURE;
    }
    switch (id) {
        case CAMERA_CMD_GET_CONFIG:
            CHECK_RETURN_RET(CameraGetConfigSwitchCtrlId(ifName, camDev->driverName, reqData, &ctrlConfig));
            ret = CameraSensorGetConfig(camDev, CAMERA_CMD_GET_CONFIG, &ctrlConfig);
            CHECK_RETURN_RET(CameraGetCtrlData(rspData, ctrlConfig));
            break;
        case CAMERA_CMD_GET_FMT:
            ret = CameraSensorGetConfig(camDev, CAMERA_CMD_GET_FMT, &ctrlConfig);
            CHECK_RETURN_RET(CameraGetFmtData(rspData, ctrlConfig));
            break;
        case CAMERA_CMD_GET_CROP:
            ret = CameraSensorGetConfig(camDev, CAMERA_CMD_GET_CROP, &ctrlConfig);
            CHECK_RETURN_RET(CameraGetCropData(rspData, ctrlConfig));
            break;
        case CAMERA_CMD_GET_PARM:
            ret = CameraSensorGetConfig(camDev, CAMERA_CMD_GET_PARM, &ctrlConfig);
            CHECK_RETURN_RET(CameraGetParmData(rspData, ctrlConfig));
            break;
        case CAMERA_CMD_GET_CROPCAP:
            ret = CameraSensorGetConfig(camDev, CAMERA_CMD_GET_CROPCAP, &ctrlConfig);
            CHECK_RETURN_RET(CameraGetCropCapData(rspData, ctrlConfig));
            break;
        default:
            HDF_LOGE("%s: unknow command", __func__);
            return HDF_ERR_NOT_SUPPORT;
    }

    return ret;
}

static int32_t CameraSensorSetConfig(struct CameraDevice *camDev, uint8_t cmdId, struct CameraCtrlConfig *ctrlConfig)
{
    int32_t ret;
    struct CameraDeviceDriver *deviceDriver = GetDeviceDriver(camDev);
    if (deviceDriver == NULL) {
        HDF_LOGE("%s: deviceDriver ptr is null!", __func__);
        return HDF_FAILURE;
    }
    if (deviceDriver->sensor.devOps.SensorSetConfig == NULL) {
        HDF_LOGE("%s: SensorSetConfig ptr is null!", __func__);
        return HDF_FAILURE;
    }
    switch (cmdId) {
        case CAMERA_CMD_SET_CONFIG:
            ret = deviceDriver->sensor.devOps.SensorSetConfig(ctrlConfig,
                        camDev->driverName, CAMERA_CMD_SET_CONFIG);
            break;
        case CAMERA_CMD_SET_FMT:
            ret = deviceDriver->sensor.devOps.SensorSetConfig(ctrlConfig,
                        camDev->driverName, CAMERA_CMD_SET_FMT);
            break;
        case CAMERA_CMD_SET_CROP:
            ret = deviceDriver->sensor.devOps.SensorSetConfig(ctrlConfig,
                        camDev->driverName, CAMERA_CMD_SET_CROP);
            break;
        case CAMERA_CMD_SET_PARM:
            ret = deviceDriver->sensor.devOps.SensorSetConfig(ctrlConfig,
                        camDev->driverName, CAMERA_CMD_SET_PARM);
            break;
        default:
            HDF_LOGE("%s: unknow command", __func__);
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }

    return ret;
}

static int32_t CameraSetCtrlData(struct HdfSBuf *reqData, struct CameraCtrlConfig *ctrlConfig)
{
    bool isFailed = false;
    isFailed = isFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->ctrl.id);
    isFailed = isFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->ctrl.value);
    CHECK_RETURN_RET(isFailed);
    return HDF_SUCCESS;
}

static int32_t CameraSetFmtData(int camId, const char *driverName,
                            struct HdfSBuf *reqData, struct CameraCtrlConfig *ctrlConfig)
{
    bool isFailed = false;
    isFailed = isFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.pixel.format);
    isFailed = isFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.pixel.width);
    isFailed = isFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.pixel.height);
    CHECK_RETURN_RET(isFailed);
    CHECK_RETURN_RET(CheckFmtType(camId, driverName, SENSOR_TYPE, ctrlConfig));
    CHECK_RETURN_RET(CheckFormatNum(camId, driverName, SENSOR_TYPE, ctrlConfig));
    return HDF_SUCCESS;
}

static int32_t CameraSetCropData(struct HdfSBuf *reqData, struct CameraCtrlConfig *ctrlConfig)
{
    bool isFailed = false;
    isFailed = isFailed || !HdfSbufReadInt32(reqData, &ctrlConfig->pixelFormat.cop.left);
    isFailed = isFailed || !HdfSbufReadInt32(reqData, &ctrlConfig->pixelFormat.cop.top);
    isFailed = isFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.cop.width);
    isFailed = isFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.cop.height);
    CHECK_RETURN_RET(isFailed);
    return HDF_SUCCESS;
}

static int32_t CameraSetParmData(int camId, const char *driverName,
                            struct HdfSBuf *reqData, struct CameraCtrlConfig *ctrlConfig)
{
    bool isFailed = false;
    isFailed = isFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.fps.denominator);
    isFailed = isFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.fps.numerator);
    CHECK_RETURN_RET(isFailed);
    CHECK_RETURN_RET(CheckFrameRate(camId, driverName, SENSOR_TYPE, ctrlConfig));
    return HDF_SUCCESS;
}

static int32_t CameraSetConfigSwitchCtrlId(int camId, const char *driverName,
    struct HdfSBuf *reqData, struct CameraCtrlConfig ctrlConfig)
{
    uint32_t num;
    struct CameraConfigRoot *SensorConfigRoot = NULL;

    SensorConfigRoot = HdfCameraGetConfigRoot();
    if (SensorConfigRoot == NULL) {
        HDF_LOGE("%s: get SensorConfigRoot failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &ctrlConfig.ctrl.id)) {
        HDF_LOGE("%s: Read ctrlConfig.ctrl.id failed!", __func__);
        return HDF_FAILURE;
    }
    num = GetDeviceNum(driverName, camId, SENSOR_TYPE);
    HDF_LOGD("%s: deviceNum = %{public}d", __func__, num);
    switch (ctrlConfig.ctrl.id) {
        case CAMERA_CMD_SET_EXPOSURE_MODE:
            if ((SensorConfigRoot->deviceConfig[camId].sensor.sensor[num].exposure) == 0) {
                HDF_LOGD("%s: senor not support set exposure", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;
        case CAMERA_CMD_SET_HFLIP:
            if ((SensorConfigRoot->deviceConfig[camId].sensor.sensor[num].mirror) == 0) {
                HDF_LOGD("%s: senor not support set mirror", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;
        default:
            HDF_LOGE("%s: unknow command", __func__);
            return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t CameraSetConfigSwitchId(int id, const char *ifName, const char *driverName,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret = 0;
    uint32_t camId = 0;
    struct CameraDevice *camDev = NULL;
    struct CameraCtrlConfig ctrlConfig = {};
    if (ifName == NULL) {
        HDF_LOGE("%s: get ifName failed!", __func__);
        return HDF_FAILURE;
    }
    sscanf(ifName, "%d*%s", camId);
    camDev = CameraDeviceGetInstByName(ifName);
    if (camDev == NULL) {
        HDF_LOGE("%s: camDev not found! ifName=%{public}s", __func__, ifName);
        return HDF_FAILURE;
    }
    if (strcpy_s(camDev->driverName, DRIVER_NAME_SIZE, driverName) != EOK) {
        HDF_LOGE("%s: fail: strcpy_s driverName fail!", __func__);
        return HDF_FAILURE;
    }
    switch (id) {
        case CAMERA_CMD_SET_CONFIG:
            CHECK_RETURN_RET(CameraSetConfigSwitchCtrlId(camId, camDev->driverName, reqData, ctrlConfig));
            CHECK_RETURN_RET(CameraSetCtrlData(reqData, &ctrlConfig));
            break;
        case CAMERA_CMD_SET_FMT:
            CHECK_RETURN_RET(CameraSetFmtData(camId, camDev->driverName, reqData, &ctrlConfig));
            break;
        case CAMERA_CMD_SET_CROP:
            CHECK_RETURN_RET(CameraSetCropData(reqData, &ctrlConfig));
            break;
        case CAMERA_CMD_SET_PARM:
            CHECK_RETURN_RET(CameraSetParmData(camId, camDev->driverName, reqData, &ctrlConfig));
            break;
        default:
            HDF_LOGE("%s: unknow command", __func__);
            return HDF_ERR_NOT_SUPPORT;
    }
    ret = CameraSensorSetConfig(camDev, id, &ctrlConfig);

    return ret;
}

int32_t CameraCmdSensorPowerUp(int type, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct CameraDevice *camDev = NULL;
    const char *ifName = NULL;
    const char *driverName = NULL;

    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: Read ifName failed!", __func__);
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(CheckCameraDevice(ifName, type));
    driverName = HdfSbufReadString(reqData);
    if (driverName == NULL) {
        HDF_LOGE("%s: Read driverName failed!", __func__);
        return HDF_FAILURE;
    }
    camDev = CameraDeviceGetInstByName(ifName);
    if (camDev == NULL) {
        HDF_LOGE("%s: camDev not found! ifName=%{public}s", __func__, ifName);
        return HDF_FAILURE;
    }
    if (strcpy_s(camDev->driverName, DRIVER_NAME_SIZE, driverName) != EOK) {
        HDF_LOGE("%s: fail: strcpy_s driverName fail!", __func__);
        return HDF_FAILURE;
    }
    ret = CameraSensorPowerUp(camDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: CameraSensorPowerUp failed, ret = %{public}d", __func__, ret);
    }

    return ret;
}

int32_t CameraCmdSensorPowerDown(int type, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct CameraDevice *camDev = NULL;
    const char *ifName = NULL;
    const char *driverName = NULL;

    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: Read ifName failed!", __func__);
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(CheckCameraDevice(ifName, type));
    driverName = HdfSbufReadString(reqData);
    if (driverName == NULL) {
        HDF_LOGE("%s: Read driverName failed", __func__);
        return HDF_FAILURE;
    }
    camDev = CameraDeviceGetInstByName(ifName);
    if (camDev == NULL) {
        HDF_LOGE("%s: camDev not found! ifName=%{public}s", __func__, ifName);
        return HDF_FAILURE;
    }
    if (strcpy_s(camDev->driverName, DRIVER_NAME_SIZE, driverName) != EOK) {
        HDF_LOGE("%s: fail: strcpy_s driverName fail!", __func__);
        return HDF_FAILURE;
    }
    ret = CameraSensorPowerDown(camDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to power down sensor, ret = %{public}d", __func__, ret);
    }

    return ret;
}

int32_t CameraCmdSensorGetConfig(int type, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret, id;
    const char *ifName = NULL;
    const char *driverName = NULL;

    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(reqData, &id)) {
        HDF_LOGE("%s: Read id failed!", __func__);
        return HDF_FAILURE;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: Read ifName failed!", __func__);
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(CheckCameraDevice(ifName, type));
    driverName = HdfSbufReadString(reqData);
    if (driverName == NULL) {
        HDF_LOGE("%s: Read driverName failed!", __func__);
        return HDF_FAILURE;
    }
    ret = CameraGetConfigSwitchId(id, ifName, driverName, reqData, rspData);

    return ret;
}

int32_t CameraCmdSensorSetConfig(int type, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret, id;
    const char *ifName = NULL;
    const char *driverName = NULL;

    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(reqData, &id)) {
        HDF_LOGE("%s: Read id failed!", __func__);
        return HDF_FAILURE;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: Read ifName failed!", __func__);
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(CheckCameraDevice(ifName, type));
    driverName = HdfSbufReadString(reqData);
    if (driverName == NULL) {
        HDF_LOGE("%s: Read driverName failed!", __func__);
        return HDF_FAILURE;
    }
    ret = CameraSetConfigSwitchId(id, ifName, driverName, reqData, rspData);

    return ret;
}