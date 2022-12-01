/*
 * Copyright (c) 2022 Jiangsu Hoperun Software Co., Ltd.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include <securec.h>
#include "i2c_core.h"
#include "i2c_if.h"
#include "device_resource_if.h"
#include "osal_mutex.h"
#include "bl_i2c.h"

#define HAL_I2C_ID_NUM 0

struct I2cResource {
    uint32_t port;
    uint32_t freq;
    uint32_t subaddr_len;
    uint16_t subaddr;
};

struct I2cDevice {
    struct OsalMutex mutex;
    struct I2cResource resource;
    i2c_msg_t pstmsg;
    uint32_t port;
    uint32_t freq;
};

/* I2cHostMethod method definitions */
static int32_t i2cHostTransfer(struct I2cCntlr *cntlr, struct I2cMsg *msgs, int16_t count);

struct I2cMethod g_i2cHostMethod = {
    .transfer = i2cHostTransfer,
};

static int32_t i2c_send(struct I2cMsg *msg, struct I2cDevice *device)
{
    uint16_t length;
    uint32_t flag;

    device->pstmsg.addr = msg->addr;
    device->pstmsg.buf = msg->buf;
    device->pstmsg.len = msg->len;
    device->pstmsg.block = I2C_M_NO_BLOCK;
    device->pstmsg.idex = 0;
    device->pstmsg.i2cx = 0;

    if (msg->flags == I2C_FLAG_READ) {
        device->pstmsg.direct = I2C_M_READ;
    } else {
        device->pstmsg.direct = I2C_M_WRITE;
    }

    if (device->resource.subaddr > 0) {
        device->pstmsg.subflag = 1;
        device->pstmsg.subaddr = device->resource.subaddr;
        device->pstmsg.sublen = device->resource.subaddr_len;
    } else {
        device->pstmsg.subflag = 0;
    }

    i2c_transfer_onemsg_no_block(&device->pstmsg);

}

static int32_t i2c_transfer(struct I2cDevice *device, struct I2cMsg *msgs, int16_t count)
{
    int ret;
    struct I2cMsg *msg = NULL;
    
    uint32_t i2cPort;
    if (device == NULL || msgs == NULL) {
        HDF_LOGE("%s: device or  msgs is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    i2cPort = (uint32_t)device->port;
    if (i2cPort != HAL_I2C_ID_NUM) {
        HDF_LOGE("i2c port %u not support\r\n", i2cPort);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (HDF_SUCCESS != OsalMutexLock(&device->mutex)) {
        HDF_LOGE("%s %d OsalMutexTimedLock fail\r\n", __func__, __LINE__);
        return HDF_ERR_TIMEOUT;
    }
    for (int32_t i = 0; i < count; i++) {
        msg = &msgs[i];
        i2c_send(msg, device);
    }
    OsalMutexUnlock(&device->mutex);
    return count;
}

static int32_t i2cHostTransfer(struct I2cCntlr *cntlr, struct I2cMsg *msgs, int16_t count)
{
    struct I2cDevice *device = NULL;
    if (cntlr == NULL || msgs == NULL || cntlr->priv == NULL) {
        HDF_LOGE("%s: I2cCntlr or msgs is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    device = (struct I2cDevice *)cntlr->priv;
    if (device == NULL) {
        HDF_LOGE("%s: I2cDevice is NULL\r\n", __func__);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    return i2c_transfer(device, msgs, count);
}


static uint32_t GetI2cDeviceResource(struct I2cDevice *device,
                                     const struct DeviceResourceNode *resourceNode)
{
    /* uint32_t tempPin = 0; */
    struct I2cResource *resource = NULL;
    struct DeviceResourceIface *dri = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("device or resourceNode is NULL\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s %d: invalid parameter\r\n", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }

    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (dri->GetUint32(resourceNode, "port", &resource->port, NULL) != HDF_SUCCESS) {
        HDF_LOGE("i2c config port fail\r\n");
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "freq", &resource->freq, NULL) != HDF_SUCCESS) {
        HDF_LOGE("i2c config freq fail\r\n");
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "subaddr_len", &resource->subaddr_len, NULL) != HDF_SUCCESS) {
        HDF_LOGE("i2c config subaddr_len fail\r\n");
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "subaddr", &resource->subaddr, NULL) != HDF_SUCCESS) {
        HDF_LOGE("i2c config subaddr fail\r\n");
        return HDF_FAILURE;
    }
printf("%s: resource->subaddr %u\n", __func__, resource->subaddr);
    return HDF_SUCCESS;
}


int32_t InitI2cDevice(struct I2cDevice *device)
{
    int32_t ret = -1;
    uint32_t i2cPort;
    uint32_t freq;
    struct I2cResource *resource = NULL;
    
    if (device == NULL) {
        HDF_LOGE("device is NULL\r\n");
        return HDF_ERR_INVALID_PARAM;
    }
    
    resource = &device->resource;
    freq = resource->port;
    i2cPort = resource->port;

    if (i2cPort != HAL_I2C_ID_NUM) {
        HDF_LOGE("i2c port %u not support\r\n", i2cPort);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalMutexInit(&device->mutex) != HDF_SUCCESS) {
        HDF_LOGE("%s %d OsalMutexInit fail\r\n", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if (HDF_SUCCESS != OsalMutexLock(&device->mutex)) {
        HDF_LOGE("%s %d osMutexWait fail\r\n", __func__, __LINE__);
        return HDF_ERR_TIMEOUT;
    }

    hal_i2c_init(i2cPort, freq);

    OsalMutexUnlock(&device->mutex);
    return HDF_SUCCESS;
}

static int32_t AttachI2cDevice(struct I2cCntlr *host, struct HdfDeviceObject *device)
{
    int32_t ret;
    struct I2cDevice *i2cDevice = NULL;
    struct I2cResource *resource = NULL;

    if (device == NULL || host == NULL) {
        HDF_LOGE("%s: device or host is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    i2cDevice = (struct I2cDevice *)OsalMemAlloc(sizeof(struct I2cDevice));
    if (i2cDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc i2cDevice error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    (void)memset_s(i2cDevice, sizeof(struct I2cDevice), 0, sizeof(struct I2cDevice));
    ret = GetI2cDeviceResource(i2cDevice, device->property);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(i2cDevice);
        return HDF_FAILURE;
    }
    resource = &i2cDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("%s %d: invalid parameter\r\n", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }

    host->priv = i2cDevice;
    host->busId = i2cDevice->port;

    return InitI2cDevice(i2cDevice);
}

/* HdfDriverEntry method definitions */
static int32_t i2cDriverBind(struct HdfDeviceObject *device);
static int32_t i2cDriverInit(struct HdfDeviceObject *device);
static void i2cDriverRelease(struct HdfDeviceObject *device);

static int32_t i2cDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: I2c device object is NULL\r\n", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t i2cDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct I2cCntlr *host = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    host = (struct I2cCntlr *)OsalMemAlloc(sizeof(struct I2cCntlr));
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    (void)memset_s(host, sizeof(struct I2cCntlr), 0, sizeof(struct I2cCntlr));
    host->ops = &g_i2cHostMethod;
    device->priv = (void *)host;

    ret = AttachI2cDevice(host, device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: attach error\r\n", __func__);
        i2cDriverRelease(device);
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }
    ret = I2cCntlrAdd(host);
    if (ret != HDF_SUCCESS) {
        i2cDriverRelease(device);
        return HDF_FAILURE;
    }
    return ret;
}

static void i2cDriverRelease(struct HdfDeviceObject *device)
{
    struct I2cCntlr *i2cCntrl = NULL;
    struct I2cDevice *i2cDevice = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return;
    }
    i2cCntrl = device->priv;
    if (i2cCntrl == NULL || i2cCntrl->priv == NULL) {
        HDF_LOGE("%s: i2cCntrl is NULL\r\n", __func__);
        return;
    }
    i2cCntrl->ops = NULL;
    i2cDevice = (struct I2cDevice *)i2cCntrl->priv;
    OsalMemFree(i2cCntrl);

    if (i2cDevice != NULL) {
        OsalMutexDestroy(&i2cDevice->mutex);
        OsalMemFree(i2cDevice);
    }
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_i2cDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "BL_I2C_MODULE_HDF",
    .Bind = i2cDriverBind,
    .Init = i2cDriverInit,
    .Release = i2cDriverRelease,
};

// Initialize HdfDriverEntry
HDF_INIT(g_i2cDriverEntry);
