/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef RTC_FUZZER
#define RTC_FUZZER

#define FUZZ_PROJECT_NAME "rtc_fuzzer"

enum class ApiTestCmd {
    RTC_TEST_WRITETIME = 0,
    RTC_TEST_WRITEALARM,
    RTC_TEST_ALARMINTERRUPTENABLE,
    RTC_TEST_SETFREQ,
    RTC_TEST_WRITEREG,
};

#endif
