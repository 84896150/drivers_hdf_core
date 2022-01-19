/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "uart_if.h"
#include "hdf_log.h"
#include "hdf_io_service_if.h"
#include "osal_mem.h"
#include "securec.h"


#define HDF_LOG_TAG uart_if_u_c
#define UART_HOST_NAME_LEN 32

static void *UartGetObjGetByBusNum(uint32_t num)
{
    int ret;
    void *obj = NULL;
    char *name = NULL;

    name = (char *)OsalMemCalloc(UART_HOST_NAME_LEN + 1);
    if (name == NULL) {
        return NULL;
    }
    ret = snprintf_s(name, UART_HOST_NAME_LEN + 1, UART_HOST_NAME_LEN,
        "HDF_PLATFORM_UART_%u", num);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        OsalMemFree(name);
        return NULL;
    }

    obj = (void *)HdfIoServiceBind(name);
    OsalMemFree(name);
    return obj;
}

static void UartPutObjByPointer(const void *obj)
{
    if (obj == NULL) {
        return;
    }
    HdfIoServiceRecycle((struct HdfIoService *)obj);
};

DevHandle UartOpen(uint32_t port)
{
    int32_t ret;
    void *handle = NULL;

    handle = UartGetObjGetByBusNum(port);
    if (handle == NULL) {
        HDF_LOGE("%s: get handle error", __func__);
        return NULL;
    }

    struct HdfIoService *service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        UartPutObjByPointer(handle);
        return NULL;
    }
    ret = service->dispatcher->Dispatch(&service->object, UART_IO_INIT, NULL, NULL);

    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UartHostInit error, ret %d", __func__, ret);
        UartPutObjByPointer(handle);
        return NULL;
    }

    return (DevHandle)handle;
}

void UartClose(DevHandle handle)
{
    int32_t ret;
    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return;
    }

    struct HdfIoService *service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        UartPutObjByPointer(handle);
        return;
    }
    ret = service->dispatcher->Dispatch(&service->object, UART_IO_DEINIT, NULL, NULL);

    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UartHostDeinit error, ret %d", __func__, ret);
    }
    UartPutObjByPointer(handle);
}

static int32_t UartUserReceive(DevHandle handle, void *data, uint32_t size, enum UartIoCmd cmd)
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
    reply = HdfSBufObtain(size + sizeof(uint64_t));
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
    if (size != rLen && cmd != UART_IO_READ) {
        HDF_LOGE("%s: read error, size %u, rLen %u", __func__, size, rLen);
        ret = HDF_FAILURE;
        goto __EXIT;
    }
    if (memcpy_s(data, size, rBuf, rLen) != EOK) {
        HDF_LOGE("%s: memcpy rBuf failed", __func__);
        ret = HDF_ERR_IO;
        goto __EXIT;
    }
    if (cmd == UART_IO_READ) {
        ret = rLen;
    }
__EXIT:
    HdfSBufRecycle(reply);
    return ret;
}

static int32_t UartUserSend(DevHandle handle, void *data, uint32_t size, enum UartIoCmd cmd)
{
    int32_t ret;
    struct HdfSBuf *buf = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    buf = HdfSBufObtain(size);
    if (buf == NULL) {
        HDF_LOGE("%s: failed to obtain buf", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteBuffer(buf, data, size)) {
        HDF_LOGE("%s: sbuf write buffer failed", __func__);
        HdfSBufRecycle(buf);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, cmd, buf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to write, ret %d", __func__, ret);
    }
    HdfSBufRecycle(buf);
    return ret;
}


int32_t UartRead(DevHandle handle, uint8_t *data, uint32_t size)
{
    return UartUserReceive(handle, data, size, UART_IO_READ);
}

int32_t UartWrite(DevHandle handle, uint8_t *data, uint32_t size)
{
    return UartUserSend(handle, data, size, UART_IO_WRITE);
}

int32_t UartGetBaud(DevHandle handle, uint32_t *baudRate)
{
    return UartUserReceive(handle, baudRate, sizeof(*baudRate), UART_IO_GET_BAUD);
}

int32_t UartSetBaud(DevHandle handle, uint32_t baudRate)
{
    return UartUserSend(handle, &baudRate, sizeof(baudRate), UART_IO_SET_BAUD);
}

int32_t UartGetAttribute(DevHandle handle, struct UartAttribute *attribute)
{
    return UartUserReceive(handle, attribute, sizeof(*attribute), UART_IO_GET_ATTRIBUTE);
}

int32_t UartSetAttribute(DevHandle handle, struct UartAttribute *attribute)
{
    return UartUserSend(handle, attribute, sizeof(*attribute), UART_IO_SET_ATTRIBUTE);
}

int32_t UartSetTransMode(DevHandle handle, enum UartTransMode mode)
{
    return UartUserSend(handle, &mode, sizeof(mode), UART_IO_SET_TRANSMODE);
}
