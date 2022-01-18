/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "gnss_stream.h"

static int32_t GnssDeviceOpsPre(struct GnssDeviceInfo *deviceInfo, struct HdfSBuf *data, struct GnssStreamRequest *reqData)
{
    if (!HdfSbufReadInt32(data, &(reqData->gnssId))) {
        GNSS_LOG_ERR("sbuf read gnssId failed");
        return HDF_ERR_INVALID_PARAM;
    }

    reqData->gnssName = HdfSbufReadString(data);
    if (reqData->gnssName == NULL) {
        GNSS_LOG_ERR("sbuf read gnssName failed");
        return HDF_ERR_INVALID_PARAM;
    }

    if (deviceInfo->gnssInfo.gnssDeviceId != reqData->gnssId ||
        strcmp(deviceInfo->gnssInfo.gnssName, reqData->gnssName) != 0) {
        GNSS_LOG_ERR("device is not exist");
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}
static int32_t GnssDeviceOpen(struct GnssDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret = HDF_SUCCESS;
    struct GnssStreamRequest reqData;
    (void)reply;
    
    if (GnssDeviceOpsPre(deviceInfo, data, &reqData) != HDF_SUCCESS) {
        GNSS_LOG_ERR("device is not exist");
        return HDF_ERR_INVALID_PARAM;
    }
    ret = deviceInfo->Open(deviceInfo);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("gnss Driver open device failed");
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

static int32_t GnssDeviceClose(struct GnssDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret = HDF_SUCCESS;
    struct GnssStreamRequest reqData;
    if (GnssDeviceOpsPre(deviceInfo, data, &reqData) != HDF_SUCCESS) {
        GNSS_LOG_ERR("device is not exist");
        return HDF_ERR_INVALID_PARAM;
    }
    ret = deviceInfo->Close(deviceInfo);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("gnss Driver close device failed");
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}
static int32_t GnssDeviceReadCommon(struct GnssDeviceInfo *deviceInfo, char *data, uint32_t dataLen, int32_t *replyLen)
{
    int32_t ret = HDF_SUCCESS;
    
    data = (char *)OsalMemCalloc(dataLen);
    if (data == NULL) {
        GNSS_LOG_ERR("respData.RespData malloc failed");
        return HDF_ERR_INVALID_PARAM;
    }
    ret = deviceInfo->Read(deviceInfo, data, dataLen, replyLen);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("gnss Driver Read failed");
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

static int32_t GnssGetAllGnssInfo(struct GnssDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    char *respData = NULL;
    uint32_t dataLen = GNSS_ALL_INFO_MAX_DATA_LEN;
    struct GnssStreamRequest reqData;
    int32_t replyLen = 0;
    int32_t ret;
    if (GnssDeviceOpsPre(deviceInfo, data, &reqData) != HDF_SUCCESS) {
        GNSS_LOG_ERR("device is not exist");
        return HDF_ERR_INVALID_PARAM;
    }
    respData = (char *)OsalMemCalloc(dataLen);
    if (data == NULL) {
        GNSS_LOG_ERR("respData.RespData malloc failed");
        return HDF_ERR_INVALID_PARAM;
    }
    ret = deviceInfo->Read(deviceInfo, respData, dataLen, &replyLen);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("gnss Driver Read failed");
        return HDF_ERR_INVALID_PARAM;
    }
    GNSS_LOG_INFO("AllGnssInfo dataLen = %d", replyLen);
    GNSS_LOG_INFO("AllGnssInfo data = %s", respData);
    
    if (!HdfSbufWriteInt32(reply, replyLen)) {
        GNSS_LOG_ERR("HdfSbufWriteInt32  replyLen failed");
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteBuffer(reply, respData, replyLen)) {
        GNSS_LOG_ERR("HdfSbufWriteBuffer  respData failed");
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMemFree(respData);
    return HDF_SUCCESS;
}

static int32_t GnssGetGpggaInfo(struct GnssDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    char *respData = NULL;
    uint32_t dataLen = GNSS_ALL_INFO_MAX_DATA_LEN;
    struct GnssStreamRequest reqData;
    char *ptr = NULL;
    int32_t offset;
    int32_t len = 0;
    if (GnssDeviceOpsPre(deviceInfo, data, &reqData) != HDF_SUCCESS) {
        GNSS_LOG_ERR("device is not exist");
        return HDF_ERR_INVALID_PARAM;
    }
    if (GnssDeviceReadCommon(deviceInfo, respData, dataLen, &len) != HDF_SUCCESS) {
        GNSS_LOG_ERR("GnssDeviceReadCommon read failed");
        return HDF_ERR_INVALID_PARAM;
    }
    if (respData == NULL) {
        GNSS_LOG_ERR("respData is NULL");
        return HDF_FAILURE;
    }
    ptr = strstr(respData, "$GPGSV");
    if (ptr == NULL) {
        GNSS_LOG_ERR("cannot find \"$GPRMC\".");
        return -1;
    }
    offset = ptr - respData + strlen("$GPGGA");
    HdfSbufWriteString(reply, respData + offset);
    OsalMemFree(respData);
    
    return HDF_SUCCESS;
}

static int32_t GnssGetGpgsvInfo(struct GnssDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    char *respData = NULL;
    uint32_t dataLen = GNSS_ALL_INFO_MAX_DATA_LEN;
    struct GnssStreamRequest reqData;
    char *ptr = NULL;
    int32_t offset;
    int32_t len = 0;
    if (GnssDeviceOpsPre(deviceInfo, data, &reqData) != HDF_SUCCESS) {
        GNSS_LOG_ERR("device is not exist");
        return HDF_ERR_INVALID_PARAM;
    }
    if (GnssDeviceReadCommon(deviceInfo, respData, dataLen, &len) != HDF_SUCCESS) {
        GNSS_LOG_ERR("GnssDeviceReadCommon read failed");
        return HDF_ERR_INVALID_PARAM;
    }
    if (respData == NULL) {
        GNSS_LOG_ERR("respData is NULL");
        return HDF_FAILURE;
    }
    ptr = strstr(respData, "$GPGSV");
    if (ptr == NULL) {
        GNSS_LOG_ERR("cannot find \"$GPGSV\".");
        return -1;
    }
    offset = ptr - respData + strlen("$GPGSV");
    HdfSbufWriteString(reply, respData + offset);
    OsalMemFree(respData);
    return HDF_SUCCESS;
}

static struct GnssStreamHandleList g_GnssStreamHandleList[] = {
    {GNSSDRV_STREAM_IOCTL_OPEN, GnssDeviceOpen},
    {GNSSDRV_STREAM_IOCTL_CLOSE, GnssDeviceClose},
    {GNSSDRV_READ_ALL_GNSS_INFO, GnssGetAllGnssInfo},
    {GNSSDRV_READ_GPGGA_INFO, GnssGetGpggaInfo},  // GPS info
    {GNSSDRV_READ_GPGSV_INFO, GnssGetGpgsvInfo},  // 星历信息
};

static int32_t GnssStreamDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    for (unsigned int i = 0; i < HDF_ARRAY_SIZE(g_GnssStreamHandleList); ++i) {
        if ((cmdId == (int)(g_GnssStreamHandleList[i].cmd)) && (g_GnssStreamHandleList[i].func != NULL)) {
            return g_GnssStreamHandleList[i].func(&g_gnssDeviceInfo, data, reply);
        }
    }
    GNSS_LOG_ERR("invaild [cmdId=%d]", cmdId);
    return HDF_FAILURE;
}

static int32_t GnssStreamBind(struct HdfDeviceObject *device)
{
    struct GnssStreamHost *streamHost = NULL;
    GNSS_LOG_ERR("entry.");

    if (device == NULL) {
        GNSS_LOG_ERR("Input params check error: device is NULL.");
        return HDF_FAILURE;
    }

    streamHost = (struct GnssStreamHost *)OsalMemCalloc(sizeof(*streamHost));
    if (streamHost == NULL) {
        GNSS_LOG_ERR("Malloc streamHost fail!");
        return HDF_FAILURE;
    }
    streamHost->device = device;
    device->service = &streamHost->service;

    streamHost->service.Dispatch = GnssStreamDispatch;
    GNSS_LOG_ERR("success.");
    return HDF_SUCCESS;
}

static int32_t GnssStreamInit(struct HdfDeviceObject *device)
{
    (void)device;
    
    return HDF_SUCCESS;
}

static void GnssStreamRelease(struct HdfDeviceObject *device)
{
    struct GnssStreamHost *streamHost = NULL;

    if (device == NULL) {
        GNSS_LOG_ERR("Input params check error: device is NULL.");
        return;
    }

    streamHost = (struct GnssStreamHost *)device->service;
    if (streamHost == NULL) {
        GNSS_LOG_ERR("streamHost is NULL.");
        return;
    }
    GNSS_LOG_INFO("success.");
    OsalMemFree(streamHost);
}


/* g_gnssStreamEntry definitions */
struct HdfDriverEntry g_gnssStreamEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_GNSS_STREAM",
    .Bind = GnssStreamBind,
    .Init = GnssStreamInit,
    .Release = GnssStreamRelease,
};
HDF_INIT(g_gnssStreamEntry);
