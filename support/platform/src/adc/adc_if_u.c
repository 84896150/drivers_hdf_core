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

#define HDF_LOG_TAG adc_if_u_c
#define ADC_SERVICE_NAME "HDF_PLATFORM_ADC_MANAGER"

static struct HdfIoService *AdcManagerGetService(void)
{
    static struct HdfIoService *service = NULL;

    if (service != NULL) {
        return service;
    }
    service = (struct HdfIoService *)HdfIoServiceBind(ADC_SERVICE_NAME);
    if (service == NULL) {
        HDF_LOGE("AdcManagerGetService: fail to get adc manager!");
    }
    return service;
}

DevHandle AdcOpen(uint32_t number)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    uint32_t handle;

    service = AdcManagerGetService();
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
        goto __ERR;
    }

    ret = service->dispatcher->Dispatch(&service->object, ADC_IO_OPEN, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("AdcOpen: service call open fail:%d", ret);
        goto __ERR;
    }

    if (!HdfSbufReadUint32(reply, &handle)) {
        HDF_LOGE("AdcOpen: read handle fail!");
        goto __ERR;
    }
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return (DevHandle)(uint32_t)handle;
__ERR:
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return NULL;
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

    if (!HdfSbufWriteUint32(data, (uint32_t)handle)) {
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

    if (!HdfSbufWriteUint32(data, (uint32_t)handle)) {
        HDF_LOGE("AdcRead: write handle fail!");
	    ret = HDF_ERR_IO;
        goto __EXIT;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)channel)) {
        HDF_LOGE("%s: write adc number failed!", __func__);
	    ret = HDF_ERR_IO;
        goto __EXIT;
    }
    ret = service->dispatcher->Dispatch(&service->object, ADC_IO_READ, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("AdcRead: failed to send service call:%d", ret);
        goto __EXIT;
    }

	if (!HdfSbufReadUint32(reply, val)) {
	HDF_LOGE("%s: read sbuf failed", __func__);
	    ret = HDF_ERR_IO;
        goto __EXIT;
    }

    goto __EXIT;
__EXIT:
	HdfSBufRecycle(data);
	HdfSBufRecycle(reply);
    return ret;
}
