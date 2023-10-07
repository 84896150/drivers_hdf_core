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

#ifndef OHOS_HDI_TEST_V1_1_TESTINTERFACESTUB_H
#define OHOS_HDI_TEST_V1_1_TESTINTERFACESTUB_H

#include <ipc_object_stub.h>
#include <message_option.h>
#include <message_parcel.h>
#include <object_collector.h>
#include <refbase.h>
#include "v1_0/test_interface_stub.h"
#include "v1_1/itest_interface.h"

namespace OHOS {
namespace HDI {
namespace Test {
namespace V1_1 {

using namespace OHOS;
class TestInterfaceStub : public IPCObjectStub {
public:
    explicit TestInterfaceStub(const sptr<ITestInterface> &impl);
    virtual ~TestInterfaceStub();

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    static int32_t TestInterfaceStubSetTestConfig_(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption, sptr<OHOS::HDI::Test::V1_1::ITestInterface> impl);

    static int32_t TestInterfaceStubStructVersionTest_(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption, sptr<OHOS::HDI::Test::V1_1::ITestInterface> impl);

    static int32_t TestInterfaceStubTestUnregister_(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption, sptr<OHOS::HDI::Test::V1_1::ITestInterface> impl);

private:
    int32_t TestInterfaceStubSetTestConfig(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption);

    int32_t TestInterfaceStubStructVersionTest(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption);

    int32_t TestInterfaceStubTestUnregister(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption);

    int32_t TestInterfaceStubEnableTest(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption);

    int32_t TestInterfaceStubDisableTest(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption);

    int32_t TestInterfaceStubRegister(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption);

    int32_t TestInterfaceStubUnregister(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption);

    int32_t TestInterfaceStubGetVersion(MessageParcel& testInterfaceData, MessageParcel& testInterfaceReply, MessageOption& testInterfaceOption);


    static inline ObjectDelegator<OHOS::HDI::Test::V1_1::TestInterfaceStub, OHOS::HDI::Test::V1_1::ITestInterface> objDelegator_;
    sptr<OHOS::HDI::Test::V1_1::ITestInterface> impl_;
};
} // V1_1
} // Test
} // HDI
} // OHOS

#endif // OHOS_HDI_TEST_V1_1_TESTINTERFACESTUB_H