/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <securec.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_mem.h"
#include "sensor_device_manager.h"
#include "sensor_ppg_driver.h"

#define PPG_IRQ_GPIO_NO 1
#define PPG_MAX_FIFO_BUF_LEN 3200
#define HDF_LOG_TAG sensor_ppg_driver_c
#define HDF_PPG_WORK_QUEUE "hdf_ppg_work_queue"

#define CHECK_PPG_INIT_RETURN_VALUE(drvData, ret) do {                          \
    if (((drvData) == NULL) || ((drvData)->detectFlag == false)) {              \
        HDF_LOGE("%s:line %d ppg not detected and return ret", __func__, __LINE__); \
        return (ret);                                                           \
    }                                                                           \
} while (0)

static uint8_t g_fifoBuf[PPG_MAX_FIFO_BUF_LEN] = {0};
static struct PpgDrvData *g_ppgDrvData = NULL;

static struct PpgDrvData *PpgGetDrvData(void)
{
    return g_ppgDrvData;
}

static int32_t PpgReportInt(uint8_t *dataBuf, uint16_t dataLen)
{
    int32_t ret;
    OsalTimespec time = {0};
    struct SensorReportEvent event = {
        .sensorId = 129,
        .option = 0,
        .mode = 0,
        .dataLen = dataLen,
        .data = dataBuf,
    };

    if (OsalGetTime(&time) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get time failed", __func__);
        return HDF_FAILURE;
    }
    event.timestamp = time.sec * SENSOR_SECOND_CONVERT_NANOSECOND + time.usec * SENSOR_CONVERT_UNIT;

    ret = ReportSensorEvent(&event);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Cs1262 report data failed", __func__);
    }

    return ret;
}

static void PpgDataWorkEntry(void *arg)
{
    int32_t ret;
    struct PpgDrvData *drvData = (struct PpgDrvData *)arg;
    uint16_t readLen = 0;

    CHECK_NULL_PTR_RETURN(drvData);
    CHECK_NULL_PTR_RETURN(drvData->chipData.opsCall.ReadData);

    ret = drvData->chipData.opsCall.ReadData(g_fifoBuf, sizeof(g_fifoBuf), &readLen);
    if((ret != HDF_SUCCESS) || (readLen > sizeof(g_fifoBuf))) {
        HDF_LOGE("%s: Ppg read data failed", __func__);
        return;
    }

    if(PpgReportInt(g_fifoBuf, readLen) != HDF_SUCCESS) {
        HDF_LOGE("%s: Cs1262ReportInt fail", __func__);
    }
}

static int32_t SetPpgEnable(void)
{
    int32_t ret;
    struct PpgDrvData *drvData = PpgGetDrvData();

    CHECK_PPG_INIT_RETURN_VALUE(drvData, HDF_ERR_NOT_SUPPORT);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->chipData.opsCall.Enable, HDF_ERR_INVALID_PARAM);

    if(drvData->enable) {
        HDF_LOGE("%s: Ppg sensor is enabled", __func__);
        return HDF_SUCCESS;
    }

    ret = EnablePpgIrq(PPG_IRQ_GPIO_NO);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("Gpio enable ppg irq failed: %d", ret);
        return HDF_FAILURE;
    }

    ret = drvData->chipData.opsCall.Enable();
    if(ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SetPpgEnable failed", __func__);
        return HDF_FAILURE;
    }

    drvData->enable = true;
    return HDF_SUCCESS;
}

static int32_t SetPpgDisable(void)
{
    int32_t ret;
    struct PpgDrvData *drvData = PpgGetDrvData();

    CHECK_PPG_INIT_RETURN_VALUE(drvData, HDF_ERR_NOT_SUPPORT);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->chipData.opsCall.Disable, HDF_ERR_INVALID_PARAM);

    if(!drvData->enable) {
        HDF_LOGE("%s: Ppg sensor had disable", __func__);
        return HDF_SUCCESS;
    }

    ret = drvData->chipData.opsCall.Disable();
    if(ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SetPpgDisable failed", __func__);
        return HDF_FAILURE;
    }

    drvData->enable = false;
    return HDF_SUCCESS;
}

static int32_t SetPpgBatch(int64_t samplingInterval, int64_t interval)
{
    (void)interval;
    struct PpgDrvData *drvData = PpgGetDrvData();
    CHECK_PPG_INIT_RETURN_VALUE(drvData, HDF_ERR_NOT_SUPPORT);

    drvData->interval = samplingInterval;
    return HDF_SUCCESS;
}

// realTime interval TODO
static int32_t SetPpgMode(int32_t mode)
{
    int32_t ret;
    struct PpgDrvData *drvData = PpgGetDrvData();
    CHECK_PPG_INIT_RETURN_VALUE(drvData, HDF_ERR_NOT_SUPPORT);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->chipData.opsCall.SetMode, HDF_ERR_INVALID_PARAM);

    if(drvData->enable) {
        HDF_LOGE("%s: Ppg sensor had enabled", __func__);
        return HDF_FAILURE;
    }

    ret = drvData->chipData.opsCall.SetMode(mode);
    if(ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SetPpgMode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SetPpgOption(uint32_t option)
{
    int32_t ret;
    struct PpgDrvData *drvData = PpgGetDrvData();

    CHECK_PPG_INIT_RETURN_VALUE(drvData, HDF_ERR_NOT_SUPPORT);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->chipData.opsCall.SetOption, HDF_ERR_INVALID_PARAM);

    ret = drvData->chipData.opsCall.SetOption(option);
    if(ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SetPpgOption failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t PpgReadInt(uint16_t gpio, void *data)
{
    struct PpgDrvData *drvData = PpgGetDrvData();
    CHECK_PPG_INIT_RETURN_VALUE(drvData, HDF_ERR_NOT_SUPPORT);

    if(!drvData->enable) {
        HDF_LOGE("%s: ppg not enabled", __func__);
        return HDF_SUCCESS;
    }

    if (!HdfAddWork(&drvData->ppgWorkQueue, &drvData->ppgWork)) {
        HDF_LOGE("%s: Ppg add work queue failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t RegisterSensorDev(struct PpgCfgData *cfg)
{
    struct SensorDeviceInfo devData = {
        .ops.Enable = SetPpgEnable,
        .ops.Disable = SetPpgDisable,
        .ops.SetBatch = SetPpgBatch,
        .ops.SetMode = SetPpgMode,
        .ops.SetOption = SetPpgOption
    };

    if(memcpy_s(&devData.sensorInfo, sizeof(devData.sensorInfo),
                &(cfg->sensorCfg.sensorInfo), sizeof(cfg->sensorCfg.sensorInfo)) != EOK) {
        HDF_LOGE("%s: copy sensor info failed", __func__);
        return HDF_FAILURE;
    }

    if (AddSensorDevice(&devData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Add ppg device failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t RegisterPpgChip(struct PpgChipData *chipData)
{
    int32_t ret = 0;
    struct PpgDrvData *drvData = PpgGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(chipData, HDF_ERR_INVALID_PARAM);

    if (drvData->detectFlag == true) {
        HDF_LOGE("%s: ppg sensor have detected", __func__);
        return HDF_SUCCESS;
    }

    (void)memcpy_s(&drvData->chipData, sizeof(struct PpgChipData), chipData, sizeof(*chipData));

    ret = SetPpgIrq(PPG_IRQ_GPIO_NO, PpgReadInt);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Add ppg device failed", __func__);
        return HDF_FAILURE;
    }

    if (RegisterSensorDev(drvData->chipData.cfgData) != HDF_SUCCESS) {
        HDF_LOGE("%s: ppg sensor register failed", __func__);
        return HDF_SUCCESS;
    }

    HDF_LOGI("%s: ppg sensor construct driver success!", __func__);
    drvData->detectFlag = true;

    return HDF_SUCCESS;
}

static int32_t DispatchPpg(struct HdfDeviceIoClient *client, int cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t PpgBindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct PpgDrvData *drvData = (struct PpgDrvData *)OsalMemCalloc(sizeof(*drvData));
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_MALLOC_FAIL);

    drvData->ioService.Dispatch = DispatchPpg;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_ppgDrvData = drvData;
    return HDF_SUCCESS;
}

int32_t InitPpgDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct PpgDrvData *drvData = (struct PpgDrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if(HdfWorkQueueInit(&drvData->ppgWorkQueue, HDF_PPG_WORK_QUEUE) != HDF_SUCCESS) {
        HDF_LOGE("%s: Ppg init work queue failed", __func__);
        return HDF_FAILURE;
    }

    if(HdfWorkInit(&drvData->ppgWork, PpgDataWorkEntry, drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Ppg create thread failed", __func__);
        return HDF_FAILURE;
    }

    drvData->initStatus = true;
    drvData->enable = false;
    drvData->detectFlag = false;

    HDF_LOGI("%s: init Ppg driver success", __func__);
    return HDF_SUCCESS;
}

void ReleasePpgDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);
    struct PpgDrvData *drvData = (struct PpgDrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    HdfWorkDestroy(&drvData->ppgWork);
    HdfWorkQueueDestroy(&drvData->ppgWorkQueue);

    PpgDestructCfgData();
    (void)memset_s(drvData, sizeof(struct PpgDrvData), 0, sizeof(struct PpgDrvData));
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_sensorPpgDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_PPG",
    .Bind = PpgBindDriver,
    .Init = InitPpgDriver,
    .Release = ReleasePpgDriver,
};

HDF_INIT(g_sensorPpgDevEntry);
