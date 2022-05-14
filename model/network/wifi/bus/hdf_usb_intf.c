/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_usb_intf.h"
#include "osal_mem.h"
#include "wifi_inc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static int32_t HdfGetUsbInfo(struct BusDev *dev, struct BusConfig *busCfg)
{
    HDF_LOGI("%s:get usb info", __func__);
    return HDF_SUCCESS;
}

static void HdfUsbReleaseDev(struct BusDev *dev)
{
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return;
    }
    if (dev->priData.data != NULL) {
        dev->priData.release(dev->priData.data);
        dev->priData.data = NULL;
    }
    OsalMemFree(dev);
    dev = NULL;
}

static int32_t HdfUsbEnableFunc(struct BusDev *dev)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfUsbDisableFunc(struct BusDev *dev)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfUsbCliamIrq(struct BusDev *dev, IrqHandler *handler, void *data)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static void HdfUsbClaimHost(struct BusDev *dev)
{
    HDF_LOGI("%s:", __func__);
}

static void HdfUsbReleaseHost(struct BusDev *dev)
{
    HDF_LOGI("%s:", __func__);
}


static int32_t HdfUsbReleaseIrq(struct BusDev *dev)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfUsbReset(struct BusDev *dev)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfUsbReadN(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfUsbReadFunc0(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfUsbReadSpcReg(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf, uint32_t sg_len)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfUsbWriteN(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfUsbWriteFunc0(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfUsbWriteSpcReg(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf, uint32_t sg_len)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfUsbSetBlk(struct BusDev *dev, uint32_t blkSize)
{
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static struct DevHandle *HdfGetDevHandle(struct BusDev *dev, const struct HdfConfigWlanBus *busCfg)
{
    int32_t cnt;
    struct HdfConfigWlanChipList *tmpChipList = NULL;

    struct HdfConfigWlanRoot *rootConfig = HdfWlanGetModuleConfigRoot();
    if (rootConfig == NULL) {
        HDF_LOGE("%s: NULL ptr!", __func__);
        return NULL;
    }
    tmpChipList = &rootConfig->wlanConfig.chipList;

    cnt = 0;
    if (cnt == tmpChipList->chipInstSize || cnt == WLAN_MAX_CHIP_NUM) {
        HDF_LOGE("%s: NO usb card detected!", __func__);
        return NULL;
    }
    dev->devBase = NULL;
    dev->priData.driverName = tmpChipList->chipInst[cnt].driverName;
    return NULL;
}

static int32_t HdfUsbInit(struct BusDev *dev, const struct HdfConfigWlanBus *busCfg)
{
    if (dev == NULL || busCfg == NULL) {
        HDF_LOGE("%s: input parameter error!", __func__);
        return HDF_FAILURE;
    }
    (void)HdfGetDevHandle(dev, busCfg);
    HDF_LOGI("%s:", __func__);
    return HDF_SUCCESS;
}

static void HdfSetBusOps(struct BusDev *dev)
{
    dev->ops.getBusInfo = HdfGetUsbInfo;
    dev->ops.deInit = HdfUsbReleaseDev;
    dev->ops.init = HdfUsbInit;

    dev->ops.readData = HdfUsbReadN;
    dev->ops.writeData = HdfUsbWriteN;
    dev->ops.bulkRead = HdfUsbReadSpcReg;
    dev->ops.bulkWrite = HdfUsbWriteSpcReg;
    dev->ops.readFunc0 = HdfUsbReadFunc0;
    dev->ops.writeFunc0 = HdfUsbWriteFunc0;

    dev->ops.claimIrq = HdfUsbCliamIrq;
    dev->ops.releaseIrq = HdfUsbReleaseIrq;
    dev->ops.disableBus = HdfUsbDisableFunc;
    dev->ops.reset = HdfUsbReset;

    dev->ops.claimHost = HdfUsbClaimHost;
    dev->ops.releaseHost = HdfUsbReleaseHost;
}
int32_t HdfWlanBusAbsInit(struct BusDev *dev, const struct HdfConfigWlanBus *busConfig)
{
    if (dev == NULL) {
        HDF_LOGE("%s:set usb device ops failed!", __func__);
        return HDF_FAILURE;
    }
    HdfSetBusOps(dev);
    return HdfUsbInit(dev, busConfig);
}

int32_t HdfWlanConfigBusAbs(uint8_t busId)
{
    return HDF_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
