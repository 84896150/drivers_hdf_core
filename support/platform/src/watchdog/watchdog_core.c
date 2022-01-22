/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "watchdog_core.h"
#include "hdf_log.h"
#include "watchdog_if.h"

#define HDF_LOG_TAG watchdog_core

int32_t WatchdogGetPrivData(struct WatchdogCntlr *cntlr)
{
HDF_LOGE("%s: enter!", __func__);
    if (cntlr == NULL || cntlr->ops == NULL) {
        HDF_LOGE("cntlr or cntlr->ops is null");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops->getPriv != NULL) {
        return cntlr->ops->getPriv(cntlr);
    }
HDF_LOGE("%s: done!", __func__);
    return HDF_SUCCESS;
}

int32_t WatchdogReleasePriv(struct WatchdogCntlr *cntlr)
{
HDF_LOGE("%s: enter!", __func__);
    if (cntlr == NULL || cntlr->ops == NULL) {
        HDF_LOGE("cntlr or cntlr->ops is null");
        return HDF_SUCCESS;
    }
    if (cntlr->ops->releasePriv != NULL) {
        cntlr->ops->releasePriv(cntlr);
    }
HDF_LOGE("%s: done!", __func__);
    return HDF_SUCCESS;
}

int32_t WatchdogCntlrGetStatus(struct WatchdogCntlr *cntlr, int32_t *status)
{
    int32_t ret;
    uint32_t flags;
HDF_LOGE("%s: enter!", __func__);
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->getStatus == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    if (status == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (OsalSpinLockIrqSave(&cntlr->lock, &flags) != HDF_SUCCESS) {
        return HDF_ERR_DEVICE_BUSY;
    }
    ret = cntlr->ops->getStatus(cntlr, status);
    (void)OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
HDF_LOGE("%s: done!", __func__);
    return ret;
}

int32_t WatchdogCntlrStart(struct WatchdogCntlr *cntlr)
{
    int32_t ret;
    uint32_t flags;
HDF_LOGE("%s: enter!", __func__);
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->start == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalSpinLockIrqSave(&cntlr->lock, &flags) != HDF_SUCCESS) {
        return HDF_ERR_DEVICE_BUSY;
    }
    ret = cntlr->ops->start(cntlr);
    (void)OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
HDF_LOGE("%s: done!", __func__);
    return ret;
}

int32_t WatchdogCntlrStop(struct WatchdogCntlr *cntlr)
{
    int32_t ret;
    uint32_t flags;
HDF_LOGE("%s: enter!", __func__);
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->stop == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalSpinLockIrqSave(&cntlr->lock, &flags) != HDF_SUCCESS) {
        return HDF_ERR_DEVICE_BUSY;
    }
    ret = cntlr->ops->stop(cntlr);
    (void)OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
HDF_LOGE("%s: done!", __func__);
    return ret;
}

int32_t WatchdogCntlrSetTimeout(struct WatchdogCntlr *cntlr, uint32_t seconds)
{
    int32_t ret;
    uint32_t flags;
HDF_LOGE("%s: enter!", __func__);
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->setTimeout == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalSpinLockIrqSave(&cntlr->lock, &flags) != HDF_SUCCESS) {
        return HDF_ERR_DEVICE_BUSY;
    }
    ret = cntlr->ops->setTimeout(cntlr, seconds);
    (void)OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
HDF_LOGE("%s: done!", __func__);
    return ret;
}

int32_t WatchdogCntlrGetTimeout(struct WatchdogCntlr *cntlr, uint32_t *seconds)
{
    int32_t ret;
    uint32_t flags;
HDF_LOGE("%s: enter!", __func__);
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->getTimeout == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    if (seconds == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (OsalSpinLockIrqSave(&cntlr->lock, &flags) != HDF_SUCCESS) {
        return HDF_ERR_DEVICE_BUSY;
    }
    ret = cntlr->ops->getTimeout(cntlr, seconds);
    (void)OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
HDF_LOGE("%s: done!", __func__);
    return ret;
}

int32_t WatchdogCntlrFeed(struct WatchdogCntlr *cntlr)
{
    int32_t ret;
    uint32_t flags;
HDF_LOGE("%s: enter!", __func__);
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cntlr->ops == NULL || cntlr->ops->feed == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalSpinLockIrqSave(&cntlr->lock, &flags) != HDF_SUCCESS) {
        return HDF_ERR_DEVICE_BUSY;
    }
    ret = cntlr->ops->feed(cntlr);
    (void)OsalSpinUnlockIrqRestore(&cntlr->lock, &flags);
HDF_LOGE("%s: done!", __func__);
    return ret;
}

static int32_t WatchdogUserGetStatus(struct WatchdogCntlr *cntlr, struct HdfSBuf *reply)
{
    int32_t ret;
    int32_t status;
HDF_LOGE("%s: enter!", __func__);
    ret = WatchdogCntlrGetStatus(cntlr, &status);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WatchdogCntlrGetStatus failed, ret: %d", __func__, ret);
        return ret;
    }

    if (!HdfSbufWriteBuffer(reply, &status, sizeof(status))) {
        HDF_LOGE("%s: sbuf write buffer failed", __func__);
        return HDF_ERR_IO;
    }
HDF_LOGE("%s: done!", __func__);
    return HDF_SUCCESS;
}

static int32_t WatchdogUserSetTimeout(struct WatchdogCntlr *cntlr, struct HdfSBuf *data)
{
    size_t size;
    uint32_t *seconds = NULL;
HDF_LOGE("%s: enter!", __func__);
    if (!HdfSbufReadBuffer(data, (const void **)&seconds, &size)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }
HDF_LOGE("%s: done!", __func__);
    return WatchdogCntlrSetTimeout(cntlr, *seconds);
}

static int32_t WatchdogUserGetTimeout(struct WatchdogCntlr *cntlr, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t seconds;
HDF_LOGE("%s: enter!", __func__);
    ret = WatchdogCntlrGetTimeout(cntlr, &seconds);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WatchdogCntlrGetTimeout failed, ret: %d", __func__, ret);
        return ret;
    }
    
    if (!HdfSbufWriteBuffer(reply, &seconds, sizeof(seconds))) {
        HDF_LOGE("%s: sbuf write buffer failed", __func__);
        return HDF_ERR_IO;
    }
HDF_LOGE("%s: done!", __func__);
    return HDF_SUCCESS;
}

static int32_t WatchdogIoDispatch(struct HdfDeviceIoClient *client, int cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct WatchdogCntlr *cntlr = NULL;
HDF_LOGE("%s: enter!", __func__);
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

    cntlr = (struct WatchdogCntlr *)client->device->service;
    switch (cmd) {
        case WATCHDOG_IO_GET_PRIV:
            return WatchdogGetPrivData(cntlr);
        case WATCHDOG_IO_RELEASE_PRIV:
            return WatchdogReleasePriv(cntlr);
        case WATCHDOG_IO_GET_STATUS:
            return WatchdogUserGetStatus(cntlr, reply);
        case WATCHDOG_IO_START:
            return WatchdogCntlrStart(cntlr);
        case WATCHDOG_IO_STOP:
            return WatchdogCntlrStop(cntlr);
        case WATCHDOG_IO_SET_TIMEOUT:
            return WatchdogUserSetTimeout(cntlr, data);
        case WATCHDOG_IO_GET_TIMEOUT:
            return WatchdogUserGetTimeout(cntlr, reply);
        case WATCHDOG_IO_FEED:
            return WatchdogCntlrFeed(cntlr);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
HDF_LOGE("%s: done!", __func__);
}

int32_t WatchdogCntlrAdd(struct WatchdogCntlr *cntlr)
{
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->device == NULL) {
        HDF_LOGE("WatchdogCntlrAdd: no device associated!");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->ops == NULL) {
        HDF_LOGE("WatchdogCntlrAdd: no ops supplied!");
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = OsalSpinInit(&cntlr->lock);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("WatchdogCntlrAdd: spinlock init fail!");
        return ret;
    }

    cntlr->device->service = &cntlr->service;
    cntlr->device->service->Dispatch = WatchdogIoDispatch;
    return HDF_SUCCESS;
}

void WatchdogCntlrRemove(struct WatchdogCntlr *cntlr)
{
    if (cntlr == NULL) {
        return;
    }

    if (cntlr->device == NULL) {
        HDF_LOGE("WatchdogCntlrRemove: no device associated!");
        return;
    }

    cntlr->device->service = NULL;
    (void)OsalSpinDestroy(&cntlr->lock);
}
