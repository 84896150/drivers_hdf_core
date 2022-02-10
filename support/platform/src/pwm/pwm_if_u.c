/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pwm_if.h"
#include "hdf_log.h"
#include "hdf_io_service_if.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG pwm_if_u_c
#define PWM_NAME_LEN 32

static void *PwmGetDevByNum(uint32_t num)
{
    int32_t ret;
    char *name = NULL;
    void *pwm = NULL;

    name = (char *)OsalMemCalloc(PWM_NAME_LEN + 1);
    if (name == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __func__);
        return NULL;
    }

    ret = snprintf_s(name, PWM_NAME_LEN + 1, PWM_NAME_LEN, "HDF_PLATFORM_PWM_%u", num);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        OsalMemFree(name);
        return NULL;
    }

    pwm = (void *)HdfIoServiceBind(name);
    if (pwm == NULL) {
        HDF_LOGE("%s: HdfIoServiceBind failed", __func__);
        OsalMemFree(name);
        return NULL;
    }

    OsalMemFree(name);
    return pwm;
}

static void PwmPutObjByPointer(const void *obj)
{
    if (obj == NULL) {
        return;
    }

    HdfIoServiceRecycle((struct HdfIoService *)obj);
}

DevHandle PwmOpen(uint32_t num)
{
    int32_t ret;
    void *pwm = PwmGetDevByNum(num);

    if (pwm == NULL) {
        HDF_LOGE("%s: dev is null", __func__);
        return NULL;
    }

    struct HdfIoService *service = (struct HdfIoService *)pwm;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        PwmPutObjByPointer(pwm);
        return NULL;
    }

    ret = service->dispatcher->Dispatch(&service->object, PWM_IO_GET, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceGet error, ret %d", __func__, ret);
        PwmPutObjByPointer(pwm);
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
    
    struct HdfIoService *service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        PwmPutObjByPointer(handle);
        return;
    }

    ret = service->dispatcher->Dispatch(&service->object, PWM_IO_PUT, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDevicePut error, ret %d", __func__, ret);
    }

    PwmPutObjByPointer(handle);
}

static int32_t PwmUserReceive(DevHandle handle, void *data, uint32_t size, enum PwmIoCmd cmd)
{
    int32_t ret;
    uint32_t rLen;
    const void *rBuf = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    /* Four bytes are used to store the buffer length, and four bytes are used to align the memory. */
    reply = HdfSbufObtain(size + sizeof(uint64_t));
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = service->dispatcher->Dispatch(&service->object, cmd, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to read, ret %d", __func__, ret);
        goto __EXIT;
    }
    if (!HdfSbufReadBuffer(reply, &rBuf, &rLen)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        ret = HDF_ERR_IO;
        goto __EXIT;
    }
    
    if (memcpy_s(data, size, rBuf, rLen) != EOK) {
        HDF_LOGE("%s: memcpy rBuf failed", __func__);
        ret = HDF_ERR_IO;
        goto __EXIT;
    }
    
__EXIT:
    HdfSbufRecycle(reply);
    return ret;
}

static int32_t PwmUserSend(DevHandle handle, void *data, uint32_t size, enum PwmIoCmd cmd)
{
    int32_t ret;
    struct HdfSBuf *buf = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    buf = HdfSbufObtain(size);
    if (buf == NULL) {
        HDF_LOGE("%s: failed to obtain buf", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteBuffer(buf, data, size)) {
        HDF_LOGE("%s: sbuf write buffer failed", __func__);
        HdfSbufRecycle(buf);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, cmd, buf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to write, ret %d", __func__, ret);
    }
    HdfSbufRecycle(buf);
    return ret;
}

int32_t PwmSetPeriod(DevHandle handle, uint32_t period)
{
    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return PwmUserSend(handle, &period, sizeof(period), PWM_IO_SET_PERIOD);
}

int32_t PwmSetDuty(DevHandle handle, uint32_t duty)
{
    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return PwmUserSend(handle, &duty, sizeof(duty), PWM_IO_SET_DUTY);
}

int32_t PwmSetPolarity(DevHandle handle, uint8_t polarity)
{
    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return PwmUserSend(handle, &polarity, sizeof(polarity), PWM_IO_SET_POLARITY);
}

int32_t PwmEnable(DevHandle handle)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct HdfIoService *service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        PwmPutObjByPointer(handle);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = service->dispatcher->Dispatch(&service->object, PWM_IO_ENABLE, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceEnable error, ret %d", __func__, ret);
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

    struct HdfIoService *service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        PwmPutObjByPointer(handle);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = service->dispatcher->Dispatch(&service->object, PWM_IO_DISABLE, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceDisable error, ret %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t PwmSetConfig(DevHandle handle, struct PwmConfig *config)
{
    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (config == NULL) {
        HDF_LOGE("%s: config is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return PwmUserSend(handle, config, sizeof(*config), PWM_IO_SET_CONFIG);
}

int32_t PwmGetConfig(DevHandle handle, struct PwmConfig *config)
{
    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (config == NULL) {
        HDF_LOGE("%s: config is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return PwmUserReceive(handle, config, sizeof(*config), PWM_IO_GET_CONFIG);
}