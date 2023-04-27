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

#include "v1_1/face_auth_interface_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdi_support.h>
#include <iproxy_broker.h>
#include <iservmgr_hdi.h>
#include <message_option.h>
#include <message_parcel.h>
#include <string_ex.h>

#define HDF_LOG_TAG    face_auth_interface_proxy

namespace OHOS {
namespace HDI {
namespace FaceAuth {
namespace V1_1 {

template<typename InterfaceType>
static bool ReadInterface(MessageParcel &parcel, sptr<InterfaceType>& object);

sptr<OHOS::HDI::FaceAuth::V1_1::IFaceAuthInterface> OHOS::HDI::FaceAuth::V1_1::IFaceAuthInterface::Get(bool isStub)
{
    return IFaceAuthInterface::Get("face_auth_interface_service", isStub);
}

sptr<OHOS::HDI::FaceAuth::V1_1::IFaceAuthInterface> OHOS::HDI::FaceAuth::V1_1::IFaceAuthInterface::Get(const std::string& serviceName, bool isStub)
{
    if (isStub) {
        std::string desc = Str16ToStr8(OHOS::HDI::FaceAuth::V1_1::IFaceAuthInterface::GetDescriptor());
        void *impl = LoadHdiImpl(desc.c_str(), serviceName == "face_auth_interface_service" ? "service" : serviceName.c_str());
        if (impl == nullptr) {
            HDF_LOGE("failed to load hdi impl %{public}s", desc.data());
            return nullptr;
        }
        return reinterpret_cast<OHOS::HDI::FaceAuth::V1_1::IFaceAuthInterface *>(impl);
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

    sptr<OHOS::HDI::FaceAuth::V1_1::IFaceAuthInterface> proxy = OHOS::HDI::hdi_facecast<OHOS::HDI::FaceAuth::V1_1::IFaceAuthInterface>(remote);
    if (proxy == nullptr) {
        HDF_LOGE("%{public}s:iface_cast failed!", __func__);
        return nullptr;
    }

    uint32_t serMajorVer = 0;
    uint32_t serMinorVer = 0;
    int32_t faceAuthInterfaceRet = proxy->GetVersion(serMajorVer, serMinorVer);
    if (faceAuthInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:get version failed!", __func__);
        return nullptr;
    }

    if (serMajorVer != 1 || serMinorVer < 1) {
        HDF_LOGE("%{public}s:check version failed! version of service:%u.%u, version of client:1.1", __func__, serMajorVer, serMinorVer);
        return nullptr;
    }

    return proxy;
}


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
int32_t OHOS::HDI::FaceAuth::V1_1::FaceAuthInterfaceProxy::GetExecutorListV1_1(std::vector<sptr<OHOS::HDI::FaceAuth::V1_1::IExecutor>>& executorList)
{
    return OHOS::HDI::FaceAuth::V1_1::FaceAuthInterfaceProxy::GetExecutorListV1_1_(executorList, Remote());
}

int32_t OHOS::HDI::FaceAuth::V1_1::FaceAuthInterfaceProxy::GetExecutorList(std::vector<sptr<OHOS::HDI::FaceAuth::V1_0::IExecutor>>& executorList)
{
    return OHOS::HDI::FaceAuth::V1_0::FaceAuthInterfaceProxy::GetExecutorList_(executorList, Remote());
}

int32_t OHOS::HDI::FaceAuth::V1_1::FaceAuthInterfaceProxy::GetVersion(uint32_t& majorVer, uint32_t& minorVer)
{
    return OHOS::HDI::FaceAuth::V1_0::FaceAuthInterfaceProxy::GetVersion_(majorVer, minorVer, Remote());
}

int32_t OHOS::HDI::FaceAuth::V1_1::FaceAuthInterfaceProxy::GetExecutorListV1_1_(std::vector<sptr<OHOS::HDI::FaceAuth::V1_1::IExecutor>>& executorList, const sptr<IRemoteObject> remote)
{
    MessageParcel faceAuthInterfaceData;
    MessageParcel faceAuthInterfaceReply;
    MessageOption faceAuthInterfaceOption(MessageOption::TF_SYNC);

    if (!faceAuthInterfaceData.WriteInterfaceToken(OHOS::HDI::FaceAuth::V1_1::IFaceAuthInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!faceAuthInterfaceData.WriteBool(false)) {
        HDF_LOGE("%{public}s:failed to write flag of memory setting!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t faceAuthInterfaceRet = remote->SendRequest(CMD_FACE_AUTH_INTERFACE_GET_EXECUTOR_LIST_V1_1, faceAuthInterfaceData, faceAuthInterfaceReply, faceAuthInterfaceOption);
    if (faceAuthInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, faceAuthInterfaceRet);
        return faceAuthInterfaceRet;
    }

    uint32_t executorListSize = 0;
    if (!faceAuthInterfaceReply.ReadUint32(executorListSize)) {
        HDF_LOGE("%{public}s: failed to read size", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDI_CHECK_VALUE_RETURN(executorListSize, >, HDI_BUFF_MAX_SIZE / sizeof(sptr<OHOS::HDI::FaceAuth::V1_1::IExecutor>), HDF_ERR_INVALID_PARAM);
    executorList.clear();
    executorList.reserve(executorListSize);
    for (uint32_t i0 = 0; i0 < executorListSize; ++i0) {
        sptr<OHOS::HDI::FaceAuth::V1_1::IExecutor> value0;
        if (!ReadInterface<OHOS::HDI::FaceAuth::V1_1::IExecutor>(faceAuthInterfaceReply, value0)) {
            HDF_LOGE("%{public}s: failed to read interface object", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
        executorList.push_back(value0);
    }

    return faceAuthInterfaceRet;
}

} // V1_1
} // FaceAuth
} // HDI
} // OHOS
