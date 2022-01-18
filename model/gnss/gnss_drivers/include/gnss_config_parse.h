/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef GNSS_CONFIG_PARSE_H
#define GNSS_CONFIG_PARSE_H

#include "hdf_log.h"
#include "i2c_if.h"
#include "spi_if.h"
#include "uart_if.h"
#include "osal_thread.h"

#define GNSS_SPI_MAX_SPEED 115200

struct DevHandle {
    void *object;
};

enum GnssBusType {
    GNSS_BUS_UART = 0,
    GNSS_BUS_SPI = 1,
    GNSS_BUS_I2C = 2,
};
struct GnssUartCfg {
    void* handle;
    uint32_t busNum;
    uint32_t busSpeed;
    uint32_t reserved;  // reserved 
};

struct GnssI2cCfg {
    void* handle;
    uint32_t busNum;
    uint32_t busSpeed;
    uint32_t devAddr;  // Address of the I2C device
};

struct GnssSpiCfg {
    void* handle;
    uint32_t busNum;
    uint32_t busSpeed;
    uint32_t csNum;  // SPI device csnum
};

struct GnssBusCfg {
    uint8_t busType; // enum gnssmodule use BusType
    union {
        struct GnssUartCfg uartCfg;
        struct GnssI2cCfg i2cCfg;
        struct GnssSpiCfg spiCfg;
    };
};

#define GNSS_INFO_NAME_MAX_LEN   32

struct GnssBasicInfo {
    char gnssName[GNSS_INFO_NAME_MAX_LEN];      /* gnssmodule name */
    char vendorName[GNSS_INFO_NAME_MAX_LEN];    /* gnssmodule vendor */
    int32_t gnssDeviceId;                       /* gnssDeviceId, defined by the gnss driver developer */
};

#endif /* GNSS_CONFIG_PARSE_H */
