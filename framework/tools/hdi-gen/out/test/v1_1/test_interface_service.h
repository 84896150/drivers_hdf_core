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

#ifndef OHOS_HDI_TEST_V1_1_TESTINTERFACESERVICE_H
#define OHOS_HDI_TEST_V1_1_TESTINTERFACESERVICE_H

#include "v1_1/itest_interface.h"

namespace OHOS {
namespace HDI {
namespace Test {
namespace V1_1 {
class TestInterfaceService : public OHOS::HDI::Test::V1_1::ITestInterface {
public:
    TestInterfaceService() = default;
    virtual ~TestInterfaceService() = default;

    int32_t SetTestConfig(int32_t testType, const std::vector<uint8_t>& data) override;

    int32_t StructVersionTest(int32_t testType, const std::vector<uint8_t>& data,
         const OHOS::HDI::Test::V1_1::WaveParam& testWPIn, OHOS::HDI::Test::V1_0::WaveParam& testWPOut, const WaveParam& testWPNoVersion) override;

    int32_t TestUnregister(const sptr<OHOS::HDI::Test::V1_0::ITestCallback>& callbackObj) override;

    int32_t EnableTest(int32_t testType) override;

    int32_t DisableTest(int32_t testType) override;

    int32_t Register(const sptr<OHOS::HDI::Test::V1_0::ITestCallback>& callbackObj) override;

    int32_t Unregister(const sptr<OHOS::HDI::Test::V1_0::ITestCallback>& callbackObj) override;

};
} // V1_1
} // Test
} // HDI
} // OHOS

#endif // OHOS_HDI_TEST_V1_1_TESTINTERFACESERVICE_H