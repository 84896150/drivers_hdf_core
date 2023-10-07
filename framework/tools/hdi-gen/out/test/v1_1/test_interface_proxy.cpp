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

#include "v1_1/test_interface_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdi_support.h>
#include <iservmgr_hdi.h>
#include <message_option.h>
#include <message_parcel.h>
#include <object_collector.h>
#include <string_ex.h>

#define HDF_LOG_TAG    test_interface_proxy

namespace OHOS {
namespace HDI {
namespace Test {
namespace V1_1 {

template<typename ElementType>
static bool WritePodArray(MessageParcel &parcel, const std::vector<ElementType> &data);

sptr<OHOS::HDI::Test::V1_1::ITestInterface> OHOS::HDI::Test::V1_1::ITestInterface::Get(bool isStub)
{
    return ITestInterface::Get("test_interface_service", isStub);
}

sptr<OHOS::HDI::Test::V1_1::ITestInterface> OHOS::HDI::Test::V1_1::ITestInterface::Get(const std::string& serviceName, bool isStub)
{
    if (isStub) {
        std::string desc = Str16ToStr8(OHOS::HDI::Test::V1_1::ITestInterface::GetDescriptor());
        void *impl = LoadHdiImpl(desc.c_str(), serviceName == "test_interface_service" ? "service" : serviceName.c_str());
        if (impl == nullptr) {
            HDF_LOGE("failed to load hdi impl %{public}s", desc.data());
            return nullptr;
        }
        return reinterpret_cast<OHOS::HDI::Test::V1_1::ITestInterface *>(impl);
    }

    using namespace OHOS::HDI::ServiceManager::V1_0;
    auto servMgr = IServiceManager::Get();
    if (servMgr == nullptr) {
        HDF_LOGE("%{public}s:get IServiceManager failed!", __func__);
        return nullptr;
    }

    sptr<IRemoteObject> remote = servMgr->GetService(serviceName.c_str());
    if (remote == nullptr) {
        HDF_LOGE("%{public}s:get remote object failed!", __func__);
        return nullptr;
    }

    sptr<OHOS::HDI::Test::V1_1::ITestInterface> proxy = OHOS::HDI::hdi_facecast<OHOS::HDI::Test::V1_1::ITestInterface>(remote);
    if (proxy == nullptr) {
        HDF_LOGE("%{public}s:iface_cast failed!", __func__);
        return nullptr;
    }

    uint32_t serMajorVer = 0;
    uint32_t serMinorVer = 0;
    int32_t testInterfaceRet = proxy->GetVersion(serMajorVer, serMinorVer);
    if (testInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:get version failed!", __func__);
        return nullptr;
    }

    if (serMajorVer != 1) {
        HDF_LOGE("%{public}s:check version failed! version of service:%u.%u, version of client:1.1", __func__, serMajorVer, serMinorVer);
        return nullptr;
    }

    return proxy;
}


template<typename ElementType>
static bool WritePodArray(MessageParcel &parcel, const std::vector<ElementType> &data)
{
    if (!parcel.WriteUint32(data.size())) {
        HDF_LOGE("%{public}s: failed to write data size", __func__);
        return false;
    }
    if (data.empty()) {
        return true;
    }
    if (!parcel.WriteUnpadBuffer((const void*)data.data(), sizeof(ElementType) * data.size())) {
        HDF_LOGE("%{public}s: failed to write array", __func__);
        return false;
    }
    return true;
}
int32_t OHOS::HDI::Test::V1_1::TestInterfaceProxy::SetTestConfig(int32_t testType, const std::vector<uint8_t>& data)
{
    return OHOS::HDI::Test::V1_1::TestInterfaceProxy::SetTestConfig_(testType, data, Remote());
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceProxy::StructVersionTest(int32_t testType, const std::vector<uint8_t>& data,
     const OHOS::HDI::Test::V1_1::WaveParam& testWPIn, OHOS::HDI::Test::V1_0::WaveParam& testWPOut, const WaveParam& testWPNoVersion)
{
    return OHOS::HDI::Test::V1_1::TestInterfaceProxy::StructVersionTest_(testType, data, testWPIn, testWPOut, testWPNoVersion, Remote());
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceProxy::TestUnregister(const sptr<OHOS::HDI::Test::V1_0::ITestCallback>& callbackObj)
{
    return OHOS::HDI::Test::V1_1::TestInterfaceProxy::TestUnregister_(callbackObj, Remote());
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceProxy::EnableTest(int32_t testType)
{
    return OHOS::HDI::Test::V1_0::TestInterfaceProxy::EnableTest_(testType, Remote());
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceProxy::DisableTest(int32_t testType)
{
    return OHOS::HDI::Test::V1_0::TestInterfaceProxy::DisableTest_(testType, Remote());
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceProxy::Register(const sptr<OHOS::HDI::Test::V1_0::ITestCallback>& callbackObj)
{
    return OHOS::HDI::Test::V1_0::TestInterfaceProxy::Register_(callbackObj, Remote());
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceProxy::Unregister(const sptr<OHOS::HDI::Test::V1_0::ITestCallback>& callbackObj)
{
    return OHOS::HDI::Test::V1_0::TestInterfaceProxy::Unregister_(callbackObj, Remote());
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceProxy::GetVersion(uint32_t& majorVer, uint32_t& minorVer)
{
    return OHOS::HDI::Test::V1_0::TestInterfaceProxy::GetVersion_(majorVer, minorVer, Remote());
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceProxy::SetTestConfig_(int32_t testType, const std::vector<uint8_t>& data,
     const sptr<IRemoteObject> remote)
{
    MessageParcel testInterfaceData;
    MessageParcel testInterfaceReply;
    MessageOption testInterfaceOption(MessageOption::TF_SYNC);

    if (!testInterfaceData.WriteInterfaceToken(OHOS::HDI::Test::V1_1::ITestInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!testInterfaceData.WriteInt32(testType)) {
        HDF_LOGE("%{public}s: write testType failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!WritePodArray(testInterfaceData, data)) {
        HDF_LOGE("%{public}s: failed to write data", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t testInterfaceRet = remote->SendRequest(CMD_TEST_INTERFACE_SET_TEST_CONFIG, testInterfaceData, testInterfaceReply, testInterfaceOption);
    if (testInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, testInterfaceRet);
        return testInterfaceRet;
    }

    return testInterfaceRet;
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceProxy::StructVersionTest_(int32_t testType,
     const std::vector<uint8_t>& data, const OHOS::HDI::Test::V1_1::WaveParam& testWPIn, OHOS::HDI::Test::V1_0::WaveParam& testWPOut, const WaveParam& testWPNoVersion, const sptr<IRemoteObject> remote)
{
    MessageParcel testInterfaceData;
    MessageParcel testInterfaceReply;
    MessageOption testInterfaceOption(MessageOption::TF_SYNC);

    if (!testInterfaceData.WriteInterfaceToken(OHOS::HDI::Test::V1_1::ITestInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!testInterfaceData.WriteInt32(testType)) {
        HDF_LOGE("%{public}s: write testType failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!WritePodArray(testInterfaceData, data)) {
        HDF_LOGE("%{public}s: failed to write data", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!WaveParamBlockMarshalling(testInterfaceData, testWPIn)) {
        HDF_LOGE("%{public}s: write testWPIn failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!WaveParamBlockMarshalling(testInterfaceData, testWPNoVersion)) {
        HDF_LOGE("%{public}s: write testWPNoVersion failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t testInterfaceRet = remote->SendRequest(CMD_TEST_INTERFACE_STRUCT_VERSION_TEST, testInterfaceData, testInterfaceReply, testInterfaceOption);
    if (testInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, testInterfaceRet);
        return testInterfaceRet;
    }

    if (!WaveParamBlockUnmarshalling(testInterfaceReply, testWPOut)) {
        HDF_LOGE("%{public}s: read testWPOut failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return testInterfaceRet;
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceProxy::TestUnregister_(const sptr<OHOS::HDI::Test::V1_0::ITestCallback>& callbackObj, const sptr<IRemoteObject> remote)
{
    MessageParcel testInterfaceData;
    MessageParcel testInterfaceReply;
    MessageOption testInterfaceOption(MessageOption::TF_SYNC);

    if (!testInterfaceData.WriteInterfaceToken(OHOS::HDI::Test::V1_1::ITestInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: parameter callbackObj is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!testInterfaceData.WriteRemoteObject(OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(callbackObj, OHOS::HDI::Test::V1_0::ITestCallback::GetDescriptor()))) {
        HDF_LOGE("%{public}s: write callbackObj failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t testInterfaceRet = remote->SendRequest(CMD_TEST_INTERFACE_TEST_UNREGISTER, testInterfaceData, testInterfaceReply, testInterfaceOption);
    if (testInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, testInterfaceRet);
        return testInterfaceRet;
    }

    return testInterfaceRet;
}

} // V1_1
} // Test
} // HDI
} // OHOS
