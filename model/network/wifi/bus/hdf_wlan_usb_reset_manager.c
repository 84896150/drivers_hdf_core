/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "gpio_if.h"
#include "osal_mem.h"
#include "hdf_log.h"
#include "wifi_module.h"
#include "hdf_wlan_chipdriver_manager.h"
#include "securec.h"

#define HDF_LOG_TAG HDF_WIFI_CORE

/**
 * @brief constant reset manage type
 */
enum ResetType {
    RESET_ALWAYS_ON = 0,
    RESET_MANAGED_BY_GPIO = 1
};

struct HdfWlanResetData {
    struct HdfConfWlanRest resetCfg;
    uint8_t bootUpHoldTime;
};

struct ResetManagerImpl {
    struct ResetManager base;
    struct HdfWlanResetData resetData;
};

/**
 * @brief HdfUsbChipReset
 */
int32_t HdfUsbChipReset(struct ResetManager *resetManager)
{
    return HDF_SUCCESS;
}
/* to release reset manager resource */
int32_t HdfWlanUsbResetMgrRelease(struct ResetManager* resetMgr)
{
    if (resetMgr == NULL) {
        HDF_LOGE("%s: resetMgr already deinit or input para error ", __func__);
        return HDF_FAILURE;
    }
    OsalMemFree(resetMgr);
    return HDF_SUCCESS;
}
/* create power reset manager */
struct ResetManager* HdfWlanCreateResetManager(const struct HdfConfWlanRest *configReset, uint8_t bootUpTime)
{
    struct ResetManagerImpl *resetMgrImpl = NULL;
    if (configReset == NULL) {
        HDF_LOGE("%s: configReset is NULL", __func__);
        return NULL;
    }
    resetMgrImpl = (struct ResetManagerImpl *)OsalMemCalloc(sizeof(struct ResetManagerImpl));
    if (resetMgrImpl == NULL) {
        HDF_LOGE("%s: OsalMemCalloc fail! ", __func__);
        return NULL;
    }
    if (memcpy_s(&resetMgrImpl->resetData.resetCfg, sizeof(struct HdfConfWlanRest),
        configReset, sizeof(struct HdfConfWlanRest)) != EOK) {
        HDF_LOGE("%s: memcpy_s is NULL", __func__);
        OsalMemFree(resetMgrImpl);
        return NULL;
    }
    resetMgrImpl->resetData.bootUpHoldTime = bootUpTime;
    resetMgrImpl->base.Reset = HdfUsbChipReset;
    resetMgrImpl->base.Release = HdfWlanUsbResetMgrRelease;

    return (struct ResetManager*)resetMgrImpl;
}