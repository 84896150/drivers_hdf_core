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
    uint8_t tx_pin;
    uint8_t rx_pin;
};

enum UartDeviceState {
    UART_DEVICE_UNINITIALIZED = 0x0u,
    UART_DEVICE_INITIALIZED = 0x1u,
};
struct uart_config {
    uint32_t baudRate;
    uint8_t dataBits ;
    uint8_t parity ;
    uint8_t stopBits ;
    uint8_t rts ;
    uint8_t cts ;
    uint8_t fifoRxEn ;
    uint8_t fifoTxEn ;
};

struct UartDevice {
    struct IDeviceIoService ioService;
    struct UartResource resource;
    uint32_t uartId;
    bool initFlag;
    uint32_t transMode;
    struct uart_config config;
    uart_bus_t *uart_bus;
};

static int32_t HalUartSend(struct UartDevice *device, const uint8_t *data, uint32_t size, uint32_t timeOut)
{
    int32_t ret = HDF_SUCCESS;

    if (data == NULL || size == 0) {
        HDF_LOGE("%s %d Invalid input \r\n", __FILE__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    device->uart_bus->write(data, size);

    return ret;
}

static int32_t HalUartRecv(struct UartDevice *device, uint8_t *data, uint32_t expectSize,
                           uint32_t *recvSize, uint32_t timeOut)
{
    int32_t ret = HDF_FAILURE;
    uint32_t recvedLen = 0;
    if (data == NULL || expectSize == 0 || recvSize == NULL) {
        HDF_LOGE("%s %d Invalid input \r\n", __FILE__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    recvedLen = device->uart_bus->read((uint8_t *)data, expectSize, timeOut);

    if (recvSize != NULL) {
        *recvSize = recvedLen;
    }
    return HDF_SUCCESS;
}

static void HalUartHandlerInit(struct UartDevice *device)
{
    uint32_t uartId;

    if (device == NULL) {
        HDF_LOGE("%s: INVALID PARAM", __func__);
        return;
    }

    uartId = device->uartId;
}

static void UartStart(struct UartDevice *device)
{
    uint32_t uartId;
    struct UartResource *resource = &device->resource;
    if (device == NULL) {
        HDF_LOGE("%s: INVALID PARAM", __func__);
        return;
    }
    uartId = device->uartId;


    struct uart_platform_data_t u_arg = {0};

    u_arg.tx_pin = resource->tx_pin;
    u_arg.rx_pin = resource->rx_pin;

    if (resource->rxDMA) {

        u_arg.rx_cbuf_size = UART_RX_CBUF_SIZE;
        u_arg.frame_length = UART_RX_FRAME_LENGTH;

        u_arg.rx_cbuf = OsalMemAlloc(u_arg.rx_cbuf_size * u_arg.frame_length);
        if (!u_arg.rx_cbuf) {

            HDF_LOGE("%s  malloc fail\n", __func__);
        }
    }
    u_arg.rx_timeout = UART_RX_TIMEOUT;
    u_arg.isr_cbfun = NULL;

    u_arg.baud = resource->baudRate;
    u_arg.is_9bit = 0;



    device->uart_bus = uart_dev_open(&u_arg);

    HDF_LOGI("%s %ld\r\n", __FUNCTION__, uartId);
    HalUartHandlerInit(device);
}

/* HdfDriverEntry method definitions */
static int32_t UartDriverBind(struct HdfDeviceObject *device);
static int32_t UartDriverInit(struct HdfDeviceObject *device);
static void UartDriverRelease(struct HdfDeviceObject *device);

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_UartDriverEntry = {
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
struct UartHostMethod g_uartHostMethod = {
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

static int InitUartDevice(struct UartHost *host)
{
    HDF_LOGI("%s: Enter", __func__);
    struct UartDevice *uartDevice = NULL;
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    if (uartDevice == NULL) {
        HDF_LOGE("%s: INVALID OBJECT", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (!uartDevice->initFlag) {
        HDF_LOGI("uart %ld device init", uartDevice->uartId);
        UartStart(uartDevice);
        uartDevice->initFlag = true;
    }
    return HDF_SUCCESS;
}

static uint32_t GetUartDeviceResource(struct UartDevice *device, const struct DeviceResourceNode *resourceNode)
{
    struct DeviceResourceIface *dri = NULL;
    struct UartResource *resource = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: INVALID PARAM", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s: INVALID OBJECT", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid");
        return HDF_ERR_INVALID_PARAM;
    }
    if (dri->GetUint32(resourceNode, "num", &resource->num, 0) != HDF_SUCCESS) {
        HDF_LOGE("uart config read num fail");
        return HDF_FAILURE;
    }
    if (dri->GetUint32(resourceNode, "baudrate", &resource->baudRate, 0) != HDF_SUCCESS) {
        HDF_LOGE("uart config read baudrate fail");
        return HDF_FAILURE;
    }
    if (dri->GetUint32(resourceNode, "parity", &resource->parity, 0) != HDF_SUCCESS) {
        HDF_LOGE("uart config read parity fail");
        return HDF_FAILURE;
    }
    if (dri->GetUint32(resourceNode, "stopBit", &resource->stopBit, 0) != HDF_SUCCESS) {
        HDF_LOGE("uart config read stopBit fail");
        return HDF_FAILURE;
    }
    char *tmpString = NULL;
    if (dri->GetString(resourceNode, "tx_pin", &tmpString, NULL) != HDF_SUCCESS) {
        HDF_LOGE("uart config read stopBit fail");
        return HDF_FAILURE;
    }
    resource->tx_pin = get_gpio(tmpString);
    if (dri->GetString(resourceNode, "rx_pin", &tmpString, NULL) != HDF_SUCCESS) {
        HDF_LOGE("uart config read stopBit fail");
        return HDF_FAILURE;
    }
    resource->rx_pin = get_gpio(tmpString);
    device->config.stopBits = resource->stopBit;
    device->config.dataBits = resource->baudRate;
    device->config.parity   = resource->parity;
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
    struct UartHost *devService;
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
    HDF_LOGI("Enter %s:", __func__);
    uint32_t uartId;
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
    if (uartDevice == NULL) {
        HDF_LOGE("%s: INVALID OBJECT", __func__);
        return;
    }
    uartId = uartDevice->uartId;
    host->method = NULL;

}

static int32_t UartDriverInit(struct HdfDeviceObject *device)
{
    HDF_LOGI("Enter %s:", __func__);
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
    HDF_LOGI("%s: Enter\r\n", __func__);
    if (host == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    InitUartDevice(host);
    return HDF_SUCCESS;
}

static int32_t UartHostDevDeinit(struct UartHost *host)
{
    HDF_LOGI("%s: Enter", __func__);
    uint32_t uartId;
    struct UartDevice *uartDevice = NULL;
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    if (uartDevice == NULL) {
        HDF_LOGE("%s: INVALID OBJECT", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    uartId = uartDevice->uartId;
    uartDevice->initFlag = false;
    return HDF_SUCCESS;
}
static int32_t UartHostDevWrite(struct UartHost *host, uint8_t *data, uint32_t size)
{
    struct UartDevice *device = NULL;
    uint32_t portId;

    if (host == NULL || data == NULL || size == 0 || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    device = (struct UartDevice *)host->priv;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    HDF_LOGI("%s   %d\n", __func__, __LINE__);
    portId = device->uartId;
    HalUartSend(device, data, size, HDF_UART_TMO);
    return HDF_SUCCESS;
}

static int32_t UartHostDevRead(struct UartHost *host, uint8_t *data, uint32_t size)
{
    uint32_t recvSize = 0;
    int32_t ret;
    uint32_t uartId;
    struct UartDevice *uartDevice = NULL;
    if (host == NULL || data == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    if (uartDevice == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    uartId = uartDevice->uartId;
    ret = HalUartRecv(uartDevice, data, size, &recvSize, HDF_UART_TMO);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("uart %ld recev error\r\n", uartId);
        return ret;
    }
    ret = recvSize;
    return ret;
}

static int32_t UartHostDevSetBaud(struct UartHost *host, uint32_t baudRate)
{
    HDF_LOGI("%s: Enter", __func__);
    struct UartDevice *uartDevice = NULL;
    uint32_t uartId;
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    if (uartDevice == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    uartId = uartDevice->uartId;
    uartDevice->config.baudRate = baudRate;
    uartDevice->uart_bus->set_baud(baudRate) ;
    return HDF_SUCCESS;
}

static int32_t UartHostDevGetBaud(struct UartHost *host, uint32_t *baudRate)
{
    HDF_LOGI("%s: Enter", __func__);
    struct UartDevice *uartDevice = NULL;
    uint32_t uartId;
    if (host == NULL || baudRate == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    if (uartDevice == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    uartId = uartDevice->uartId;
    *baudRate = uartDevice->config.baudRate;
    return HDF_SUCCESS;
}

static int32_t UartHostDevSetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    struct UartDevice *uartDevice = NULL;
    uint32_t uartId;
    if (host == NULL || attribute == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    if (uartDevice == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    uartId = uartDevice->uartId;
    uartDevice->config.parity = attribute->parity;
    switch (attribute->stopBits) {
    	case UART_ATTR_STOPBIT_1:
        	uartDevice->config.stopBits = UART_ATTR_STOPBIT_1;
        	break;
    	case UART_ATTR_STOPBIT_2:
        	uartDevice->config.stopBits = UART_ATTR_STOPBIT_2;
        	break;
    		default:
        	uartDevice->config.stopBits = UART_ATTR_STOPBIT_1;
        	break;
    }
    uartDevice->config.rts = attribute->rts;
    uartDevice->config.cts = attribute->cts;
    return HDF_SUCCESS;
}

static int32_t UartHostDevGetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    HDF_LOGI("%s: Enter", __func__);
    struct UartDevice *uartDevice = NULL;
    if (host == NULL || attribute == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    if (uartDevice == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    attribute->parity = uartDevice->config.parity;
    attribute->stopBits = uartDevice->config.stopBits;
    attribute->rts = uartDevice->config.rts;
    attribute->cts = uartDevice->config.cts;
    return HDF_SUCCESS;
}

static int32_t UartHostDevSetTransMode(struct UartHost *host, enum UartTransMode mode)
{
    HDF_LOGI("%s: Enter", __func__);
    struct UartDevice *uartDevice = NULL;
    uint32_t uartId;
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uartDevice = (struct UartDevice *)host->priv;
    if (uartDevice == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    uartId = uartDevice->uartId;

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
