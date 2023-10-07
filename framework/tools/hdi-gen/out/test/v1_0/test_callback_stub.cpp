/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "v1_0/test_callback_stub.h"
#include <hdf_base.h>
#include <hdf_log.h>

#define HDF_LOG_TAG    test_callback_stub

namespace OHOS {
namespace HDI {
namespace Test {
namespace V1_0 {



OHOS::HDI::Test::V1_0::TestCallbackStub::TestCallbackStub(const sptr<OHOS::HDI::Test::V1_0::ITestCallback> &impl)
    : IPCObjectStub(OHOS::HDI::Test::V1_0::ITestCallback::GetDescriptor()), impl_(impl)
{
}

OHOS::HDI::Test::V1_0::TestCallbackStub::~TestCallbackStub()
{
    ObjectCollector::GetInstance().RemoveObject(impl_);
}

int32_t OHOS::HDI::Test::V1_0::TestCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    switch (code) {
        case CMD_TEST_CALLBACK_GET_VERSION:
            return TestCallbackStubGetVersion(data, reply, option);
        case CMD_TEST_CALLBACK_ON_DATA_EVENT:
            return TestCallbackStubOnDataEvent(data, reply, option);
        default: {
            HDF_LOGE("%{public}s: cmd %{public}d is not supported", __func__, code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t OHOS::HDI::Test::V1_0::TestCallbackStub::TestCallbackStubOnDataEvent(MessageParcel& testCallbackData, MessageParcel& testCallbackReply, MessageOption& testCallbackOption)
{
    return OHOS::HDI::Test::V1_0::TestCallbackStub::TestCallbackStubOnDataEvent_(testCallbackData, testCallbackReply, testCallbackOption, impl_);
}

int32_t OHOS::HDI::Test::V1_0::TestCallbackStub::TestCallbackStubGetVersion(MessageParcel& testCallbackData, MessageParcel& testCallbackReply, MessageOption& testCallbackOption)
{
    return OHOS::HDI::Test::V1_0::TestCallbackStub::TestCallbackStubGetVersion_(testCallbackData, testCallbackReply, testCallbackOption, impl_);
}

int32_t OHOS::HDI::Test::V1_0::TestCallbackStub::TestCallbackStubOnDataEvent_(MessageParcel& testCallbackData, MessageParcel& testCallbackReply, MessageOption& testCallbackOption, sptr<OHOS::HDI::Test::V1_0::ITestCallback> impl)
{
    if (testCallbackData.ReadInterfaceToken() != OHOS::HDI::Test::V1_0::ITestCallback::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HdfTestEvent event;
    if (!HdfTestEventBlockUnmarshalling(testCallbackData, event)) {
        HDF_LOGE("%{public}s: read event failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t testCallbackRet = impl->OnDataEvent(event);
    if (testCallbackRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, testCallbackRet);
        return testCallbackRet;
    }

    return testCallbackRet;
}

int32_t OHOS::HDI::Test::V1_0::TestCallbackStub::TestCallbackStubGetVersion_(MessageParcel& testCallbackData, MessageParcel& testCallbackReply, MessageOption& testCallbackOption, sptr<OHOS::HDI::Test::V1_0::ITestCallback> impl)
{
    if (testCallbackData.ReadInterfaceToken() != OHOS::HDI::Test::V1_0::ITestCallback::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint32_t majorVer = 0;

    uint32_t minorVer = 0;

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t testCallbackRet = impl->GetVersion(majorVer, minorVer);
    if (testCallbackRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, testCallbackRet);
        return testCallbackRet;
    }

    if (!testCallbackReply.WriteUint32(majorVer)) {
        HDF_LOGE("%{public}s: write majorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!testCallbackReply.WriteUint32(minorVer)) {
        HDF_LOGE("%{public}s: write minorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return testCallbackRet;
}
} // V1_0
} // Test
} // HDI
} // OHOS
