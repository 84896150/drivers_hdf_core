/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PWM_TEST_H
#define PWM_TEST_H

#include "hdf_device_desc.h"
#include "pwm_if.h"

int32_t PwmTestExecute(int cmd);

enum PwmTestCmd {
    PWM_SET_PERIOD_TEST = 0,
    PWM_SET_DUTY_TEST,
    PWM_SET_POLARITY_TEST,
    PWM_ENABLE_TEST,
    PWM_DISABLE_TEST,
    PWM_SET_CONFIG_TEST,
    PWM_GET_CONFIG_TEST,
    PWM_RELIABILITY_TEST,
    PWM_TEST_CMD_MAX,
};

struct PwmTestConfig {
    uint32_t num;
    struct PwmConfig cfg;
    struct PwmConfig originCfg;
};

struct PwmTester {
    struct PwmTestConfig config;
    DevHandle handle;
};

#endif /* PWM_TEST_H */
