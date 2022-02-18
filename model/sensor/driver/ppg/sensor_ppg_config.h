/*
 * Copyright (c) 2020-2021 Chipsea Technologies (Shenzhen) Corp., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PPG_CFG_H
#define PPG_CFG_H

#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "spi_if.h"
#include "gpio_if.h"
#include "hdf_log.h"
#include "sensor_device_type.h"
#include "sensor_platform_if.h"
#include "sensor_config_parser.h"

#define PPG_PIN_CONFIG_MAX_ITEM 30
#define PPG_INFO_NAME_MAX_LEN   16
#define PPG_TYPE_GPIO 0
#define PPG_TYPE_IRQ 1

#define PPG_IRQ_NO(cfg) ((cfg)->pinCfg.gpios[0].gpioNo)

enum MultiUsePinSetCfgIndex {
    PPG_PIN_CFG_ADDR_INDEX = 0,
    PPG_PIN_CFG_ADDR_LEN_INDEX,
    PPG_PIN_CFG_ADDR_VALUE_INDEX,
    PPG_PIN_CFG_INDEX_MAX,
};

struct MultiUsePinSetCfg {
    uint32_t regAddr;
    uint32_t regLen;
    uint32_t regValue;
};

struct IrqFuncCfg {
    uint16_t pinIndex;
    GpioIrqFunc func;
};

enum GpiosPinMode {
    PPG_PIN_LOW = 0,
    PPG_PIN_HIGH = 1,
    PPG_PIN_IRQ = 2,
};

enum GpiosPinSetCfgIndex {
    PPG_GPIO_PIN_INDEX = 0,
    PPG_GPIO_NO_INDEX,
    PPG_GPIO_TYPE_INDEX,
    PPG_GPIO_DIR_TYPE_INDEX,
    PPG_GPIO_MODE_INDEX,
    PPG_GPIOS_INDEX_MAX,
};

struct GpiosPinSetCfg {
    uint16_t pinIndex;
    uint16_t gpioNo;
    uint16_t gpioType;
    uint16_t dirType;
    union {
        uint16_t gpioValue;
        uint16_t triggerMode;
    };
};

struct PpgPinCfg {
    uint32_t gpioNum;
    struct MultiUsePinSetCfg *multiUsePin;
    struct GpiosPinSetCfg *gpios;
};

// struct PpgBusCfg {
//     DevHandle handle;
//     struct SpiDevInfo spiDevInfo;
//     struct SpiCfg spiCfg;
// };

struct PpgCfgData {
    struct SensorCfgData sensorCfg;
    struct PpgPinCfg pinCfg;
};

int32_t EnablePpgIrq(uint16_t gpioNo);
int32_t SetPpgIrq(uint16_t pinIndex, GpioIrqFunc irqFunc);
void PpgDestructCfgData(void);
int32_t PpgConstructCfgData(const struct DeviceResourceNode * node, struct PpgCfgData **cfgData);
struct PpgCfgData *GetPpgConfig(void);

#endif /* SENSOR_PPG_CONFIG_H */
