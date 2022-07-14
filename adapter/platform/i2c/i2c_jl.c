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

#include "iic_hw.h"
#include "gpio.h"

#define DEC_NUM 10
#define GROUP_PIN_NUM 4
#define I2C_INVALID_ADDR 0xFFFF
#define HAL_I2C_ID_NUM 1

struct I2cResource {
    uint32_t port;
    uint32_t sclPin;
    uint32_t sdaPin;
    uint32_t speed;
};

struct I2cDevice {
    uint16_t devAddr;      
    uint32_t addressWidth;
    struct OsalMutex mutex;
    uint32_t port;
    struct I2cResource resource;
};

static int32_t i2cDriverBind(struct HdfDeviceObject *device);
static int32_t i2cDriverInit(struct HdfDeviceObject *device);
static void i2cDriverRelease(struct HdfDeviceObject *device);

struct HdfDriverEntry g_i2cDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_PLATFORM_I2C",
    .Bind = i2cDriverBind,
    .Init = i2cDriverInit,
    .Release = i2cDriverRelease,
};

HDF_INIT(g_i2cDriverEntry);

struct hw_iic_config hw_iic_cfg[] = {
    {
        .port = 'A',
        .baudrate = 100000,
        .hdrive   = 0,
        .io_filter = 1,
        .io_pu      = 1,
    },
};
struct iic_iomapping {
    u8 scl;
    u8 sda;
    char group;
};
static const struct iic_iomapping hwiic_iomap[HAL_I2C_ID_NUM][GROUP_PIN_NUM] = {
    {
        {IO_PORT_DP, IO_PORT_DM},    
        {IO_PORTC_04, IO_PORTC_05},  
        {IO_PORTB_04, IO_PORTB_06},  
        {IO_PORTA_05, IO_PORTA_06},  
    },
};

static int32_t i2cHostTransfer(struct I2cCntlr *cntlr, struct I2cMsg *msgs, int16_t count);

struct I2cMethod g_i2cHostMethod = {
    .transfer = i2cHostTransfer,
};

int32_t InitI2cDevice(struct I2cDevice *device)
{
    int32_t ret = -1;
    uint32_t i2cPort;
    struct I2cResource *resource = NULL;

    if (device == NULL) {
        HDF_LOGE("device is NULL\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &device->resource;
    device->port = resource->port;
    i2cPort = device->port;
    if (i2cPort >= HAL_I2C_ID_NUM) {
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

    for (int i = 0; i < GROUP_PIN_NUM ; i++) {
        if (resource->sclPin == hwiic_iomap[device->port][i].scl
            && resource->sdaPin == hwiic_iomap[device->port][i].sda) {
            hw_iic_cfg[device->port].port = 'A' + i;
            break;
        }
    }

    HDF_LOGI("hw iic port%d  group%c\n", device->port, hw_iic_cfg[device->port].port);

    hw_iic_init(device->port);
    OsalMutexUnlock(&device->mutex);
    return HDF_SUCCESS;
}

static int32_t HostRestI2cDevice(struct I2cDevice *device)
{
    int32_t ret = -1;
    struct I2cResource *resource = NULL;
    uint32_t i2cPort;

    if (device == NULL) {
        HDF_LOGE("%s %d device is null\r\n", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s %d: invalid parameter\r\n", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    device->port = resource->port;
    i2cPort = device->port;
    if (i2cPort > HAL_I2C_ID_NUM) {
        HDF_LOGE("i2c port %u not support\r\n", i2cPort);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static uint32_t GetI2cDeviceResource(struct I2cDevice *device,
                                     const struct DeviceResourceNode *resourceNode)
{
    char *tempString = NULL;
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

    if (dri->GetString(resourceNode, "sclPin", &tempString, NULL) != HDF_SUCCESS) {
        HDF_LOGE("i2c config sclPin fail\r\n");
        return HDF_FAILURE;
    }

    resource->sclPin = get_gpio(tempString);

    if (dri->GetString(resourceNode, "sdaPin", &tempString, NULL) != HDF_SUCCESS) {
        HDF_LOGE("i2c config sdaPin fail\r\n");
        return HDF_FAILURE;
    }
    resource->sdaPin = get_gpio(tempString);

    if (dri->GetUint32(resourceNode, "speed", &resource->speed, 0) != HDF_SUCCESS) {
        HDF_LOGE("i2c config speed fail\r\n");
        return HDF_FAILURE;
    }
    HDF_LOGI("i2c%d speed:%d\n", resource->port, resource->speed);
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

static int32_t i2cDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: I2c device object is NULL\r\n", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
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

static char i2c_read_byte(uint8_t ifack, uint8_t ifstop)
{
    uint8_t data;
    hw_iic_start(0);
    data = hw_iic_rx_byte(0, ifack);
    if (ifstop) {
        hw_iic_stop(0);
    }
    return data;

}

static void i2c_write_byte(uint8_t data, uint8_t ifstart)
{
    if (ifstart) {
        hw_iic_start(0);
    }
    hw_iic_tx_byte(0, data);
    hw_iic_stop(0);
}


static int32_t i2c_send(struct I2cMsg *msg)
{
    uint16_t len;
    uint8_t ifack, ifstop, ifstart;

    len = msg->len;
    ifstop = 0;
    if (msg->flags & I2C_FLAG_READ) {
        ifack = msg->flags & I2C_FLAG_READ_NO_ACK ? 0 : 1;
        for (int32_t j = 0; j < len; j++) {
            if (((msg->flags & I2C_FLAG_STOP) && j) == (len - 1)) {
                ifstop = 1;
            }

            msg->buf[j] = i2c_read_byte(ifack, ifstop);

            HDF_LOGI("%s read %c\n", __func__, msg->buf[j]);
        }
    } else {
        ifack = msg->flags & I2C_FLAG_IGNORE_NO_ACK ? 0 : 1;
        for (int32_t j = 0; j < len; j++) {
            if (((msg->flags & I2C_FLAG_NO_START) == 0) && (j == 0)) {
                ifstart = 1;
            }
            HDF_LOGI("%s write %x\n", __func__, msg->buf[j]);
            i2c_write_byte(msg->buf[j], ifstart);
        }
    }
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
    if (i2cPort > HAL_I2C_ID_NUM) {
        HDF_LOGE("i2c port %u not support\r\n", i2cPort);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (HDF_SUCCESS != OsalMutexLock(&device->mutex)) {
        HDF_LOGE("%s %d OsalMutexTimedLock fail\r\n", __func__, __LINE__);
        return HDF_ERR_TIMEOUT;
    }
    for (int32_t i = 0; i < count; i++) {
        msg = &msgs[i];
        i2c_send(msg);
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
