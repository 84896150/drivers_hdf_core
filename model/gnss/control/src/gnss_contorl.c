/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */


#include "gnss_contorl.h"

static int32_t GnssDeviceSetPowerMode(struct GnssDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    struct GnssContorlRequest reqData;
    uint32_t status = 0;
    if (!HdfSbufReadInt32(data, &(reqData.gnssId))) {
        GNSS_LOG_ERR("sbuf read gnssId failed");
        return HDF_ERR_INVALID_PARAM;
    }
    reqData.gnssName = HdfSbufReadString(data);
    if (reqData.gnssName == NULL) {
        GNSS_LOG_ERR("sbuf read gnssName failed");
        return HDF_ERR_INVALID_PARAM;
    }
    if (deviceInfo->gnssInfo.gnssDeviceId != reqData.gnssId ||
        strcmp(deviceInfo->gnssInfo.gnssName, reqData.gnssName) != 0) {
        GNSS_LOG_ERR("device is not exist");
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadInt32(data, &status)) {
        GNSS_LOG_ERR("sbuf read gnssId failed");
        return HDF_ERR_INVALID_PARAM;
    }
    ret = deviceInfo->SetPowerMode(deviceInfo, status);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("SetPowerMode failed");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static struct GnssControlCmdHandleList g_GnssContorlHandleList[] = {
    {GNSSDRV_CTRL_SET_POWER_MODE, GnssDeviceSetPowerMode},
};

static int32_t GnssControlDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    for (unsigned int i = 0; i < HDF_ARRAY_SIZE(g_GnssContorlHandleList); ++i) {
        if ((cmdId == (int)(g_GnssContorlHandleList[i].cmd)) && (g_GnssContorlHandleList[i].func != NULL)) {
            return g_GnssContorlHandleList[i].func(&g_gnssDeviceInfo, data, reply);
        }
    }
    GNSS_LOG_ERR("invaild [cmdId=%d]", cmdId);
    return HDF_FAILURE;
}

static int32_t GnssControlBind(struct HdfDeviceObject *device)
{
    struct GnssControlHost *controlHost = NULL;
    GNSS_LOG_INFO("entry.");

    if (device == NULL) {
        GNSS_LOG_ERR("Input params check error: device is NULL.");
        return HDF_FAILURE;
    }

    controlHost = (struct GnssControlHost *)OsalMemCalloc(sizeof(*controlHost));
    if (controlHost == NULL) {
        GNSS_LOG_ERR("Malloc controlHost fail!");
        return HDF_FAILURE;
    }
    controlHost->device = device;
    device->service = &controlHost->service;

    controlHost->service.Dispatch = GnssControlDispatch;
    GNSS_LOG_ERR("success.");
    return HDF_SUCCESS;
}

static int32_t GnssControlInit(struct HdfDeviceObject *device)
{
    (void)device;
    GNSS_LOG_INFO("success.");
    return HDF_SUCCESS;
}

static void GnssControlRelease(struct HdfDeviceObject *device)
{
    struct GnssControlHost *controlHost = NULL;

    if (device == NULL) {
        GNSS_LOG_ERR("Input params check error: device is NULL.");
        return;
    }

    controlHost = (struct GnssControlHost *)device->service;
    if (controlHost == NULL) {
        GNSS_LOG_ERR("controlHost is NULL.");
        return;
    }
    OsalMemFree(controlHost);
}

/* g_gnssControlEntry definitions */
struct HdfDriverEntry g_gnssControlEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_GNSS_CONTROL",
    .Bind = GnssControlBind,
    .Init = GnssControlInit,
    .Release = GnssControlRelease,
};
HDF_INIT(g_gnssControlEntry);
