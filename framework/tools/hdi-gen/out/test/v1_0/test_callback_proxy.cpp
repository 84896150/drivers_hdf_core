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

#include "v1_0/test_callback_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdi_support.h>
#include <message_option.h>
#include <message_parcel.h>
#include <string_ex.h>

#define HDF_LOG_TAG    test_callback_proxy

namespace OHOS {
namespace HDI {
namespace Test {
namespace V1_0 {


int32_t OHOS::HDI::Test::V1_0::TestCallbackProxy::OnDataEvent(const HdfTestEvent& event)
{
    return OHOS::HDI::Test::V1_0::TestCallbackProxy::OnDataEvent_(event, Remote());
}

int32_t OHOS::HDI::Test::V1_0::TestCallbackProxy::GetVersion(uint32_t& majorVer, uint32_t& minorVer)
{
    return OHOS::HDI::Test::V1_0::TestCallbackProxy::GetVersion_(majorVer, minorVer, Remote());
}

int32_t OHOS::HDI::Test::V1_0::TestCallbackProxy::OnDataEvent_(const HdfTestEvent& event,
     const sptr<IRemoteObject> remote)
{
    MessageParcel testCallbackData;
    MessageParcel testCallbackReply;
    MessageOption testCallbackOption(MessageOption::TF_SYNC);

    if (!testCallbackData.WriteInterfaceToken(OHOS::HDI::Test::V1_0::ITestCallback::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfTestEventBlockMarshalling(testCallbackData, event)) {
        HDF_LOGE("%{public}s: write event failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t testCallbackRet = remote->SendRequest(CMD_TEST_CALLBACK_ON_DATA_EVENT, testCallbackData, testCallbackReply, testCallbackOption);
    if (testCallbackRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, testCallbackRet);
        return testCallbackRet;
    }

    return testCallbackRet;
}

int32_t OHOS::HDI::Test::V1_0::TestCallbackProxy::GetVersion_(uint32_t& majorVer, uint32_t& minorVer,
     const sptr<IRemoteObject> remote)
{
    MessageParcel testCallbackData;
    MessageParcel testCallbackReply;
    MessageOption testCallbackOption(MessageOption::TF_SYNC);

    if (!testCallbackData.WriteInterfaceToken(OHOS::HDI::Test::V1_0::ITestCallback::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t testCallbackRet = remote->SendRequest(CMD_TEST_CALLBACK_GET_VERSION, testCallbackData, testCallbackReply, testCallbackOption);
    if (testCallbackRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, testCallbackRet);
        return testCallbackRet;
    }

    if (!testCallbackReply.ReadUint32(majorVer)) {
        HDF_LOGE("%{public}s: read majorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!testCallbackReply.ReadUint32(minorVer)) {
        HDF_LOGE("%{public}s: read minorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return testCallbackRet;
}

} // V1_0
} // Test
} // HDI
} // OHOS
