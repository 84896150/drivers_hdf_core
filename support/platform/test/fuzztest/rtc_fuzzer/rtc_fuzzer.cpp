/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "rtc_fuzzer.h"
#include <iostream>
#include "random.h"
#include "securec.h"
#include "hdf_base.h"
#include "rtc_if.h"

using namespace std;

namespace {
constexpr int32_t MIN = 0;
constexpr int32_t MAX = 4;
const uint8_t RTC_USER_INDEX = 8;
const int32_t MAX_LEN = 15;
}

struct AllParameters {
    uint32_t desFreq;
    uint8_t desValue;
    uint8_t desEnable;
    struct RtcTime paraTime;
    enum RtcAlarmIndex paraAlarmIndex;
};

union ConvertToParam {
    uint8_t data[MAX_LEN];
    struct AllParameters params;
};

namespace OHOS {
    bool RtcFuzzTest(const uint8_t *data, size_t size)
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
        switch (static_cast<ApiTestCmd>(number)) {
            case ApiTestCmd::RTC_TEST_WRITETIME:
                RtcWriteTime(handle, &convertor.params.paraTime);
                break;
            case ApiTestCmd::RTC_TEST_WRITEALARM:
                RtcWriteAlarm(handle, convertor.params.paraAlarmIndex,  &convertor.params.paraTime);
                break;
            case ApiTestCmd::RTC_TEST_ALARMINTERRUPTENABLE:
                RtcAlarmInterruptEnable(handle, convertor.params.paraAlarmIndex, convertor.params.desEnable);
                break;
            case ApiTestCmd::RTC_TEST_SETFREQ:
                RtcSetFreq(handle, convertor.params.desFreq);
                break;
            case ApiTestCmd::RTC_TEST_WRITEREG:
                RtcWriteReg(handle, RTC_USER_INDEX, convertor.params.desValue);
                break;
            default:
                break;
        }
        RtcClose(handle);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::RtcFuzzTest(data, size);
    return 0;
}
