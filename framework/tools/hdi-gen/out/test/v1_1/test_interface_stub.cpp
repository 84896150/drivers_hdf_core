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

#include "v1_1/test_interface_stub.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdi_support.h>
#include <iproxy_broker.h>
#include <securec.h>
#include <string_ex.h>

#define HDF_LOG_TAG    test_interface_stub

namespace OHOS {
namespace HDI {
namespace Test {
namespace V1_1 {
template<typename InterfaceType>
static bool ReadInterface(MessageParcel &parcel, sptr<InterfaceType>& object);
template<typename ElementType>
static bool ReadPodArray(MessageParcel &parcel, std::vector<ElementType> &data);


template<typename InterfaceType>
static bool ReadInterface(MessageParcel &parcel, sptr<InterfaceType>& object)
{
    sptr<IRemoteObject> remote = parcel.ReadRemoteObject();
    if (remote == nullptr) {
        HDF_LOGE("%{public}s: read an invalid remote object", __func__);
        return false;
    }

    object = hdi_facecast<InterfaceType>(remote);
    if (object == nullptr) {
        HDF_LOGE("%{public}s: failed to cast interface object", __func__);
        return false;
    }
    return true;
}

template<typename ElementType>
static bool ReadPodArray(MessageParcel &parcel, std::vector<ElementType> &data)
{
    data.clear();
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        HDF_LOGE("%{public}s: failed to read size", __func__);
        return false;
    }

    if (size == 0) {
        return true;
    }
    const ElementType *dataPtr = reinterpret_cast<const ElementType*>(parcel.ReadUnpadBuffer(sizeof(ElementType) * size));
    if (dataPtr == nullptr) {
        HDF_LOGI("%{public}s: failed to read data", __func__);
        return false;
    }
    data.assign(dataPtr, dataPtr + size);
    return true;
}

sptr<OHOS::HDI::Test::V1_1::ITestInterface> OHOS::HDI::Test::V1_1::ITestInterface::Get(bool isStub)
{
    return OHOS::HDI::Test::V1_1::ITestInterface::Get("test_interface_service", isStub);
}

sptr<OHOS::HDI::Test::V1_1::ITestInterface> OHOS::HDI::Test::V1_1::ITestInterface::Get(const std::string& serviceName, bool isStub)
{
    if (!isStub) {
        return nullptr;
    }
    std::string desc = Str16ToStr8(OHOS::HDI::Test::V1_1::ITestInterface::GetDescriptor());
    void *impl = LoadHdiImpl(desc.c_str(), serviceName == "test_interface_service" ? "service" : serviceName.c_str());
    if (impl == nullptr) {
        HDF_LOGE("failed to load hdi impl %{public}s", desc.c_str());
        return nullptr;
    }
    return reinterpret_cast<OHOS::HDI::Test::V1_1::ITestInterface *>(impl);
}


OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStub(const sptr<OHOS::HDI::Test::V1_1::ITestInterface> &impl)
    : IPCObjectStub(OHOS::HDI::Test::V1_1::ITestInterface::GetDescriptor()), impl_(impl)
{
}

OHOS::HDI::Test::V1_1::TestInterfaceStub::~TestInterfaceStub()
{
    ObjectCollector::GetInstance().RemoveObject(impl_);
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    switch (code) {
        case CMD_TEST_INTERFACE_GET_VERSION:
            return TestInterfaceStubGetVersion(data, reply, option);
        case CMD_TEST_INTERFACE_SET_TEST_CONFIG:
            return TestInterfaceStubSetTestConfig(data, reply, option);
        case CMD_TEST_INTERFACE_STRUCT_VERSION_TEST:
            return TestInterfaceStubStructVersionTest(data, reply, option);
        case CMD_TEST_INTERFACE_TEST_UNREGISTER:
            return TestInterfaceStubTestUnregister(data, reply, option);
        case CMD_TEST_INTERFACE_ENABLE_TEST:
            return TestInterfaceStubEnableTest(data, reply, option);
        case CMD_TEST_INTERFACE_DISABLE_TEST:
            return TestInterfaceStubDisableTest(data, reply, option);
        case CMD_TEST_INTERFACE_REGISTER:
            return TestInterfaceStubRegister(data, reply, option);
        case CMD_TEST_INTERFACE_UNREGISTER:
            return TestInterfaceStubUnregister(data, reply, option);
        default: {
            HDF_LOGE("%{public}s: cmd %{public}d is not supported", __func__, code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubSetTestConfig(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption)
{
    return OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubSetTestConfig_(testInterfaceData, testInterfaceReply, testInterfaceOption, impl_);
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubStructVersionTest(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption)
{
    return OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubStructVersionTest_(testInterfaceData, testInterfaceReply, testInterfaceOption, impl_);
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubTestUnregister(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption)
{
    return OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubTestUnregister_(testInterfaceData, testInterfaceReply, testInterfaceOption, impl_);
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubEnableTest(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption)
{
    return OHOS::HDI::Test::V1_0::TestInterfaceStub::TestInterfaceStubEnableTest_(testInterfaceData, testInterfaceReply, testInterfaceOption, impl_);
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubDisableTest(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption)
{
    return OHOS::HDI::Test::V1_0::TestInterfaceStub::TestInterfaceStubDisableTest_(testInterfaceData, testInterfaceReply, testInterfaceOption, impl_);
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubRegister(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption)
{
    return OHOS::HDI::Test::V1_0::TestInterfaceStub::TestInterfaceStubRegister_(testInterfaceData, testInterfaceReply, testInterfaceOption, impl_);
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubUnregister(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption)
{
    return OHOS::HDI::Test::V1_0::TestInterfaceStub::TestInterfaceStubUnregister_(testInterfaceData, testInterfaceReply, testInterfaceOption, impl_);
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubGetVersion(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption)
{
    return OHOS::HDI::Test::V1_0::TestInterfaceStub::TestInterfaceStubGetVersion_(testInterfaceData, testInterfaceReply, testInterfaceOption, impl_);
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubSetTestConfig_(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption, sptr<OHOS::HDI::Test::V1_1::ITestInterface> impl)
{
    if (testInterfaceData.ReadInterfaceToken() != OHOS::HDI::Test::V1_1::ITestInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t testType = 0;
    if (!testInterfaceData.ReadInt32(testType)) {
        HDF_LOGE("%{public}s: read testType failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    std::vector<uint8_t> data;
    if (!ReadPodArray(testInterfaceData, data)) {
        HDF_LOGE("%{public}s: failed to read data", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t testInterfaceRet = impl->SetTestConfig(testType, data);
    if (testInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, testInterfaceRet);
        return testInterfaceRet;
    }

    return testInterfaceRet;
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubStructVersionTest_(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption, sptr<OHOS::HDI::Test::V1_1::ITestInterface> impl)
{
    if (testInterfaceData.ReadInterfaceToken() != OHOS::HDI::Test::V1_1::ITestInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t testType = 0;
    if (!testInterfaceData.ReadInt32(testType)) {
        HDF_LOGE("%{public}s: read testType failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    std::vector<uint8_t> data;
    if (!ReadPodArray(testInterfaceData, data)) {
        HDF_LOGE("%{public}s: failed to read data", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    WaveParam testWPIn;
    if (!WaveParamBlockUnmarshalling(testInterfaceData, testWPIn)) {
        HDF_LOGE("%{public}s: read testWPIn failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    WaveParam testWPOut;

    WaveParam testWPNoVersion;
    if (!WaveParamBlockUnmarshalling(testInterfaceData, testWPNoVersion)) {
        HDF_LOGE("%{public}s: read testWPNoVersion failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t testInterfaceRet = impl->StructVersionTest(testType, data, testWPIn, testWPOut, testWPNoVersion);
    if (testInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, testInterfaceRet);
        return testInterfaceRet;
    }

    if (!WaveParamBlockMarshalling(testInterfaceReply, testWPOut)) {
        HDF_LOGE("%{public}s: write testWPOut failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return testInterfaceRet;
}

int32_t OHOS::HDI::Test::V1_1::TestInterfaceStub::TestInterfaceStubTestUnregister_(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption, sptr<OHOS::HDI::Test::V1_1::ITestInterface> impl)
{
    if (testInterfaceData.ReadInterfaceToken() != OHOS::HDI::Test::V1_1::ITestInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    sptr<OHOS::HDI::Test::V1_0::ITestCallback> callbackObj;
    if (!ReadInterface<OHOS::HDI::Test::V1_0::ITestCallback>(testInterfaceData, callbackObj)) {
        HDF_LOGE("%{public}s: failed to read interface object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t testInterfaceRet = impl->TestUnregister(callbackObj);
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
