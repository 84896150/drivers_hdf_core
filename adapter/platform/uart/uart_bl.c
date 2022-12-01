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
#include "bl602_uart.h"
#include "bl602_hbn.h"

#define HDF_UART_TMO 1000
#define TMO_MS_UNIT_CONV (1000)
#define HDF_LOG_TAG uartDev

#define UART_DEV_SERVICE_NAME_PREFIX "HDF_PLATFORM_UART%d"
#define MAX_DEV_NAME_SIZE 32

struct UartResource {
    uint32_t num;      /* UART port num */
    uint32_t tx_pin ;
    uint32_t rx_pin ;
    uint32_t cts_pin ;
    uint32_t rts_pin ;
    uint32_t uart_div ;
};

enum UartDeviceState {
    UART_DEVICE_UNINITIALIZED = 0x0u,
    UART_DEVICE_INITIALIZED = 0x1u,
};

struct UartDevice {
    struct IDeviceIoService ioService;
    struct UartResource resource;
    uint32_t uartId;
    bool initFlag;
    uint32_t transMode;
    UART_CFG_Type uartCfg;
    uint32_t uart_div ;
};

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
        uartDevice->initFlag = true;
    }

    return HDF_SUCCESS;
}

/* UartHostMethod implementations */
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

    uartDevice->initFlag = false;
    return HDF_SUCCESS;
}
static int32_t UartHostDevWrite(struct UartHost *host, uint8_t *data, uint32_t size)
{
    struct UartDevice *device = NULL;
    uint32_t portId;
    uint32_t i = 0;

    if (host == NULL || data == NULL || size == 0 || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    device = (struct UartDevice *)host->priv;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    HDF_LOGI("%s   %d\n",__func__,__LINE__);
    portId = device->uartId;

    while (i < size) {
        bl_uart_data_send(portId, ((uint8_t*)data)[i]);
        i++;
    }
    return HDF_SUCCESS;
}

static int32_t UartHostDevRead(struct UartHost *host, uint8_t *data, uint32_t size)
{
    int ch;
    uint32_t counter = 0;
    uint32_t recv_size;
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

    while (counter < size && (ch = bl_uart_data_recv(uartId)) >= 0) {
        ((uint8_t *)data)[counter] = ch;
        counter++;
    }

    recv_size = counter;
    return recv_size;
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
    bl_uart_setconfig(uartId, baudRate, UART_PARITY_NONE);

    uartDevice->uartCfg.baudRate = baudRate;

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

    *baudRate = uartDevice->uartCfg.baudRate;
    return HDF_SUCCESS;
}

static int32_t UartDevSetAttribute(struct UartDevice *uartDevice, struct UartAttribute *attribute)
{
    switch(attribute->dataBits) {
        case UART_ATTR_DATABIT_8:
            uartDevice->uartCfg.dataBits = UART_DATABITS_8;
            break;
        case UART_ATTR_DATABIT_7:
            uartDevice->uartCfg.dataBits = UART_DATABITS_7;
            break;
        case UART_ATTR_DATABIT_6:
            uartDevice->uartCfg.dataBits = UART_DATABITS_6;
            break;
        case UART_ATTR_DATABIT_5:
            uartDevice->uartCfg.dataBits = UART_DATABITS_5;
            break;
        default:
            break;
    }

    switch(attribute->stopBits) {
        case UART_ATTR_STOPBIT_1:
            uartDevice->uartCfg.stopBits = UART_STOPBITS_1;
            break;
        case UART_ATTR_STOPBIT_1P5:
            uartDevice->uartCfg.stopBits = UART_STOPBITS_1_5;
            break;
        case UART_ATTR_STOPBIT_2:
            uartDevice->uartCfg.stopBits = UART_STOPBITS_2;
            break;
        default:
            break;
    }

    switch(attribute->parity) {
        case UART_ATTR_PARITY_NONE:
            uartDevice->uartCfg.parity = UART_PARITY_NONE;
            break;
        case UART_ATTR_PARITY_ODD:
            uartDevice->uartCfg.parity = UART_PARITY_ODD;
            break;
        case UART_ATTR_PARITY_EVEN:
            uartDevice->uartCfg.parity = UART_PARITY_EVEN;
            break;
        default:
            break;
    }

    switch(attribute->rts) {
        case UART_ATTR_RTS_DIS:
            uartDevice->uartCfg.rtsSoftwareControl = DISABLE;
            break;
        case UART_ATTR_RTS_EN:
            uartDevice->uartCfg.rtsSoftwareControl = ENABLE;
            break;
        default:
            break;
    }

    switch(attribute->cts) {
        case UART_ATTR_CTS_DIS:
            uartDevice->uartCfg.ctsFlowControl = DISABLE;
            break;
        case UART_ATTR_CTS_EN:
            uartDevice->uartCfg.ctsFlowControl = ENABLE;
            break;
        default:
            break;
    }

}

static int32_t UartHostDevSetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    HDF_LOGI("%s: Enter", __func__);
    struct UartDevice *uartDevice = NULL;
    uint32_t uartId;
    uint32_t uart_div = 0;
    uint32_t uart_clk_init = 0;
    uint32_t tx_pin = 0;
    uint32_t rx_pin = 0;
    uint32_t cts_pin = 0;
    uint32_t rts_pin = 0;


    if (host == NULL || attribute == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uartDevice = (struct UartDevice *)host->priv;
    if (uartDevice == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
printf("%s : uartDevice->uart_div %u\n", __func__, uartDevice->uart_div);
    uartId = uartDevice->uartId;
    uart_div = uartDevice->uart_div;
    uartDevice->uartCfg.uartClk = (160 * 1000 * 1000) / (uart_div + 1);
    uartDevice->uartCfg.ctsFlowControl = DISABLE;
    uartDevice->uartCfg.byteBitInverse = UART_LSB_FIRST;

    UartDevSetAttribute(uartDevice, attribute);

    /* enable clk */
    if (0 == uart_clk_init) {
        GLB_Set_UART_CLK(1, HBN_UART_CLK_160M, uart_div);
        uart_clk_init = 1;
    }

printf("%s : uartDevice->resource.tx_pin %u\n", __func__, uartDevice->resource.tx_pin);
    tx_pin = uartDevice->resource.tx_pin;
    rx_pin = uartDevice->resource.rx_pin;
    cts_pin = uartDevice->resource.cts_pin;
    rts_pin = uartDevice->resource.rts_pin;
    /* gpio init */
    gpio_init(uartId, tx_pin, rx_pin, cts_pin, rts_pin);

    /* Disable all interrupt */
    UART_IntMask(uartId, UART_INT_ALL, MASK);

    /* Disable uart before config */
    UART_Disable(uartId, UART_TXRX);

    /* UART init */
    UART_Init(uartId, &(uartDevice->uartCfg));

    /* Enable tx free run mode */
    UART_TxFreeRun(uartId, ENABLE);

    /* Enable uart */
    UART_Enable(uartId, UART_TXRX);

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

    attribute->parity = uartDevice->uartCfg.parity;
    attribute->stopBits = uartDevice->uartCfg.stopBits;
    attribute->rts = uartDevice->uartCfg.rtsSoftwareControl;
    attribute->cts = uartDevice->uartCfg.ctsFlowControl;
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
    if (dri->GetUint32(resourceNode, "tx_pin", &resource->tx_pin, 0) != HDF_SUCCESS) {
        HDF_LOGE("uart config read tx_pin fail");
        return HDF_FAILURE;
    }
    if (dri->GetUint32(resourceNode, "rx_pin", &resource->rx_pin, 0) != HDF_SUCCESS) {
        HDF_LOGE("uart config read rx_pin fail");
        return HDF_FAILURE;
    }
    if (dri->GetUint32(resourceNode, "cts_pin", &resource->cts_pin, 0) != HDF_SUCCESS) {
        HDF_LOGE("uart config read cts_pin fail");
        return HDF_FAILURE;
    }
    if (dri->GetUint32(resourceNode, "rts_pin", &resource->rts_pin, 0) != HDF_SUCCESS) {
        HDF_LOGE("uart config read rts_pin fail");
        return HDF_FAILURE;
    }
    if (dri->GetUint32(resourceNode, "uart_div", &resource->uart_div, 0) != HDF_SUCCESS) {
        HDF_LOGE("uart config read uart_div fail");
        return HDF_FAILURE;
    }

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

    uartDevice = (struct UartDevice *)OsalMemCalloc(sizeof(struct UartDevice) , 1);
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

/* HdfDriverEntry method definitions */
static int32_t UartDriverBind(struct HdfDeviceObject *device);
static int32_t UartDriverInit(struct HdfDeviceObject *device);
static void UartDriverRelease(struct HdfDeviceObject *device);

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_UartDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "BL_UART_MODULE_HDF",
    .Bind = UartDriverBind,
    .Init = UartDriverInit,
    .Release = UartDriverRelease,
};

/* Initialize HdfDriverEntry */
HDF_INIT(g_UartDriverEntry);
