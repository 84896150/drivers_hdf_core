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

#include "v1_1/test_interface_service.h"
#include <hdf_base.h>

#define HDF_LOG_TAG    test_interface_service

namespace OHOS {
namespace HDI {
namespace Test {
namespace V1_1 {
extern "C" ITestInterface *TestInterfaceImplGetInstance(void)
{
    return new (std::nothrow) TestInterfaceService();
}

int32_t TestInterfaceService::SetTestConfig(int32_t testType, const std::vector<uint8_t>& data)
{
    return HDF_SUCCESS;
}

int32_t TestInterfaceService::StructVersionTest(int32_t testType, const std::vector<uint8_t>& data,
     const OHOS::HDI::Test::V1_1::WaveParam& testWPIn, OHOS::HDI::Test::V1_0::WaveParam& testWPOut, const WaveParam& testWPNoVersion)
{
    return HDF_SUCCESS;
}

int32_t TestInterfaceService::TestUnregister(const sptr<OHOS::HDI::Test::V1_0::ITestCallback>& callbackObj)
{
    return HDF_SUCCESS;
}

int32_t TestInterfaceService::EnableTest(int32_t testType)
{
    return HDF_SUCCESS;
}

int32_t TestInterfaceService::DisableTest(int32_t testType)
{
    return HDF_SUCCESS;
}

int32_t TestInterfaceService::Register(const sptr<OHOS::HDI::Test::V1_0::ITestCallback>& callbackObj)
{
    return HDF_SUCCESS;
}

int32_t TestInterfaceService::Unregister(const sptr<OHOS::HDI::Test::V1_0::ITestCallback>& callbackObj)
{
    return HDF_SUCCESS;
}

} // V1_1
} // Test
} // HDI
} // OHOS
