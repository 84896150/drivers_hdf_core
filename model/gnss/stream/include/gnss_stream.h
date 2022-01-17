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

#ifndef GNSS_STREAM_H
#define GNSS_STREAM_H

#include "securec.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_dlist.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "gnss_driver.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define GNSS_GPGGA_INFO_MAX_DATA_LEN  512
#define GNSS_GPGSV_INFO_MAX_DATA_LEN  512
struct GnssStreamRequest {
    int32_t gnssId;
    const char *gnssName;
    char *requestData;
    int32_t dataLen;
};

struct GnssStreamResp {
    char *respData;
    int32_t dataLen;
};

enum GnssStreamMethodCmd {
    GNSSDRV_STREAM_IOCTL_OPEN = 0,
    GNSSDRV_STREAM_IOCTL_CLOSE,
    GNSSDRV_READ_ALL_GNSS_INFO,
    GNSSDRV_READ_GPGGA_INFO,
    GNSSDRV_READ_GPGSV_INFO,
    GNSSDRV_STREAM_IOCTL_WRITE,
    GNSSDRV_STREAM_IOCTL_BUTT,
};

struct GnssStreamHandleList {
    enum GnssStreamMethodCmd cmd;
    int32_t (*func)(struct GnssDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply);
};

struct GnssStreamHost {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    void *priv;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* GNSS_STREAM_H */
