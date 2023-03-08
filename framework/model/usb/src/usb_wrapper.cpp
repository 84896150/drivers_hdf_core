/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "usb_wrapper.h"

#include "hdf_log.h"
#include "hdf_usb_pnp_manage.h"
#include "hisysevent.h"
#include "idevmgr_hdi.h"

using OHOS::HDI::DeviceManager::V1_0::IDeviceManager;
using namespace OHOS::HiviewDFX;
#ifndef __LITEOS__
int32_t UsbPnpRegisterDevice(const char *serviceName)
{
    std::string serName(serviceName);
    OHOS::sptr<IDeviceManager> devMgr = IDeviceManager::Get();
    int32_t ret = devMgr->LoadDevice(serName);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s, load %{public}s failed", __func__, serviceName);
        return ret;
    }
    return ret;
}

int32_t UsbPnpUnregisterDevice(const char *serviceName)
{
    std::string serName(serviceName);
    OHOS::sptr<IDeviceManager> devMgr = IDeviceManager::Get();
    int32_t ret = devMgr->UnloadDevice(serName);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s, %{public}s unload  failed", __func__, serviceName);
    }
    return ret;
}

void UsbDDKDriverMatchFailEvent(const struct UsbPnpNotifyMatchInfoTable *infoTable)
{
    HiSysEventWrite(HiSysEvent::Domain::HDF_USB, "RECOGNITION_FAIL", HiSysEvent::EventType::FAULT,
        "DEVICE_NAME", std::to_string(infoTable->busNum) + "-" + std::to_string(infoTable->devNum),
        "DEVICE_PROTOCOL", infoTable->deviceInfo.deviceProtocol, "DEVICE_CLASS", infoTable->deviceInfo.deviceClass,
        "VENDOR_ID", infoTable->deviceInfo.vendorId, "PRODUCT_ID", infoTable->deviceInfo.productId,
        "VERSION", "1.0.0", "FAIL_REASON", 1, "FAIL_INFO", "Driver matching failed");
}
#endif
