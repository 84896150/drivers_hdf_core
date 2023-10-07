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

#include "v1_1/test_types.h"
#include <hdf_log.h>
#include <message_parcel.h>
#include <securec.h>

namespace OHOS {
namespace HDI {
namespace Test {
namespace V1_1 {



bool WaveParamBlockMarshalling(OHOS::MessageParcel& data, const WaveParam& dataBlock)
{
    if (!data.WriteUnpadBuffer((const void*)&dataBlock, sizeof(WaveParam))) {
        return false;
    }
    return true;
}

bool WaveParamBlockUnmarshalling(OHOS::MessageParcel& data, WaveParam& dataBlock)
{
    const WaveParam *dataBlockPtr = reinterpret_cast<const WaveParam*>(data.ReadUnpadBuffer(sizeof(WaveParam)));
    if (dataBlockPtr == NULL) {
        return false;
    }

    if (memcpy_s(&dataBlock, sizeof(WaveParam), dataBlockPtr, sizeof(WaveParam)) != EOK) {
        return false;
    }
    return true;
}

} // V1_1
} // Test
} // HDI
} // OHOS
