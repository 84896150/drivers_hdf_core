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
#include "gpio.h"
#include "mcpwm.h"

#define PWM_CH0   0
#define PWM_CH1   1
#define PWM_CH2   2
#define PWM_CH3   3
#define PWM_CH4   4
#define PWM_CH5   5
#define PERCENT   100

struct PwmResource {
    uint32_t channel;
    uint32_t aligned_mode;
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
    struct pwm_platform_data pwmCfg;
    uint32_t aligned_mode;
    uint32_t channel;

    if (pwm == NULL || config == NULL) {
        HDF_LOGE("%s\r\n", __FUNCTION__);
        return HDF_FAILURE;
    }

    prvPwm = (struct PwmDevice *)PwmGetPriv(pwm);
    if (prvPwm == NULL) {
        return HDF_FAILURE;
    }

    pwmCfg.duty = config->duty * PERCENT;
    pwmCfg.frequency = prvPwm->resource.freq;
    channel = prvPwm->resource.channel;
    aligned_mode = prvPwm->resource.aligned_mode;
    pwmCfg.complementary_en = 1;                  

    switch (channel) {
        case PWM_CH0:
            pwmCfg.pwm_ch_num = pwm_ch0;
            pwmCfg.pwm_timer_num = pwm_timer0;
            pwmCfg.h_pin = IO_PORTA_00;
            pwmCfg.l_pin = IO_PORTA_01;
            break;
        case PWM_CH1:
            pwmCfg.pwm_ch_num = pwm_ch1;
            pwmCfg.pwm_timer_num = pwm_timer1;
            pwmCfg.h_pin = -1;
            pwmCfg.l_pin = IO_PORTB_02;
            break;
        case PWM_CH2:
            pwmCfg.pwm_ch_num = pwm_ch2;
            pwmCfg.pwm_timer_num = pwm_timer2;
            pwmCfg.h_pin = -1;
            pwmCfg.l_pin = -1;
            break;
        case PWM_CH3:
            pwmCfg.pwm_ch_num = pwm_ch3;
            pwmCfg.pwm_timer_num = pwm_timer3;
            pwmCfg.h_pin = -1;
            pwmCfg.l_pin = IO_PORTB_10;
            break;
        case PWM_CH4:
            pwmCfg.pwm_ch_num = pwm_ch4;
            pwmCfg.pwm_timer_num = pwm_timer4;
            pwmCfg.h_pin = -1;
            pwmCfg.l_pin = IO_PORTA_10;
            break;
        case PWM_CH5:
            pwmCfg.pwm_ch_num = pwm_ch5;
            pwmCfg.pwm_timer_num = pwm_timer5;
            pwmCfg.h_pin = IO_PORTC_04;
            pwmCfg.l_pin = IO_PORTC_05;
            break;
        default:
            break;
    }

    if (aligned_mode == 0) {
        pwmCfg.pwm_aligned_mode = pwm_edge_aligned;       // 边沿对齐模式
    } else {
        pwmCfg.pwm_aligned_mode = pwm_center_aligned;     // 中心对齐模式
    }

    if (config->status == PWM_ENABLE_STATUS) {
        mcpwm_init(&pwmCfg);
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

    if (dri->GetUint32(resourceNode, "aligned_mode", &resource->aligned_mode, 0) != HDF_SUCCESS) {
        HDF_LOGE("read aligned_mode fail\r\n");
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: resource->aligned_mode %u\r\n", __func__, resource->aligned_mode);

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
    .moduleName = "HDF_PLATFORM_PWM",
    .Bind = PwmDriverBind,
    .Init = PwmDriverInit,
    .Release = PwmDriverRelease,
};

// Initialize HdfDriverEntry
HDF_INIT(g_pwmDriverEntry);
