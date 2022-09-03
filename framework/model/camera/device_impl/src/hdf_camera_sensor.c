/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_camera_utils.h"
#include "hdf_camera_event.h"
#include "hdf_log.h"
#include "hdf_camera_device_manager.h"
#include "hdf_camera_config_parser.h"
#include "hdf_camera_sensor.h"
#include "securec.h"

#define HDF_LOG_TAG HDF_CAMERA_CORE

static int32_t CameraSensorPowerUp(struct CameraDevice *camDev, const char *driverName)
{
    struct HdfDeviceDriver *deviceDriver = GetDeviceDriver(camDev);
    if (deviceDriver == NULL) {
        HDF_LOGE("%s: bad camera device found!", __func__);
        return HDF_FAILURE;
    }
    return deviceDriver->sensor.devOps.SensorPowerUp(camDev, driverName);
}

static int32_t CameraSensorPowerDown(struct CameraDevice *camDev, const char *driverName)
{
    struct HdfDeviceDriver *deviceDriver = GetDeviceDriver(camDev);
    if (deviceDriver == NULL) {
        HDF_LOGE("%s: bad camera device found!", __func__);
        return HDF_FAILURE;
    }
    return deviceDriver->sensor.devOps.SensorPowerDown(camDev, driverName);
}

static int32_t CameraSensorGetConfig(struct CameraDevice *camDev, const char *driverName,
                                uint8_t cmdId, struct CameraCtrlConfig *ctrlConfig)
{
    int32_t ret;
    struct HdfDeviceDriver *deviceDriver = GetDeviceDriver(camDev);
    if (deviceDriver == NULL) {
        HDF_LOGE("%s: bad camera device found!", __func__);
        return HDF_FAILURE;
    }

    switch (cmdId) {
        case CAMERA_CMD_G_CONFIG:
            ret = deviceDriver->sensor.devOps.SensorGetConfig(ctrlConfig, driverName,
                            CAMERA_CMD_G_CONFIG);
            break;
        case CAMERA_CMD_G_FMT:
            ret = deviceDriver->sensor.devOps.SensorGetConfig(ctrlConfig, driverName,
                            CAMERA_CMD_G_FMT);
            break;
        case CAMERA_CMD_G_CROP:
            ret = deviceDriver->sensor.devOps.SensorGetConfig(ctrlConfig, driverName,
                            CAMERA_CMD_G_CROP);
            break;
        case CAMERA_CMD_G_PARM:
            ret = deviceDriver->sensor.devOps.SensorGetConfig(ctrlConfig, driverName,
                            CAMERA_CMD_G_PARM);
            break;
        case CAMERA_CMD_G_CROPCAP:
            ret = deviceDriver->sensor.devOps.SensorGetConfig(ctrlConfig, driverName,
                            CAMERA_CMD_G_CROPCAP);
            break;
        default:
            HDF_LOGE("%s: unknow command", __func__);
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }

    return ret;
}

static int32_t CameraGetFmtData(struct HdfSBuf *rspData, struct CameraCtrlConfig ctrlConfig)
{
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.pixel.format);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.pixel.width);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.pixel.height);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.pixel.sizeImage);
    CHECK_RETURN_RET(isSerializeFailed);
    return HDF_SUCCESS;
}

static int32_t CameraGetCropData(struct HdfSBuf *rspData, struct CameraCtrlConfig ctrlConfig)
{
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(rspData, ctrlConfig.pixelFormat.cop.left);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteInt32(rspData, ctrlConfig.pixelFormat.cop.top);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cop.width);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cop.height);
    CHECK_RETURN_RET(isSerializeFailed);
    return HDF_SUCCESS;
}

static int32_t CameraGetParmData(struct HdfSBuf *rspData, struct CameraCtrlConfig ctrlConfig)
{
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.fps.numerator);
    isSerializeFailed = isSerializeFailed || !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.fps.denominator);
    CHECK_RETURN_RET(isSerializeFailed);
    return HDF_SUCCESS;
}

static int32_t CameraGetCropCapData(struct HdfSBuf *rspData, struct CameraCtrlConfig ctrlConfig)
{
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed ||
        !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.bounds.height);
    isSerializeFailed = isSerializeFailed ||
        !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.bounds.top);
    isSerializeFailed = isSerializeFailed ||
        !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.bounds.width);
    isSerializeFailed = isSerializeFailed ||
        !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.defrect.height);
    isSerializeFailed = isSerializeFailed ||
        !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.defrect.left);
    isSerializeFailed = isSerializeFailed ||
        !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.defrect.top);
    isSerializeFailed = isSerializeFailed ||
        !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.defrect.width);
    isSerializeFailed = isSerializeFailed ||
        !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.pixelaspect.denominator);
    isSerializeFailed = isSerializeFailed ||
        !HdfSbufWriteUint32(rspData, ctrlConfig.pixelFormat.cropCap.pixelaspect.numerator);
    CHECK_RETURN_RET(isSerializeFailed);
    return HDF_SUCCESS;
}

static int32_t CameraGetConfigSwitchCtrlId(const char *ifName, const char *driverName, struct HdfSBuf *reqData,
                                    struct CameraCtrlConfig *ctrlConfig)
{
    uint32_t camId = 0;
    uint32_t num;
    struct HdfConfigCameraRoot *hdfSensorRoot = NULL;

    sscanf(ifName, "%d*%s", camId);
    hdfSensorRoot = HdfCameraGetConfigRoot();
    if (!HdfSbufReadUint32(reqData, &ctrlConfig->ctrl.id)) {
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "ctrlConfig.ctrl.id");
        return HDF_FAILURE;
    }
    num = JudgeDeviceNum(driverName, camId, SENSOR_TYPE);
    HDF_LOGE("%s: deviceNum = %d", __func__, num);
    switch (ctrlConfig->ctrl.id) {
        case CAMERA_CMD_G_EXPOSURE_MODE:
            if ((hdfSensorRoot->cameraConfig.abilityConfig.deviceConfig[camId].sensor.sensor[num].exposure) == 0) {
                HDF_LOGE("%s: senor not support set exposure", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;

        case CAMERA_CMD_G_HFLIP:
            if ((hdfSensorRoot->cameraConfig.abilityConfig.deviceConfig[camId].sensor.sensor[num].mirror) == 0) {
                HDF_LOGE("%s: senor not support set mirror", __func__);
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
        HDF_LOGE("%s: camDev not found! ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    switch (id) {
        case CAMERA_CMD_G_CONFIG:
            CHECK_RETURN_RET(CameraGetConfigSwitchCtrlId(ifName, driverName, reqData, &ctrlConfig));
            ret = CameraSensorGetConfig(camDev, driverName, CAMERA_CMD_G_CONFIG, &ctrlConfig);
            if (!HdfSbufWriteUint32(rspData, ctrlConfig.ctrl.value)) {
                HDF_LOGE("%s: Write request data failed! ParamName=%s", __func__, "value");
                return HDF_FAILURE;
            }
            break;
        case CAMERA_CMD_G_FMT:
            ret = CameraSensorGetConfig(camDev, driverName, CAMERA_CMD_G_FMT, &ctrlConfig);
            CHECK_RETURN_RET(CameraGetFmtData(rspData, ctrlConfig));
            break;
        case CAMERA_CMD_G_CROP:
            ret = CameraSensorGetConfig(camDev, driverName, CAMERA_CMD_G_CROP, &ctrlConfig);
            CHECK_RETURN_RET(CameraGetCropData(rspData, ctrlConfig));
            break;
        case CAMERA_CMD_G_PARM:
            ret = CameraSensorGetConfig(camDev, driverName, CAMERA_CMD_G_PARM, &ctrlConfig);
            CHECK_RETURN_RET(CameraGetParmData(rspData, ctrlConfig));
            break;
        case CAMERA_CMD_G_CROPCAP:
            ret = CameraSensorGetConfig(camDev, driverName, CAMERA_CMD_G_CROPCAP, &ctrlConfig);
            CHECK_RETURN_RET(CameraGetCropCapData(rspData, ctrlConfig));
            break;
        default:
            HDF_LOGE("%s: unknow command", __func__);
            return HDF_ERR_NOT_SUPPORT;
    }

    return ret;
}

static int32_t CameraSensorSetConfig(struct CameraDevice *camDev, const char *driverName,
                        uint8_t cmdId, struct CameraCtrlConfig *ctrlConfig)
{
    int32_t ret;
    struct HdfDeviceDriver *deviceDriver = GetDeviceDriver(camDev);
    if (deviceDriver == NULL) {
        HDF_LOGE("%s: bad camera device found!", __func__);
        return HDF_FAILURE;
    }

    switch (cmdId) {
        case CAMERA_CMD_S_CONFIG:
            ret = deviceDriver->sensor.devOps.SensorSetConfig(ctrlConfig,
                        driverName, CAMERA_CMD_S_CONFIG);
            break;
        case CAMERA_CMD_S_FMT:
            ret = deviceDriver->sensor.devOps.SensorSetConfig(ctrlConfig,
                        driverName, CAMERA_CMD_S_FMT);
            break;
        case CAMERA_CMD_S_CROP:
            ret = deviceDriver->sensor.devOps.SensorSetConfig(ctrlConfig,
                        driverName, CAMERA_CMD_S_CROP);
            break;
        case CAMERA_CMD_S_PARM:
            ret = deviceDriver->sensor.devOps.SensorSetConfig(ctrlConfig,
                        driverName, CAMERA_CMD_S_PARM);
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
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->ctrl.id);
    isSerializeFailed = isSerializeFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->ctrl.value);
    CHECK_RETURN_RET(isSerializeFailed);
    return HDF_SUCCESS;
}

static int32_t CameraSetFmtData(int camId, const char *driverName,
                            struct HdfSBuf *reqData, struct CameraCtrlConfig *ctrlConfig)
{
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.pixel.format);
    isSerializeFailed = isSerializeFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.pixel.width);
    isSerializeFailed = isSerializeFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.pixel.height);
    CHECK_RETURN_RET(isSerializeFailed);
    CHECK_RETURN_RET(JudgeFmtType(camId, driverName, SENSOR_TYPE, ctrlConfig));
    CHECK_RETURN_RET(JudgeFormatNum(camId, driverName, SENSOR_TYPE, ctrlConfig));
    return HDF_SUCCESS;
}

static int32_t CameraSetCropData(struct HdfSBuf *reqData, struct CameraCtrlConfig *ctrlConfig)
{
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufReadInt32(reqData, &ctrlConfig->pixelFormat.cop.left);
    isSerializeFailed = isSerializeFailed || !HdfSbufReadInt32(reqData, &ctrlConfig->pixelFormat.cop.top);
    isSerializeFailed = isSerializeFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.cop.width);
    isSerializeFailed = isSerializeFailed || !HdfSbufReadUint32(reqData, &ctrlConfig->pixelFormat.cop.height);
    CHECK_RETURN_RET(isSerializeFailed);
    return HDF_SUCCESS;
}

static int32_t CameraSetParmData(int camId, const char *driverName,
                            struct HdfSBuf *reqData, struct CameraCtrlConfig *ctrlConfig)
{
    bool isSerializeFailed = false;
    isSerializeFailed = isSerializeFailed || !HdfSbufReadUint32(reqData,
                                            &ctrlConfig->pixelFormat.fps.denominator);
    isSerializeFailed = isSerializeFailed || !HdfSbufReadUint32(reqData,
                                            &ctrlConfig->pixelFormat.fps.numerator);
    CHECK_RETURN_RET(isSerializeFailed);
    CHECK_RETURN_RET(JudgeFrameRate(camId, driverName, SENSOR_TYPE, ctrlConfig));
    return HDF_SUCCESS;
}

static int32_t CameraSetConfigSwitchCtrlId(int camId, const char *driverName, struct HdfSBuf *reqData,
                                    struct CameraCtrlConfig ctrlConfig)
{
    uint32_t num;
    struct HdfConfigCameraRoot *hdfSensorRoot = NULL;

    hdfSensorRoot = HdfCameraGetConfigRoot();
    if (!HdfSbufReadUint32(reqData, &ctrlConfig.ctrl.id)) {
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "ctrlConfig.ctrl.id");
        return HDF_FAILURE;
    }
    num = JudgeDeviceNum(driverName, camId, SENSOR_TYPE);
    HDF_LOGE("%s: deviceNum = %d", __func__, num);
    switch (ctrlConfig.ctrl.id) {
        case CAMERA_CMD_S_EXPOSURE_MODE:
            if ((hdfSensorRoot->cameraConfig.abilityConfig.deviceConfig[camId].sensor.sensor[num].exposure) == 0) {
                HDF_LOGE("%s: senor not support set exposure", __func__);
                return HDF_ERR_NOT_SUPPORT;
            }
            break;
        case CAMERA_CMD_S_HFLIP:
            if ((hdfSensorRoot->cameraConfig.abilityConfig.deviceConfig[camId].sensor.sensor[num].mirror) == 0) {
                HDF_LOGE("%s: senor not support set mirror", __func__);
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

    sscanf(ifName, "%d*%s", camId);
    camDev = CameraDeviceGetInstByName(ifName);
    if (camDev == NULL) {
        HDF_LOGE("%s: camDev not found! ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    switch (id) {
        case CAMERA_CMD_S_CONFIG:
            CHECK_RETURN_RET(CameraSetConfigSwitchCtrlId(camId, driverName, reqData, ctrlConfig));
            CHECK_RETURN_RET(CameraSetCtrlData(reqData, &ctrlConfig));
            ret = CameraSensorSetConfig(camDev, driverName, CAMERA_CMD_S_CONFIG, &ctrlConfig);
            break;
        case CAMERA_CMD_S_FMT:
            CHECK_RETURN_RET(CameraSetFmtData(camId, driverName, reqData, &ctrlConfig));
            ret = CameraSensorSetConfig(camDev, driverName, CAMERA_CMD_S_FMT, &ctrlConfig);
            break;
        case CAMERA_CMD_S_CROP:
            CHECK_RETURN_RET(CameraSetCropData(reqData, &ctrlConfig));
            ret = CameraSensorSetConfig(camDev, driverName, CAMERA_CMD_S_CROP, &ctrlConfig);
            break;
        case CAMERA_CMD_S_PARM:
            CHECK_RETURN_RET(CameraSetParmData(camId, driverName, reqData, &ctrlConfig));
            ret = CameraSensorSetConfig(camDev, driverName, CAMERA_CMD_S_PARM, &ctrlConfig);
            break;
        default:
            HDF_LOGE("%s: unknow command", __func__);
            return HDF_ERR_NOT_SUPPORT;
    }

    return ret;
}

int32_t CameraCmdSensorPowerUp(int type, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct CameraDevice *camDev = NULL;
    const char *ifName = NULL;
    const const char *driverName = NULL;

    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "ifName");
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(JudgeCameraDevice(ifName, type));
    driverName = HdfSbufReadString(reqData);
    if (driverName == NULL) {
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "driverName");
        return HDF_FAILURE;
    }
    camDev = CameraDeviceGetInstByName(ifName);
    if (camDev == NULL) {
        HDF_LOGE("%s: camDev not found! ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    ret = CameraSensorPowerUp(camDev, driverName);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: CameraSensorPowerUp failed, ret = %d", __func__, ret);
    }

    return ret;
}

int32_t CameraCmdSensorPowerDown(int type, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct CameraDevice *camDev = NULL;
    const char *ifName = NULL;
    const const char *driverName = NULL;

    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "ifName");
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(JudgeCameraDevice(ifName, type));
    driverName = HdfSbufReadString(reqData);
    if (driverName == NULL) {
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "driverName");
        return HDF_FAILURE;
    }
    camDev = CameraDeviceGetInstByName(ifName);
    if (camDev == NULL) {
        HDF_LOGE("%s: camDev not found! ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    ret = CameraSensorPowerDown(camDev, driverName);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to power down sensor, ret = %d", __func__, ret);
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
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "id");
        return HDF_FAILURE;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "ifName");
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(JudgeCameraDevice(ifName, type));
    driverName = HdfSbufReadString(reqData);
    if (driverName == NULL) {
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "driverName");
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
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "id");
        return HDF_FAILURE;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "ifName");
        return HDF_FAILURE;
    }
    CHECK_RETURN_RET(JudgeCameraDevice(ifName, type));
    driverName = HdfSbufReadString(reqData);
    if (driverName == NULL) {
        HDF_LOGE("%s: Read request data failed! ParamName=%s", __func__, "driverName");
        return HDF_FAILURE;
    }
    ret = CameraSetConfigSwitchId(id, ifName, driverName, reqData, rspData);

    return ret;
}