/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "rtc_fuzzer.h"
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include "hdf_base.h"
#include "random.h"
#include "rtc_if.h"
#include "securec.h"


using namespace std;

namespace {
constexpr int32_t MIN = 0;
constexpr int32_t MAX = 2;
const uint8_t RTC_USER_INDEX = 8;
const int32_t MAX_LEN = 15;
}


struct AllParameters {
    uint32_t desFreq;
    uint8_t desValue;
    uint8_t desEnable;
};

union ConvertToParam {
    uint8_t data[MAX_LEN];
    struct AllParameters params;
    struct RtcTime paraTime;
    enum RtcAlarmIndex paraAlarmIndex;
    RtcAlarmCallback paraCb;
};

namespace OHOS {
    bool RtcFuzzTest(const uint8_t* data, size_t size)
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
        handle = RtcOpen();
        switch (static_cast<ApiNumber>(number)) {
            case ApiNumber::NUM_ZERO:
                RtcWriteTime(handle, &convertor.paraTime);
                break;
            case ApiNumber::NUM_ONE:
                RtcReadAlarm(handle, convertor.paraAlarmIndex, &convertor.paraTime);
                break;
            case ApiNumber::NUM_TWO:
                RtcWriteAlarm(handle, convertor.paraAlarmIndex,  &convertor.paraTime);
                break;
            case ApiNumber::NUM_THREE:
                RtcRegisterAlarmCallback(handle, convertor.paraAlarmIndex, convertor.paraCb);
                break;
            case ApiNumber::NUM_FOUR:
                RtcAlarmInterruptEnable(handle, convertor.paraAlarmIndex, convertor.params.desEnable);
                break;
            case ApiNumber::NUM_FIVE:
                RtcSetFreq(handle, convertor.params.desFreq);
                break;
            case ApiNumber::NUM_SIX:
                RtcWriteReg(handle, RTC_USER_INDEX, convertor.params.desValue);
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
    OHOS::RtcFuzzTest(data, size);
    return 0;
}
