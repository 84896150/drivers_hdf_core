 /*
  * Copyright (c) 2023 Huawei Device Co., Ltd.
  *
  * HDF is dual licensed: you can use it either under the terms of
  * the GPL, or the BSD license, at your option.
  * See the LICENSE file in the root of this repository for complete details.
  */

#ifndef USB_WRAPPER_H
#define USB_WRAPPER_H
#include "hdf_base.h"
#include "hdf_remote_service.h"
#include "servmgr_hdi.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifndef __LITEOS__
int32_t UsbPnpRegisterDevice(const char *serviceName);
int32_t UsbPnpUnregisterDevice(const char *serviceName);
void UsbDDKDriverMatchFailEvent(const struct UsbPnpNotifyMatchInfoTable *infoTable);
#endif
#ifdef __cplusplus
};
#endif
#endif
