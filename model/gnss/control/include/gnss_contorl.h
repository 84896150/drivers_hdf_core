/*
 * Copyright (C) 2021 HiHope Open Source Organization.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GNSS_CONTROL_H
#define GNSS_CONTROL_H

#include "securec.h"
#include "hdf_base.h"
#include "hdf_dlist.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "device_resource_if.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "gnss_driver.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct GnssContorlRequest {
    int32_t gnssId;
    const char *gnssName;
    char *requestData;
    int32_t dataLen;
};

struct GnssContorlResp {
    char *respData;
    int32_t dataLen;
};

enum GnssControlMethodCmd {
    GNSSDRV_CTRL_SET_POWER_MODE,
    GNSSDRV_CTRL_IOCTRL_ELEM_BUTT,
};

struct GnssControlCmdHandleList {
    enum GnssControlMethodCmd cmd;
    int32_t (*func)(struct GnssDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply);
};

struct GnssControlHost {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    void *priv;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* GNSS_CONTROL_H */
