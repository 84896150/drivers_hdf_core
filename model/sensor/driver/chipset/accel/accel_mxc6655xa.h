/*
 * Copyright (C) 2022 HiHope Open Source Organization.
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

#ifndef ACCEL_MXC6655XA_H
#define ACCEL_MXC6655XA_H

#include "sensor_accel_driver.h"
#include "sensor_config_parser.h"

/* ACCEL DATA REGISTERS ADDR */
#define MXC6655XA_ACCEL_X_MSB_ADDR              0X03
#define MXC6655XA_ACCEL_X_LSB_ADDR              0X04
#define MXC6655XA_ACCEL_Y_MSB_ADDR              0X05
#define MXC6655XA_ACCEL_Y_LSB_ADDR              0X06
#define MXC6655XA_ACCEL_Z_MSB_ADDR              0X07
#define MXC6655XA_ACCEL_Z_LSB_ADDR              0X08
#define MXC6655XA_STATUS_ADDR                   0X02

/* default HZ */
#define MXC6655XA_ACCEL_DEFAULT_ODR_100HZ       100
#define MXC6655XA_ACCEL_DEFAULT_ODR_25HZ        25

/* ACCEL RANGE */
#define MXC6655XA_ACCEL_RANGE_2G                0X03
#define MXC6655XA_ACCEL_RANGE_4G                0X05
#define MXC6655XA_ACCEL_RANGE_8G                0X08
#define MXC6655XA_ACCEL_RANGE_16G               0X0C

/* ACC sensitivity */
#define MXC6655XA_ACC_SENSITIVITY_2G            61
#define MXC6655XA_ACC_SENSITIVITY_4G            122
#define MXC6655XA_ACC_SENSITIVITY_8G            244
#define MXC6655XA_ACC_SENSITIVITY_16G           488

/* ACCEL DATA READY */
#define MXC6655XA_ACCEL_DATA_READY_MASK         0x80

int32_t ReadMxc6655xaData(struct SensorCfgData *cfg, struct SensorReportEvent *event);
struct Mxc6655xaDrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct SensorCfgData *sensorCfg;
};

#endif /* ACCEL_MXC6655XA_H */
