/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */


#include "pwm_if.h"
#include "devsvc_manager_clnt.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "pwm_core.h"
#include "securec.h"

#define HDF_LOG_TAG pwm_if_c
#define PWM_NAME_LEN 32

static void *PwmGetDevByNum(uint32_t num)
{
    int32_t ret;
    char *name = NULL;
    void *pwm = NULL;

    name = (char *)OsalMemCalloc(PWM_NAME_LEN + 1);
    if (name == NULL) {
        return NULL;
    }
    ret = snprintf_s(name, PWM_NAME_LEN + 1, PWM_NAME_LEN, "HDF_PLATFORM_PWM_%u", num);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        OsalMemFree(name);
        return NULL;
    }
    pwm = (void *)DevSvcManagerClntGetService(name);
    OsalMemFree(name);
    return pwm;
}

DevHandle PwmOpen(uint32_t num)
{
    int32_t ret;
    void *pwm = PwmGetDevByNum(num);

    if (pwm == NULL) {
        HDF_LOGE("%s: dev is null", __func__);
        return NULL;
    }

    ret = PwmDeviceGet((struct PwmDev *)pwm);
    if(ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceGet failed, ret: %d", __func__, ret);
        return NULL;
    }
    return (DevHandle)pwm;
}

void PwmClose(DevHandle handle)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s: dev is null", __func__);
        return;
    }

    ret = PwmDevicePut((struct PwmDev *)handle);
    HDF_LOGI("func: %s, ret: %d", __func__, ret);
}

int32_t PwmSetPeriod(DevHandle handle, uint32_t period)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = PwmDeviceSetPeriod((struct PwmDev *)handle, period);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceSetPeriod failed, ret: %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t PwmSetDuty(DevHandle handle, uint32_t duty)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = PwmDeviceSetDuty((struct PwmDev *)handle, duty);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceSetDuty failed, ret: %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t PwmSetPolarity(DevHandle handle, uint8_t polarity)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = PwmDeviceSetPolarity((struct PwmDev *)handle, polarity);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceSetPolarity failed, ret: %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t PwmEnable(DevHandle handle)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = PwmDeviceEnable((struct PwmDev *)handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceEnable failed, ret: %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t PwmDisable(DevHandle handle)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = PwmDeviceDisable((struct PwmDev *)handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceDisable failed, ret: %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t PwmSetConfig(DevHandle handle, struct PwmConfig *config)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (config == NULL) {
        HDF_LOGE("%s: config is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = PwmDeviceSetConfig((struct PwmDev *)handle, config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmSetConfig failed, ret: %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t PwmGetConfig(DevHandle handle, struct PwmConfig *config)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (config == NULL) {
        HDF_LOGE("%s: config is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = PwmDeviceGetConfig((struct PwmDev *)handle, config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceGetConfig failed, ret: %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}
