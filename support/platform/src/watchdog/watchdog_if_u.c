/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "watchdog_if.h"
#include "securec.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG watchdog_if_u

#define WATCHDOG_ID_MAX   8
#define WATCHDOG_NAME_LEN 32

static void *WatchdogGetById(int16_t wdtId)
{
    char *serviceName = NULL;
    void *obj = NULL;

    if (wdtId < 0 || wdtId >= WATCHDOG_ID_MAX) {
        HDF_LOGE("WatchdogGetById: invalid id:%d", wdtId);
        return NULL;
    }

    serviceName = OsalMemCalloc(WATCHDOG_NAME_LEN + 1);
    if (serviceName == NULL) {
        return NULL;
    }

    if (snprintf_s(serviceName, WATCHDOG_NAME_LEN + 1, WATCHDOG_NAME_LEN,
        "HDF_PLATFORM_WATCHDOG_%d", wdtId) < 0) {
        HDF_LOGE("WatchdogGetById: format service name fail!");
        OsalMemFree(serviceName);
        return NULL;
    }

    obj = (void *)HdfIoServiceBind(serviceName);
    if (obj == NULL) {
        HDF_LOGE("WatchdogGetById: get obj fail!");
    }

    OsalMemFree(serviceName);
    return obj;
}

static void WatchdogPutObjByPointer(const void *obj)
{
    if (obj == NULL) {
        return;
    }
    HdfIoServiceRecycle((struct HdfIoService *)obj);
}

static int32_t WatchdogUserReceive(DevHandle handle, void *data, uint32_t size, enum WatchdogIoCmd cmd)
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

static int32_t WatchdogUserSend(DevHandle handle, void *data, uint32_t size, enum WatchdogIoCmd cmd)
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

int32_t WatchdogOpen(int16_t wdtId, DevHandle *handle)
{
    int32_t ret;
    int32_t temp;

    *handle = WatchdogGetById(wdtId);
    if (*handle == NULL) {
        *handle = NULL;
        return HDF_FAILURE;
    }

    temp = WatchdogUserReceive(*handle, &ret, sizeof(ret), WATCHDOG_IO_GET_PRIV);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WatchdogGetPrivData error, ret %d", __func__, ret);
        WatchdogPutObjByPointer(*handle);
        return ret;
    }
    
    return ret;
}

void WatchdogClose(DevHandle handle)
{
    int32_t ret;

    struct HdfIoService *service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        WatchdogPutObjByPointer(handle);
        return;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_RELEASE_PRIV, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WatchdogReleasePriv error, ret %d", __func__, ret);
    }
    WatchdogPutObjByPointer(handle);
}

int32_t WatchdogGetStatus(DevHandle handle, int32_t *status)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return WatchdogUserReceive(handle, status, sizeof(*status), WATCHDOG_IO_GET_STATUS);
}

int32_t WatchdogStart(DevHandle handle)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t ret;

    struct HdfIoService *service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        WatchdogPutObjByPointer(handle);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_START, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WatchdogCntlrStart error, ret %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t WatchdogStop(DevHandle handle)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t ret;

    struct HdfIoService *service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        WatchdogPutObjByPointer(handle);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_STOP, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WatchdogCntlrStop error, ret %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t WatchdogSetTimeout(DevHandle handle, uint32_t seconds)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    return WatchdogUserSend(handle, &seconds, sizeof(seconds), WATCHDOG_IO_SET_TIMEOUT);
}

int32_t WatchdogGetTimeout(DevHandle handle, uint32_t *seconds)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    return WatchdogUserReceive(handle, seconds, sizeof(*seconds), WATCHDOG_IO_GET_TIMEOUT);
}

int32_t WatchdogFeed(DevHandle handle)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t ret;

    struct HdfIoService *service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        WatchdogPutObjByPointer(handle);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_FEED, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WatchdogCntlrFeed error, ret %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}