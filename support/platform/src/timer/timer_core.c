/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "timer_core.h"
#include "timer_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"

#define HDF_LOG_TAG timer_core

#ifdef HDF_LOGD
#undef HDF_LOGD
#define HDF_LOGD(fmt, args...) dprintf("[HDF]-[TIMER]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##args)
#endif

#ifdef HDF_LOGI
#undef HDF_LOGI
#define HDF_LOGI(fmt, args...) dprintf("[HDF]-[TIMER]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##args)
#endif

#ifdef HDF_LOGE
#undef HDF_LOGE
#define HDF_LOGE(fmt, args...) dprintf("[HDF]-[TIMER]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##args)
#endif

struct TimerManager {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct DListHead timerListHead;
    struct OsalMutex lock;
};

static struct TimerManager *g_timerManager = NULL;

static void TimerCntrlTest(void);
int32_t TimerCntrlAdd(struct TimerCntrl *cntrl)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(cntrl->ops, HDF_ERR_INVALID_PARAM);
    struct TimerCntrl *pos = NULL;
    struct TimerCntrl *tmp = NULL;
    struct TimerManager *manager = g_timerManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->timerListHead, struct TimerCntrl, node) {
        if (cntrl->info.id == pos->info.id) {
            HDF_LOGE("%s: timer[%d] existed", __func__, cntrl->info.id);
            return HDF_FAILURE;
        }
    }

    // init info
    if (OsalMutexInit(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexInit %d failed", __func__, cntrl->info.id);
        return HDF_FAILURE;
    }

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %d failed", __func__, cntrl->info.id);
        return HDF_ERR_DEVICE_BUSY;
    }
    DListInsertTail(&cntrl->node, &manager->timerListHead);
    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGI("%s: add timer id[%d] success", __func__, cntrl->info.id);

    if (cntrl->info.id == 7) {
        TimerCntrlTest();
    }
    return HDF_SUCCESS;
}

int32_t TimerCntrlRemove(const uint32_t id)
{
    struct TimerCntrl *pos = NULL;
    struct TimerCntrl *tmp = NULL;
    struct TimerManager *manager = g_timerManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_FAILURE);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock failed", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->timerListHead, struct TimerCntrl, node) {
        if (id == pos->info.id) {
            if ((pos->ops->remove != NULL) && (pos->ops->remove(pos) != HDF_SUCCESS)) {
                HDF_LOGE("%s: remove %d failed", __func__, pos->info.id);
            }
            DListRemove(&pos->node);
            (void)OsalMutexDestroy(&pos->lock);
            OsalMemFree(pos);
            break;
        }
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGI("%s: remove timer %d success", __func__, id);
    return HDF_SUCCESS;
}

struct TimerCntrl *TimerCntrlOpen(const uint32_t id)
{
    struct TimerCntrl *pos = NULL;
    struct TimerManager *manager = g_timerManager;
    CHECK_NULL_PTR_RETURN_VALUE(manager, NULL);

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock failed", __func__);
        return NULL;
    }

    DLIST_FOR_EACH_ENTRY(pos, &manager->timerListHead, struct TimerCntrl, node) {
        if (id == pos->info.id) {
            HDF_LOGD("%s: open %d success", __func__, id);
            (void)OsalMutexUnlock(&manager->lock);
            return pos;
        }
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGE("%s: open %d failed", __func__, id);
    return NULL;
}

int32_t TimerCntrlClose(struct TimerCntrl *cntrl)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);
    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %d failed", __func__, cntrl->info.id);
        return HDF_ERR_DEVICE_BUSY;
    }
    if ((cntrl->ops->close != NULL) && (cntrl->ops->close(cntrl) != HDF_SUCCESS)) {
        HDF_LOGE("%s: close %d failed", __func__, cntrl->info.id);
        (void)OsalMutexUnlock(&cntrl->lock);
        return HDF_FAILURE;
    }
    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

int32_t TimerCntrlSet(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);
    CHECK_NULL_PTR_RETURN_VALUE(cb, HDF_ERR_INVALID_OBJECT);
    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %d failed", __func__, cntrl->info.id);
        return HDF_ERR_DEVICE_BUSY;
    }
    if ((cntrl->ops->set != NULL) && (cntrl->ops->set(cntrl, useconds, cb) != HDF_SUCCESS)) {
        HDF_LOGE("%s: set %d failed", __func__, cntrl->info.id);
        (void)OsalMutexUnlock(&cntrl->lock);
        return HDF_FAILURE;
    }

    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

int32_t TimerCntrlSetOnce(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);
    CHECK_NULL_PTR_RETURN_VALUE(cb, HDF_ERR_INVALID_OBJECT);

    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %d failed", __func__, cntrl->info.id);
        return HDF_ERR_DEVICE_BUSY;
    }
    if ((cntrl->ops->setOnce != NULL) && (cntrl->ops->setOnce(cntrl, useconds, cb) != HDF_SUCCESS)) {
        HDF_LOGE("%s: setOnce %d failed", __func__, cntrl->info.id);
        (void)OsalMutexUnlock(&cntrl->lock);
        return HDF_FAILURE;
    }

    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

int32_t TimerCntrlGet(struct TimerCntrl *cntrl, uint32_t *useconds, bool *isPeriod)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);
    CHECK_NULL_PTR_RETURN_VALUE(useconds, HDF_ERR_INVALID_OBJECT);
    CHECK_NULL_PTR_RETURN_VALUE(isPeriod, HDF_ERR_INVALID_OBJECT);

    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %d failed", __func__, cntrl->info.id);
        return HDF_ERR_DEVICE_BUSY;
    }
    *useconds = cntrl->info.useconds;
    *isPeriod = cntrl->info.isPeriod;
    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

int32_t TimerCntrlStart(struct TimerCntrl *cntrl)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);

    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %d failed", __func__, cntrl->info.id);
        return HDF_ERR_DEVICE_BUSY;
    }
    if ((cntrl->ops->start != NULL) && (cntrl->ops->start(cntrl) != HDF_SUCCESS)) {
        HDF_LOGE("%s: start %d failed", __func__, cntrl->info.id);
        (void)OsalMutexUnlock(&cntrl->lock);
        return HDF_FAILURE;
    }
    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

int32_t TimerCntrlStop(struct TimerCntrl *cntrl)
{
    CHECK_NULL_PTR_RETURN_VALUE(cntrl, HDF_ERR_INVALID_OBJECT);

    if (OsalMutexLock(&cntrl->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: OsalMutexLock %d failed", __func__, cntrl->info.id);
        return HDF_ERR_DEVICE_BUSY;
    }
    if ((cntrl->ops->stop != NULL) && (cntrl->ops->stop(cntrl) != HDF_SUCCESS)) {
        HDF_LOGE("%s: stop %d failed", __func__, cntrl->info.id);
        (void)OsalMutexUnlock(&cntrl->lock);
        return HDF_FAILURE;
    }

    (void)OsalMutexUnlock(&cntrl->lock);
    return HDF_SUCCESS;
}

int32_t TimerListRemoveAll(void)
{
    struct TimerCntrl *pos = NULL;
    struct TimerCntrl *tmp = NULL;
    struct TimerManager *manager = g_timerManager;

    if (OsalMutexLock(&manager->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: lock regulator manager fail", __func__);
        return HDF_ERR_DEVICE_BUSY;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->timerListHead, struct TimerCntrl, node) {
        if ((pos->ops->remove != NULL) && (pos->ops->remove(pos) != HDF_SUCCESS)) {
            HDF_LOGE("%s: remove %d failed", __func__, pos->info.id);
        }
        DListRemove(&pos->node);
        (void)OsalMutexDestroy(&pos->lock);
        OsalMemFree(pos);
    }

    (void)OsalMutexUnlock(&manager->lock);
    HDF_LOGI("%s: remove all regulator success", __func__);
    return HDF_SUCCESS;
}

static int32_t TimerPeriodHandleCbTest(void)
{
    static int count = 0;
    HDF_LOGI("------------->%s:---->[%d] success", __func__, count);
    count++;
    return HDF_SUCCESS;
}

static int32_t TimerOnceHandleCbTest(void)
{
    HDF_LOGI("------------->%s: success", __func__);
    return HDF_SUCCESS;
}

static void TimerCntrlTest(void)
{
    return;
    struct TimerCntrl * cntrl = TimerCntrlOpen(0);
    CHECK_NULL_PTR_RETURN(cntrl);

    TimerCntrlSet(cntrl, 10000, TimerPeriodHandleCbTest);
    TimerCntrlStart(cntrl);

    struct TimerCntrl * cntrl2 = TimerCntrlOpen(2);
    CHECK_NULL_PTR_RETURN(cntrl2);

    TimerCntrlSetOnce(cntrl2, 20000, TimerOnceHandleCbTest);
    TimerCntrlStart(cntrl2);

    return;
}

static int32_t TimerManagerBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct TimerManager *manager = NULL;

    HDF_LOGI("%s: Enter!", __func__);
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_OBJECT);

    manager = (struct TimerManager *)OsalMemCalloc(sizeof(*manager));
    if (manager == NULL) {
        HDF_LOGE("%s: malloc manager fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = OsalMutexInit(&manager->lock);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: mutex init fail:%d", __func__, ret);
        OsalMemFree(manager);
        manager = NULL;
        return HDF_FAILURE;
    }

    manager->device = device;
    device->service = &manager->service;
    DListHeadInit(&manager->timerListHead);
    g_timerManager = manager;

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t TimerManagerInit(struct HdfDeviceObject *device)
{
    (void)device;

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

static void TimerManagerRelease(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: Enter!", __func__);
    CHECK_NULL_PTR_RETURN(device);

    if (TimerListRemoveAll() != HDF_SUCCESS) {
        HDF_LOGE("%s: failed", __func__);
    }

    struct TimerManager *manager = (struct TimerManager *)device->service;
    CHECK_NULL_PTR_RETURN(manager);
    OsalMutexDestroy(&manager->lock);
    OsalMemFree(manager);
    g_timerManager = NULL;
}

struct HdfDriverEntry g_timerManagerEntry = {
    .moduleVersion = 1,
    .Bind = TimerManagerBind,
    .Init = TimerManagerInit,
    .Release = TimerManagerRelease,
    .moduleName = "HDF_PLATFORM_TIMER_MANAGER",
};

HDF_INIT(g_timerManagerEntry);
