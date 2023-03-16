/*
 * Copyright (c) 2022 Jiangsu Hoperun Software Co., Ltd.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include <stdio.h>
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "pwm_core.h"
#include "hdf_log.h"
#include "bl602_pwm.h"
#include "bl_pwm.h"

struct PwmResource {
    uint32_t channel;
    uint32_t freq;
};

struct PwmDevice {
    struct IDeviceIoService ioService;
    struct PwmConfig *cfg;
    struct PwmResource resource;
};

static int32_t PwmDevSetConfig(struct PwmDev *pwm, struct PwmConfig *config);

struct PwmMethod g_pwmmethod = {
    .setConfig = PwmDevSetConfig,
};

static int32_t PwmDevSetConfig(struct PwmDev *pwm, struct PwmConfig *config)
{
    struct PwmDevice *prvPwm = NULL;
    PWM_CH_CFG_Type pwmCfg;

    if (pwm == NULL || config == NULL) {
        HDF_LOGE("%s\r\n", __FUNCTION__);
        return HDF_FAILURE;
    }

    prvPwm = (struct PwmDevice *)PwmGetPriv(pwm);
    if (prvPwm == NULL) {
        return HDF_FAILURE;
    }

    int32_t freq = BL_PWM_CLK/(config->period);
    if ((freq < 2000) || (freq > 800000)) {
        return -1;
    }

    switch(config->polarity) {
        case PWM_NORMAL_POLARITY:
            pwmCfg.pol = PWM_POL_NORMAL;
            break;
        case PWM_INVERTED_POLARITY:
            pwmCfg.pol = PWM_POL_INVERT;
            break;
        default:
            break;
    }

    pwmCfg.ch = prvPwm->resource.channel;
    pwmCfg.clk = PWM_CLK_XCLK;
    pwmCfg.stopMode = PWM_STOP_ABRUPT;
    pwmCfg.clkDiv = 0;
    pwmCfg.period = config->period;
    pwmCfg.threshold1 = 0;
    pwmCfg.threshold2 = (uint16_t)((config->period)/100 * (config->duty));
    pwmCfg.intPulseCnt = 0;

    PWM_Channel_Disable(pwmCfg.ch);

    if (config->status == PWM_ENABLE_STATUS) {
        PWM_Channel_Init(&pwmCfg);
    }

    return HDF_SUCCESS;
}

static uint32_t GetPwmDeviceResource(
    struct PwmDevice *device, const struct DeviceResourceNode *resourceNode)
{
    uint32_t tempPin = 0;
    struct DeviceResourceIface *dri = NULL;
    struct PwmResource *resource = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("resource or device is NULL\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is NULL\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }

    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    if (dri->GetUint32(resourceNode, "channel", &resource->channel, 0) != HDF_SUCCESS) {
        HDF_LOGE("read channel fail\r\n");
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: resource->channel %u\r\n", __func__, resource->channel);

    if (dri->GetUint32(resourceNode, "freq", &resource->freq, 0) != HDF_SUCCESS) {
        HDF_LOGE("read freq fail\r\n");
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: resource->freq %u\r\n", __func__, resource->freq);

    return HDF_SUCCESS;
}

static int32_t AttachPwmDevice(struct PwmDev *host, struct HdfDeviceObject *device)
{
    int32_t ret;
    struct PwmDevice *pwmDevice = NULL;
    if (device == NULL || device->property == NULL || host == NULL) {
        HDF_LOGE("%s: param is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    pwmDevice = (struct PwmDevice *)OsalMemCalloc(sizeof(struct PwmDevice));
    if (pwmDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc pwmDevice error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = GetPwmDeviceResource(pwmDevice, device->property);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(pwmDevice);
        return HDF_FAILURE;
    }

    host->priv = pwmDevice;

    return HDF_SUCCESS;
} 

static int32_t PwmDriverBind(struct HdfDeviceObject *device)
{
    static struct PwmDev devService;

    if (device == NULL) {
        HDF_LOGE("hdfDevice object is null!\r\n");
        return HDF_FAILURE;
    }

    device->service = &devService.service;
    devService.device = device;
    HDF_LOGI("Enter %s\r\n", __func__);

    return HDF_SUCCESS;
}

static int32_t PwmDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct PwmDev *host = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    HDF_LOGI("Enter %s:\r\n", __func__);

    host = (struct PwmDev *)OsalMemCalloc(sizeof(struct PwmDev));
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = AttachPwmDevice(host, device);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(host);
        HDF_LOGE("%s:attach error\r\n", __func__);
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }

    host->method = &g_pwmmethod;
    ret = PwmDeviceAdd(device, host);
    if (ret != HDF_SUCCESS) {
        PwmDeviceRemove(device, host);
        OsalMemFree(host->device);
        OsalMemFree(host);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    HDF_LOGI("PwmDriverInit success!\r\n");
    return HDF_SUCCESS;
}

static void PwmDriverRelease(struct HdfDeviceObject *device)
{
    struct PwmDev *host = NULL;

    if (device == NULL || device->service == NULL) {
        HDF_LOGE("device is null\r\n");
        return;
    }

    host = (struct PwmDev *)device->service;
    if (host != NULL && host->device != NULL) {
        host->method = NULL;
        OsalMemFree(host->device);
        OsalMemFree(host);
        host->device = NULL;
        host = NULL;
    }

    device->service = NULL;
    HDF_LOGI("PwmDriverRelease finish!!\r\n");
    return;
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_pwmDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "BL_PWM_MODULE_HDF",
    .Bind = PwmDriverBind,
    .Init = PwmDriverInit,
    .Release = PwmDriverRelease,
};

// Initialize HdfDriverEntry
HDF_INIT(g_pwmDriverEntry);
