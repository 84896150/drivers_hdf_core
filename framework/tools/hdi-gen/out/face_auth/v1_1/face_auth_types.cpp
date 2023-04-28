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

#include "v1_1/face_auth_types.h"
#include <hdf_log.h>
#include <message_parcel.h>
#include <securec.h>

namespace OHOS {
namespace HDI {
namespace FaceAuth {
namespace V1_1 {



bool PropertyBlockMarshalling(OHOS::MessageParcel& data, const Property& dataBlock)
{
    if (!data.WriteUint64(dataBlock.authSubType)) {
        HDF_LOGE("%{public}s: write dataBlock.authSubType failed!", __func__);
        return false;
    }

    if (!data.WriteInt32(dataBlock.lockoutDuration)) {
        HDF_LOGE("%{public}s: write dataBlock.lockoutDuration failed!", __func__);
        return false;
    }

    if (!data.WriteInt32(dataBlock.remainAttempts)) {
        HDF_LOGE("%{public}s: write dataBlock.remainAttempts failed!", __func__);
        return false;
    }

    if (!data.WriteCString(dataBlock.enrollProgress.c_str())) {
        HDF_LOGE("%{public}s: write dataBlock.enrollProgress failed!", __func__);
        return false;
    }

    if (!data.WriteCString(dataBlock.sensorInfo.c_str())) {
        HDF_LOGE("%{public}s: write dataBlock.sensorInfo failed!", __func__);
        return false;
    }
    return true;
}

bool PropertyBlockUnmarshalling(OHOS::MessageParcel& data, Property& dataBlock)
{
    if (!data.ReadUint64(dataBlock.authSubType)) {
        HDF_LOGE("%{public}s: read dataBlock.authSubType failed!", __func__);
        return false;
    }

    if (!data.ReadInt32(dataBlock.lockoutDuration)) {
        HDF_LOGE("%{public}s: read dataBlock.lockoutDuration failed!", __func__);
        return false;
    }

    if (!data.ReadInt32(dataBlock.remainAttempts)) {
        HDF_LOGE("%{public}s: read dataBlock.remainAttempts failed!", __func__);
        return false;
    }

    const char* enrollProgressCp = data.ReadCString();
    if (enrollProgressCp == nullptr) {
        HDF_LOGE("%{public}s: read enrollProgressCp failed", __func__);
        return false;
    }
    dataBlock.enrollProgress = enrollProgressCp;

    const char* sensorInfoCp = data.ReadCString();
    if (sensorInfoCp == nullptr) {
        HDF_LOGE("%{public}s: read sensorInfoCp failed", __func__);
        return false;
    }
    dataBlock.sensorInfo = sensorInfoCp;
    return true;
}

bool SaCommandParamNoneBlockMarshalling(OHOS::MessageParcel& data, const SaCommandParamNone& dataBlock)
{
    if (!data.WriteUnpadBuffer((const void*)&dataBlock, sizeof(SaCommandParamNone))) {
        return false;
    }
    return true;
}

bool SaCommandParamNoneBlockUnmarshalling(OHOS::MessageParcel& data, SaCommandParamNone& dataBlock)
{
    const SaCommandParamNone *dataBlockPtr = reinterpret_cast<const SaCommandParamNone*>(data.ReadUnpadBuffer(sizeof(SaCommandParamNone)));
    if (dataBlockPtr == NULL) {
        return false;
    }

    if (memcpy_s(&dataBlock, sizeof(SaCommandParamNone), dataBlockPtr, sizeof(SaCommandParamNone)) != EOK) {
        return false;
    }
    return true;
}

bool SaCommandBlockMarshalling(OHOS::MessageParcel& data, const SaCommand& dataBlock)
{
    if (!data.WriteUnpadBuffer((const void*)&dataBlock, sizeof(SaCommand))) {
        return false;
    }
    return true;
}

bool SaCommandBlockUnmarshalling(OHOS::MessageParcel& data, SaCommand& dataBlock)
{
    const SaCommand *dataBlockPtr = reinterpret_cast<const SaCommand*>(data.ReadUnpadBuffer(sizeof(SaCommand)));
    if (dataBlockPtr == NULL) {
        return false;
    }

    if (memcpy_s(&dataBlock, sizeof(SaCommand), dataBlockPtr, sizeof(SaCommand)) != EOK) {
        return false;
    }
    return true;
}

} // V1_1
} // FaceAuth
} // HDI
} // OHOS
