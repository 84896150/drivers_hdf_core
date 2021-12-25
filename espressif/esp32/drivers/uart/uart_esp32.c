#include "los_event.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "uart_core.h"
#include "uart_if.h"
#include "uart_esp32.h"
#include "uart.h"
#include "uart_types.h"

#define HDF_LOG_TAG uart_esp32
/*-------------------------------------------------------------*/
/* Interface declare */
/*-------------------------------------------------------------*/
struct UartDriverData {
    uint32_t baudRate;
    enum UartTransMode transMode;
    struct UartAttribute attr;
};

/*-------------------------------------------------------------*/
/* Private Interface */
/*-------------------------------------------------------------*/
static uart_parity_t parityHdi2Esp32(unsigned int parity)
{
    switch (parity)
    {
        case UART_ATTR_PARITY_EVEN:
            return UART_PARITY_EVEN;
        case UART_ATTR_PARITY_ODD:
            return UART_PARITY_ODD;
        default:
            return  UART_PARITY_DISABLE;
    }
}

static uart_hw_flowcontrol_t flowcontrolHdi2Esp32(unsigned int rts, unsigned int cts)
{
    uart_hw_flowcontrol_t ret = UART_HW_FLOWCTRL_DISABLE;

    if (UART_ATTR_RTS_EN == rts)
    {
        ret |= UART_HW_FLOWCTRL_RTS;
    }

    if (UART_ATTR_CTS_EN == cts)
    {
        ret |= UART_HW_FLOWCTRL_CTS;
    }

    return ret;
}

static int32_t getConfigFromHcs(struct UartHost *uartHost, const struct DeviceResourceNode *node)
{
    struct UartDriverData* esp32c3DriverData = NULL;
    uint32_t tmp;
    struct DeviceResourceIface *iface = NULL;

    esp32c3DriverData = (struct UartDriverData *)OsalMemCalloc(sizeof(struct UartDriverData));
    if (NULL == esp32c3DriverData) {
        HDF_LOGE("%s: OsalMemCalloc is failed", __func__);
        return HDF_FAILURE;
    }

    iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE); 
    if (iface == NULL || iface->GetUint32 == NULL) {
        HDF_LOGE("%s: face is invalid", __func__);
        (void)OsalMemFree(esp32c3DriverData);
        uartHost->priv = NULL;
        return HDF_FAILURE;
    }

    if (iface->GetUint32(node, "num", &uartHost->num, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read num fail", __func__);
        (void)OsalMemFree(esp32c3DriverData);
        uartHost->priv = NULL;
        return HDF_FAILURE;
    }
    if (iface->GetUint32(node, "baudrate", &esp32c3DriverData->baudRate, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read baudrate fail", __func__);
        (void)OsalMemFree(esp32c3DriverData);
        uartHost->priv = NULL;
        return HDF_FAILURE;
    }
    if (iface->GetUint32(node, "databits", &tmp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read databits fail", __func__);
        (void)OsalMemFree(esp32c3DriverData);
        uartHost->priv = NULL;
        return HDF_FAILURE;
    }
    esp32c3DriverData->attr.dataBits = tmp;
    if (iface->GetUint32(node, "parity", &tmp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read parity fail", __func__);
        (void)OsalMemFree(esp32c3DriverData);
        uartHost->priv = NULL;
        return HDF_FAILURE;
    }
    esp32c3DriverData->attr.parity = tmp;
    if (iface->GetUint32(node, "stopbits", &tmp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read stopbits fail", __func__);
        (void)OsalMemFree(esp32c3DriverData);
        uartHost->priv = NULL;
        return HDF_FAILURE;
    }
    esp32c3DriverData->attr.stopBits = tmp;
    if (iface->GetUint32(node, "rts", &tmp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read rts fail", __func__);
        (void)OsalMemFree(esp32c3DriverData);
        uartHost->priv = NULL;
        return HDF_FAILURE;
    }
    esp32c3DriverData->attr.rts = tmp;
    if (iface->GetUint32(node, "cts", &tmp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read cts fail", __func__);
        (void)OsalMemFree(esp32c3DriverData);
        uartHost->priv = NULL;
        return HDF_FAILURE;
    }
    esp32c3DriverData->attr.cts = tmp;
    if (iface->GetUint32(node, "fifoRxEn", &tmp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fifoRxEn fail", __func__);
        (void)OsalMemFree(esp32c3DriverData);
        uartHost->priv = NULL;
        return HDF_FAILURE;
    }
    esp32c3DriverData->attr.fifoRxEn = tmp;
    if (iface->GetUint32(node, "fifoTxEn", &tmp, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fifoTxEn fail", __func__);
        (void)OsalMemFree(esp32c3DriverData);
        uartHost->priv = NULL;
        return HDF_FAILURE;
    }
    esp32c3DriverData->attr.fifoTxEn = tmp;

    uartHost->priv = (void*)esp32c3DriverData;
    return HDF_SUCCESS;
}

static int32_t setConfig(struct UartHost *host)
{
    uart_config_t esp32cfg;
    struct UartDriverData* udd = NULL;

    if (NULL == host)
    {
        HDF_LOGE("%s: host is null", __func__);
        return HDF_FAILURE;
    }

    if (NULL == host->priv)
    {
        HDF_LOGE("%s: host->priv is null", __func__);
        return HDF_FAILURE;
    }

    if (host->num != UART_NUM_0 &&
        host->num != UART_NUM_1)
    {
        HDF_LOGE("%s: host->num is not %d neither %d.", __func__, UART_NUM_0, UART_NUM_1);
        return HDF_ERR_INVALID_PARAM;
    }

    udd = (struct UartDriverData*)host->priv;

    /**hdi -> esp32**/
    esp32cfg.baud_rate = udd->baudRate;
    esp32cfg.data_bits = udd->attr.dataBits;
    esp32cfg.parity    = parityHdi2Esp32(udd->attr.parity);
    esp32cfg.stop_bits = udd->attr.stopBits;
    esp32cfg.flow_ctrl = flowcontrolHdi2Esp32(udd->attr.rts, udd->attr.cts);
    if (ESP_OK != uart_param_config(host->num, &esp32cfg))
    {
        HDF_LOGE("%s: uart_param_config is failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

/*-------------------------------------------------------------*/
/* Hdi Interface */
/*-------------------------------------------------------------*/
static int32_t Esp32UartPollEvent(struct UartHost *host, void *filep, void *table)
{
    return HDF_ERR_NOT_SUPPORT;
}

static int32_t Esp32UartSetTransMode(struct UartHost *host, enum UartTransMode mode)
{
    return HDF_ERR_NOT_SUPPORT;
}

static int32_t Esp32UartGetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    struct UartDriverData *udd = NULL;

    if (host == NULL || host->priv == NULL || attribute == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    udd = (struct UartDriverData *)host->priv;
    // if (udd->state != UART_STATE_USEABLE) {
    //     return HDF_FAILURE;
    // }
    attribute->cts = udd->attr.cts;
    attribute->dataBits = udd->attr.dataBits;
    attribute->fifoRxEn = udd->attr.fifoRxEn;
    attribute->fifoTxEn = udd->attr.fifoTxEn;
    attribute->parity = udd->attr.parity;
    attribute->rts = udd->attr.rts;
    attribute->stopBits = udd->attr.stopBits;
    return HDF_SUCCESS;
}

static int32_t Esp32UartSetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    struct UartDriverData *udd = NULL;
    uart_config_t esp32cfg = {0};

    if (host == NULL || host->priv == NULL || attribute == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    udd = (struct UartDriverData *)host->priv;
    udd->attr.cts = attribute->cts;
    udd->attr.dataBits = attribute->dataBits;
    udd->attr.fifoRxEn = attribute->fifoRxEn;
    udd->attr.fifoTxEn = attribute->fifoTxEn;
    udd->attr.parity = attribute->parity;
    udd->attr.rts = attribute->rts;
    udd->attr.stopBits = attribute->stopBits;

    return setConfig(host);
}

static int32_t Esp32UartGetBaud(struct UartHost *host, uint32_t *baudRate)
{
    if (NULL == host)
    {
        HDF_LOGE("%s: invalid parameter: host is null.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (NULL == baudRate)
    {
        HDF_LOGE("%s: invalid parameter: baudRate is null.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (NULL == host->priv)
    {
        HDF_LOGE("%s: invalid parameter: host->priv is null.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (host->num >= UART_NUM_MAX)
    {
        HDF_LOGE("%s: invalid parameter: host->num(%d) is greater than %d.", __func__, host->num, UART_NUM_MAX);
        return HDF_ERR_INVALID_PARAM;
    }

    *baudRate = ((struct UartDriverData*)host->priv)->baudRate;

    return HDF_SUCCESS;
}

static int32_t Esp32UartSetBaud(struct UartHost *host, uint32_t baudRate)
{
    if (NULL == host)
    {
        HDF_LOGE("%s: invalid parameter: host is null.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (NULL == host->priv)
    {
        HDF_LOGE("%s: invalid parameter: host->priv is null.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (host->num >= UART_NUM_MAX)
    {
        HDF_LOGE("%s: invalid parameter: host->num(%d) is greater than %d.", __func__, host->num, UART_NUM_MAX);
        return HDF_ERR_INVALID_PARAM;
    }

    ((struct UartDriverData*)host->priv)->baudRate = baudRate;

    return uart_set_baudrate(host->num, baudRate);
}

static int32_t Esp32UartWrite(struct UartHost *host, uint8_t *data, uint32_t size)
{
    int writedSize = 0;

    if (NULL == host)
    {
        HDF_LOGE("%s: invalid parameter: host is null.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (NULL == data)
    {
        HDF_LOGE("%s: invalid parameter: data is null.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (host->num >= UART_NUM_MAX)
    {
        HDF_LOGE("%s: invalid parameter: host->num(%d) is greater than %d.", __func__, host->num, UART_NUM_MAX);
        return HDF_ERR_INVALID_PARAM;
    }

    writedSize = uart_write_bytes(host->num, (void*)data, size);
    if (-1 == writedSize)
    {
        HDF_LOGE("%s: uart_write_bytes() is failed. errno=%d.", __func__, host->num, writedSize);
        return HDF_FAILURE;
    }
    if (writedSize != (int)size)
    {
        HDF_LOGE("%s: uart_write_bytes() only writed %d bytes, "
                "but %d bytes need to be writed.", __func__, writedSize, size);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t Esp32UartRead(struct UartHost *host, uint8_t *data, uint32_t size)
{
    int readSize = 0;

    if (NULL == host)
    {
        HDF_LOGE("%s: invalid parameter: host is null.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (NULL == data)
    {
        HDF_LOGE("%s: invalid parameter: data is null.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (host->num >= UART_NUM_MAX)
    {
        HDF_LOGE("%s: invalid parameter: host->num(%d) is greater than %d.", __func__, host->num, UART_NUM_MAX);
        return HDF_ERR_INVALID_PARAM;
    }

    readSize = uart_read_bytes(host->num, (void*)data, size, 0U);
    if (0 > readSize)
    {
        HDF_LOGE("%s: uart_read_bytes() is failed. errno=%d.", __func__, readSize);
        return HDF_FAILURE;
    }

    return readSize;
}

static int32_t Esp32UartDeinit(struct UartHost *host)
{
    if (NULL == host)
    {
        HDF_LOGE("%s: host is null", __func__);
        return HDF_FAILURE;
    }

    if (host->num != UART_NUM_0 &&
        host->num != UART_NUM_1)
    {
        HDF_LOGE("%s: host->num is not %d neither %d.", __func__, UART_NUM_0, UART_NUM_1);
        return HDF_ERR_INVALID_PARAM;
    }

    return uart_driver_delete(host->num);
}

static int32_t Esp32UartInit(struct UartHost *host)
{
    esp_err_t espRet = ESP_OK;

    /*设置parameter*/
    if (HDF_SUCCESS != setConfig(host))
    {
        HDF_LOGE("%s: Set config failed.", __func__);
        return HDF_FAILURE;
    }

    if (NULL == host)
    {
        HDF_LOGE("%s: host is NULL.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (NULL == host->priv)
    {
        HDF_LOGE("%s: host->priv is NULL.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGE("%s: host->num = %d.", __func__, host->num);

    /*设置pin*/
    //参数的校验已经在 setconfig中做过了，因此此处不再校验
    espRet = uart_set_pin(host->num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if(ESP_OK != espRet)
    {
        HDF_LOGE("%s: uart_set_pin() is failed.errno=%d.", __func__, espRet);
        return HDF_FAILURE;
    }

    /*install device*/
    espRet = uart_driver_install(
        host->num,
        SOC_UART_FIFO_LEN+128,
        SOC_UART_FIFO_LEN+128,
        20, NULL,
        0);
    if(ESP_OK != espRet)
    {
        HDF_LOGE("%s: uart_driver_install() is failed.errno=%d.", __func__, espRet);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

struct UartHostMethod g_uart_method = {
    .Init = Esp32UartInit,
    .Deinit = Esp32UartDeinit,
    .Read = Esp32UartRead,
    .Write = Esp32UartWrite,
    .SetBaud = Esp32UartSetBaud,
    .GetBaud = Esp32UartGetBaud,
    .SetAttribute = Esp32UartSetAttribute,
    .GetAttribute = Esp32UartGetAttribute,
    .SetTransMode = Esp32UartSetTransMode,
    .pollEvent = Esp32UartPollEvent,
};

/*-------------------------------------------------------------*/
/* Init and DeInit Function */
/*-------------------------------------------------------------*/
static int32_t Esp32UartDeviceBind(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: entry", __func__);
    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return (UartHostCreate(device) == NULL) ? HDF_FAILURE : HDF_SUCCESS;
}

int32_t Esp32UartDeviceInit(struct HdfDeviceObject *device)
{
    struct UartHost *host = NULL;

    HDF_LOGI("%s: entry", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    host = UartHostFromDevice(device);
    if (host == NULL) {
        HDF_LOGE("%s: host is null", __func__);
        return HDF_FAILURE;
    }

    /*从hcs解析必要属性*/
    if (NULL == device->property)
    {
        HDF_LOGE("%s: device->property is null", __func__);
        return HDF_FAILURE;
    }
    if (HDF_SUCCESS != getConfigFromHcs(host, device->property))
    {
        HDF_LOGE("%s: getConfigFromHcs() is failed", __func__);
        return HDF_FAILURE;
    }

    /*给Method赋值,框架才能调用到对应的方法*/
    host->method = &g_uart_method;

    /*增加设备节点*/
    // UartAddDev(host);

    HDF_LOGI("%s: dev service:%s init success!", __func__, HdfDeviceGetServiceName(device));
    return HDF_SUCCESS;
}

void Esp32UartDeviceRelease(struct HdfDeviceObject *device)
{
    struct UartHost *host = NULL;

    HDF_LOGI("%s: entry", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
    host = UartHostFromDevice(device);
    if (host == NULL) {
        HDF_LOGE("%s: host is null", __func__);
        return;
    }

    /*删除设备节点*/
    // UartRemoveDev(host);

    if (NULL != host->priv)
    {
        OsalMemFree(host->priv);
    }
    
    UartHostDestroy(host);
}

struct HdfDriverEntry g_uart_driver_entry = {
    .moduleVersion = 1,
    .Bind = Esp32UartDeviceBind,
    .Init = Esp32UartDeviceInit,
    .Release = Esp32UartDeviceRelease,
    .moduleName = "esp32_uart_driver",
};

HDF_INIT(g_uart_driver_entry);
