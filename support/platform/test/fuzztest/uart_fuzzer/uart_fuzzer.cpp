/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "uart_fuzzer.h"
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include "uart_if.h"
#include "hdf_base.h"
#include "random.h"
#include "securec.h"

using namespace std;

namespace {
constexpr int32_t MIN = 0;
constexpr int32_t MAX = 2;
const int32_t UART_TEST_PORT = 1;
const int32_t MAX_LEN = 15;
}

struct AllParameters {
    uint32_t desBaudRate;
};

union ConvertToParam {
    uint8_t data[MAX_LEN];
    struct AllParameters params;
    struct UartAttribute paraAttribute;
    enum UartTransMode paraMode;
};

namespace OHOS {
    bool UartFuzzTest(const uint8_t* data, size_t size)
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
        handle = UartOpen(UART_TEST_PORT);
        switch (static_cast<ApiNumber>(number)) {
            case ApiNumber::NUM_ZERO:
                UartSetBaud(handle, convertor.params.desBaudRate);
                break;
            case ApiNumber::NUM_ONE:
                UartSetAttribute(handle, &convertor.paraAttribute);
                break;
            case ApiNumber::NUM_TWO:
                UartSetTransMode(handle, convertor.paraMode);
                break;
            default:
                break;
        }
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::UartFuzzTest(data, size);
    return 0;
}
