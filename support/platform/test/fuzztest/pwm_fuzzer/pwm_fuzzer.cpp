/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pwm_fuzzer.h"
#include <iostream>
#include "random.h"
#include "securec.h"
#include "hdf_base.h"
#include "pwm_if.h"

using namespace std;

namespace {
constexpr int32_t MIN = 0;
constexpr int32_t MAX = 2;
const int32_t PWM_TEST_NUM = 1;
const int32_t MAX_LEN = 15;
}

struct AllParameters {
    uint32_t descPer;
    uint32_t descDuty;
    uint8_t descPolar;
};

union ConvertToParam {
    uint8_t data[MAX_LEN];
    struct AllParameters params;
};

namespace OHOS {
    bool PwmFuzzTest(const uint8_t *data, size_t size)
    {
        int32_t number;
        DevHandle handle = nullptr;
        union ConvertToParam convertor;

        if (data == nullptr) {
            return false;
        }

        if (memcpy_s (convertor.data, MAX_LEN, data, MAX_LEN) != EOK) {
            return false;
        }

        number = randNum(MIN,MAX);
        handle = PwmOpen(PWM_TEST_NUM);
        switch (static_cast<ApiTestCmd>(number)) {
            case ApiTestCmd::PWM_TEST_SET_PERIOD:
                PwmSetPeriod(handle, convertor.params.descPer);
                break;
            case ApiTestCmd::PWM_TEST_SET_DUTY:
                PwmSetDuty(handle, convertor.params.descDuty);
                break;
            case ApiTestCmd::PWM_TEST_SET_POLARITY:
                PwmSetPolarity(handle, convertor.params.descPolar);
                break;
            default:
                break;
        }
        PwmClose(handle);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::PwmFuzzTest(data, size);
    return 0;
}
