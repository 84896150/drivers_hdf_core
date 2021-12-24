#include <securec.h>
#include "osal_mem.h"
#include "hdf_device_desc.h"
#include "hdf_touch.h"
#include "input_i2c_ops.h"
#include "touch_ft5336.h"
#include "hdf_log.h"
#include <linux/khadas-tca6408.h>
#include "input_config.h"
#include "hdf_input_device_manager.h"
#include "hdf_types.h"
#include "device_resource_if.h"
#include "gpio_if.h"
#include "hdf_device_desc.h"
#include "osal_io.h"
#include "event_hub.h"

#define TP_SWAP(x, y)  do {\
                typeof(x) z = x;\
                x = y;\
                y = z; \
} while (0)

static int32_t ChipInit(ChipDevice *device)
{
    return HDF_SUCCESS;
}

static int32_t ChipResume(ChipDevice *device)
{
    return HDF_SUCCESS;
}

static int32_t ChipSuspend(ChipDevice *device)
{
    return HDF_SUCCESS;
}

static int Ft5336_Identify(ChipDevice *device, const InputI2cClient *i2cClient)
{
    u8 rdbuf[EDT_NAME_LEN];
    char *p = NULL;
    int error = 0;
    char fw_version[EDT_NAME_LEN];

    memset(rdbuf, 0, sizeof(rdbuf));

    error = InputI2cRead(i2cClient, "\xbb", 1, rdbuf, EDT_NAME_LEN - 1);
    if (error){
        return error;
    }
    /* Probe content for something consistent.
     * M06 starts with a response byte.
     * M09/Generic does not provide model number information.
     */
    if (!(strncasecmp(rdbuf + 1, "EP0", 3))) {
        device->chipCfg->chipVersion = EDT_M06;
        HDF_LOGI("%s: version = EDT_M06", __func__);

        rdbuf[EDT_NAME_LEN - 1] = '\0';
        if (rdbuf[EDT_NAME_LEN - 2] == '$'){
            rdbuf[EDT_NAME_LEN - 2] = '\0';
        }
        p = strchr(rdbuf, '*');
        if (p){
            *p++ = '\0';
        }
        strlcpy(fw_version, p ? p : "", EDT_NAME_LEN);
    } else {
        /* since there are only two versions around (M06, M09) */
        device->chipCfg->chipVersion = EDT_M09;
        HDF_LOGI("%s: version = EDT_M09;", __func__);

        error = InputI2cRead(i2cClient, "\xA6", 1, rdbuf, 2);
        if (error){
            return error;
        }
        strlcpy(fw_version, rdbuf, 2);
    }

    HDF_LOGI("%s: fw_version = %s", __func__, fw_version);
    return 0;
}

static int32_t ChipDetect(ChipDevice *device)
{
    uint8_t regAddr;
    uint8_t regValue = 0;
    int32_t ret;

    InputI2cClient *i2cClient = &device->driver->i2cClient;

    regAddr = WORK_REGISTER_REPORT_RATE;
    ret = InputI2cWrite(i2cClient, &regAddr, 1);
    CHIP_CHECK_RETURN(ret);
    ret = InputI2cRead(i2cClient, &regAddr, 1, &regValue, 1);
    CHIP_CHECK_RETURN(ret);
    HDF_LOGI("%s: Report rate is %u * 10", __func__, regValue);

    Ft5336_Identify(device, i2cClient);

    (void)ChipInit(device);
    (void)ChipResume(device);
    (void)ChipSuspend(device);

    return HDF_SUCCESS;
}

static int32_t ParsePointData(ChipDevice *device, FrameData *frame, uint8_t *buf, uint8_t pointNum, int32_t version)
{
    int32_t i;
    int32_t x,y;
    int32_t offset, tplen, crclen;
    int32_t type;
    switch (version) {
    case EDT_M06:
        offset = 5; /* where the actual touch data starts */
        tplen = 4;  /* data comes in so called frames */
        crclen = 1; /* length of the crc data */
        break;

    case EDT_M09:
        offset = 3;
        tplen = 6;
        crclen = 0;
        break;

    default:
        return HDF_FAILURE;
    }

    for (i = 0; i < pointNum; i++) {
        u8 *rdbuf = &buf[i * tplen + offset];
        
        type = rdbuf[0] >> 6;
        /* ignore Reserved events */
        if (type == TOUCH_EVENT_RESERVED){
            continue;
        }

        /* M06 sometimes sends bogus coordinates in TOUCH_DOWN */
        if (version == EDT_M06 && type == TOUCH_EVENT_DOWN){
            continue;
        }

        if (type == TOUCH_EVENT_UP){
            frame->definedEvent = type;
            continue;
        }

        x = ((rdbuf[0] << 8) | rdbuf[1]) & 0x0fff;
        y = ((rdbuf[2] << 8) | rdbuf[3]) & 0x0fff;

#if TP_CHANGE_X2Y
        TP_SWAP(x, y);
#endif

#if TP_Y_REVERSE_ENABLE
        y = device->boardCfg->attr.resolutionY - y;
#endif

#if TP_X_REVERSE_ENABLE
        x = device->boardCfg->attr.resolutionX - x;
#endif
//    HDF_LOGI("%s : x = %u , y = %u ", __func__, x, y);

        frame->fingers[i].x = x;
        frame->fingers[i].y = y;
        frame->fingers[i].trackId = (rdbuf[2] >> 4) & 0x0f;
        frame->fingers[i].status = rdbuf[0] >> 6;
        frame->fingers[i].valid = true;
        frame->definedEvent = frame->fingers[i].status;

        if ((frame->fingers[i].status == TOUCH_DOWN || frame->fingers[i].status == TOUCH_CONTACT) && (pointNum == 0)) {
            HDF_LOGE("%s: abnormal event data from driver chip", __func__);
            return HDF_FAILURE;
        }
    }
   
    return HDF_SUCCESS;
}

static int32_t ChipDataHandle(ChipDevice *device)
{
    uint8_t reg = 0x0;
    uint8_t pointNum;
    uint8_t buf[POINT_BUFFER_LEN_M09] = {0};
    int32_t ret = 0;

    InputI2cClient *i2cClient = &device->driver->i2cClient;
    FrameData *frame = &device->driver->frameData;

    switch (device->chipCfg->chipVersion) {
    case EDT_M06:
        reg = 0xf9; /* tell the controller to send touch data */
        ret = InputI2cRead(i2cClient, &reg, 1, buf, POINT_BUFFER_LEN_M06);
        break;

    case EDT_M09:
        ret = InputI2cRead(i2cClient, &reg, 1, buf, POINT_BUFFER_LEN_M09);
        break;

    default:
        return HDF_FAILURE;
    }
    CHIP_CHECK_RETURN(ret);

    OsalMutexLock(&device->driver->mutex);
    (void)memset_s(frame, sizeof(FrameData), 0, sizeof(FrameData));
    
    pointNum = buf[2] & GT_FINGER_NUM_MASK;
    if (pointNum > MAX_POINT) {
        OsalMutexUnlock(&device->driver->mutex);
        return HDF_FAILURE;
    }

    if(pointNum == 0){
        frame->realPointNum = 0;
        frame->definedEvent = TOUCH_EVENT_UP;
        OsalMutexUnlock(&device->driver->mutex);
        return HDF_SUCCESS;
    }
    
    frame->realPointNum = pointNum;

    ret = ParsePointData(device, frame, buf, pointNum, device->chipCfg->chipVersion);
    if (ret != HDF_SUCCESS) {
        OsalMutexUnlock(&device->driver->mutex);
        return ret;
    }
    OsalMutexUnlock(&device->driver->mutex);

    return HDF_SUCCESS;
}

static void SetAbility(ChipDevice *device)
{
    device->driver->inputDev->abilitySet.devProp[0] = SET_BIT(INPUT_PROP_DIRECT);
    device->driver->inputDev->abilitySet.eventType[0] = SET_BIT(EV_SYN) |
        SET_BIT(EV_KEY) | SET_BIT(EV_ABS);
    device->driver->inputDev->abilitySet.absCode[0] = SET_BIT(ABS_X) | SET_BIT(ABS_Y);
    device->driver->inputDev->abilitySet.absCode[1] = SET_BIT(ABS_MT_POSITION_X) |
        SET_BIT(ABS_MT_POSITION_Y) | SET_BIT(ABS_MT_TRACKING_ID);
    device->driver->inputDev->abilitySet.keyCode[3] = SET_BIT(KEY_UP) | SET_BIT(KEY_DOWN);
    device->driver->inputDev->attrSet.axisInfo[ABS_X].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_X].max = device->boardCfg->attr.resolutionX - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_X].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_Y].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_Y].max = device->boardCfg->attr.resolutionY - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_Y].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_X].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_X].max = device->boardCfg->attr.resolutionX - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_X].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_Y].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_Y].max = device->boardCfg->attr.resolutionY - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_Y].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_TRACKING_ID].max = MAX_POINT;
}

static struct TouchChipOps g_ft5336ChipOps = {
    .Init = ChipInit,
    .Detect = ChipDetect,
    .Resume = ChipResume,
    .Suspend = ChipSuspend,
    .DataHandle = ChipDataHandle,
    .SetAbility = SetAbility,
};

static void FreeChipConfig(TouchChipCfg *config)
{
    if (config->pwrSeq.pwrOn.buf != NULL) {
        OsalMemFree(config->pwrSeq.pwrOn.buf);
    }

    if (config->pwrSeq.pwrOff.buf != NULL) {
        OsalMemFree(config->pwrSeq.pwrOff.buf);
    }

    if (config->pwrSeq.resume.buf != NULL) {
        OsalMemFree(config->pwrSeq.resume.buf);
    }

    if (config->pwrSeq.suspend.buf != NULL) {
        OsalMemFree(config->pwrSeq.suspend.buf);
    }
    
    OsalMemFree(config);
}

static TouchChipCfg *ChipConfigInstance(struct HdfDeviceObject *device)
{
    TouchChipCfg *chipCfg = (TouchChipCfg *)OsalMemAlloc(sizeof(TouchChipCfg));
    if (chipCfg == NULL) {
        HDF_LOGE("%s: instance chip config failed", __func__);
        return NULL;
    }
    (void)memset_s(chipCfg, sizeof(TouchChipCfg), 0, sizeof(TouchChipCfg));

    if (ParseTouchChipConfig(device->property, chipCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: parse chip config failed", __func__);
        OsalMemFree(chipCfg);
        chipCfg = NULL;
    }
    return chipCfg;
}

static ChipDevice *ChipDeviceInstance(void)
{
    ChipDevice *chipDev = (ChipDevice *)OsalMemAlloc(sizeof(ChipDevice));
    if (chipDev == NULL) {
        HDF_LOGE("%s: instance chip device failed", __func__);
        return NULL;
    }
    (void)memset_s(chipDev, sizeof(ChipDevice), 0, sizeof(ChipDevice));
    return chipDev;
}

extern int tca6408_output_set_value(u8 value, u8 mask);
static int32_t EdtFocalChipInit(struct HdfDeviceObject *device)
{
    TouchChipCfg *chipCfg = NULL;
    ChipDevice *chipDev = NULL;
    if (device == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    chipCfg = ChipConfigInstance(device);   //解析input_config中的chipConfig
    if (chipCfg == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    chipDev = ChipDeviceInstance();
    if (chipDev == NULL) {
        FreeChipConfig(chipCfg);
        return HDF_FAILURE;
    }

    tca6408_output_set_value(TCA_TP_RST_MASK, TCA_TP_RST_MASK);
    usleep_range(5000, 6000);
    tca6408_output_set_value(TCA_TP_RSTT_MASK,TCA_TP_RST_MASK);
    msleep(300);
    tca6408_output_set_value(TCA_TP_RST_MASK,TCA_TP_RST_MASK);

    chipDev->chipCfg = chipCfg;
    chipDev->ops = &g_ft5336ChipOps;
    chipDev->chipName = chipCfg->chipName;
    chipDev->vendorName = chipCfg->vendorName;

    if (RegisterChipDevice(chipDev) != HDF_SUCCESS) {
        HDF_LOGI("%s: fail", __func__);
        OsalMemFree(chipDev);
    }

#if TP_CHANGE_X2Y
    uint32_t resolution = chipDev->boardCfg->attr.resolutionX;
    chipDev->boardCfg->attr.resolutionX = chipDev->boardCfg->attr.resolutionY;
    chipDev->boardCfg->attr.resolutionY = resolution;
#endif

    HDF_LOGI("%s: init succ, chipName = %s", __func__, chipCfg->chipName);
    return HDF_SUCCESS;

EXIT1:
    OsalMemFree(chipDev);
EXIT:
    FreeChipConfig(chipCfg);
    return HDF_FAILURE;
}

struct HdfDriverEntry g_edttouchfocalchipentry = {
    .moduleVersion = 1,
    .moduleName = "EDT_TOUCH_FT5336",
    .Init = EdtFocalChipInit,
};

HDF_INIT(g_edttouchfocalchipentry);
