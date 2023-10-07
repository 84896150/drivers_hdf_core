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

#ifndef OHOS_HDI_TEST_V1_1_ITESTINTERFACE_H
#define OHOS_HDI_TEST_V1_1_ITESTINTERFACE_H

#include <stdint.h>
#include <vector>
#include <hdf_base.h>
#include <hdi_base.h>
#include "test/v1_0/itest_callback.h"
#include "test/v1_0/itest_interface.h"
#include "test/v1_0/test_types.h"
#include "test/v1_1/test_types.h"

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
namespace HDI {
namespace Test {
namespace V1_1 {
using namespace OHOS;
using namespace OHOS::HDI;
using namespace OHOS::HDI::Test::V1_0;

enum {
    CMD_TEST_INTERFACE_SET_TEST_CONFIG = 5,
    CMD_TEST_INTERFACE_STRUCT_VERSION_TEST = 6,
    CMD_TEST_INTERFACE_TEST_UNREGISTER = 7,
};

class ITestInterface : public OHOS::HDI::Test::V1_0::ITestInterface {
public:
    DECLARE_HDI_DESCRIPTOR(u"ohos.hdi.test.v1_1.ITestInterface");

    virtual ~ITestInterface() = default;

    static sptr<OHOS::HDI::Test::V1_1::ITestInterface> Get(bool isStub = false);
    static sptr<OHOS::HDI::Test::V1_1::ITestInterface> Get(const std::string &serviceName, bool isStub = false);

    virtual int32_t SetTestConfig(int32_t testType, const std::vector<uint8_t>& data) = 0;

    virtual int32_t StructVersionTest(int32_t testType, const std::vector<uint8_t>& data,
         const OHOS::HDI::Test::V1_1::WaveParam& testWPIn, OHOS::HDI::Test::V1_0::WaveParam& testWPOut, const WaveParam& testWPNoVersion) = 0;

    virtual int32_t TestUnregister(const sptr<OHOS::HDI::Test::V1_0::ITestCallback>& callbackObj) = 0;

    int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer) override
    {
        majorVer = 1;
        minorVer = 1;
        return HDF_SUCCESS;
    }

    const std::u16string GetDesc() override
    {
        return metaDescriptor_;
    }
};
} // V1_1
} // Test
} // HDI
} // OHOS

#endif // OHOS_HDI_TEST_V1_1_ITESTINTERFACE_H