/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "hdf_camera_config_parser.h"

#define HDF_LOG_TAG HDF_CAMERA_CORE

static struct HdfConfigCameraRoot g_configCameraRoot = { 0 };

static int32_t ParseCameraSensorStatus(struct DeviceResourceIface *drsOps,
        const struct DeviceResourceNode *node, struct HdfConfigCameraSensorDevice *sensorConfig)
{
    int32_t ret;

    ret = drsOps->GetString(node, "name", &sensorConfig->name, NULL);
    CHECK_PARSER_CONFIG_RET(ret, "name");
    ret = drsOps->GetUint8(node, "id", &sensorConfig->id, 0);
    CHECK_PARSER_CONFIG_RET(ret, "id");
    ret = drsOps->GetUint8(node, "exposure", &sensorConfig->exposure, 0);
    CHECK_PARSER_CONFIG_RET(ret, "exposure");
    ret = drsOps->GetUint8(node, "mirror", &sensorConfig->mirror, 0);
    CHECK_PARSER_CONFIG_RET(ret, "mirror");
    ret = drsOps->GetUint32(node, "heightMaxNum", &sensorConfig->heightMaxNum, 0);
    CHECK_PARSER_CONFIG_RET(ret, "heightMaxNum");
    ret = drsOps->GetUint32(node, "widthMaxNum", &sensorConfig->widthMaxNum, 0);
    CHECK_PARSER_CONFIG_RET(ret, "widthMaxNum");
    ret = drsOps->GetUint32(node, "frameRateMaxNum", &sensorConfig->frameRateMaxNum, 0);
    CHECK_PARSER_CONFIG_RET(ret, "frameRateMaxNum");
    ret = drsOps->GetUint8(node, "bufferCount", &sensorConfig->bufferCount, 0);
    CHECK_PARSER_CONFIG_RET(ret, "bufferCount");

    return HDF_SUCCESS;
}

static int32_t ParseCameraSensorDevice(const struct DeviceResourceNode *node,
                                        struct HdfConfigCameraSensorDevice *sensorConfig)
{
    int32_t ret, i;
    struct DeviceResourceIface *drsOps = NULL;
    if (node == NULL || sensorConfig == NULL) {
        HDF_LOGE("%s: invalid node or sensorConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL || drsOps->GetUint8 == NULL ||
        drsOps->GetUint8Array == NULL || drsOps->GetElemNum == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = ParseCameraSensorStatus(drsOps, node, sensorConfig);
    CHECK_PARSER_CONFIG_RET(ret, "SensorStatus");
    sensorConfig->bufferTypeNum = drsOps->GetElemNum(node, "bufferType");
    if (sensorConfig->bufferTypeNum <= 0 || sensorConfig->bufferTypeNum > BUFFER_TYPE_COUNT) {
        HDF_LOGE("%s: parser bufferType element num failed! num = %d", __func__, sensorConfig->bufferTypeNum);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: get bufferType element num = %d!", __func__, sensorConfig->bufferTypeNum);

    ret = drsOps->GetUint32Array(node, "bufferType", sensorConfig->bufferType, sensorConfig->bufferTypeNum, 0);
    CHECK_PARSER_CONFIG_RET(ret, "bufferType");
    for (i = 0; i < sensorConfig->bufferTypeNum; i++) {
        HDF_LOGE("%s: get bufferType[%d] = %d", __func__, i, sensorConfig->bufferType[i]);
    }
    sensorConfig->formatTypeNum = drsOps->GetElemNum(node, "formatType");
    if (sensorConfig->formatTypeNum <= 0 || sensorConfig->formatTypeNum > FORMAT_TYPE_COUNT) {
        HDF_LOGE("%s: parser formatType element num failed! num = %d", __func__, sensorConfig->formatTypeNum);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: get formatType element num = %d!", __func__, sensorConfig->formatTypeNum);

    ret = drsOps->GetUint32Array(node, "formatType", sensorConfig->formatType, sensorConfig->formatTypeNum, 0);
    CHECK_PARSER_CONFIG_RET(ret, "formatType");
    for (i = 0; i < sensorConfig->formatTypeNum; i++) {
        HDF_LOGE("%s: get formatType[%d] = %d", __func__, i, sensorConfig->formatType[i]);
    }
    HDF_LOGD("%s: name=%s, id=%d, exposure=%d, mirror=%d, heightMaxNum=%d, widthMaxNum=%d, frameRateMaxNum=%d, "
    "bufferCount=%d", __func__, sensorConfig->name, sensorConfig->id, sensorConfig->exposure, sensorConfig->mirror,
    sensorConfig->heightMaxNum, sensorConfig->widthMaxNum, sensorConfig->frameRateMaxNum, sensorConfig->bufferCount);
    return HDF_SUCCESS;
}

static int32_t ParseCameraIspDevice(const struct DeviceResourceNode *node,
    struct DeviceResourceIface *drsOps, struct HdfConfigCameraIspConfig *ispConfig)
{
    int32_t ret;

    ret = drsOps->GetString(node, "name", &ispConfig->name, NULL);
    CHECK_PARSER_CONFIG_RET(ret, "name");
    ret = drsOps->GetUint8(node, "id", &ispConfig->id, 0);
    CHECK_PARSER_CONFIG_RET(ret, "id");
    ret = drsOps->GetUint8(node, "brightness", &ispConfig->brightness, 0);
    CHECK_PARSER_CONFIG_RET(ret, "brightness");
    ret = drsOps->GetUint8(node, "contrast", &ispConfig->contrast, 0);
    CHECK_PARSER_CONFIG_RET(ret, "contrast");
    ret = drsOps->GetUint8(node, "saturation", &ispConfig->saturation, 0);
    CHECK_PARSER_CONFIG_RET(ret, "saturation");
    ret = drsOps->GetUint8(node, "sharpness", &ispConfig-> sharpness, 0);
    CHECK_PARSER_CONFIG_RET(ret, "sharpness");
    ret = drsOps->GetUint8(node, "gain", &ispConfig->gain, 0);
    CHECK_PARSER_CONFIG_RET(ret, "gain");
    ret = drsOps->GetUint8(node, "gamma", &ispConfig->gamma, 0);
    CHECK_PARSER_CONFIG_RET(ret, "gamma");
    ret = drsOps->GetUint8(node, "whiteBalance", &ispConfig->whiteBalance, 0);
    CHECK_PARSER_CONFIG_RET(ret, "whiteBalance");

    return HDF_SUCCESS;
}

static int32_t ParseCameraVcmDevice(const struct DeviceResourceNode *node,
                                    struct HdfConfigCameraVcmDevice *vcmConfig)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    if (node == NULL || vcmConfig == NULL) {
        HDF_LOGE("%s: invalid node or vcmConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetString(node, "name", &vcmConfig->name, NULL);
    CHECK_PARSER_CONFIG_RET(ret, "name");
    ret = drsOps->GetUint8(node, "id", &vcmConfig->id, 0);
    CHECK_PARSER_CONFIG_RET(ret, "id");
    ret = drsOps->GetUint8(node, "autoFocus", &vcmConfig->autoFocus, 0);
    CHECK_PARSER_CONFIG_RET(ret, "autoFocus");
    ret = drsOps->GetUint8(node, "zoom", &vcmConfig->zoom, 0);
    CHECK_PARSER_CONFIG_RET(ret, "zoom");
    ret = drsOps->GetUint8(node, "zoomMaxNum", &vcmConfig->zoomMaxNum, 0);
    CHECK_PARSER_CONFIG_RET(ret, "zoomMaxNum");

    HDF_LOGD("%s: name=%s, id=%d, autoFocus=%d, zoom=%d, zoomMaxNum=%d", __func__,
    vcmConfig->name, vcmConfig->id, vcmConfig->autoFocus, vcmConfig->zoom, vcmConfig->zoomMaxNum);
    return HDF_SUCCESS;
}

static int32_t ParseCameraLensDevice(const struct DeviceResourceNode *node,
                                    struct HdfConfigCameraLensDevice *lensConfig)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    if (node == NULL || lensConfig == NULL) {
        HDF_LOGE("%s: invalid node or lensConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetString(node, "name", &lensConfig->name, NULL);
    CHECK_PARSER_CONFIG_RET(ret, "name");
    ret = drsOps->GetUint8(node, "id", &lensConfig->id, 0);
    CHECK_PARSER_CONFIG_RET(ret, "id");
    ret = drsOps->GetUint8(node, "aperture", &lensConfig->aperture, 0);
    CHECK_PARSER_CONFIG_RET(ret, "aperture");

    HDF_LOGD("%s: name=%s, id=%d, aperture=%d", __func__,
    lensConfig->name, lensConfig->id, lensConfig->aperture);
    return HDF_SUCCESS;
}

static int32_t ParseCameraFlashDevice(const struct DeviceResourceNode *node,
    struct DeviceResourceIface *drsOps, struct HdfConfigCameraFlashConfig *flashConfig)
{
    int32_t ret;
    ret = drsOps->GetString(node, "name", &flashConfig->name, NULL);
    CHECK_PARSER_CONFIG_RET(ret, "name");
    ret = drsOps->GetUint8(node, "id", &flashConfig->id, 0);
    CHECK_PARSER_CONFIG_RET(ret, "id");
    ret = drsOps->GetUint8(node, "flashMode", &flashConfig->flashMode, 0);
    CHECK_PARSER_CONFIG_RET(ret, "flashMode");
    ret = drsOps->GetUint8(node, "flashIntensity", &flashConfig->flashIntensity, 0);
    CHECK_PARSER_CONFIG_RET(ret, "flashIntensity");
    return HDF_SUCCESS;
}

static int32_t ParseCameraUvcStatus(struct DeviceResourceIface *drsOps,
        const struct DeviceResourceNode *node, struct HdfConfigCameraUvcDevice *uvcConfig)
{
    int32_t ret;
    ret = drsOps->GetString(node, "name", &uvcConfig->name, NULL);
    CHECK_PARSER_CONFIG_RET(ret, "name");
    ret = drsOps->GetUint8(node, "id", &uvcConfig->id, 0);
    CHECK_PARSER_CONFIG_RET(ret, "id");
    ret = drsOps->GetUint8(node, "exposure", &uvcConfig->exposure, 0);
    CHECK_PARSER_CONFIG_RET(ret, "exposure");
    ret = drsOps->GetUint8(node, "mirror", &uvcConfig->mirror, 0);
    CHECK_PARSER_CONFIG_RET(ret, "mirror");
    ret = drsOps->GetUint32(node, "heightMaxNum", &uvcConfig->heightMaxNum, 0);
    CHECK_PARSER_CONFIG_RET(ret, "heightMaxNum");
    ret = drsOps->GetUint32(node, "widthMaxNum", &uvcConfig->widthMaxNum, 0);
    CHECK_PARSER_CONFIG_RET(ret, "widthMaxNum");
    ret = drsOps->GetUint32(node, "frameRateMaxNum", &uvcConfig->frameRateMaxNum, 0);
    CHECK_PARSER_CONFIG_RET(ret, "frameRateMaxNum");
    ret = drsOps->GetUint8(node, "bufferCount", &uvcConfig->bufferCount, 0);
    CHECK_PARSER_CONFIG_RET(ret, "bufferCount");
    ret = drsOps->GetUint8(node, "brightness", &uvcConfig->brightness, 0);
    CHECK_PARSER_CONFIG_RET(ret, "brightness");
    ret = drsOps->GetUint8(node, "contrast", &uvcConfig->contrast, 0);
    CHECK_PARSER_CONFIG_RET(ret, "contrast");
    ret = drsOps->GetUint8(node, "saturation", &uvcConfig->saturation, 0);
    CHECK_PARSER_CONFIG_RET(ret, "saturation");
    ret = drsOps->GetUint8(node, "sharpness", &uvcConfig-> sharpness, 0);
    CHECK_PARSER_CONFIG_RET(ret, "sharpness");
    ret = drsOps->GetUint8(node, "gain", &uvcConfig->gain, 0);
    CHECK_PARSER_CONFIG_RET(ret, "gain");
    ret = drsOps->GetUint8(node, "gamma", &uvcConfig->gamma, 0);
    CHECK_PARSER_CONFIG_RET(ret, "gamma");
    ret = drsOps->GetUint8(node, "whiteBalance", &uvcConfig->whiteBalance, 0);
    CHECK_PARSER_CONFIG_RET(ret, "whiteBalance");
    return HDF_SUCCESS;
}

static int32_t ParseCameraUvcDevice(const struct DeviceResourceNode *node,
                                    struct HdfConfigCameraUvcDevice *uvcConfig)
{
    int32_t i, ret;
    struct DeviceResourceIface *drsOps = NULL;

    if (node == NULL || uvcConfig == NULL) {
        HDF_LOGE("%s: invalid node or uvcConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL || drsOps->GetUint8 == NULL ||
        drsOps->GetUint8Array == NULL || drsOps->GetElemNum == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    uvcConfig->bufferTypeNum = drsOps->GetElemNum(node, "bufferType");
    if (uvcConfig->bufferTypeNum <= 0 || uvcConfig->bufferTypeNum > BUFFER_TYPE_COUNT) {
        HDF_LOGE("%s: parser bufferType element num failed! num = %d", __func__, uvcConfig->bufferTypeNum);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: get bufferType element num = %d!", __func__, uvcConfig->bufferTypeNum);
    ret = drsOps->GetUint32Array(node, "bufferType", uvcConfig->bufferType, uvcConfig->bufferTypeNum, 0);
    CHECK_PARSER_CONFIG_RET(ret, "bufferType");
    for (i = 0; i < uvcConfig->bufferTypeNum; i++) {
        HDF_LOGE("%s: get bufferType[%d] = %d", __func__, i, uvcConfig->bufferType[i]);
    }
    uvcConfig->formatTypeNum = drsOps->GetElemNum(node, "formatType");
    if (uvcConfig->formatTypeNum <= 0 || uvcConfig->formatTypeNum > FORMAT_TYPE_COUNT) {
        HDF_LOGE("%s: parser formatType element num failed! num = %d", __func__, uvcConfig->formatTypeNum);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: get formatType element num = %d!", __func__, uvcConfig->formatTypeNum);
    ret = drsOps->GetUint32Array(node, "formatType", uvcConfig->formatType, uvcConfig->formatTypeNum, 0);
    CHECK_PARSER_CONFIG_RET(ret, "formatType");
    for (i = 0; i < uvcConfig->formatTypeNum; i++) {
        HDF_LOGE("%s: get formatType[%d] = %d", __func__, i, uvcConfig->formatType[i]);
    }
    ret = ParseCameraUvcStatus(drsOps, node, uvcConfig);
    CHECK_PARSER_CONFIG_RET(ret, "UvcStatus");

    HDF_LOGD("%s: name=%s, id=%d, exposure=%d, mirror=%d, heightMaxNum=%d, widthMaxNum=%d, frameRateMaxNum=%d, "
    "bufferCount=%d, brightness=%d, contrast=%d, saturation=%d,  sharpness=%d, gain=%d, gamma=%d, whiteBalance=%d",
    __func__, uvcConfig->name, uvcConfig->id, uvcConfig->exposure, uvcConfig->mirror, uvcConfig->heightMaxNum,
    uvcConfig->widthMaxNum, uvcConfig->frameRateMaxNum, uvcConfig->bufferCount, uvcConfig->brightness,
    uvcConfig->contrast, uvcConfig->saturation, uvcConfig-> sharpness, uvcConfig->gain, uvcConfig->gamma,
    uvcConfig->whiteBalance);
    return HDF_SUCCESS;
}

static int32_t ParseCameraSensorConfig(const struct DeviceResourceNode *node,
                                        struct HdfConfigCameraSensorConfig *sensorConfig)
{
    struct DeviceResourceNode *childNode = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;
    uint32_t cnt = 0;

    if (node == NULL || sensorConfig == NULL) {
        HDF_LOGE("%s: invalid node or sensorConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "mode", &sensorConfig->mode, 0);
    CHECK_PARSER_CONFIG_RET(ret, "mode");
    if (sensorConfig->mode == 0) {
        HDF_LOGE("%s: not support sensor!", __func__);
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(node, childNode)
    {
        if (ParseCameraSensorDevice(childNode, &sensorConfig->sensor[cnt]) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
        cnt++;
        sensorConfig->sensorNum++;
    }
    HDF_LOGE("%s: sensorConfig->sensorNum = %d!", __func__, sensorConfig->sensorNum);

    return HDF_SUCCESS;
}

static int32_t ParseCameraIspConfig(const struct DeviceResourceNode *node,
                                        struct HdfConfigCameraIspConfig *ispConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (node == NULL || ispConfig == NULL) {
        HDF_LOGE("%s: invalid node or ispConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "mode", &ispConfig->mode, 0);
    CHECK_PARSER_CONFIG_RET(ret, "mode");
    if (ispConfig->mode == 0) {
        HDF_LOGE("%s: not support isp!", __func__);
    }
    ret = ParseCameraIspDevice(node, drsOps, ispConfig);
    if (ret == HDF_FAILURE) {
        HDF_LOGE("%s: ParseCameraIspDevice failed!", __func__);
    }

    HDF_LOGD("%s: name=%s, id=%d, brightness=%d, contrast=%d, saturation=%d, "
    " sharpness=%d, gain=%d, gamma=%d, whiteBalance=%d", __func__, ispConfig->name,
    ispConfig->id, ispConfig->brightness, ispConfig->contrast, ispConfig->saturation,
    ispConfig-> sharpness, ispConfig->gain, ispConfig->gamma, ispConfig->whiteBalance);

    return HDF_SUCCESS;
}

static int32_t ParseCameraLensConfig(const struct DeviceResourceNode *node,
                                        struct HdfConfigCameraLensConfig *lensConfig)
{
    struct DeviceResourceNode *childNode = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;
    uint32_t cnt = 0;

    if (node == NULL || lensConfig == NULL) {
        HDF_LOGE("%s: invalid node or lensConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "mode", &lensConfig->mode, 0);
    CHECK_PARSER_CONFIG_RET(ret, "mode");
    if (lensConfig->mode == 0) {
        HDF_LOGE("%s: not support lens!", __func__);
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(node, childNode)
    {
        if (ParseCameraLensDevice(childNode, &lensConfig->lens[cnt]) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
        cnt++;
        lensConfig->lensNum++;
    }
    HDF_LOGE("%s: lensConfig->lensNum = %d!", __func__, lensConfig->lensNum);

    return HDF_SUCCESS;
}

static int32_t ParseCameraVcmConfig(const struct DeviceResourceNode *node,
                                        struct HdfConfigCameraVcmConfig *vcmConfig)
{
    struct DeviceResourceNode *childNode = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;
    uint32_t cnt = 0;

    if (node == NULL || vcmConfig == NULL) {
        HDF_LOGE("%s: invalid node or vcmConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "mode", &vcmConfig->mode, 0);
    CHECK_PARSER_CONFIG_RET(ret, "mode");
    if (vcmConfig->mode == 0) {
        HDF_LOGE("%s: not support vcm!", __func__);
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(node, childNode)
    {
        if (ParseCameraVcmDevice(childNode, &vcmConfig->vcm[cnt]) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
        cnt++;
        vcmConfig->vcmNum++;
    }
    HDF_LOGE("%s: vcmConfig->vcmNum = %d!", __func__, vcmConfig->vcmNum);

    return HDF_SUCCESS;
}

static int32_t ParseCameraFlashConfig(const struct DeviceResourceNode *node,
                                        struct HdfConfigCameraFlashConfig *flashConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (node == NULL || flashConfig == NULL) {
        HDF_LOGE("%s: invalid node or flashConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "mode", &flashConfig->mode, 0);
    CHECK_PARSER_CONFIG_RET(ret, "mode");
    if (flashConfig->mode == 0) {
        HDF_LOGE("%s: not support flash!", __func__);
    }
    ret = ParseCameraFlashDevice(node, drsOps, flashConfig);
    if (ret == HDF_FAILURE) {
        HDF_LOGE("%s: ParseCameraFlashDevice failed!", __func__);
    }

    HDF_LOGD("%s: name=%s, id=%d, flashMode=%d, flashIntensity=%d", __func__,
    flashConfig->name, flashConfig->id, flashConfig->flashMode, flashConfig->flashIntensity);

    return HDF_SUCCESS;
}

static int32_t ParseCameraUvcConfig(const struct DeviceResourceNode *node,
                                        struct HdfConfigCameraUvcConfig *uvcConfig)
{
    struct DeviceResourceNode *childNode = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;
    uint32_t cnt = 0;

    if (node == NULL || uvcConfig == NULL) {
        HDF_LOGE("%s: invalid node or uvcConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "mode", &uvcConfig->mode, 0);
    CHECK_PARSER_CONFIG_RET(ret, "mode");
    if (uvcConfig->mode == 0) {
        HDF_LOGE("%s: not support uvc!", __func__);
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(node, childNode)
    {
        if (ParseCameraUvcDevice(childNode, &uvcConfig->uvc[cnt]) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
        cnt++;
        uvcConfig->uvcNum++;
    }
    HDF_LOGE("%s: uvcConfig->uvcNum = %d!", __func__, uvcConfig->uvcNum);

    return HDF_SUCCESS;
}

static int32_t ParseCameraDeviceConfig(const struct DeviceResourceNode *node,
                                        struct HdfConfigDeviceConfig *deviceConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    const struct DeviceResourceNode *sensorConfigNode = NULL;
    const struct DeviceResourceNode *ispConfigNode = NULL;
    const struct DeviceResourceNode *vcmConfigNode = NULL;
    const struct DeviceResourceNode *lensConfigNode = NULL;
    const struct DeviceResourceNode *flashConfigNode = NULL;
    const struct DeviceResourceNode *uvcConfigNode = NULL;

    if (node == NULL || deviceConfig == NULL) {
        HDF_LOGE("%s: invalid node or deviceConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL || drsOps->GetString == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetString(node, "driverName", &deviceConfig->driverName, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: get driverName fail!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: driverName = %s", __func__, deviceConfig->driverName);

    sensorConfigNode = drsOps->GetChildNode(node, "SensorConfig");
    ispConfigNode = drsOps->GetChildNode(node, "IspConfig");
    vcmConfigNode = drsOps->GetChildNode(node, "VcmConfig");
    lensConfigNode = drsOps->GetChildNode(node, "LensConfig");
    flashConfigNode = drsOps->GetChildNode(node, "FlashConfig");
    uvcConfigNode = drsOps->GetChildNode(node, "UvcConfig");
    if (sensorConfigNode == NULL || ispConfigNode == NULL || vcmConfigNode == NULL ||
        lensConfigNode == NULL || flashConfigNode == NULL || uvcConfigNode == NULL) {
        HDF_LOGE("%s: get child node fail!", __func__);
        return HDF_FAILURE;
    }

    CHECK_PARSER_CONFIG_RET(ParseCameraSensorConfig(sensorConfigNode, &deviceConfig->sensor), "SensorConfig");
    CHECK_PARSER_CONFIG_RET(ParseCameraIspConfig(ispConfigNode, &deviceConfig->isp), "IspConfig");
    CHECK_PARSER_CONFIG_RET(ParseCameraVcmConfig(vcmConfigNode, &deviceConfig->vcm), "VcmConfig");
    CHECK_PARSER_CONFIG_RET(ParseCameraLensConfig(lensConfigNode, &deviceConfig->lens), "LensConfig");
    CHECK_PARSER_CONFIG_RET(ParseCameraFlashConfig(flashConfigNode, &deviceConfig->flash), "FlashConfig");
    CHECK_PARSER_CONFIG_RET(ParseCameraUvcConfig(uvcConfigNode, &deviceConfig->uvc), "UvcConfig");

    return HDF_SUCCESS;
}

static int32_t ParseCameraAbilityConfig(const struct DeviceResourceNode *node,
                                        struct HdfConfigCameraAbilityConfig *abilityConfig)
{
    struct DeviceResourceNode *childNode = NULL;
    uint32_t cnt = 0;

    if (node == NULL || abilityConfig == NULL) {
        HDF_LOGE("%s: invalid node or abilityConfig!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(node, childNode)
    {
        if (ParseCameraDeviceConfig(childNode, &abilityConfig->deviceConfig[cnt]) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
        cnt++;
        abilityConfig->deviceNum++;
    }
    HDF_LOGE("%s: abilityConfig->deviceNum = %d!", __func__, abilityConfig->deviceNum);

    return HDF_SUCCESS;
}

static int32_t ParseCameraConfig(const struct DeviceResourceNode *node,
                                        struct HdfConfigCameraConfig *cameraConfig)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;
    const struct DeviceResourceNode *cameraConfigNode = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    cameraConfigNode = drsOps->GetChildNode(node, "abilityConfig");
    if (cameraConfigNode == NULL) {
        HDF_LOGE("%s: get child node fail!", __func__);
        return HDF_FAILURE;
    }
    ret = ParseCameraAbilityConfig(cameraConfigNode, &cameraConfig->abilityConfig);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: ParseCameraAbilityConfig failed!", __func__);
        return HDF_FAILURE;
    }
    return ret;
}

int32_t HdfParseCameraConfig(const struct DeviceResourceNode *node)
{
    if (node == NULL) {
        HDF_LOGE("%s: node = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return ParseCameraConfig(node, &g_configCameraRoot.cameraConfig);
}

struct HdfConfigCameraRoot *HdfCameraGetConfigRoot()
{
    return &g_configCameraRoot;
}
