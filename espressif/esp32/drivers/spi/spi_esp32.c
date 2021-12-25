#include "spi_esp32.h"
#include "device_resource_if.h"
#include "dmac_core.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_irq.h"
#include "osal_mem.h"
#include "osal_sem.h"
#include "osal_time.h"
#include "spi_core.h"
#include "spi_types.h"
#include "spi_common.h"
#include "spi_master.h"
#include "spi_core.h"
#include "spi_if.h"

#define HDF_LOG_TAG spi_esp32

#define ESP32_SPI_BUS_AMOUNT 3;

/*-------------------------------------------------------------*/
/* Internal structure */
/*-------------------------------------------------------------*/
struct Esp32 {
    struct SpiCntlr *cntlr;
    struct DListHead deviceList;
    struct OsalSem sem;
    uint32_t irqNum;
    uint32_t busNum;
    uint32_t numCs;
    uint32_t curCs;
    uint8_t command_bits;           ///< Default amount of bits in command phase (0-16), used when ``SPI_TRANS_VARIABLE_CMD`` is not used, otherwise ignored.
    uint8_t address_bits;           ///< Default amount of bits in address phase (0-64), used when ``SPI_TRANS_VARIABLE_ADDR`` is not used, otherwise ignored.
    uint8_t dummy_bits;             ///< Amount of dummy bits to insert between address and data phase
    uint8_t mode;
    uint16_t duty_cycle_pos;
    int clock_speed_hz;             ///< Clock speed, divisors of 80MHz, in Hz. See ``SPI_MASTER_FREQ_*``.
    int spics_io_num;
    int queue_size;
    int mosi_io_num;
    int miso_io_num;
    int sclk_io_num;
    int quadwp_io_num;
    int quadhd_io_num;
};

/*-------------------------------------------------------------*/
/* Private Interface */
/*-------------------------------------------------------------*/
static struct SpiDev* getSpiDeviceByCsNum(struct Esp32* esp32, uint32_t cs)
{
    struct SpiDev *dev = NULL;
    struct SpiDev *tmpDev = NULL;

    if (esp32 == NULL || esp32->numCs <= cs) {
        HDF_LOGE("%s: fail : esp32 == NULL || esp32->numCs <= cs", __func__);
        return NULL;
    }
    HDF_LOGE("success : esp32->numCs > cs");
    DLIST_FOR_EACH_ENTRY_SAFE(dev, tmpDev, &(esp32->deviceList), struct SpiDev, list) {
        if (dev->csNum  == cs) {
            break;
        }
    }
    
    return dev;
}

int32_t Esp32SpiClose(struct SpiCntlr *cntlr)
{
    (void)cntlr;
    return HDF_SUCCESS;
}

int32_t Esp32SpiOpen(struct SpiCntlr *cntlr)
{
    (void)cntlr;
    return HDF_SUCCESS;
}

static int32_t Esp32SpiGetCfg(struct SpiCntlr *cntlr, struct SpiCfg *cfg)
{
    struct SpiDev* spiDevice = NULL;
    struct Esp32* esp32 = NULL;

    if (cntlr == NULL || cntlr->priv == NULL || cfg == NULL) {
        HDF_LOGE("%s: cntlr priv or cfg is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    
    esp32 = (struct Esp32 *)cntlr->priv;
    spiDevice = getSpiDeviceByCsNum(esp32, cntlr->curCs);
    if (spiDevice == NULL) {
        HDF_LOGE("%s: getSpiDeviceByCsNum() is failed!", __func__);
        return HDF_FAILURE;
    }

    cfg->maxSpeedHz     = spiDevice->cfg.maxSpeedHz;
    cfg->mode           = spiDevice->cfg.mode;
    cfg->transferMode   = spiDevice->cfg.transferMode;
    cfg->bitsPerWord    = spiDevice->cfg.bitsPerWord;

    return HDF_SUCCESS;
}

static int32_t Esp32SpiSetCfg(struct SpiCntlr *cntlr, struct SpiCfg *cfg)
{
    struct SpiDev* spiDevice = NULL;
    struct Esp32* esp32 = NULL;

    if (cntlr == NULL || cntlr->priv == NULL || cfg == NULL) {
        HDF_LOGE("%s: cntlr priv or cfg is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    
    esp32 = (struct Esp32 *)cntlr->priv;
    spiDevice = getSpiDeviceByCsNum(esp32, cntlr->curCs);
    if (spiDevice == NULL) {
        HDF_LOGE("%s: getSpiDeviceByCsNum() is failed!", __func__);
        return HDF_FAILURE;
    }

    spiDevice->cfg.maxSpeedHz   = cfg->maxSpeedHz;
    spiDevice->cfg.mode         = cfg->mode;
    spiDevice->cfg.transferMode = cfg->transferMode;
    spiDevice->cfg.bitsPerWord  = cfg->bitsPerWord;

    return HDF_SUCCESS; 
}

static int32_t SpiGetBaseCfgFromHcs(struct Esp32 *esp32, const struct DeviceResourceNode *node)
{
    struct DeviceResourceIface *iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    if (iface == NULL || iface->GetUint8 == NULL || iface->GetUint16 == NULL || iface->GetUint32 == NULL) {
        HDF_LOGE("%s: face is invalid", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint32(node, "busNum", &esp32->busNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read busNum fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint32(node, "numCs", &esp32->numCs, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read numCs fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint8(node, "command_bits", &esp32->command_bits, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read command_bits fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint8(node, "address_bits", &esp32->address_bits, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read address_bits fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint8(node, "dummy_bits", &esp32->dummy_bits, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read dummy_bits fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint8(node, "mode", &esp32->mode, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read mode fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint32(node, "clock_speed_hz", &esp32->clock_speed_hz, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read clock_speed_hz fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint16(node, "duty_cycle_pos", &esp32->duty_cycle_pos, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read duty_cycle_pos fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint32(node, "spics_io_num", &esp32->spics_io_num, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read spics_io_num fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint32(node, "queue_size", &esp32->queue_size, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read queue_size fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint32(node, "mosi_io_num", &esp32->mosi_io_num, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read mosi_io_num fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint32(node, "miso_io_num", &esp32->miso_io_num, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read miso_io_num fail", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint32(node, "sclk_io_num", &esp32->sclk_io_num, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read sclk_io_num fail", __func__);
        return HDF_FAILURE;
    }
    
    return 0;
}

static int32_t Esp32SpiTransfer(struct SpiCntlr *cntlr, struct SpiMsg *msg, uint32_t count) 
{
    esp_err_t espRet;
    spi_dma_chan_t dmaChannel;
    struct SpiDev* spiDev = NULL;
    struct Esp32 *esp32 = NULL;
    int32_t ret;
    spi_device_handle_t handle;

    if (msg == NULL || cntlr == NULL) {
        HDF_LOGE("%s: msg == NULL || cntlr == NULL !!!!", __func__);
        return HDF_FAILURE;
    }

    if (msg->rbuf == NULL && msg->wbuf == NULL) {
        HDF_LOGE("%s: msg->rbuf == NULL || msg->wbuf == NULL !!!!", __func__);
        return HDF_FAILURE;
    }

    esp32 = (struct Esp32 *)OsalMemCalloc(sizeof(*esp32));
    if (esp32 == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        return HDF_FAILURE;
    }

    ret = SpiGetBaseCfgFromHcs(esp32, cntlr->device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SpiGetRegCfgFromHcs error", __func__);
        OsalMemFree(esp32);
        return HDF_FAILURE;
    }

    spi_device_interface_config_t espDevCfg = {
        .command_bits = esp32->command_bits,
        .address_bits = esp32->address_bits,
        .dummy_bits = esp32->dummy_bits,
        .clock_speed_hz = esp32->clock_speed_hz,
        .duty_cycle_pos = esp32->duty_cycle_pos,
        .mode = esp32->mode,
        .spics_io_num = esp32->spics_io_num,
        .queue_size = esp32->queue_size
    };

    spi_bus_config_t buscfg = {
        .mosi_io_num = esp32->mosi_io_num,
        .miso_io_num = esp32->miso_io_num,
        .sclk_io_num = esp32->sclk_io_num,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    struct spi_transaction_t espTransaction = {
        .length =  msg->len,
        .rxlength = msg->len,
        .tx_buffer = msg->wbuf,
        .rx_buffer = msg->rbuf
    };
    
    if (NULL == cntlr)
    {
        OsalMemFree(esp32);
        HDF_LOGE("%s: cntlr is NLL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (NULL == cntlr->priv)
    {
        OsalMemFree(esp32);
        HDF_LOGE("%s: cntlr->priv is NLL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    esp32 = (struct Esp32*)cntlr->priv;

    spiDev = getSpiDeviceByCsNum(esp32, cntlr->curCs);
    if (NULL == spiDev)
    {
        OsalMemFree(esp32);
        HDF_LOGE("%s: Device(%d) is not be openned!", __func__, cntlr->curCs);
        return HDF_FAILURE;
    }

    if (SPI_DMA_TRANSFER == spiDev->cfg.transferMode)
    {
        dmaChannel = SPI_DMA_CH_AUTO;
    }
    else
    {
        dmaChannel = SPI_DMA_DISABLED;
    }

    espRet = spi_bus_initialize(esp32->busNum, &buscfg, dmaChannel);
    if (ESP_OK != espRet)
    {
        OsalMemFree(esp32);
        HDF_LOGE("%s: spi_bus_initialize() is failed! errno=%d", __func__, espRet);
        return HDF_FAILURE;
    }

    for (uint32_t i = 0; i < count; i++)
    {
        espRet = spi_bus_add_device(esp32->busNum, &espDevCfg, &handle);
        if (ESP_OK != espRet)
        {
            OsalMemFree(esp32);
            HDF_LOGE("%s: spi_bus_add_device() is failed! errno=%d", __func__, espRet);
            return HDF_FAILURE;
        }

        espRet = spi_device_polling_transmit(handle, &espTransaction);
        if (ESP_OK != espRet)
        {
            OsalMemFree(esp32);
            HDF_LOGE("%s: spi_device_transmit() is failed! errno=%d", __func__, espRet);
            return HDF_FAILURE;
        }

        espRet = spi_bus_remove_device(handle);
        if (ESP_OK != espRet)
        {
            OsalMemFree(esp32);
            HDF_LOGE("%s: spi_bus_remove_device() is failed! errno=%d", __func__, espRet);
            return HDF_FAILURE;
        }
    }

    espRet = spi_bus_free(esp32->busNum);
    if (ESP_OK != espRet)
    {
        OsalMemFree(esp32);
        HDF_LOGE("%s: spi_bus_free() is failed! errno=%d", __func__, espRet);
        return HDF_FAILURE;
    }

    return 0;
}

struct SpiCntlrMethod g_SpiMethod = {
    .Transfer = Esp32SpiTransfer,
    .SetCfg = Esp32SpiSetCfg,
    .GetCfg = Esp32SpiGetCfg,
    .Open = Esp32SpiOpen,
    .Close = Esp32SpiClose,
};

/*-------------------------------------------------------------*/
/* Init and DeInit Function */
/*-------------------------------------------------------------*/
static int32_t Esp32SpiDeviceBind(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: entry", __func__);
    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return (SpiCntlrCreate(device) == NULL) ? HDF_FAILURE : HDF_SUCCESS;
}

static int32_t Esp32SpiDeviceInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    esp_err_t espRet=ESP_FAIL;
    struct SpiCntlr *cntlr = NULL;
    struct Esp32 *esp32 = NULL;

    HDF_LOGI("%s: entry", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: ptr is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    cntlr = SpiCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return HDF_FAILURE;
    }

    if (device->property == NULL) {
        HDF_LOGE("%s: property is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    esp32 = (struct Esp32 *)OsalMemCalloc(sizeof(*esp32));
    if (esp32 == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        return HDF_FAILURE;
    }

    ret = SpiGetBaseCfgFromHcs(esp32, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SpiGetBaseCfgFromHcs error", __func__);
        OsalMemFree(esp32);
        return HDF_FAILURE;
    }

    DListHeadInit(&esp32->deviceList);
    esp32->cntlr = cntlr;
    cntlr->priv = esp32;
    cntlr->busNum = esp32->busNum;
    cntlr->method = &g_SpiMethod;
    cntlr->numCs = esp32->numCs;
    HDF_LOGI("%s: end", __func__);
    return HDF_SUCCESS;
}

static void Esp32SpiDeviceRelease(struct HdfDeviceObject *device)
{
    struct SpiCntlr *cntlr = NULL;
    struct Esp32 *esp32 = NULL;

    HDF_LOGI("%s: entry", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
    cntlr = SpiCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("%s: cntlr is null", __func__);
        return;
    }

    esp32 =  (struct Esp32 *)cntlr->priv;
    if (NULL != esp32)
    {
        OsalMemFree(esp32);
        cntlr->priv = NULL;
    }
    
    SpiCntlrDestroy(cntlr);
}

struct HdfDriverEntry g_spiDriverEntry = {
    .moduleVersion = 1,
    .Bind = Esp32SpiDeviceBind,
    .Init = Esp32SpiDeviceInit,
    .Release = Esp32SpiDeviceRelease,
    .moduleName = "esp32_spi_driver",
};

HDF_INIT(g_spiDriverEntry);