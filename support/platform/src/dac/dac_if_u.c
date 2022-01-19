/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "dac_if.h"
#include "hdf_io_service_if.h"
#include "platform_core.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG dac_if_c
#define DAC_SERVICE_NAME "HDF_PLATFORM_DAC_MANAGER"

static void *DacManagerServiceGet(void)
{
    static void *manager = NULL;

    if (manager != NULL) {
        return manager;
    }
    manager = (void *)HdfIoServiceBind("HDF_PLATFORM_DAC_MANAGER");
    if (manager == NULL) {
        HDF_LOGE("%s: failed to get dac manager service!", __func__);
    }
    return manager;
}

DevHandle DacOpen(uint32_t number)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    uint32_t handle;

    service = (struct HdfIoService *)DacManagerServiceGet();
    if (service == NULL) {
        return NULL;
    }
    data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return NULL;
    }
    reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        return NULL;
    }

    if (!HdfSbufWriteUint16(data, (uint16_t)number)) {
        HDF_LOGE("DacOpen: write number fail!");
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return NULL;
    }

    ret = service->dispatcher->Dispatch(&service->object, DAC_IO_OPEN, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("DacOpen: service call open fail:%d", ret);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return NULL;
    }

    if (!HdfSbufReadUint32(reply, &handle)) {
        HDF_LOGE("DacOpen: read handle fail!");
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return NULL;
    }
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return (DevHandle)(uintptr_t)handle;
}

void DacClose(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;

    service = (struct HdfIoService *)DacManagerServiceGet();
    if (service == NULL) {
        return;
    }

    data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("DacClose: write handle fail!");
        HdfSBufRecycle(data);
        return;
    }

    ret = service->dispatcher->Dispatch(&service->object, DAC_IO_CLOSE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("DacClose: close handle fail:%d", ret);
    }
    HdfSBufRecycle(data);
}

int32_t DacWrite(DevHandle handle, uint32_t channel, uint32_t val)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;

    service = (struct HdfIoService *)DacManagerServiceGet();
    if (service == NULL) {
        return HDF_PLT_ERR_DEV_GET;
    }

    data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("DacWrite: write handle fail!");
        HdfSBufRecycle(data);
        return HDF_ERR_IO;
    }

    if (!HdfSbufWriteUint32(data, channel)) {
        HDF_LOGE("DacWrite: write channel fail!");
        HdfSBufRecycle(data);
        return HDF_ERR_IO;
    }

    if (!HdfSbufWriteUint32(data, val)) {
        HDF_LOGE("DacWrite: write val fail!");
        HdfSBufRecycle(data);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, DAC_IO_WRITE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("DacWrite: write dac fail:%d", ret);
    }
    HdfSBufRecycle(data);
    return ret;
}
