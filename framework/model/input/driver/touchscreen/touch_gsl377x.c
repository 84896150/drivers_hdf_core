/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "touch_gsl377x.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "hdf_touch.h"
#include "input_i2c_ops.h"
#include "osal_mem.h"
#include <securec.h>

#define HDF_LOG_TAG GSL377X
#define MAX_POINT 10
struct OsalMutex gsl_bus_mutex;

static bool gsl_dat_verify(u8 *val, u16 len)
{
    u16 dat_len = 0;
    u16 dat_vfy = 0;
    u16 ref_vfy = 0;
    u32 i;

    if (val == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return false;
    }
    if (len < sizeof(u16) + sizeof(u16)) {
        return false;
    }
    memcpy_s(&dat_len, sizeof(dat_len), val, sizeof(dat_len));
    if (dat_len != len) {
        HDF_LOGE("%s: len error: %d vs %d", __func__, dat_len, len);
        return false;
    }
    for (i = 0; i < len - sizeof(u16); i++) {
        ref_vfy += val[i];
    }
    ref_vfy = ~ref_vfy;
    memcpy_s(&dat_vfy, sizeof(dat_vfy), &(val[len - sizeof(u16)]), sizeof(dat_vfy));
    if (dat_vfy == ref_vfy) {
        return true;
    } else {
        HDF_LOGE("%s: verify error: %d vs %d", __func__, dat_vfy, ref_vfy);
        return false;
    }
}
static int gsl_cmd_read(InputI2cClient *client, uint16_t cmd, uint8_t *val, uint32_t len)
{
    int ret = 0;
    uint16_t frm_len = 0;
    u8 dat[CMD_READ_RESP_LEN] = {0};

    if (!client || !cmd || !val || len < sizeof(u16) + sizeof(u16)) {
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMutexLock(&gsl_bus_mutex);
    switch (cmd) {
        case GSL_CHK_STATUS_CMD:
        /* fall through */
        case GSL_FW_VER_CMD:
        /* fall through */
        case GSL_HW_VER_CMD:
        /* fall through */
        case GSL_CFG_VER_CMD:
        /* fall through */
        case GSL_SENSOR_ID_CMD:
            ret = InputI2cWrite(client, (char *)&cmd, sizeof(cmd));
            OsalMDelay(1);
            ret = InputI2cRead(client, NULL, 0, dat, sizeof(dat));
            if (gsl_dat_verify(dat, sizeof(dat))) {
                memcpy_s(val, len, &dat[sizeof(u16)], sizeof(u32));
                ret = HDF_SUCCESS;
            } else {
                HDF_LOGE("cmd %#x verify data failed", cmd);
                ret = HDF_ERR_IO;
            }
            break;
        case GSL_GET_TOUCH_CMD:
            ret = InputI2cWrite(client, (char *)&cmd, sizeof(cmd));
            ret = InputI2cRead(client, NULL, 0, (char *)&frm_len, sizeof(frm_len));
            if (frm_len < sizeof(u16) || frm_len > len) {
                HDF_LOGE("frame len:%d error", frm_len);
                ret = HDF_ERR_IO;
                goto out;
            }
            memcpy_s(val, len, &frm_len, sizeof(frm_len));
            ret = InputI2cRead(client, NULL, 0, &val[sizeof(u16)], frm_len - sizeof(u16));
            if (gsl_dat_verify(val, frm_len)) {
                memmove_s(val, len, &val[sizeof(u16)], frm_len - sizeof(u16) - sizeof(u16));
                ret = HDF_SUCCESS;
            } else {
                HDF_LOGE("touch data verify failed.");
                ret = HDF_ERR_IO;
            }
            break;
        default:
            ret = HDF_ERR_INVALID_PARAM;
    }
out:
    OsalMutexUnlock(&gsl_bus_mutex);
    return ret;
}
/**
 * gsl_cmd_write - write a cmd to the target device
 * @client: Handle to device data
 * @cmd: Data that will be written to the slave
 *
 * Returns negative errno, or else the number of bytes written.
 */
static int gsl_cmd_write(InputI2cClient *client, u16 cmd)
{
    u8 bl_cmds[] = {0x3A, 0x08, 0x3A, 0x5A, 0xA5, 0xC5};
    u8 fw_cmds[] = {0x3B, 0x08, 0x3B, 0x5A, 0xA5, 0xC4};
    u8 suspend_cmds[] = {0x10, 0x08, 0x05, 0xE2, 0xff};
    u8 resume_cmds[] = {0x11, 0x08};
    u8 go_gesture_cmds[] = {0x12, 0x08, 0xE5, 0xFF};
    u8 out_gesture_cmds[] = {0x13, 0x08, 0xE4, 0xFF};
    u8 *data;
    int len = 0;
    int ret = 0;

    switch (cmd) {
        case GSL_RUN_BOOTLOADER_CMD:
            data = bl_cmds;
            len = sizeof(bl_cmds);
            break;
        case GSL_RUN_FIRMWARE_CMD:
            data = fw_cmds;
            len = sizeof(fw_cmds);
            break;
        case GSL_SUSPEND_CMD:
            data = suspend_cmds;
            len = sizeof(suspend_cmds);
            break;
        case GSL_RESUME_CMD:
            data = resume_cmds;
            len = sizeof(resume_cmds);
            break;
        case GSL_ENTER_GESTURE:
            data = go_gesture_cmds;
            len = sizeof(go_gesture_cmds);
            break;
        case GSL_EXIT_GESTURE:
            data = out_gesture_cmds;
            len = sizeof(out_gesture_cmds);
            break;
        default:
            HDF_LOGE("Not support %#x cmd", cmd);
            return -EINVAL;
    }
    OsalMutexLock(&gsl_bus_mutex);
    ret = InputI2cWrite(client, data, len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("send cmd %#x failed!", cmd);
    }
    OsalMutexUnlock(&gsl_bus_mutex);
    return ret;
}

static int32_t ChipInit(ChipDevice *device)
{
    HDF_LOGI("%s: Enter", __func__);
    return HDF_SUCCESS;
}

static int32_t ChipResume(ChipDevice *device)
{
    HDF_LOGI("%s: Enter", __func__);
    return HDF_SUCCESS;
}

static int32_t ChipSuspend(ChipDevice *device)
{
    HDF_LOGI("%s: Enter", __func__);
    return HDF_SUCCESS;
}

static int32_t ChipDetect(ChipDevice *device)
{
    uint32_t regValue = 0;
    int32_t ret;
    InputI2cClient *i2cClient = &device->driver->i2cClient;

    HDF_LOGI("%s: Enter", __func__);

    ret = gsl_cmd_read(i2cClient, GSL_HW_VER_CMD, (uint8_t *)&regValue, sizeof(regValue));
    CHIP_CHECK_RETURN(ret);
    HDF_LOGI("%s: Hardware Version:%#x ", __func__, regValue);

    ret = gsl_cmd_read(i2cClient, GSL_SENSOR_ID_CMD, (uint8_t *)&regValue, sizeof(regValue));
    CHIP_CHECK_RETURN(ret);
    HDF_LOGI("%s: Sensor ID:%#x ", __func__, regValue);

    ret = gsl_cmd_read(i2cClient, GSL_FW_VER_CMD, (uint8_t *)&regValue, sizeof(regValue));
    if (le32_to_cpu(regValue) & GSL_FW_VER_MASK) {
        HDF_LOGI("app version:%#x", le32_to_cpu(regValue));
    } else {
        HDF_LOGI("bootloader version:%#x", le32_to_cpu(regValue));
    }
    HDF_LOGI("%s: Firmware Version:%#x ", __func__, regValue);

    ret = gsl_cmd_read(i2cClient, GSL_CFG_VER_CMD, (uint8_t *)&regValue, sizeof(regValue));
    HDF_LOGI("%s: Config Version:%#x ", __func__, regValue);

    (void)ChipInit(device);
    (void)ChipResume(device);
    (void)ChipSuspend(device);
    return HDF_SUCCESS;
}

static int32_t ChipDataHandle(ChipDevice *device)
{
    FrameData *frame = &device->driver->frameData;
    u8 raw[64] = {0};
    u8 rpt_id = 0;
    u8 t1;
    u8 t2;
    u8 t3;
    int i;
    int ret;
    int index = 0;

    memset_s(raw, sizeof(raw), 0, sizeof(raw));
    ret = gsl_cmd_read(&device->driver->i2cClient, GSL_GET_TOUCH_CMD, raw, sizeof(raw));
    CHIP_CHECK_RETURN(ret);

    memset_s(frame, sizeof(FrameData), 0, sizeof(FrameData));
    rpt_id = raw[index++];
    frame->realPointNum = raw[index++];
    frame->definedEvent = frame->realPointNum ? TOUCH_DOWN : TOUCH_UP;
    switch (rpt_id) {
        case 0x81:
            for (i = 0; i < frame->realPointNum; i++) {
                frame->fingers[i].trackId = raw[index++];
                t1 = raw[index++];
                t2 = raw[index++];
                t3 = raw[index++];
                frame->fingers[i].pressure = raw[index++];
                frame->fingers[i].x = ((t1 & 0x000f) << GSL_BYTE_WIDTH) + t2;
                frame->fingers[i].y = ((t1 & 0x00f0) << GSL_NIBBLE_WIDTH) + t3;
                frame->fingers[i].status = TOUCH_DOWN;
                frame->fingers[i].valid = true;
            }
            break;
        case 0x82:
            for (i = 0; i < frame->realPointNum; i++) {
                frame->fingers[i].trackId = raw[index++];
                t1 = raw[index++];
                t2 = raw[index++];
                t3 = raw[index++];
                frame->fingers[i].x = ((t1 & 0x000f) << GSL_BYTE_WIDTH) + t2;
                frame->fingers[i].y = ((t1 & 0x00f0) << GSL_NIBBLE_WIDTH) + t3;
                frame->fingers[i].pressure = 0;
                frame->fingers[i].status = TOUCH_DOWN;
                frame->fingers[i].valid = true;
            }
            break;
        default:
            HDF_LOGE("rpt_id:%#x error.", rpt_id);
            return -ENODATA;
    }
    return HDF_SUCCESS;
}

static int32_t UpdateFirmware(ChipDevice *device)
{
    HDF_LOGI("%s: bypass\n", __func__);
    return HDF_SUCCESS;
}

static void SetAbility(ChipDevice *device)
{
    #define KEY_CODE_4TH          3
    HDF_LOGI("%s: enter", __func__);
    device->driver->inputDev->abilitySet.devProp[0] = SET_BIT(INPUT_PROP_DIRECT);
    device->driver->inputDev->abilitySet.eventType[0] = SET_BIT(EV_SYN) |
        SET_BIT(EV_KEY) | SET_BIT(EV_ABS);
    device->driver->inputDev->abilitySet.absCode[0] = SET_BIT(ABS_X) | SET_BIT(ABS_Y);
    device->driver->inputDev->abilitySet.absCode[1] = SET_BIT(ABS_MT_POSITION_X) |
        SET_BIT(ABS_MT_POSITION_Y) | SET_BIT(ABS_MT_TRACKING_ID);
    device->driver->inputDev->abilitySet.keyCode[KEY_CODE_4TH] =
        SET_BIT(KEY_UP) | SET_BIT(KEY_DOWN);
    device->driver->inputDev->attrSet.axisInfo[ABS_X].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_X].max =
        device->boardCfg->attr.resolutionX - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_X].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_Y].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_Y].max =
        device->boardCfg->attr.resolutionY - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_Y].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_X].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_X].max =
        device->boardCfg->attr.resolutionX - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_X].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_Y].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_Y].max =
        device->boardCfg->attr.resolutionY - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_Y].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_TRACKING_ID].max = MAX_POINT;
}
static struct TouchChipOps g_gsl377xChipOps = {
    .Init = ChipInit,
    .Detect = ChipDetect,
    .Resume = ChipResume,
    .Suspend = ChipSuspend,
    .DataHandle = ChipDataHandle,
    .UpdateFirmware = UpdateFirmware,
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

static int32_t HdfGslChipInit(struct HdfDeviceObject *device)
{
    TouchChipCfg *chipCfg = NULL;
    ChipDevice *chipDev = NULL;
    int32_t ret;
    HDF_LOGE("%s: enter", __func__);
    if (device == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = OsalMutexInit(&gsl_bus_mutex);
    CHIP_CHECK_RETURN(ret);
    chipCfg = ChipConfigInstance(device);
    if (chipCfg == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    chipDev = ChipDeviceInstance();
    if (chipDev == NULL) {
        goto EXIT;
    }

    chipDev->chipCfg = chipCfg;
    chipDev->ops = &g_gsl377xChipOps;
    chipDev->chipName = chipCfg->chipName;
    chipDev->vendorName = chipCfg->vendorName;
    device->priv = (void *)chipDev;

    if (RegisterTouchChipDevice(chipDev) != HDF_SUCCESS) {
        goto EXIT1;
    }
    HDF_LOGI("%s: exit succ, chipName = %s", __func__, chipCfg->chipName);
    return HDF_SUCCESS;

EXIT1:
    OsalMemFree(chipDev);
EXIT:
    FreeChipConfig(chipCfg);
    OsalMutexDestroy(&gsl_bus_mutex);
    return HDF_FAILURE;
}

static void HdfGslChipRelease(struct HdfDeviceObject *device)
{
    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return;
    }
    OsalMutexDestroy(&gsl_bus_mutex);
    HDF_LOGI("%s: gsl chip is release", __func__);
}

struct HdfDriverEntry g_touchGslChipEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_TOUCH_GSL377X",
    .Init = HdfGslChipInit,
    .Release = HdfGslChipRelease,
};

HDF_INIT(g_touchGslChipEntry);