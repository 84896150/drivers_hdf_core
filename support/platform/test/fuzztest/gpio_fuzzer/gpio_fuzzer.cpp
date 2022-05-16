/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "gpio_fuzzer.h"
#include <iostream>
#include "random.h"
#include "securec.h"
#include "gpio_if.h"
#include "hdf_base.h"

using namespace std;

namespace {
constexpr int32_t MIN = 0;
constexpr int32_t MAX = 2;
const uint16_t gpioTestNum = 3;
const int32_t MAX_LEN = 10;
}

struct AllParameters {
    uint16_t descVal;
    uint16_t descDir;
    uint16_t descMode;
};

union ConvertToParam {
    uint8_t data[MAX_LEN];
    struct AllParameters params;
};

static int32_t GpioTestIrqHandler(uint16_t gpio, void *data)
{
    (void)gpio;
    (void)data;
    return 0;
}

namespace OHOS {
    bool GpioFuzzTest(const uint8_t *data, size_t size)
    {
            int32_t number;
            union ConvertToParam convertor;

            if (memcpy_s(convertor.data, MAX_LEN, data, MAX_LEN) != EOK) {
                return false;
            }
            number = randNum(MIN,MAX);
            switch (static_cast<ApiTestCmd>(number)) {
                case ApiTestCmd::GPIO_TEST_WRITE:
                    GpioWrite(gpioTestNum, convertor.params.descVal);
                    break;
                case ApiTestCmd::GPIO_TEST_SET_DIR:
                    GpioSetDir(gpioTestNum, convertor.params.descDir);
                    break;
                case ApiTestCmd::GPIO_TEST_SET_IRQ:
                    GpioSetIrq(gpioTestNum, convertor.params.descMode, GpioTestIrqHandler, &data);
                    break;                
                default:
                    break;
            }
    return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::GpioFuzzTest(data, size);
    return 0;
}
