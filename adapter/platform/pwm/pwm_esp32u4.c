/*
 * Copyright (c) 2022 Hunan OpenValley Digital Industry Development Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include <stdio.h>
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#include "hdf_device_desc.h"
#include "pwm_if.h"
#include "pwm_core.h"
#include "hdf_log.h"
#include "driver/mcpwm.h"
#include "driver/ledc.h"

#define UNIT_MAX_OUTPUT 6   // The maximum output number of each PWM unit
#define PIN_INDEX_MAX 10
#define PERIOD_FREQUENCY_MAX 1000000000
#define DUTYCYCLE_PERCENT_SIZE 8192
#define PF_PWM_COF HCS_NODE(HCS_NODE(HCS_ROOT, platform), pwm_config)
typedef struct _PwmDeviceConfig {
    int channel;
    int pwm_timer;
    int freq_hz;
    int gpio_pin;
    ledc_timer_config_t ledc_timer_config;
    ledc_channel_config_t ledc_channel_config;
} PwmDeviceConfig;

static int32_t PwmDevSetConfig(struct PwmDev* pwm, struct PwmConfig* config);
static int32_t PwmDevOpen(struct PwmDev* pwm);
static int32_t PwmDevClose(struct PwmDev* pwm);

struct PwmMethod g_pwmmethod = {
    .setConfig = PwmDevSetConfig,
    .open = PwmDevOpen,
    .close = PwmDevClose,
};

static uint32_t Period2Frequency(uint32_t period)
{
    if (period == 0) {
        return 0;
    }
    return (PERIOD_FREQUENCY_MAX)/(period);
}

static uint32_t GetEspDutyCyclePercent(uint32_t duty, uint32_t period)
{
    if (duty==0 && period == 0) {
        return 0;
    }
    return (int)(DUTYCYCLE_PERCENT_SIZE*(1.0*duty/period));
}

static uint32_t PwmFindConfig(HdfSListNode node, const char *name, I2cResource resource)
{
    uint32_t result = HDF_FAILURE;
    do {
        if (strcmp(HCS_PROP(node, match_attr), name) == 0) {
            resource->channel = (GetChannelFromStr(HCS_PROP(node, channel)));
            resource->pwm_timer = (GetTimerFromStr(HCS_PROP(node, timer)));
            resource->freq_hz = (HCS_PROP(node, freq_hz));
            resource->gpio_pin = (GetPinIndexFromStr(HCS_PROP(node, gpio_pin)));
            (resource->ledc_timer_config.duty_resolution = LEDC_TIMER_13_BIT);
            (resource->ledc_timer_config.clk_cfg = LEDC_AUTO_CLK);
            (resource->ledc_timer_config.timer_num = resource->pwm_timer);
            (resource->ledc_timer_config.speed_mode = LEDC_LOW_SPEED_MODE);
            (resource->ledc_channel_config.channel = resource->channel);
            (resource->ledc_channel_config.gpio_num = resource->gpio_pin);
            (resource->ledc_channel_config.speed_mode = LEDC_LOW_SPEED_MODE);
            (resource->ledc_channel_config.hpoint = 0);
            (resource->ledc_channel_config.timer_sel = resource->pwm_timer);
            (HDF_LOGD("<---------- %s ------------>", name));
            (HDF_LOGD("channel = %s [%d]", HCS_PROP(node, channel), resource->channel));
            (HDF_LOGD("pwm_timer = %s [%d]", HCS_PROP(node, timer), resource->pwm_timer));
            (HDF_LOGD("freq_hz = %dHz", resource->freq_hz));
            (HDF_LOGD("gpio_pin = %s [%d]", HCS_PROP(node, gpio_pin), resource->gpio_pin));
            result = HDF_SUCCESS;
        }
    } while (0)
    return result;
}

static int GetPinIndexFromStr(const char* str)
{
    if (str == NULL || strlen(str) < 10) {
        HDF_LOGE("pin index config is illegal!");
        return GPIO_NUM_NC;
    }

    char* ptr_nun = &str[9];
    int level = strlen(ptr_nun);      // 获取下标位数
    int pin_index = 0;                 // GPIO号
    for (int i = 0; i < level; i++) {
        if (i != 0) {
            pin_index = PIN_INDEX_MAX * pin_index;
        }
        int num = ptr_nun[i] - '0';
        if (num >= PIN_INDEX_MAX) {   // 字符非法
            HDF_LOGE("pin index config is illegal!");
            return GPIO_NUM_NC;
        }
        pin_index += num;
    }
    return pin_index;
}

static int GetTimerFromStr(const char* str)
{
    if (!strcmp(str, "LEDC_TIMER_0")) {
        return LEDC_TIMER_0;
    } else if (!strcmp(str, "LEDC_TIMER_1")) {
        return LEDC_TIMER_1;
    } else if (!strcmp(str, "LEDC_TIMER_2")) {
        return LEDC_TIMER_2;
    } else if (!strcmp(str, "LEDC_TIMER_3")) {
        return LEDC_TIMER_3;
    }
    HDF_LOGE("pwm_timer is illegal! must be LEDC_TIMER_0-LEDC_TIMER_03.");
    return LEDC_TIMER_MAX;
}

static int GetChannelFromStr(const char* str)
{
    if (!strcmp(str, "LEDC_CHANNEL_0")) {
        return LEDC_CHANNEL_0;
    } else if (!strcmp(str, "LEDC_CHANNEL_1")) {
        return LEDC_CHANNEL_1;
    } else if (!strcmp(str, "LEDC_CHANNEL_2")) {
        return LEDC_CHANNEL_2;
    } else if (!strcmp(str, "LEDC_CHANNEL_3")) {
        return LEDC_CHANNEL_3;
    } else if (!strcmp(str, "LEDC_CHANNEL_4")) {
        return LEDC_CHANNEL_4;
    } else if (!strcmp(str, "LEDC_CHANNEL_5")) {
        return LEDC_CHANNEL_5;
    } else if (!strcmp(str, "LEDC_CHANNEL_6")) {
        return LEDC_CHANNEL_6;
    } else if (!strcmp(str, "LEDC_CHANNEL_7")) {
        return LEDC_CHANNEL_7;
    }
    HDF_LOGE("channel is illegal! must be LEDC_CHANNEL 0-7.");
    return LEDC_CHANNEL_MAX;
}

static uint32_t GetPwmDeviceResource(PwmDeviceConfig* device, const char* deviceMatchAttr)
{
    int32_t result = HDF_FAILURE;
    if (device == NULL || deviceMatchAttr == NULL) {
        HDF_LOGE("%s: device or deviceMatchAttr is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HCS_FOREACH_CHILD_VARGS(PF_PWM_COF, PwmFindConfig, deviceMatchAttr, device);

    if (result != HDF_SUCCESS) {
        HDF_LOGE("resourceNode %s is NULL\r\n", deviceMatchAttr);
    }

    return result;
}

static int32_t AttachPwmDevice(struct PwmDev* host, struct HdfDeviceObject* device)
{
    int32_t ret;
    PwmDeviceConfig* pwmDevice = NULL;
    if (device == NULL || host == NULL) {
        HDF_LOGE("%s: param is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    pwmDevice = (PwmDeviceConfig*)OsalMemAlloc(sizeof(PwmDeviceConfig));
    if (pwmDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc pwmDevice error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = GetPwmDeviceResource(pwmDevice, device->deviceMatchAttr);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(pwmDevice);
        return HDF_FAILURE;
    }

    PwmSetPriv(host, pwmDevice);
    host->num = pwmDevice->channel;

    return HDF_SUCCESS;
}

static int32_t PwmDriverBind(struct HdfDeviceObject* device);
static int32_t PwmDriverInit(struct HdfDeviceObject* device);
static void PwmDriverRelease(struct HdfDeviceObject* device);

struct HdfDriverEntry g_pwmDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "ESP32U4_HDF_PLATFORM_PWM",
    .Bind = PwmDriverBind,
    .Init = PwmDriverInit,
    .Release = PwmDriverRelease,
};
HDF_INIT(g_pwmDriverEntry);

static int32_t PwmDriverBind(struct HdfDeviceObject* device)
{
    struct PwmDev* devService = NULL;
    if (device == NULL) {
        HDF_LOGE("hdfDevice object is null!\r\n");
        return HDF_FAILURE;
    }

    devService = (struct PwmDev*)OsalMemCalloc(sizeof(struct PwmDev));
    if (devService == NULL) {
        HDF_LOGE("malloc pwmDev failed\n");
    }
    device->service = &devService->service;
    devService->device = device;

    return HDF_SUCCESS;
}

static int32_t PwmDriverInit(struct HdfDeviceObject* device)
{
    int32_t ret;
    struct PwmDev* host = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    host = (struct PwmDev*)device->service;
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = AttachPwmDevice(host, device);
    if (ret != HDF_SUCCESS) {
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

    return HDF_SUCCESS;
}

static void PwmDriverRelease(struct HdfDeviceObject* device)
{
    struct PwmDev* host = NULL;
    if (device == NULL || device->service == NULL) {
        HDF_LOGE("device is null\r\n");
        return;
    }
    host = (struct PwmDev*)device->service;
    if (host != NULL && host->device != NULL) {
        host->method = NULL;
        OsalMemFree(host->device);
        OsalMemFree(host);
        host->device = NULL;
        host = NULL;
    }
    device->service = NULL;
    host = NULL;
    return;
}

static int32_t PwmDevSetConfig(struct PwmDev* pwm, struct PwmConfig* config)
{
    int ret;
    if (pwm == NULL || config == NULL) {
        HDF_LOGE("%s: PwmDev* pwm or struct PwmConfig *config is NULL!", __FUNCTION__);
        return HDF_ERR_INVALID_PARAM;
    }

    PwmDeviceConfig* pwmDevice = (PwmDeviceConfig*)PwmGetPriv(pwm);
    if (pwmDevice == NULL) {
        HDF_LOGE("%s: PwmDeviceConfig* pwmDevice is NULL", __FUNCTION__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (config->status == PWM_ENABLE_STATUS) { // 使能PWM
        uint32_t period_frequency = Period2Frequency(config->period);
        if (pwmDevice->ledc_timer_config.freq_hz != period_frequency) {   // 周期有频率变化
            pwmDevice->ledc_timer_config.freq_hz = period_frequency;
            ledc_timer_config(&pwmDevice->ledc_timer_config);       // 重新配置周期
            printf("\r\nconfig freq_hz = %d [period = %d]\r\n", pwmDevice->ledc_timer_config.freq_hz, config->period);
            printf("speed_mode = %d\r\n", pwmDevice->ledc_timer_config.speed_mode);
            printf("timer_num = %d\r\n", pwmDevice->ledc_timer_config.timer_num);
            printf("clk_cfg = %d\r\n", pwmDevice->ledc_timer_config.clk_cfg);
            printf("duty_resolution = %d\r\n", pwmDevice->ledc_timer_config.duty_resolution);
        }
        uint32_t ledc_duty = GetEspDutyCyclePercent(config->duty, config->period);
        if (pwmDevice->ledc_channel_config.duty != ledc_duty) { // 占空比变化
            pwmDevice->ledc_channel_config.duty = GetEspDutyCyclePercent(config->duty, config->period); // 重新设置占空比
            printf("\r\nconfig ledc_duty = %d [duty=%d <--> period=%d]\r\n",
                   pwmDevice->ledc_channel_config.duty,
                   config->duty,
                   config->period);
            printf("gpio_num = %d\r\n", pwmDevice->ledc_channel_config.gpio_num);
            printf("speed_mode = %d\r\n", pwmDevice->ledc_channel_config.speed_mode);
            printf("channel = %d\r\n", pwmDevice->ledc_channel_config.channel);
            printf("timer_sel = %d\r\n", pwmDevice->ledc_channel_config.timer_sel);
            printf("hpoint = %d\r\n", pwmDevice->ledc_channel_config.hpoint);
        }
    } else if (config->status == PWM_DISABLE_STATUS) {   // 失能PWM
        ledc_stop(pwmDevice->ledc_timer_config.speed_mode, pwmDevice->channel, config->polarity);
        printf("------->pwm output stop, set level = %d\r\n", config->polarity);
    }
    return HDF_SUCCESS;
}

static int32_t PwmDevOpen(struct PwmDev* pwm)
{
    if (pwm == NULL) {
        HDF_LOGE("%s: PwmDev* pwm is NULL", __FUNCTION__);
        return HDF_ERR_INVALID_PARAM;
    }

    PwmDeviceConfig* pwmDevice = (PwmDeviceConfig*)PwmGetPriv(pwm);
    if (pwmDevice == NULL) {
        HDF_LOGE("%s: PwmDeviceConfig* pwmDevice is NULL", __FUNCTION__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

static int32_t PwmDevClose(struct PwmDev* pwm)
{
    HDF_LOGI("——> Enter %s", __FUNCTION__);
    return HDF_SUCCESS;
}