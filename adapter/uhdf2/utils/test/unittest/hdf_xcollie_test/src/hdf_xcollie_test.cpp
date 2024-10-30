/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <chrono>
#include <hdf_core_log.h>
#include <iostream>
#include <string>
#include <thread>
#include "hdf_xcollie_test.h"
#include "xcollie/xcollie.h"

using namespace testing::ext;

namespace OHOS {

void HdfUtilsTest::SetUpTestCase()
{}

void HdfUtilsTest::TearDownTestCase()
{}

void HdfUtilsTest::SetUp()
{}

void HdfUtilsTest::TearDown()
{}

using namespace std;
using namespace OHOS::HiviewDFX;

// allocate illegal buffer
HWTEST_F(HdfUtilsTest, HdfUtilsTest001, TestSize.Level1)
{
    HDF_LOGI("Hello: %s", __func__);
    cout << "Hello" << endl;

    XCollieCallback callback = [](void *){
        cout << "callback invoke" << endl;
        // std::abort();
    };

    XCollie& inst = XCollie::GetInstance();
    int id = inst.SetTimer(string(__func__), 1, callback, nullptr, XCOLLIE_FLAG_LOG);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    inst.CancelTimer(id);


    int id2 = inst.SetTimer(string(__func__), 1, callback, nullptr, XCOLLIE_FLAG_DEFAULT);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    inst.CancelTimer(id2);
}
}