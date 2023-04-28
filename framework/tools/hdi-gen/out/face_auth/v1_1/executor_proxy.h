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

#ifndef OHOS_HDI_FACE_AUTH_V1_1_EXECUTORPROXY_H
#define OHOS_HDI_FACE_AUTH_V1_1_EXECUTORPROXY_H

#include "v1_0/executor_proxy.h"
#include "v1_1/iexecutor.h"
#include <iproxy_broker.h>

namespace OHOS {
namespace HDI {
namespace FaceAuth {
namespace V1_1 {

class ExecutorProxy : public IProxyBroker<OHOS::HDI::FaceAuth::V1_1::IExecutor> {
public:
    explicit ExecutorProxy(const sptr<IRemoteObject>& remote) : IProxyBroker<OHOS::HDI::FaceAuth::V1_1::IExecutor>(remote) {}

    virtual ~ExecutorProxy() = default;

    int32_t GetProperty(const std::vector<uint64_t>& templateIdList, const std::vector<GetPropertyType>& propertyTypes,
         Property& property) override;

    int32_t SetCachedTemplates(const std::vector<uint64_t>& templateIdList) override;

    int32_t RegisterSaCommandCallback(const sptr<OHOS::HDI::FaceAuth::V1_1::ISaCommandCallback>& callbackObj) override;

    int32_t GetExecutorInfo(ExecutorInfo& executorInfo) override;

    int32_t GetTemplateInfo(uint64_t templateId, TemplateInfo& templateInfo) override;

    int32_t OnRegisterFinish(const std::vector<uint64_t>& templateIdList,
         const std::vector<uint8_t>& frameworkPublicKey, const std::vector<uint8_t>& extraInfo) override;

    int32_t Enroll(uint64_t scheduleId, const std::vector<uint8_t>& extraInfo,
         const sptr<OHOS::HDI::FaceAuth::V1_0::IExecutorCallback>& callbackObj) override;

    int32_t Authenticate(uint64_t scheduleId, const std::vector<uint64_t>& templateIdList,
         const std::vector<uint8_t>& extraInfo, const sptr<OHOS::HDI::FaceAuth::V1_0::IExecutorCallback>& callbackObj) override;

    int32_t Identify(uint64_t scheduleId, const std::vector<uint8_t>& extraInfo,
         const sptr<OHOS::HDI::FaceAuth::V1_0::IExecutorCallback>& callbackObj) override;

    int32_t Delete(const std::vector<uint64_t>& templateIdList) override;

    int32_t Cancel(uint64_t scheduleId) override;

    int32_t SendCommand(int32_t commandId, const std::vector<uint8_t>& extraInfo,
         const sptr<OHOS::HDI::FaceAuth::V1_0::IExecutorCallback>& callbackObj) override;

    int32_t SetBufferProducer(const sptr<BufferProducerSequenceable>& bufferProducer) override;

    int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer) override;

    static int32_t GetProperty_(const std::vector<uint64_t>& templateIdList,
         const std::vector<GetPropertyType>& propertyTypes, Property& property, const sptr<IRemoteObject> remote);

    static int32_t SetCachedTemplates_(const std::vector<uint64_t>& templateIdList, const sptr<IRemoteObject> remote);

    static int32_t RegisterSaCommandCallback_(const sptr<OHOS::HDI::FaceAuth::V1_1::ISaCommandCallback>& callbackObj,
         const sptr<IRemoteObject> remote);

private:
    static inline BrokerDelegator<OHOS::HDI::FaceAuth::V1_1::ExecutorProxy> delegator_;
};

} // V1_1
} // FaceAuth
} // HDI
} // OHOS

#endif // OHOS_HDI_FACE_AUTH_V1_1_EXECUTORPROXY_H