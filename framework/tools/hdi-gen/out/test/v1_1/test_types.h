/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_HDI_TEST_V1_1_TESTTYPES_H
#define OHOS_HDI_TEST_V1_1_TESTTYPES_H

#include <cstdbool>
#include <cstdint>
#include "test/v1_0/test_types.h"

#ifndef HDI_BUFF_MAX_SIZE
#define HDI_BUFF_MAX_SIZE (1024 * 200)
#endif

#ifndef HDI_CHECK_VALUE_RETURN
#define HDI_CHECK_VALUE_RETURN(lv, compare, rv, ret) do { \
    if ((lv) compare (rv)) { \
        return ret; \
    } \
} while (false)
#endif

#ifndef HDI_CHECK_VALUE_RET_GOTO
#define HDI_CHECK_VALUE_RET_GOTO(lv, compare, rv, ret, value, table) do { \
    if ((lv) compare (rv)) { \
        ret = value; \
        goto table; \
    } \
} while (false)
#endif

namespace OHOS {
class MessageParcel;
}

namespace OHOS {
namespace HDI {
namespace Test {
namespace V1_1 {

using namespace OHOS;
using namespace OHOS::HDI::Test::V1_0;

struct WaveParam {
    int32_t waveFrequency;
    int32_t waveAmplitude;
    bool isUseGyroscope;
} __attribute__ ((aligned(8)));

bool WaveParamBlockMarshalling(OHOS::MessageParcel &data, const WaveParam& dataBlock);

bool WaveParamBlockUnmarshalling(OHOS::MessageParcel &data, WaveParam& dataBlock);

} // V1_1
} // Test
} // HDI
} // OHOS

#endif // OHOS_HDI_TEST_V1_1_TESTTYPES_H