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

#ifndef OHOS_HDI_TEST_V1_0_TESTCALLBACKSTUB_H
#define OHOS_HDI_TEST_V1_0_TESTCALLBACKSTUB_H

#include <ipc_object_stub.h>
#include <message_option.h>
#include <message_parcel.h>
#include <object_collector.h>
#include <refbase.h>
#include "v1_0/itest_callback.h"

namespace OHOS {
namespace HDI {
namespace Test {
namespace V1_0 {

using namespace OHOS;
class TestCallbackStub : public IPCObjectStub {
public:
    explicit TestCallbackStub(const sptr<ITestCallback> &impl);
    virtual ~TestCallbackStub();

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    static int32_t TestCallbackStubOnDataEvent_(MessageParcel& testCallbackData, MessageParcel& testCallbackReply, MessageOption& testCallbackOption, sptr<OHOS::HDI::Test::V1_0::ITestCallback> impl);

    static int32_t TestCallbackStubGetVersion_(MessageParcel& testCallbackData, MessageParcel& testCallbackReply, MessageOption& testCallbackOption, sptr<OHOS::HDI::Test::V1_0::ITestCallback> impl);

private:
    int32_t TestCallbackStubOnDataEvent(MessageParcel& testCallbackData, MessageParcel& testCallbackReply, MessageOption& testCallbackOption);

    int32_t TestCallbackStubGetVersion(MessageParcel& testCallbackData, MessageParcel& testCallbackReply, MessageOption& testCallbackOption);


    static inline ObjectDelegator<OHOS::HDI::Test::V1_0::TestCallbackStub, OHOS::HDI::Test::V1_0::ITestCallback> objDelegator_;
    sptr<OHOS::HDI::Test::V1_0::ITestCallback> impl_;
};
} // V1_0
} // Test
} // HDI
} // OHOS

#endif // OHOS_HDI_TEST_V1_0_TESTCALLBACKSTUB_H