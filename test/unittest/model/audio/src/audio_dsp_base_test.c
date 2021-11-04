/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_dsp_base_test.h"
#include "audio_dsp_base.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG audio_dsp_base_test
#define KCONTROL_TEST_SERVICE_NAME "dsp_service_0"

int32_t DspGetServiceNameTest(void)
{
    if (DspGetServiceName(NULL, NULL) == HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("TestDspGetServiceName fail");
        return HDF_FAILURE;
    }

    struct DeviceResourceAttr attr;
    attr.value = KCONTROL_TEST_SERVICE_NAME;
    attr.name  = "serviceName";

    struct DeviceResourceNode node;
    node.attrData = &attr;

    struct HdfDeviceObject device;
    device.property = &node;

    if (DspGetServiceName(&device, NULL) == HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("TestDspGetServiceName fail");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t DspGetDaiNameTest(void)
{
    if (DspGetDaiName(NULL, NULL) == HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("TestDspGetDaiName fail");
        return HDF_FAILURE;
    }

    struct DeviceResourceAttr attr;
    attr.value = KCONTROL_TEST_SERVICE_NAME;
    attr.name  = "dspDaiName";

    struct DeviceResourceNode node;
    node.attrData = &attr;

    struct HdfDeviceObject device;
    device.property = &node;

    if (DspGetDaiName(&device, NULL) == HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("TestDspGetDaiName fail");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

