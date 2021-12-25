#include "securec.h"
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "i2c_core.h"
#include "osal_mem.h"
#include "osal_spinlock.h"
#include "osal_time.h"
#include "plat_log.h"
#include "i2c.h"
#include "i2c_hal.h"

struct Esp32I2cCntlr {
    struct I2cCntlr cntlr;
    OsalSpinlock spin;
    int16_t bus;
    int mode;
    int sda_io_num;
    int scl_io_num;
    int sda_pullup_en;
    int scl_pullup_en;
    uint32_t clk_speed;
};

static int32_t Esp32I2cReadDrs(struct Esp32I2cCntlr *esp32, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "mode", &esp32->mode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read mode fail!", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "sda_io_num", &esp32->sda_io_num, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read sda_io_num fail!", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "scl_io_num", &esp32->scl_io_num, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read scl_io_num fail!", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "sda_pullup_en", &esp32->sda_pullup_en, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read sda_pullup_en fail!", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "scl_pullup_en", &esp32->scl_pullup_en, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read scl_pullup_en fail!", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "clk_speed", &esp32->clk_speed, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read clk_speed fail!", __func__);
        return ret;
    }

    ret = drsOps->GetUint16(node, "bus", (uint16_t *)&esp32->bus, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bus fail!", __func__);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t Esp32I2cTransfer(struct I2cCntlr *cntlr, struct I2cMsg *msgs, int16_t count)
{
    uint32_t ticks_to_wait = 100;/*Maximum waiting for ticking*/
    uint32_t i = 0;
    uint32_t ret = 0;

    if (cntlr == NULL || cntlr->priv == NULL) {
        HDF_LOGE("Esp32I2cTransfer: cntlr lor Esp32is null!");
        return HDF_ERR_INVALID_OBJECT;
    }

    for (i = 0; i < count; i++) {
        //read data
        if (msgs[i].flags == I2C_FLAG_READ) {
            if (i2c_master_read_from_device(0, msgs[i].addr,
                                      msgs[i].buf, msgs[i].len,
                                      ticks_to_wait) == 0) {
                ret++;
            }
        }
        //write data
        if (msgs[i].flags == 0) {
            if (i2c_master_write_to_device(0, msgs[i].addr,
                                     msgs[i].buf, msgs[i].len,
                                     ticks_to_wait) == 0) {
                ret++;
            }
        }
    }

    return ret;
}

static int32_t Esp32I2cLock(struct I2cCntlr *cntlr)
{
    struct Esp32I2cCntlr *esp32 = (struct Esp32I2cCntlr *)cntlr;
    
    if (esp32 != NULL) {
        return OsalSpinLock(&esp32->spin);
    }

    return HDF_SUCCESS;
}

static void Esp32I2cUnlock(struct I2cCntlr *cntlr) 
{
    struct Esp32I2cCntlr *esp32 = (struct Esp32I2cCntlr *)cntlr;

    if (esp32 != NULL) {
        (void)OsalSpinUnlock(&esp32->spin);
    }
}

static const struct I2cMethod g_I2cMethod = {
    .transfer = Esp32I2cTransfer,
};

static const struct I2cLockMethod g_lockOps = {
    .lock = Esp32I2cLock,
    .unlock = Esp32I2cUnlock,
};

/*-------------------------------------------------------------*/
/* Init and DeInit Function */
/*-------------------------------------------------------------*/
static int32_t Esp32I2cBind(struct HdfDeviceObject *device)
{
    HDF_LOGE("Esp32I2cBind start!!!!!!!!!!");
    (void)device;
    return HDF_SUCCESS;
}

static int32_t Esp32I2cParseAndInit(struct HdfDeviceObject *device, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct Esp32I2cCntlr *esp32 = NULL;
    (void)device;

    if (device == NULL || node == NULL) {
        HDF_LOGE("%s: device or node null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    esp32 = (struct Esp32I2cCntlr *)OsalMemCalloc(sizeof(*esp32));
    if (esp32 == NULL) {
        HDF_LOGE("%s: malloc esp32 fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = Esp32I2cReadDrs(esp32, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read drs fail! ret:%d", __func__, ret);
        return ret;
    }

    esp32->cntlr.priv = (void *)node;
    esp32->cntlr.busId = esp32->bus;
    esp32->cntlr.ops = &g_I2cMethod;
    esp32->cntlr.lockOps = &g_lockOps;
    (void)OsalSpinInit(&esp32->spin);
    ret = I2cCntlrAdd(&esp32->cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: add i2c controller fail:%d!", __func__, ret);
        (void)OsalSpinDestroy(&esp32->spin);
    }

    return ret;
}

static int32_t Esp32I2cInit(struct HdfDeviceObject *device)
{
    int32_t ret = HDF_SUCCESS;
    const struct DeviceResourceNode *childNode = NULL;
    int i2c_master_port = 0;
    struct Esp32I2cCntlr *esp32 = NULL;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is NULL", __func__);
        return;
    }

    esp32 = (struct Esp32I2cCntlr *)OsalMemCalloc(sizeof(*esp32));
    if (esp32 == NULL) {
        HDF_LOGE("%s: malloc esp32 fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode)
    {
        ret = Esp32I2cReadDrs(esp32, childNode);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: read drs fail! ret:%d", __func__, ret);
            return ret;
        }

        i2c_config_t conf = {
            .mode = esp32->mode,
            .sda_io_num = esp32->sda_io_num,
            .scl_io_num = esp32->scl_io_num,
            .sda_pullup_en = esp32->sda_pullup_en,
            .scl_pullup_en = esp32->scl_pullup_en,
            .master.clk_speed = esp32->clk_speed,
        };

        HDF_LOGE("%s: Enter", __func__);
        if (device == NULL || device->property == NULL) {
            HDF_LOGE("%s: device or property is NULL", __func__);
            return HDF_ERR_INVALID_OBJECT;
        }

        i2c_param_config(i2c_master_port, &conf);

        ret = i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
        if (0 != ret) {
            return HDF_ERR_INVALID_OBJECT;
        }
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        ret = Esp32I2cParseAndInit(device, childNode);
        if (ret != HDF_SUCCESS) {
            break;
        }
    }
    HDF_LOGE("Esp32I2cInit end!!!!!!!!!!");

    return ret;
}

static void Esp32I2cRemoveByNode(const struct DeviceResourceNode *node)
{
    int32_t ret;
    int16_t bus;
    struct I2cCntlr *cntlr = NULL;
    struct Esp32I2cCntlr *esp32 = NULL;
    struct DeviceResourceIface *drsOps = NULL;

    if (node == NULL) {
        HDF_LOGE("%s: node null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return;
    }

    ret = drsOps->GetUint16(node, "bus", (uint16_t *)&bus, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bus fail!", __func__);
        return;
    }

    cntlr = I2cCntlrGet(bus);
    if (cntlr != NULL && cntlr->priv == node) {
        I2cCntlrPut(cntlr);
        I2cCntlrRemove(cntlr);
        esp32 = (struct Esp32I2cCntlr *)cntlr;
        (void)OsalSpinDestroy(&esp32->spin);
        OsalMemFree(esp32);
    }
    
    return;
}

static void Esp32I2cRelease(struct HdfDeviceObject *device)
{
    const struct DeviceResourceNode *childNode = NULL;

    HDF_LOGI("%s: enter", __func__);

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is NULL", __func__);
        return;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        Esp32I2cRemoveByNode(childNode);
    }
}

struct HdfDriverEntry g_i2cDriverEntry = {
    .moduleVersion = 1,
    .Bind = Esp32I2cBind,
    .Init = Esp32I2cInit,
    .Release = Esp32I2cRelease,
    .moduleName = "esp32_i2c_driver",
};
HDF_INIT(g_i2cDriverEntry);
