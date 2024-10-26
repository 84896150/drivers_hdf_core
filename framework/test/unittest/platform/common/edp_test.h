/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef EDP_TEST_H
#define EDP_TEST_H

#include "hdf_device_desc.h"
#include "edp_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct EdpTestConfig {
    uint32_t deviceIndex;
};

struct EdpTester {
    struct EdpTestConfig config;
    DevHandle handle;
};

enum EdpTestCmd {
    EDP_TEST_CMD_DEEP_COLOR_SET = 0,
    EDP_TEST_CMD_DEEP_COLOR_GET,
    EDP_TEST_CMD_SET_VIDEO_ATTR,
    EDP_TEST_CMD_SET_AUDIO_ATTR,
    EDP_TEST_CMD_DISABLE,
    EDP_TEST_CMD_ENABLE,
    EDP_TEST_CMD_REGISTER_HPD_CB,
    EDP_TEST_CMD_UNREGISTER_HPD_CB,
    EDP_TEST_CMD_GET_CONNECTER,
    EDP_TEST_CMD_MAX,
};

struct EdpTest {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*doTest)(struct EdpTest *test, int32_t cmd);
    DevHandle handle;
    uint8_t devNo;
    uint32_t total;
    uint32_t fails;
};

static inline struct EdpTest *EdpTestServiceGet(void)
{
    return (struct EdpTest *)DevSvcManagerClntGetService("PLATFORM_EDP_TEST");
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* EDP_TEST_H */
