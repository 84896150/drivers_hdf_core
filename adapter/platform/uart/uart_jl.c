/*
 * Copyright (c) 2022 Jiangsu Hoperun Software Co., Ltd.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include <stdlib.h>
#include <string.h>
#include "osal_time.h"
#include "osal_sem.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "uart_if.h"
#include "uart_core.h"

#include "uart_dev.h"

#define HDF_UART_TMO 1000
#define TMO_MS_UNIT_CONV (1000)
#define HDF_LOG_TAG uartDev
#define UART_RX_CBUF_SIZE          32
#define UART_RX_FRAME_LENGTH       6
#define UART_RX_TIMEOUT            6
#define UART_STOP_BIT1             1
#define UART_STOP_BIT2             2

#define UART_DEV_SERVICE_NAME_PREFIX "HDF_PLATFORM_UART%d"
#define MAX_DEV_NAME_SIZE 32

struct UartResource {
    uint32_t num;      /* UART port num */
    uint32_t baudRate; /* Default baudrate */
    uint32_t wLen;     /* Default word length */
    uint32_t parity;   /* Default parity */
    uint32_t stopBit;  /* Default stop bits */
    bool txDMA;
    bool rxDMA;
    uint8_t txPin;
    uint8_t rxPin;
};

enum UartDeviceState {
    UART_DEVICE_UNINITIALIZED = 0x0u,
    UART_DEVICE_INITIALIZED = 0x1u,
};

struct UartConfig {
    uint32_t baudRate;
    uint8_t dataBits;
    uint8_t parity;
    uint8_t stopBits;
    uint8_t rts;
    uint8_t cts;
    uint8_t fifoRxEn;
    uint8_t fifoTxEn;
};

struct UartDevice {
    struct IDeviceIoService ioService;
    struct UartResource resource;
    uint32_t uartId;
    bool initFlag;
    uint32_t transMode;
    struct UartConfig Config;
    uart_bus_t *uartBus;
};

static int32_t HalUartSend(struct UartDevice *device, const uint8_t *data, uint32_t size)
{
    if (data == NULL || size == 0) {
        HDF_LOGE("%s %d Invalid input \r\n", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    device->uartBus->write(data, size);

    return HDF_SUCCESS;
}

static int32_t HalUartRecv(struct UartDevice *device, uint8_t *data, uint32_t expectSize,
                           uint32_t *recvSize, uint32_t timeOut)
{
    uint32_t recvedLen;
    if (data == NULL || expectSize == 0 || recvSize == NULL) {
        HDF_LOGE("%s %d Invalid input \r\n", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    recvedLen = device->uartBus->read((uint8_t *)data, expectSize, timeOut);
    *recvSize = recvedLen;

    return HDF_SUCCESS;
}

static void UartStart(struct UartDevice *device)
{
    struct UartResource *resource = NULL;
    struct uart_platform_data_t u_arg = {0};

    if (device == NULL) {
        HDF_LOGE("%s: INVALID PARAM", __func__);
        return;
    }
    resource = &device->resource;
    u_arg.tx_pin = resource->txPin;
    u_arg.rx_pin = resource->rxPin;

    if (resource->rxDMA != 0) {
        u_arg.rx_cbuf_size = UART_RX_CBUF_SIZE;
        u_arg.frame_length = UART_RX_FRAME_LENGTH;
        u_arg.rx_cbuf = OsalMemAlloc(u_arg.rx_cbuf_size * u_arg.frame_length);
        if (u_arg.rx_cbuf == NULL) {
            HDF_LOGE("%s  malloc fail\n", __func__);
            return;
        }
    }
    u_arg.rx_timeout = UART_RX_TIMEOUT;
    u_arg.isr_cbfun = NULL;
    u_arg.baud = resource->baudRate;
    u_arg.is_9bit = 0;

    device->uartBus = uart_dev_open(&u_arg);
}

/* HdfDriverEntry method definitions */
static int32_t UartDriverBind(struct HdfDeviceObject *device);
static int32_t UartDriverInit(struct HdfDeviceObject *device);
static void UartDriverRelease(struct HdfDeviceObject *device);

/* HdfDriverEntry definitions */
static struct HdfDriverEntry g_UartDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_PLATFORM_UART",
    .Bind = UartDriverBind,
    .Init = UartDriverInit,
    .Release = UartDriverRelease,
};

/* Initialize HdfDriverEntry */
HDF_INIT(g_UartDriverEntry);

/* UartHostMethod method definitions */
static int32_t UartHostDevInit(struct UartHost *host);
static int32_t UartHostDevDeinit(struct UartHost *host);
static int32_t UartHostDevWrite(struct UartHost *host, uint8_t *data, uint32_t size);
static int32_t UartHostDevSetBaud(struct UartHost *host, uint32_t baudRate);
static int32_t UartHostDevGetBaud(struct UartHost *host, uint32_t *baudRate);
static int32_t UartHostDevRead(struct UartHost *host, uint8_t *data, uint32_t size);
static int32_t UartHostDevSetAttribute(struct UartHost *host, struct UartAttribute *attribute);
static int32_t UartHostDevGetAttribute(struct UartHost *host, struct UartAttribute *attribute);
static int32_t UartHostDevSetTransMode(struct UartHost *host, enum UartTransMode mode);

/* UartHostMethod definitions */
static struct UartHostMethod g_uartHostMethod = {
    .Init = UartHostDevInit,
    .Deinit = UartHostDevDeinit,
    .Read = UartHostDevRead,
    .Write = UartHostDevWrite,
    .SetBaud = UartHostDevSetBaud,
    .GetBaud = UartHostDevGetBaud,
    .SetAttribute = UartHostDevSetAttribute,
    .GetAttribute = UartHostDevGetAttribute,
    .SetTransMode = UartHostDevSetTransMode,
};

static int32_t InitUartDevice(struct UartHost *host)
{
    struct UartDevice *uartDevice = NULL;
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;

    if (uartDevice->initFlag != 0) {
        HDF_LOGI("uart %u device init", uartDevice->uartId);
        UartStart(uartDevice);
        uartDevice->initFlag = true;
    }
    return HDF_SUCCESS;
}

static uint32_t GetUartDeviceResource(struct UartDevice *device, const struct DeviceResourceNode *resourceNode)
{
    struct DeviceResourceIface *dri = NULL;
    struct UartResource *resource = NULL;
    char *tmpString = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: INVALID PARAM", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &device->resource;
    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL || dri->GetString == NULL) {
        HDF_LOGE("%s: DeviceResourceIface is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (dri->GetUint32(resourceNode, "num", &resource->num, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: uart config read num fail", __func__);
        return HDF_FAILURE;
    }
    if (dri->GetUint32(resourceNode, "baudrate", &resource->baudRate, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: uart config read baudrate fail", __func__);
        return HDF_FAILURE;
    }
    if (dri->GetUint32(resourceNode, "parity", &resource->parity, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: uart config read parity fail", __func__);
        return HDF_FAILURE;
    }
    if (dri->GetUint32(resourceNode, "stopBit", &resource->stopBit, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: uart config read stopBit fail", __func__);
        return HDF_FAILURE;
    }
    if (dri->GetString(resourceNode, "tx_pin", &tmpString, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: uart config read stopBit fail", __func__);
        return HDF_FAILURE;
    }
    resource->txPin = get_gpio(tmpString);
    if (dri->GetString(resourceNode, "rx_pin", &tmpString, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: uart config read stopBit fail", __func__);
        return HDF_FAILURE;
    }
    resource->rxPin = get_gpio(tmpString);
    device->Config.stopBits = resource->stopBit;
    device->Config.dataBits = resource->baudRate;
    device->Config.parity = resource->parity;
    device->uartId = resource->num;
    return HDF_SUCCESS;
}

static int32_t AttachUartDevice(struct UartHost *uartHost, struct HdfDeviceObject *device)
{
    int32_t ret;
    struct UartDevice *uartDevice = NULL;

    if (uartHost == NULL || device == NULL || device->property == NULL) {
        HDF_LOGE("%s: property is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *) OsalMemCalloc(sizeof(struct UartDevice), 1);
    if (uartDevice == NULL) {
        HDF_LOGE("%s: OsalMemCalloc uartDevice error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = GetUartDeviceResource(uartDevice, device->property);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(uartDevice);
        return HDF_FAILURE;
    }
    uartHost->priv = uartDevice;
    return InitUartDevice(uartHost);
}

static int32_t UartDriverBind(struct HdfDeviceObject *device)
{
    struct UartHost *devService = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    devService = (struct UartHost *)OsalMemAlloc(sizeof(*devService));
    if (devService == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    devService->device = device;
    device->service = &(devService->service);
    devService->priv = NULL;
    devService->method = NULL;
    return HDF_SUCCESS;
}

static void UartDriverRelease(struct HdfDeviceObject *device)
{
    struct UartHost *host = NULL;
    struct UartDevice *uartDevice = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return;
    }
    host = UartHostFromDevice(device);
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: host is NULL", __func__);
        return;
    }
    uartDevice = (struct UartDevice *)host->priv;
    host->method = NULL;

    OsalMemFree(host);
    OsalMemFree(uartDevice);
}

static int32_t UartDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct UartHost *host = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    host = UartHostFromDevice(device);
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = AttachUartDevice(host, device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: attach error", __func__);
        return HDF_FAILURE;
    }
    host->method = &g_uartHostMethod;
    return ret;
}

static int32_t UartHostDevInit(struct UartHost *host)
{
    if (host == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    InitUartDevice(host);
    return HDF_SUCCESS;
}

static int32_t UartHostDevDeinit(struct UartHost *host)
{
    struct UartDevice *uartDevice = NULL;
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;

    uartDevice->initFlag = false;
    return HDF_SUCCESS;
}
static int32_t UartHostDevWrite(struct UartHost *host, uint8_t *data, uint32_t size)
{
    struct UartDevice *device = NULL;

    if (host == NULL || data == NULL || size == 0 || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    device = (struct UartDevice *)host->priv;

    HalUartSend(device, data, size);
    return HDF_SUCCESS;
}

static int32_t UartHostDevRead(struct UartHost *host, uint8_t *data, uint32_t size)
{
    uint32_t recvSize;
    int32_t ret;
    struct UartDevice *uartDevice = NULL;
    if (host == NULL || data == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;

    ret = HalUartRecv(uartDevice, data, size, &recvSize, HDF_UART_TMO);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: uart %ld recev error\r\n", __func__, uartDevice->uartId);
        return ret;
    }
    ret = recvSize;
    return ret;
}

static int32_t UartHostDevSetBaud(struct UartHost *host, uint32_t baudRate)
{
    struct UartDevice *uartDevice = NULL;
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    uartDevice->Config.baudRate = baudRate;
    uartDevice->uartBus->set_baud(baudRate) ;
    return HDF_SUCCESS;
}

static int32_t UartHostDevGetBaud(struct UartHost *host, uint32_t *baudRate)
{
    struct UartDevice *uartDevice = NULL;
    if (host == NULL || baudRate == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    *baudRate = uartDevice->Config.baudRate;
    return HDF_SUCCESS;
}

static int32_t UartHostDevSetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    struct UartDevice *uartDevice = NULL;
    if (host == NULL || attribute == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    uartDevice->Config.parity = attribute->parity;
    switch (attribute->stopBits) {
        case UART_ATTR_STOPBIT_1:
            uartDevice->Config.stopBits = UART_ATTR_STOPBIT_1;
            break;
        case UART_ATTR_STOPBIT_2:
            uartDevice->Config.stopBits = UART_ATTR_STOPBIT_2;
            break;
        default:
            uartDevice->Config.stopBits = UART_ATTR_STOPBIT_1;
            break;
    }
    uartDevice->Config.rts = attribute->rts;
    uartDevice->Config.cts = attribute->cts;
    return HDF_SUCCESS;
}

static int32_t UartHostDevGetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    struct UartDevice *uartDevice = NULL;
    if (host == NULL || attribute == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    attribute->parity = uartDevice->Config.parity;
    attribute->stopBits = uartDevice->Config.stopBits;
    attribute->rts = uartDevice->Config.rts;
    attribute->cts = uartDevice->Config.cts;
    return HDF_SUCCESS;
}

static int32_t UartHostDevSetTransMode(struct UartHost *host, enum UartTransMode mode)
{
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    switch (mode) {
        case UART_MODE_RD_BLOCK:
            break;
        case UART_MODE_RD_NONBLOCK:
            break;
        case UART_MODE_DMA_RX_EN:
            break;
        case UART_MODE_DMA_RX_DIS:
            break;
        case UART_MODE_DMA_TX_EN:
            break;
        case UART_MODE_DMA_TX_DIS:
            break;
        default:
            HDF_LOGE("%s: UartTransMode(%d) invalid", __func__, mode);
            break;
    }
    return HDF_SUCCESS;
}
