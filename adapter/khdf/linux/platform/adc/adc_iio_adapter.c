/*
 * adc driver adapter of linux
 *
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <linux/fs.h>
#include <linux/kernel.h>
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "adc_core.h"

#define ADC_STRING_VALUE_LEN 15
#define ADC_MAX_DEVICE_NUM 12
#define ADC_MAX_CHANNEL_NUM 128
#define ADC_CHANNEL_NAME_LEN 128
#define INT_MAX_VALUE 2147483647
#define FILE_MODE 0600
#define DECIMAL_SHIFT_LEFT 10

static const char* g_driverPathname[ADC_MAX_DEVICE_NUM][ADC_MAX_CHANNEL_NUM] = {0};
static struct file* g_fp[ADC_MAX_CHANNEL_NUM] = {0};
struct AdcIioDevice {
    struct AdcDevice device;
    uint32_t channelNum;
    volatile unsigned char *regBase;
    volatile unsigned char *pinCtrlBase;
    uint32_t regBasePhy;
    uint32_t regSize;
    uint32_t deviceNum;
    uint32_t dataWidth;
    uint32_t validChannel;
    uint32_t scanMode;
    uint32_t delta;
    uint32_t deglitch;
    uint32_t glitchSample;
    uint32_t rate;
};

static int32_t AdcIioRead(struct AdcDevice *device, uint32_t channel, uint32_t *val)
{
    int ret;
    loff_t pos = 0;
    unsigned char strValue[ADC_STRING_VALUE_LEN] = {0};

    if (device == NULL) {
        HDF_LOGE("%s: Illegal object", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (val == NULL) {
        HDF_LOGE("%s: Illegal parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct AdcIioDevice *adcDevice = (struct AdcIioDevice *)device;
    if (channel > adcDevice->channelNum || g_fp[channel] == NULL) {
        HDF_LOGE("%s: invalid channel:%u", __func__, channel);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = kernel_read(g_fp[channel], strValue, ADC_STRING_VALUE_LEN, &pos);
    if (ret < 0) {
        HDF_LOGE("%s: kernel_read fail %d", __func__, ret);
        return HDF_PLT_ERR_OS_API;
    }
    *val = simple_strtoul(strValue, NULL, 0);
    return HDF_SUCCESS;
}

static int32_t AdcIioStop(struct AdcDevice *device)
{
    int ret;
    uint32_t i;

    if (device == NULL) {
        HDF_LOGE("%s: Illegal object", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    i = 0;
    ret = HDF_FAILURE;
    while (g_fp[i] != NULL) {
        if (!IS_ERR(g_fp[i])) {
            ret = filp_close(g_fp[i], NULL);
            if (ret != 0) {
                HDF_LOGE("%s: filp_close fail", __func__);
                break;
            }
        }
        g_fp[i] = NULL;
        i++;
    }
    return ret;
}

static int32_t AdcIioStart(struct AdcDevice *device)
{
    uint32_t i;
    struct AdcIioDevice *adcDevice;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    adcDevice = (struct AdcIioDevice *)device;
    for (i = 0; i < adcDevice->channelNum; i++) {
        g_fp[i] = filp_open(g_driverPathname[adcDevice->deviceNum][i], O_RDWR | O_NOCTTY | O_NDELAY, FILE_MODE);
        if (IS_ERR(g_fp[i])) {
            HDF_LOGE("filp open fail");
            return HDF_PLT_ERR_OS_API;
        }       
    }

    return HDF_SUCCESS;
}
static const struct AdcMethod g_method = {
    .read = AdcIioRead,
    .stop = AdcIioStop,
    .start = AdcIioStart,
};

static int32_t AdcIioReadDrs(struct AdcIioDevice *adcDevice, const struct DeviceResourceNode *node)
{
    int32_t ret;
    const char *drName = NULL;
    char channelName[ADC_CHANNEL_NAME_LEN] = {0};
    int32_t drNameLen;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t i;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL || drsOps->GetString == NULL) {
        HDF_LOGE("%s: invalid drs ops", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    ret = drsOps->GetUint32(node, "deviceNum", &adcDevice->deviceNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read deviceNum failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "channelNum", &adcDevice->channelNum, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read channelNum failed", __func__);
        return ret;
    }
    for (i = 0; i < adcDevice->channelNum; i++) {
        if (sprintf_s(channelName, ADC_CHANNEL_NAME_LEN - 1, "driver_channel%d_name", i) < 0) {
            return HDF_FAILURE;
        }
        ret = drsOps->GetString(node, channelName, &drName, "/sys/bus/iio/devices/iio:device0/in_voltage0-voltage0_raw");
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: read driver_name failed", __func__);
            return ret;
        }
        g_driverPathname[adcDevice->deviceNum][i] = drName;
        drName = NULL;
    }

    ret = drsOps->GetUint32(node, "scanMode", &adcDevice->scanMode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read scanMode failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "rate", &adcDevice->rate, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read rate failed", __func__);
        return ret;
    }
    
    return HDF_SUCCESS;
}
static int32_t AdcIioParseInit(struct HdfDeviceObject *device, struct DeviceResourceNode *node)
{
    int32_t ret;
    struct AdcIioDevice *adcDevice = NULL;


    (void)device;
    adcDevice = (struct AdcIioDevice *)OsalMemCalloc(sizeof(*adcDevice));
    if (adcDevice == NULL) {
        HDF_LOGE("%s: alloc adcDevice failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = AdcIioReadDrs(adcDevice, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read drs failed:%d", __func__, ret);
        OsalMemFree(adcDevice);
        return ret;
    }
    adcDevice->device.priv = (void *)node;
    adcDevice->device.devNum = adcDevice->deviceNum;
    adcDevice->device.ops = &g_method;

    ret = AdcDeviceAdd(&adcDevice->device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: add adc device:%u failed", __func__, adcDevice->deviceNum);
        OsalMemFree(adcDevice);
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t LinuxAdcInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct DeviceResourceNode *childNode = NULL;
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = HDF_SUCCESS;
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        ret = AdcIioParseInit(device, childNode);
        if (ret != HDF_SUCCESS) {
            break;
        }
    }
    if (ret == HDF_SUCCESS) {
        HDF_LOGI("%s: adc init success.", __func__);    
    } else {
        HDF_LOGE("%s: adc init fail.", __func__);       
    }

    return ret;
}

static void AdcIioRemoveByNode(const struct DeviceResourceNode *node)
{
    int32_t ret;
    int32_t deviceNum;
    struct AdcDevice *device = NULL; 
    struct AdcIioDevice *adcDevice = NULL;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops", __func__);
        return;
    }

    ret = drsOps->GetUint32(node, "deviceNum", (uint32_t *)&deviceNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read deviceNum failed", __func__);
        return;
    }

    device = AdcDeviceGet(deviceNum);
    if (device != NULL && device->priv == node) {
        AdcDevicePut(device);
        AdcDeviceRemove(device);
        adcDevice = (struct AdcIioDevice *)device;
        OsalMemFree(adcDevice);
    }
    return;
}

static void LinuxAdcRelease(struct HdfDeviceObject *device)
{
    const struct DeviceResourceNode *childNode = NULL;
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is null", __func__);
        return;
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        AdcIioRemoveByNode(childNode);
    }
}

struct HdfDriverEntry g_adcLinuxDriverEntry = {
    .moduleVersion = 1,
    .Bind = NULL,
    .Init = LinuxAdcInit,
    .Release = LinuxAdcRelease,
    .moduleName = "linux_adc_adapter",
};
HDF_INIT(g_adcLinuxDriverEntry);
