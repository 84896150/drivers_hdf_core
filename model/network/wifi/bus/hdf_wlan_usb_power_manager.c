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

/* powerCount=1 single power source */
struct DevicePowerData {
    struct HdfConfigWlanPower power0;
    struct HdfConfigWlanPower power1;
};

struct PowerManagerImpl {
    struct PowerManager base;
    struct DevicePowerData powerDatas;
};

static int32_t HdfWlanUsbChipPowerOn(struct PowerManager* powerMgr)
{
    return HDF_SUCCESS;
}

static int32_t HdfWlanUsbChipPowerOff(struct PowerManager* powerMgr)
{
    return HDF_SUCCESS;
}

int32_t HdfWlanUsbPowerMgrRelease(struct PowerManager* powerMgr)
{
    if (powerMgr == NULL) {
        HDF_LOGW("%s: powerMgr already deinit or input para error ", __func__);
        return HDF_SUCCESS;
    }
    OsalMemFree(powerMgr);
    return HDF_SUCCESS;
}
/**
 * @brief create powers manager according to the powers config
 */
struct PowerManager* HdfWlanCreatePowerManager(const struct HdfConfWlanPowers *configPowers)
{
    struct PowerManagerImpl *powerMgrimpl = NULL;
    if (configPowers == NULL) {
        HDF_LOGE("%s: configPowers is NULL", __func__);
        return NULL;
    }
    powerMgrimpl = (struct PowerManagerImpl *)OsalMemCalloc(sizeof(struct PowerManagerImpl));
    if (powerMgrimpl == NULL) {
        HDF_LOGE("%s: OsalMemCalloc fail! ", __func__);
        return NULL;
    }
    powerMgrimpl->base.On = HdfWlanUsbChipPowerOn;
    powerMgrimpl->base.Off = HdfWlanUsbChipPowerOff;
    powerMgrimpl->base.Release = HdfWlanUsbPowerMgrRelease;
    powerMgrimpl->powerDatas.power0 = configPowers->power0;
    powerMgrimpl->powerDatas.power1 = configPowers->power1;
    return (struct PowerManager *)powerMgrimpl;
}
