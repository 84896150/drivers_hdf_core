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

#include <hdf_base.h>
#include <hdf_device_desc.h>
#include <hdf_log.h>
#include <hdf_sbuf_ipc.h>
#include "v1_1/test_interface_stub.h"

#define HDF_LOG_TAG    test_interface_driver

using namespace OHOS::HDI::Test::V1_1;

struct HdfTestInterfaceHost {
    struct IDeviceIoService ioService;
    OHOS::sptr<OHOS::IRemoteObject> stub;
};

static int32_t TestInterfaceDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    auto *hdfTestInterfaceHost = CONTAINER_OF(client->device->service, struct HdfTestInterfaceHost, ioService);

    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;
    OHOS::MessageOption option;

    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (SbufToParcel(reply, &replyParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: invalid reply sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return hdfTestInterfaceHost->stub->SendRequest(cmdId, *dataParcel, *replyParcel, option);
}

static int HdfTestInterfaceDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%{public}s: driver init start", __func__);
    return HDF_SUCCESS;
}

static int HdfTestInterfaceDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%{public}s: driver bind start", __func__);
    auto *hdfTestInterfaceHost = new (std::nothrow) HdfTestInterfaceHost;
    if (hdfTestInterfaceHost == nullptr) {
        HDF_LOGE("%{public}s: failed to create create HdfTestInterfaceHost object", __func__);
        return HDF_FAILURE;
    }

    hdfTestInterfaceHost->ioService.Dispatch = TestInterfaceDriverDispatch;
    hdfTestInterfaceHost->ioService.Open = NULL;
    hdfTestInterfaceHost->ioService.Release = NULL;

    auto serviceImpl = OHOS::HDI::Test::V1_1::ITestInterface::Get(true);
    if (serviceImpl == nullptr) {
        HDF_LOGE("%{public}s: failed to get of implement service", __func__);
        delete hdfTestInterfaceHost;
        return HDF_FAILURE;
    }

    hdfTestInterfaceHost->stub = OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(serviceImpl,
        OHOS::HDI::Test::V1_1::ITestInterface::GetDescriptor());
    if (hdfTestInterfaceHost->stub == nullptr) {
        HDF_LOGE("%{public}s: failed to get stub object", __func__);
        delete hdfTestInterfaceHost;
        return HDF_FAILURE;
    }

    deviceObject->service = &hdfTestInterfaceHost->ioService;
    return HDF_SUCCESS;
}

static void HdfTestInterfaceDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%{public}s: driver release start", __func__);
    if (deviceObject->service == nullptr) {
        return;
    }

    auto *hdfTestInterfaceHost = CONTAINER_OF(deviceObject->service, struct HdfTestInterfaceHost, ioService);
    if (hdfTestInterfaceHost != nullptr) {
        delete hdfTestInterfaceHost;
    }
}

struct HdfDriverEntry g_testinterfaceDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "",
    .Bind = HdfTestInterfaceDriverBind,
    .Init = HdfTestInterfaceDriverInit,
    .Release = HdfTestInterfaceDriverRelease,
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
HDF_INIT(g_testinterfaceDriverEntry);
#ifdef __cplusplus
}
#endif /* __cplusplus */
