/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "uart_fuzzer.h"
#include <iostream>
#include "random.h"
#include "securec.h"
#include "hdf_base.h"
#include "uart_if.h"

using namespace std;

namespace {
constexpr int32_t MIN = 0;
constexpr int32_t MAX = 2;
const int32_t UART_TEST_PORT = 1;
const int32_t MAX_LEN = 15;
}

struct AllParameters {
    uint32_t desBaudRate;
    struct UartAttribute paraAttribute;
    enum UartTransMode paraMode;
};

union ConvertToParam {
    uint8_t data[MAX_LEN];
    struct AllParameters params;
};

namespace OHOS {
    bool UartFuzzTest(const uint8_t *data, size_t size)
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
        switch (static_cast<ApiTestCmd>(number)) {
            case ApiTestCmd::UART_TEST_SET_BAUD:
                UartSetBaud(handle, convertor.params.desBaudRate);
                break;
            case ApiTestCmd::UART_TEST_SET_ATTRIBUTE:
                UartSetAttribute(handle, &convertor.params.paraAttribute);
                break;
            case ApiTestCmd::UART_TEST_SET_TRANSMODE:
                UartSetTransMode(handle, convertor.params.paraMode);
                break;
            default:
                break;
        }
        UartClose(handle);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::UartFuzzTest(data, size);
    return 0;
}
