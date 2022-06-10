/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
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
