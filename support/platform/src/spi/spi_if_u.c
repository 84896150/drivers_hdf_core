/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "spi_if.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG spi_if
#define HOST_NAME_LEN 32

struct SpiObject {
    struct SpiCntlr *cntlr;
    uint32_t csNum;
};

static struct SpiCntlr *SpiGetCntlrByBusNum(uint32_t num)
{
    int ret;
    char *name = NULL;
    struct SpiCntlr *cntlr = NULL;

    name = (char *)OsalMemCalloc(HOST_NAME_LEN + 1);
    if (name == NULL) {
        return NULL;
    }
    ret = snprintf_s(name, HOST_NAME_LEN + 1, HOST_NAME_LEN, "HDF_PLATFORM_SPI_%u", num);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        OsalMemFree(name);
        return NULL;
    }
    cntlr = (struct SpiCntlr *)HdfIoServiceBind(name);
    OsalMemFree(name);
    return cntlr;
}

static int32_t SpiMsgWriteArray(struct SpiObject *object, struct HdfSBuf *data, struct SpiMsg *msgs, uint32_t count)
{
    uint32_t i;

    if (!HdfSbufWriteUint32(data, object->csNum)) {
        HDF_LOGE("%s: write csNum failed!", __func__);
        return HDF_ERR_IO;
    }

    if (!HdfSbufWriteBuffer(data, (uint8_t *)msgs, sizeof(*msgs) * count)) {
        HDF_LOGE("%s: write msgs array failed!", __func__);
        return HDF_ERR_IO;
    }

    for (i = 0; i < count; i++) {
        if (!HdfSbufWriteBuffer(data, (uint8_t *)msgs[i].wbuf, msgs[i].len)) {
            HDF_LOGE("%s: write msg[%d] buf failed!", __func__, i);
            return HDF_ERR_IO;
        }
    }

    return HDF_SUCCESS;
}

static int32_t SpiMsgReadBack(struct HdfSBuf *data, struct SpiMsg *msg)
{
    uint32_t rLen;
    const void *rBuf = NULL;

    if (!HdfSbufReadBuffer(data, &rBuf, &rLen)) {
        HDF_LOGE("%s: read rBuf failed!", __func__);
        return HDF_ERR_IO;
    }
    if (msg->len != rLen) {
        HDF_LOGE("%s: err len:%u, rLen:%u", __func__, msg->len, rLen);
        if (rLen > msg->len) {
            rLen = msg->len;
        }
    }
    if (memcpy_s(msg->rbuf, msg->len, rBuf, rLen) != EOK) {
        HDF_LOGE("%s: memcpy rBuf failed!", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static inline int32_t SpiMsgReadArray(struct HdfSBuf *reply, struct SpiMsg *msgs, uint32_t count)
{
    uint32_t i;
    int32_t ret;

    for (i = 0; i < count; i++) {
        ret = SpiMsgReadBack(reply, &msgs[i]);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
    }
    return HDF_SUCCESS;
}

int32_t SpiTransfer(DevHandle handle, struct SpiMsg *msgs, uint32_t count)
{
    uint32_t i;
    int32_t ret;
    uint32_t recvLen = 0;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    struct SpiObject *object = NULL;

    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    object = (struct SpiObject *)handle;

    data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = SpiMsgWriteArray(object, data, msgs, count);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to write msgs!", __func__);
        goto __EXIT;
    }

    for (i = 0; i < count; i++) {
        recvLen += msgs[i].len + sizeof(uint64_t);
    }
    reply = HdfSBufObtain(recvLen);
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply!", __func__);
        ret = HDF_ERR_MALLOC_FAIL;
        goto __EXIT;
    }

    service = (struct HdfIoService *)object->cntlr;
    ret = service->dispatcher->Dispatch(&service->object, SPI_IO_TRANSFER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to send service call:%d", __func__, ret);
        goto __EXIT;
    }

    ret = SpiMsgReadArray(reply, msgs, count);
    if (ret != HDF_SUCCESS) {
        goto __EXIT;
    }

    ret = count;
__EXIT:
    if (data != NULL) {
        HdfSBufRecycle(data);
    }
    if (reply != NULL) {
        HdfSBufRecycle(reply);
    }
    return ret;
}

int32_t SpiRead(DevHandle handle, uint8_t *buf, uint32_t len)
{
    struct SpiMsg msg = {0};

    msg.wbuf = NULL;
    msg.rbuf = buf;
    msg.len = len;
    return SpiTransfer(handle, &msg, 1);
}

int32_t SpiWrite(DevHandle handle, uint8_t *buf, uint32_t len)
{
    struct SpiMsg msg = {0};

    msg.wbuf = buf;
    msg.rbuf = NULL;
    msg.len = len;
    return SpiTransfer(handle, &msg, 1);
}

int32_t SpiSetCfg(DevHandle handle, struct SpiCfg *cfg)
{
    int32_t ret;
    struct SpiObject *object = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    object = (struct SpiObject *)handle;

    data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = HdfSbufWriteUint32(data, object->csNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SpiOpen: write csNum failed!");
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }

    ret = HdfSbufWriteBuffer(data, cfg, sizeof(*cfg));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SpiOpen: write cfg failed!");
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }

    service = (struct HdfIoService *)object->cntlr;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, SPI_IO_SET_CONFIG, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed, ret is %d", __func__, ret);
        OsalMemFree(object);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

int32_t SpiGetCfg(DevHandle handle, struct SpiCfg *cfg)
{
    int32_t ret;
    uint32_t len;
    const void *rBuf = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct SpiObject *object = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: invalid handle", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    object = (struct SpiObject *)handle;

    data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply", __func__);
        HdfSBufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = HdfSbufWriteUint32(data, object->csNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SpiOpen: write csNum failed!");
        goto __EXIT;
    }

    service = (struct HdfIoService *)object->cntlr;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        ret = HDF_ERR_MALLOC_FAIL;
        goto __EXIT;
    }
    ret = service->dispatcher->Dispatch(&service->object, SPI_IO_GET_CONFIG, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed, ret is %d", __func__, ret);
        goto __EXIT;
    }

    ret = HdfSbufReadBuffer(reply, &rBuf, &len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read buffer failed", __func__);
        goto __EXIT;
    }
    if (memcpy_s(cfg, sizeof(struct SpiCfg), rBuf, len) != EOK) {
        HDF_LOGE("%s: memcpy rBuf failed", __func__);
        ret = HDF_ERR_IO;
        goto __EXIT;
    } 
__EXIT:
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

void SpiClose(DevHandle handle)
{
    int32_t ret;
    struct SpiObject *object = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is invalid", __func__);
        return;
    }
    object = (struct SpiObject *)handle;

    data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data", __func__);
        return;
    }
    if (!HdfSbufWriteUint32(data, object->csNum)) {
        HDF_LOGE("SpiOpen: write csNum failed!");
        HdfSBufRecycle(data);
        return;
    }

    service = (struct HdfIoService *)object->cntlr;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfSBufRecycle(data);
        return;
    }
    ret = service->dispatcher->Dispatch(&service->object, SPI_IO_CLOSE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed, ret is %d", __func__, ret);
        HdfSBufRecycle(data);
    }
    OsalMemFree(handle);
}

DevHandle SpiOpen(const struct SpiDevInfo *info)
{
    int32_t ret;
    struct SpiObject *object = NULL;
    struct SpiCntlr *cntlr = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    if (info == NULL) {
        return NULL;
    }
    cntlr = SpiGetCntlrByBusNum(info->busNum);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return NULL;
    }
    data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data", __func__);
        return NULL;
    }
    if (!HdfSbufWriteUint32(data, info->csNum)) {
        HDF_LOGE("SpiOpen: write csNum failed!");
        HdfSBufRecycle(data);
        return NULL;
    }
    object = (struct SpiObject *)OsalMemCalloc(sizeof(*object));
    if (object == NULL) {
        HDF_LOGE("%s: object malloc failed", __func__);
        HdfSBufRecycle(data);
        return NULL;
    }
    service = (struct HdfIoService *)cntlr;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfSBufRecycle(data);
        return NULL;
    }
    ret = service->dispatcher->Dispatch(&service->object, SPI_IO_OPEN, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed, ret is %d", __func__, ret);
        OsalMemFree(object);
        return NULL;
    }
    object->cntlr = cntlr;
    object->csNum = info->csNum;
    HdfSBufRecycle(data);
    return (DevHandle)object;
}
