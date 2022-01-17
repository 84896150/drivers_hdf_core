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
