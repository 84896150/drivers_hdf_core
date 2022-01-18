/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */


#ifndef GNSS_DEVICE_DRIVER_H
#define GNSS_DEVICE_DRIVER_H

#include "securec.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_dlist.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "gnss_driver_log.h"
#include "gnss_config_parse.h"

#define GNSS_I2C_BUS_READ_MSG_NUM           2
#define GNSS_I2C_BUS_READ_MSG_ADDR_IDX      0
#define GNSS_I2C_BUS_READ_MSG_VALUE_IDX     1

#define GNSS_I2C_BUS_WRITE_MSG_NUM           2
#define GNSS_I2C_BUS_WRITE_MSG_ADDR_IDX      0
#define GNSS_I2C_BUS_WRITE_MSG_VALUE_IDX     1

#define GNSS_I2C_BUS_REG_BUF_LEN    4
#define GNSS_I2C_BUS_BYTE_MASK      0xFF
#define GNSS_I2C_BUS_BYTE_OFFSET    8

#define GNSS_ALL_INFO_MAX_DATA_LEN  2048
#define READ_GNSS_INFO_MAX_DATA_LEN  1000

#define GNSS_DATA_FLAG  "$GP"

struct GnssCfgData {
    struct GnssBusCfg busCfg;
    struct GnssBasicInfo gnssInfo;
    const struct DeviceResourceNode *root;
};

struct GnssDrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct GnssCfgData *gnssCfg;
};

struct GnssDeviceInfo {
    struct GnssBasicInfo gnssInfo;
    int32_t (*Open)(struct GnssDeviceInfo *deviceInfo);
    int32_t (*Read)(struct GnssDeviceInfo *deviceInfo, char *resp, uint32_t len, int32_t *replyLen);
    int32_t (*Write)(struct GnssDeviceInfo *deviceInfo, char *req, uint32_t len);
    int32_t (*SetPowerMode)(struct GnssDeviceInfo *deviceInfo, uint32_t state);
    int32_t (*Close)(struct GnssDeviceInfo *deviceInfo);
};

extern int32_t GetGnssConfigData(const struct DeviceResourceNode *node, struct GnssCfgData *config);
extern struct GnssDeviceInfo g_gnssDeviceInfo;

#endif /* GNSS_DEVICE_DRIVER_H */