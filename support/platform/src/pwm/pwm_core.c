/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pwm_core.h"
#include "hdf_log.h"

#define HDF_LOG_TAG pwm_core

int32_t PwmDeviceGet(struct PwmDev *pwm)
{
    int32_t ret;

    if (pwm == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalSpinLock(&(pwm->lock));
    if (pwm->busy) {
        (void)OsalSpinUnlock(&(pwm->lock));
        HDF_LOGE("%s: pwm%u is busy", __func__, pwm->num);
        return HDF_ERR_DEVICE_BUSY;
    }
    if (pwm->method != NULL && pwm->method->open != NULL) {
        ret = pwm->method->open(pwm);
        if (ret != HDF_SUCCESS) {
            (void)OsalSpinUnlock(&(pwm->lock));
            HDF_LOGE("%s: open failed, ret %d", __func__, ret);
            return HDF_FAILURE;
        }
    }

    pwm->busy = true;
    (void)OsalSpinUnlock(&(pwm->lock));

    return HDF_SUCCESS;
}

int32_t PwmDevicePut(struct PwmDev *pwm)
{
    int32_t ret;

    if (pwm == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (pwm->method != NULL && pwm->method->close != NULL) {
        ret = pwm->method->close(pwm);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: close failed, ret %d", __func__, ret);
            return ret;
        }
    }
    (void)OsalSpinLock(&(pwm->lock));
    pwm->busy = false;
    (void)OsalSpinUnlock(&(pwm->lock));

    return HDF_SUCCESS;
}

int32_t PwmDeviceSetPeriod(struct PwmDev *pwm, uint32_t period)
{
    int32_t ret;
    struct PwmConfig config;
    uint32_t curValue;

    if (pwm == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = PwmDeviceGetConfig(pwm, &config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceGetConfig failed, ret: %d", __func__, ret);
        return ret;
    }
    curValue = config.period;
    config.period = period;

    ret = PwmDeviceSetConfig(pwm, &config);
    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success. period: %d -> %d.", __func__, curValue, config.period);
    }

    return ret;
}

int32_t PwmDeviceSetDuty(struct PwmDev *pwm, uint32_t duty)
{
    int32_t ret;
    struct PwmConfig config;
    uint32_t curValue;

    if (pwm == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = PwmDeviceGetConfig(pwm, &config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceGetConfig failed, ret: %d", __func__, ret);
        return ret;
    }
    curValue = config.duty;
    config.duty = duty;

    ret = PwmDeviceSetConfig(pwm, &config);
    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success. duty: %d -> %d.", __func__, curValue, config.duty);
    }

    return ret;
}

int32_t PwmDeviceSetPolarity(struct PwmDev *pwm, uint8_t polarity)
{
    int32_t ret;
    struct PwmConfig config;
    uint32_t curValue;

    if (pwm == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = PwmDeviceGetConfig(pwm, &config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceGetConfig failed, ret: %d", __func__, ret);
        return ret;
    }
    curValue = config.polarity;
    config.polarity = polarity;

    ret = PwmDeviceSetConfig(pwm, &config);
    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success. polarity: %d -> %d.", __func__, curValue, config.polarity);
    }

    return ret;
}

int32_t PwmDeviceEnable(struct PwmDev *pwm)
{
    int32_t ret;
    struct PwmConfig config;
    uint32_t curValue;

    if (pwm == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = PwmDeviceGetConfig(pwm, &config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceGetConfig failed, ret: %d", __func__, ret);
        return ret;
    }
    curValue = config.status;
    config.status = PWM_ENABLE_STATUS;

    ret = PwmDeviceSetConfig(pwm, &config);
    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success. enable: %d -> %d.", __func__, curValue, config.status);
    }

    return ret;
}

int32_t PwmDeviceDisable(struct PwmDev *pwm)
{
    int32_t ret;
    struct PwmConfig config;
    uint32_t curValue;

    if (pwm == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = PwmDeviceGetConfig(pwm, &config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceGetConfig failed, ret: %d", __func__, ret);
        return ret;
    }
    curValue = config.status;
    config.status = PWM_DISABLE_STATUS;

    ret = PwmDeviceSetConfig(pwm, &config);
    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: success. enable: %d -> %d.", __func__, curValue, config.status);
    }

    return ret;
}

int32_t PwmDeviceSetConfig(struct PwmDev *pwm, struct PwmConfig *config)
{
    int32_t ret;

    if (pwm == NULL || config == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (memcmp(config, &(pwm->cfg), sizeof(*config)) == 0) {
        HDF_LOGE("%s: do not need to set config", __func__);
        return HDF_SUCCESS;
    }

    if (pwm->method == NULL || pwm->method->setConfig == NULL) {
        HDF_LOGE("%s: setConfig is not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    HDF_LOGI("%s: set PwmConfig: number %u, period %u, duty %u, polarity %u, enable %u.", __func__,
        config->number, config->period, config->duty, config->polarity, config->status);

    ret = pwm->method->setConfig(pwm, config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed, ret %d", __func__, ret);
        return ret;
    }

    pwm->cfg = *config;

    return HDF_SUCCESS;
}

int32_t PwmDeviceGetConfig(struct PwmDev *pwm, struct PwmConfig *config)
{
    if (pwm == NULL || config == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    *config = pwm->cfg;

    return HDF_SUCCESS;
}

int32_t PwmSetPriv(struct PwmDev *pwm, void *priv)
{
    if (pwm == NULL) {
        HDF_LOGE("%s: pwm is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    pwm->priv = priv;
    return HDF_SUCCESS;
}

void *PwmGetPriv(struct PwmDev *pwm)
{
    if (pwm == NULL) {
        HDF_LOGE("%s: pwm is null", __func__);
        return NULL;
    }

    return pwm->priv;
}

static int32_t PwmUserSetPeriod(struct PwmDev *pwm, struct HdfSBuf *data)
{
    size_t size;
    uint32_t *period;

    if (!HdfSbufReadBuffer(data, (const void **)&period, &size)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }

    return PwmDeviceSetPeriod(pwm, *period);
}

static int32_t PwmUserSetDuty(struct PwmDev *pwm, struct HdfSBuf *data)
{
    size_t size;
    uint32_t *duty;

    if (!HdfSbufReadBuffer(data, (const void **)&duty, &size)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }

    return PwmDeviceSetDuty(pwm, *duty);
}

static int32_t PwmUserSetPolarity(struct PwmDev *pwm, struct HdfSBuf *data)
{
    size_t size;
    uint32_t *polarity;

    if (!HdfSbufReadBuffer(data, (const void **)&polarity, &size)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }

    return PwmDeviceSetPolarity(pwm, *polarity);
}

static int32_t PwmUserSetConfig(struct PwmDev *pwm, struct HdfSBuf *data)
{
    size_t size;
    struct PwmConfig *config;

    if (!HdfSbufReadBuffer(data, (const void **)&config, &size)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }

    return PwmDeviceSetConfig(pwm, config);
}

static int32_t PwmUserGetConfig(struct PwmDev *pwm, struct HdfSBuf *reply)
{
    int32_t ret;
    struct PwmConfig config;

    ret = PwmDeviceGetConfig(pwm, &config);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!HdfSbufWriteBuffer(reply, &config, sizeof(config))) {
        HDF_LOGE("%s: sbuf write buffer failed", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t PwmIoDispatch(struct HdfDeviceIoClient *client, int cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct PwmDev *pwm = NULL;

    if (client == NULL) {
        HDF_LOGE("%s: client is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (client->device == NULL) {
        HDF_LOGE("%s: client->device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (client->device->service == NULL) {
        HDF_LOGE("%s: client->device->service is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    pwm = (struct PwmDev *)client->device->service;
    switch (cmd) {
        case PWM_IO_GET:
            return PwmDeviceGet(pwm);
        case PWM_IO_PUT:
            return PwmDevicePut(pwm);
        case PWM_IO_SET_PERIOD:
            return PwmUserSetPeriod(pwm, data);
        case PWM_IO_SET_DUTY:
            return PwmUserSetDuty(pwm, data);
        case PWM_IO_SET_POLARITY:
            return PwmUserSetPolarity(pwm, data);
        case PWM_IO_ENABLE:
            return PwmDeviceEnable(pwm);
        case PWM_IO_DISABLE:
            return PwmDeviceDisable(pwm);
        case PWM_IO_SET_CONFIG:
            return PwmUserSetConfig(pwm, data);
        case PWM_IO_GET_CONFIG:
            return PwmUserGetConfig(pwm, reply);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
}

int32_t PwmDeviceAdd(struct HdfDeviceObject *obj, struct PwmDev *pwm)
{
    if (obj == NULL || pwm == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (pwm->method == NULL || pwm->method->setConfig == NULL) {
        HDF_LOGE("%s: setConfig is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (OsalSpinInit(&(pwm->lock)) != HDF_SUCCESS) {
        HDF_LOGE("%s: init spinlock fail", __func__);
        return HDF_FAILURE;
    }
    pwm->device = obj;
    obj->service = &(pwm->service);
    pwm->device->service->Dispatch = PwmIoDispatch;
    return HDF_SUCCESS;
}

void PwmDeviceRemove(struct HdfDeviceObject *obj, struct PwmDev *pwm)
{
    if (obj == NULL || pwm == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return;
    }
    (void)OsalSpinDestroy(&(pwm->lock));
    pwm->device = NULL;
    obj->service = NULL;
}
