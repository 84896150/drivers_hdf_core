/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_io_service_if.h"
#include "adc_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG adc_if_c
#define ADC_SERVICE_NAME "HDF_PLATFORM_ADC_MANAGER"

static void *AdcManagerGetService(void)
{
    static void *manager = NULL;

    if (manager != NULL) {
        return manager;
    }
    manager = (void *)HdfIoServiceBind(ADC_SERVICE_NAME);
    if (manager == NULL) {
        HDF_LOGE("AdcManagerGetService: fail to get adc manager!");
    }
    return manager;
}

DevHandle AdcOpen(uint32_t number)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    uint32_t handle;

    service = (struct HdfIoService *)AdcManagerGetService();
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

    if (!HdfSbufWriteUint32(data, (uint32_t)number)) {
        HDF_LOGE("AdcOpen: write number fail!");
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return NULL;
    }

    ret = service->dispatcher->Dispatch(&service->object, ADC_IO_OPEN, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("AdcOpen: service call open fail:%d", ret);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return NULL;
    }

    if (!HdfSbufReadUint32(reply, &handle)) {
        HDF_LOGE("AdcOpen: read handle fail!");
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return NULL;
    }
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return (DevHandle)(uintptr_t)handle;
}

void AdcClose(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;

    service = (struct HdfIoService *)AdcManagerGetService();
    if (service == NULL) {
        return;
    }

    data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("AdcClose: write handle fail!");
        HdfSBufRecycle(data);
        return;
    }

    ret = service->dispatcher->Dispatch(&service->object, ADC_IO_CLOSE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("AdcClose: close handle fail:%d", ret);
    }
    HdfSBufRecycle(data);
}

int32_t AdcRead(DevHandle handle, uint32_t channel, uint32_t *val)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
	
    service = (struct HdfIoService *)AdcManagerGetService();
    if (service == NULL) {
        return HDF_PAL_ERR_DEV_CREATE;
    }

    data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("AdcRead: failed to obtain data!");
        return HDF_ERR_MALLOC_FAIL;
    }

    reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("AdcRead: write handle fail!");
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_ERR_IO;
    }
    if (!HdfSbufWriteUint32(data, (uint16_t)channel)) {
        HDF_LOGE("%s: write adc number failed!", __func__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_ERR_IO;
    }
    ret = service->dispatcher->Dispatch(&service->object, ADC_IO_READ, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("AdcRead: failed to send service call:%d", ret);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return ret;
    }

	if (!HdfSbufReadUint32(reply, val)) {
	HDF_LOGE("%s: read sbuf failed", __func__);
	HdfSBufRecycle(data);
	HdfSBufRecycle(reply);
	return HDF_ERR_IO;
    }

    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return HDF_SUCCESS;
}
