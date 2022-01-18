/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
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
