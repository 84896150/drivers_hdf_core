/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_accessory_base_test.h"
#include "audio_accessory_base.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG audio_accessory_base_test

int32_t AccessoryI2cReadWriteTest(void)
{
    struct AudioAddrConfig addrConfig;

    if (AccessoryI2cReadWrite(NULL, 0) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AccessoryI2cReadWrite(&addrConfig, 0) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AccessoryRegBitsReadTest(void)
{
    struct AudioMixerControl reg;
    uint32_t regValue = 0;

    if (AccessoryRegBitsRead(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AccessoryRegBitsRead(&reg, &regValue) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AccessoryRegBitsUpdateTest(void)
{
    struct AudioMixerControl reg;

    if (AccessoryRegBitsUpdate(reg) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}


int32_t AcessoryDeviceFrequencyParseTest(void)
{
    uint16_t freq = 0;
    if (AcessoryDeviceFrequencyParse(0, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AcessoryDeviceFrequencyParse(0, &freq) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AccessoryDaiParamsUpdateTest(void)
{
    struct DaiParamsVal value;
    if (AccessoryDaiParamsUpdate(value) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AccessoryDeviceCfgGetTest(void)
{
    if (AccessoryDeviceCfgGet(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    struct AccessoryData accessoryData;
    struct AccessoryTransferData accessoryTransferData;

    memset(&accessoryData, 0, sizeof(struct AccessoryData));
    memset(&accessoryTransferData, 0, sizeof(struct AccessoryTransferData));
    if (AccessoryDeviceCfgGet(&accessoryData, &accessoryTransferData) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AccessoryDeviceCtrlRegInitTest(void)
{
    if (AccessoryDeviceCtrlRegInit() == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AccessoryDeviceRegReadTest(void)
{
    if (AccessoryDeviceRegRead(NULL, 0, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    uint32_t val;
    struct AccessoryDevice codec;
    if (AccessoryDeviceRegRead(&codec, 0, &val) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AccessoryDeviceRegWriteTest(void)
{
    if (AccessoryDeviceRegWrite(NULL, 0, 0) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    struct AccessoryDevice codec;
    if (AccessoryDeviceRegWrite(&codec, 0, 0) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AccessoryGetConfigInfoTest(void)
{
    struct HdfDeviceObject device;
    struct AccessoryData codecData;

    if (AccessoryGetConfigInfo(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AccessoryGetConfigInfo(&device, &codecData) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

