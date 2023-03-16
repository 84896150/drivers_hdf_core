/*
 * Copyright (c) 2022 Jiangsu Hoperun Software Co., Ltd.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include <string.h>
#include "hdf_log.h"
#include "spi_core.h"
#include "spi_if.h"
#include "device_resource_if.h"
#include "osal_mutex.h"
#include "osal_sem.h"

#include "bl602_spi.h"

#define SPI_DMA_MAX 65535
#define MAX_SPI_NUMBER 1

struct SpiResource {
    uint32_t num;
     uint32_t speed;
    enum SpiTransferMode transmode;
    uint32_t mode; // TLS_SPI_MODE_x
    uint32_t dataSize;
    uint8_t    port; 
};

struct SpiDevice {
    uint32_t spiId;
    struct SpiResource resource;
    struct OsalMutex mutex;
};

static void SpiIomuxInit(struct SpiDevice *spiDevice)
{
    struct SpiResource *resource = NULL;
    uint32_t spiPort;
    HDF_LOGI("%s: Enter\r\n", __func__);

    if (spiDevice == NULL) {
        HDF_LOGE("%s: invalid parameter\r\n", __func__);
        return;
    }

    resource = &spiDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is null\r\n");
		/* return HDF_ERR_INVALID_OBJECT; */
    }

    /* spiPort = spiDevice->spiId; */
 
}

/**
 * Spi send
 *
 * @param[in]  spiId   the spi bus id
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 * @param[in]  timeOut  timeOut in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t HalSpiSend(struct SpiDevice *spiDevice, const uint8_t *data, uint16_t size)
{
    int32_t ret = 0;
    SPI_ID_Type spi_id;
    uint32_t len = size;
    struct SpiResource *resource = NULL;
    int32_t status = HDF_FAILURE;
    if (spiDevice == NULL || data == NULL || size == 0) {
        HDF_LOGE("spi input para err\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &spiDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is null\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }
    status = OsalMutexLock(&spiDevice->mutex);
    if (HDF_SUCCESS != status) {
        HDF_LOGE("%s spi_mutex wait error = 0x%X!\r\n", __func__, status);
        return HDF_ERR_TIMEOUT;
    }

    if(spiDevice->spiId = 0) {
        spi_id = SPI_ID_0;
    } else {
        spi_id = SPI_ID_MAX;
    }

    ret = SPI_Send_8bits(spi_id, data, size, SPI_TIMEOUT_ENABLE);
    if(ret == SUCCESS) {
        ret = 0;
    } else if(ret == ERROR) {
        HDF_LOGE("%s spi dma send error = 0x%X!\r\n", __func__, ret);
        ret = 1;
    } else {
        ret = 2;
    }

    OsalMutexUnlock(&spiDevice->mutex);
    return ret;
}

/**
 * SpiRecv
 *
 * @param[in]   spiId   the spi bus id
 * @param[out]  data     spi recv data
 * @param[in]   size     spi recv data size
 * @param[in]   timeOut  timeOut in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t HalSpiRecv(struct SpiDevice *spiDevice, uint8_t *data, uint16_t size)
{
    int32_t ret = 0;
    uint32_t len = size;
    uint32_t remainder = 0;
    int32_t status = HDF_FAILURE;
    uint8_t *cmd = NULL;
    SPI_ID_Type spi_id;
    struct SpiResource *resource = NULL;
    if (spiDevice == NULL || data == NULL || size == 0) {
        HDF_LOGE("spi input para err\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &spiDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is null\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }
    cmd = (uint8_t *)OsalMemAlloc(len);
    if (cmd == NULL) {
        HDF_LOGE("%s OsalMemAlloc size %ld error\r\n", __FUNCTION__, len);
        return HDF_ERR_MALLOC_FAIL;
    }

    memset_s(cmd, len, 0, len);

    status = OsalMutexLock(&spiDevice->mutex);
    if (HDF_SUCCESS != status) {
        HDF_LOGE("%s spi_mutex wait error = 0x%X!\r\n", __func__, status);
        OsalMemFree(cmd);
        return HDF_ERR_TIMEOUT;
    }
    remainder = len <= SPI_DMA_MAX ? len : SPI_DMA_MAX;

    if(spiDevice->spiId = 0) {
        spi_id = SPI_ID_0;
    } else {
        spi_id = SPI_ID_MAX;
    }

    ret = SPI_Recv_8bits(spi_id, data, remainder, SPI_TIMEOUT_ENABLE);
    if(ret == SUCCESS) {
        ret = 0;
    } else if(ret == ERROR) {
        HDF_LOGE("%s spi dma send error = 0x%X!\r\n", __func__, ret);
        ret = 1;
    } else {
        ret = 2;
    }

    len -= remainder;
    data += remainder;

    OsalMutexUnlock(&spiDevice->mutex);
    OsalMemFree(cmd);
    return ret;
}

int32_t HalSpiSendRecv(struct SpiDevice *spiDevice, uint8_t *txData, uint16_t Size, uint8_t *rxData)
{
    int32_t ret;
    int32_t status;
    SPI_ID_Type spi_id;
    struct SpiResource *resource = NULL;
    if (spiDevice == NULL || txData == NULL || rxData == NULL ) {
        HDF_LOGE("spi input para err\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &spiDevice->resource;
    status = OsalMutexLock(&spiDevice->mutex);
    if (HDF_SUCCESS != status) {
        HDF_LOGE("%s spi_mutex wait error = 0x%X!\r\n", __func__, status);
        return HDF_ERR_TIMEOUT;
    }

    if(spiDevice->spiId = 0) {
        spi_id = SPI_ID_0;
    } else {
        spi_id = SPI_ID_MAX;
    }

    ret = SPI_SendRecv_8bits(spi_id, txData, rxData, Size, SPI_TIMEOUT_ENABLE);
    if(ret == SUCCESS) {
        ret = 0;
    } else if(ret == ERROR) {
        HDF_LOGE("%s spi dma send error = 0x%X!\r\n", __func__, ret);
        ret = 1;
    } else {
        ret = 2;
    }

    OsalMutexUnlock(&spiDevice->mutex);
    return ret;
}

/* SPI Method */
static int32_t SpiDevGetCfg(struct SpiCntlr *spiCntlr, struct SpiCfg *spiCfg);
static int32_t SpiDevSetCfg(struct SpiCntlr *spiCntlr, struct SpiCfg *spiCfg);
static int32_t SpiDevTransfer(struct SpiCntlr *spiCntlr, struct SpiMsg *spiMsg, uint32_t count);
static int32_t SpiDevOpen(struct SpiCntlr *spiCntlr);
static int32_t SpiDevClose(struct SpiCntlr *spiCntlr);

struct SpiCntlrMethod g_SpiCntlrMethod = {
    .Open = SpiDevOpen,
    .Close = SpiDevClose,
    .GetCfg = SpiDevGetCfg,
    .SetCfg = SpiDevSetCfg,
    .Transfer = SpiDevTransfer,
};

static int32_t SpiDevOpen(struct SpiCntlr *spiCntlr)
{
    HDF_LOGI("Enter %s\r\n", __func__);
    int ret;
    struct SpiDevice *spiDevice = NULL;
    struct SpiResource *resource = NULL;
    if (spiCntlr == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    if (spiDevice == NULL) {
        HDF_LOGE("spi device is NULL\r\n");
        return HDF_DEV_ERR_NO_DEVICE;
    }
    resource = &spiDevice->resource;

    return HDF_SUCCESS;
}

static int32_t SpiDevClose(struct SpiCntlr *spiCntlr)
{
    int ret;
    struct SpiDevice *spiDevice = NULL;
    if (spiCntlr == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    if (spiDevice == NULL) {
        HDF_LOGE("spi device is NULL\r\n");
        return HDF_DEV_ERR_NO_DEVICE;
    }

    return HDF_SUCCESS;
}

static int32_t SpiDevGetCfg(struct SpiCntlr *spiCntlr, struct SpiCfg *spiCfg)
{
    struct SpiDevice *spiDevice = NULL;
    if (spiCntlr == NULL || spiCfg == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    if (spiDevice == NULL) {
        return HDF_DEV_ERR_NO_DEVICE;
    }
     spiCfg->maxSpeedHz = spiDevice->resource.speed;
    spiCfg->mode = spiDevice->resource.mode;
    spiCfg->transferMode = spiDevice->resource.transmode;
    spiCfg->bitsPerWord = spiDevice->resource.dataSize;

    return HDF_SUCCESS;
}
static int32_t SpiDevSetCfg(struct SpiCntlr *spiCntlr, struct SpiCfg *spiCfg)
{
    struct SpiDevice *spiDevice = NULL;
    SPI_ID_Type spi_id;
    SPI_CFG_Type spicfg;

    if (spiCntlr == NULL || spiCfg == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    if (spiDevice == NULL) {
        return HDF_DEV_ERR_NO_DEVICE;
    }

    if(spiDevice->spiId = 0) {
        spi_id = SPI_ID_0;
    } else {
        spi_id = SPI_ID_MAX;
    }

    spicfg.deglitchEnable = DISABLE;
    spicfg.continuousEnable = ENABLE;
    spicfg.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST,
    spicfg.bitSequence = SPI_BIT_INVERSE_MSB_FIRST,
    spicfg.frameSize = SPI_FRAME_SIZE_8;

    SPI_Init(spi_id,&spicfg);

    spiDevice->resource.speed = spiCfg->maxSpeedHz;
    spiDevice->resource.mode = spiCfg->mode;
    spiDevice->resource.transmode = spiCfg->transferMode;
    spiDevice->resource.dataSize  = spiCfg->bitsPerWord;

    return HDF_SUCCESS;
}

static int32_t SpiDevTransfer(struct SpiCntlr *spiCntlr, struct SpiMsg *spiMsg, uint32_t count)
{
    uint32_t spiId;
    struct SpiDevice *spiDevice = NULL;
    struct SpiMsg *msg = NULL;
    if (spiCntlr == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    spiId = spiDevice->spiId;
    HDF_LOGI("%s: %u Enter\r\n", __func__, spiId);
    for (size_t i = 0; i < count; i++) {
        msg = &spiMsg[i];
        /* pull down cs at first */

        if ((msg->wbuf != NULL) && (msg->rbuf == NULL)) {
            HalSpiSend(spiDevice, msg->wbuf, msg->len);
        }
        if ((msg->rbuf != NULL) && (msg->wbuf == NULL)) {
            HalSpiRecv(spiDevice, msg->rbuf, msg->len);
        }
        if ((msg->wbuf != NULL) && (msg->rbuf != NULL)) {
            HalSpiSendRecv(spiDevice, msg->wbuf, msg->len, msg->rbuf);
        }
    }
    return HDF_SUCCESS;
}

/* HdfDriverEntry method definitions */
static int32_t SpiDriverBind(struct HdfDeviceObject *device);
static int32_t SpiDriverInit(struct HdfDeviceObject *device);
static void SpiDriverRelease(struct HdfDeviceObject *device);

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_SpiDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "BL_SPI_MODULE_HDF",
    .Bind = SpiDriverBind,
    .Init = SpiDriverInit,
    .Release = SpiDriverRelease,
};

HDF_INIT(g_SpiDriverEntry);

static int32_t InitSpiDevice(struct SpiDevice *spiDevice)
{
    uint32_t spiPort;
    struct SpiResource *resource = NULL;
    if (spiDevice == NULL) {
        HDF_LOGE("%s: invalid parameter\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &spiDevice->resource;
    spiPort = spiDevice->spiId;

    SpiIomuxInit(spiDevice);
	
    if (spiDevice->mutex.realMutex == NULL) {
        if (OsalMutexInit(&spiDevice->mutex) != HDF_SUCCESS) {
            HDF_LOGE("spi Mutex create failed!\r\n");
            return HDF_FAILURE;
        }
    }
    /* tls_spi_init(); */
    return HDF_SUCCESS;
}

/* get spi config from hcs file */
static int32_t GetSpiDeviceResource(struct SpiDevice *spiDevice, const struct DeviceResourceNode *resourceNode)
{
    uint32_t relPin;
    struct SpiResource *resource = NULL;
    struct DeviceResourceIface *dri = NULL;
    if (spiDevice == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: PARAM is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &spiDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("%s: resource is NULL\r\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);   // open HDF
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    if (dri->GetUint32(resourceNode, "num", &resource->num, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read num fail\r\n");
        return HDF_FAILURE;
    }
    spiDevice->spiId = resource->num;

    return HDF_SUCCESS;
}

int32_t AttachSpiDevice(struct SpiCntlr *spiCntlr, struct HdfDeviceObject *device)
{
    int32_t ret;
    struct SpiDevice *spiDevice = NULL;

    if (spiCntlr == NULL || device == NULL || device->property == NULL) {
        HDF_LOGE("%s: property is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    spiDevice = (struct SpiDevice *)OsalMemAlloc(sizeof(struct SpiDevice));
    if (spiDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc spiDevice error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    spiDevice->mutex.realMutex = NULL;

    ret = GetSpiDeviceResource(spiDevice, device->property);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(spiDevice);
        return HDF_FAILURE;
    }

    spiCntlr->priv = spiDevice;
    spiCntlr->busNum = spiDevice->spiId;
    return InitSpiDevice(spiDevice);
}

static int32_t SpiDriverBind(struct HdfDeviceObject *device)
{
    struct SpiCntlr *spiCntlr = NULL;
    if (device == NULL) {
        HDF_LOGE("Sample device object is null!\r\n");
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("Enter %s:\r\n", __func__);
    spiCntlr = (struct SpiCntlr *)OsalMemAlloc(sizeof(struct SpiCntlr));
    if (spiCntlr == NULL) {
        HDF_LOGE("%s: host is NULL\r\n", __func__);
        return HDF_FAILURE;
    }
    device->service = &spiCntlr->service;
    spiCntlr->device = device;
    spiCntlr->priv = NULL;
    return HDF_SUCCESS;
}

static int32_t SpiDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct SpiCntlr *spiCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("Enter %s:", __func__);
    spiCntlr = SpiCntlrFromDevice(device);
    if (spiCntlr == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL", __func__);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    ret = AttachSpiDevice(spiCntlr, device); // SpiCntlr add SpiDevice to priv
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: attach error\r\n", __func__);
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }

    if (OsalMutexInit(&spiCntlr->lock) != HDF_SUCCESS) {
        HDF_LOGE("spi ctrl lock create failed!\r\n");
        return HDF_FAILURE;
    }

    spiCntlr->method = &g_SpiCntlrMethod; // register callback

    return ret;
}

static void SpiDriverRelease(struct HdfDeviceObject *device)
{
    struct SpiCntlr *spiCntlr = NULL;
    struct SpiDevice *spiDevice = NULL;

    HDF_LOGI("Enter %s\r\n", __func__);

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return;
    }

    spiCntlr = SpiCntlrFromDevice(device);
    if (spiCntlr == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return;
    }

    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    if (spiDevice != NULL) {
        OsalMemFree(spiDevice);
    }
    return;
}
