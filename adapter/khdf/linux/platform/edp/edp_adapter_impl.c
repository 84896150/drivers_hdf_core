/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include "osal.h"
#include "platform_if.h"
#include "platform_device.h"
#include "hdf_device_desc.h"
#include "device_resource_if.h"

#include "edp_core.h"
#include "edp_if.h"
#include "edp_dispatch.h"
#include "securec.h"

#include "edp_linux_def.h"

#define HDF_LOG_TAG edp_adapter_impl

static inline HdfEdpAdapterData *EdpAdapterPrivToEdpData(struct EdpCntlr *cntlr)
{
    if (cntlr == NULL) {
        return NULL;
    }
    return (HdfEdpAdapterData *)cntlr->priv;
}

static int32_t EdpAdapterDeepColorSet(struct EdpCntlr *cntlr, enum EdpColorDepth color)
{
    int32_t ret = -1;
    HdfEdpAdapterData *edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    ret = LinuxEdpDeepColorSet(edpAdapterData, color);
    return ret;
}

static int32_t EdpAdapterDeepColorGet(struct EdpCntlr *cntlr, enum EdpColorDepth *color)
{
    int32_t ret = -1;
    uint32_t colorGet = 0;
    HdfEdpAdapterData *edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    ret = LinuxEdpDeepColorGet(edpAdapterData, &colorGet);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s, LinuxEdpDeepColorGet failed %d", __func__, ret);
        return ret;
    }

    *color = colorGet;
    return ret;
}

static int32_t EdpAdapterAvmuteSet(struct EdpCntlr *cntlr, bool enable)
{
    int32_t ret = HDF_FAILURE;
    HdfEdpAdapterData *edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (cntlr == NULL) {
        HDF_LOGE("%s, invalid cntlr data\n", __func__);
        return ret;
    }

    HDF_LOGD("Audio Settings is not supported by eDP.");
    ret = LinuxEdpAvMuteSet(edpAdapterData, enable);
    return ret;
}

static int32_t EdpAdapterSetVideoAttribute(struct EdpCntlr *cntlr, const struct EdpVideoAttr *attr)
{
    int32_t ret = -1;
    HdfEdpAdapterData *edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (cntlr == NULL || attr == NULL) {
        HDF_LOGE("%s, invalid cntlr data\n", __func__);
        return ret;
    }
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    ret = LinuxEdpSetVideoAttribute(edpAdapterData, (const void *)attr);

    return ret;
}

static int32_t EdpAdapterSetAudioAttribute(struct EdpCntlr *cntlr, const struct EdpAudioAttr *attr)
{
    int32_t ret = HDF_FAILURE;
    HdfEdpAdapterData *edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (cntlr == NULL || attr == NULL) {
        HDF_LOGE("%s, invalid cntlr data\n", __func__);
        return ret;
    }
    HDF_LOGD("Audio Settings is not supported by eDP.");
    ret = LinuxEdpSetAudioAttribute(edpAdapterData, attr);
    return ret;
}

static int32_t EdpAdapterRegisterHpdCallbackFunc(struct EdpCntlr *cntlr, struct EdpHpdCallbackInfo *callback)
{
    int32_t ret = HDF_FAILURE;
    HdfEdpAdapterData *edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (cntlr == NULL || callback == NULL) {
        HDF_LOGE("%s, invalid cntlr data\n", __func__);
        return ret;
    }

    if (cntlr->hpdCbInfo != NULL) {
        HDF_LOGE("%s, hpd callback has been registered!\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = LinuxEdpRegisterHpdCallbackFunc(edpAdapterData, (void *)callback);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s, LinuxEdpRegisterHpdCallbackFunc failed\n", __func__);
        return ret;
    }

    cntlr->hpdCbInfo = (struct EdpHpdCallbackInfo *)OsalMemCalloc(sizeof(struct EdpHpdCallbackInfo));
    cntlr->hpdCbInfo->data = callback->data;
    return HDF_SUCCESS;
}

static int32_t EdpAdapterUnregisterHpdCallbackFunc(struct EdpCntlr *cntlr)
{
    int32_t ret = HDF_FAILURE;
    HdfEdpAdapterData *edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (cntlr == NULL) {
        HDF_LOGE("%s, invalid cntlr data\n", __func__);
        return ret;
    }
    ret = LinuxEdpUnRegisterHpdCallbackFunc(edpAdapterData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s, LinuxEdpRegisterHpdCallbackFunc failed\n", __func__);
        return ret;
    }

    OsalMemFree(cntlr->hpdCbInfo);
    cntlr->hpdCbInfo = NULL;
    return HDF_SUCCESS;
}

static int32_t EdpAdapterGetConnector(struct EdpCntlr *cntlr, DevHandle *connector)
{
    int32_t ret = HDF_FAILURE;
    HdfEdpAdapterData *edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (cntlr == NULL || connector == NULL) {
        HDF_LOGE("%s, invalid cntlr data\n", __func__);
        return ret;
    }

    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    ret = LinuxEdpGetConnector(edpAdapterData, (void **)connector);
    return ret;
}

static int32_t EdpAdapterEnable(struct EdpCntlr *cntlr)
{
    int32_t ret = HDF_FAILURE;
    HdfEdpAdapterData *edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (cntlr == NULL) {
        HDF_LOGE("%s, invalid cntlr data\n", __func__);
        return ret;
    }
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    ret = LinuxEdpEnable(edpAdapterData);
    return ret;
}

static int32_t EdpAdapterDisable(struct EdpCntlr *cntlr)
{
    int32_t ret = HDF_FAILURE;
    HdfEdpAdapterData *edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (cntlr == NULL) {
        HDF_LOGE("%s, invalid cntlr data\n", __func__);
        return ret;
    }
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    ret = LinuxEdpDisable(edpAdapterData);
    return ret;
}

static struct EdpCntlrOps g_edpCntlrOps = {
    .open = NULL,
    .close = NULL,
    .enable = EdpAdapterEnable,
    .disable = EdpAdapterDisable,
    .avmuteSet = EdpAdapterAvmuteSet,
    .deepColorSet = EdpAdapterDeepColorSet,
    .deepColorGet = EdpAdapterDeepColorGet,
    .setVideoAttribute = EdpAdapterSetVideoAttribute,
    .setAudioAttribute = EdpAdapterSetAudioAttribute,
    .registerHpdCallbackFunc = EdpAdapterRegisterHpdCallbackFunc,
    .unregisterHpdCallbackFunc = EdpAdapterUnregisterHpdCallbackFunc,
    .getConnector = EdpAdapterGetConnector,
};

static int32_t EdpAdapterOpsInit(struct EdpCntlr *cntlr)
{
    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    cntlr->ops = &g_edpCntlrOps;

    return HDF_SUCCESS;
}

static int32_t EdpAdapterOpsDeInit(struct EdpCntlr *cntlr)
{
    cntlr->ops = NULL;
    return HDF_SUCCESS;
}

static int32_t EdpAdapterInit(struct EdpCntlr *cntlr)
{
    int32_t ret = -1;
    HdfEdpAdapterData *edpAdapterData = NULL;
    if (cntlr == NULL) {
        HDF_LOGE("%s, device handle is null", __func__);
        return ret;
    }

    edpAdapterData = (HdfEdpAdapterData *)OsalMemCalloc(sizeof(HdfEdpAdapterData));
    if (NULL == edpAdapterData) {
        HDF_LOGE("Alloc mem for adapter data failed\n");
        return ret;
    }

    ret = LinuxEdpAdapterInit(edpAdapterData);
    if (HDF_SUCCESS != ret) {
        OsalMemFree(edpAdapterData);
        return ret;
    }

    EdpAdapterOpsInit(cntlr);
    cntlr->hpdCbInfo = NULL;
    cntlr->priv = (void *)edpAdapterData;
    edpAdapterData->hdfHdl = (void *)cntlr;
    return ret;
}

static int32_t EdpCntlrInit(struct EdpCntlr *cntlr, struct HdfDeviceObject *obj)
{
    int32_t ret;
    uint32_t num = 0;
    struct DeviceResourceIface *iface = NULL;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (obj == NULL) {
        HDF_LOGE("EdpCntlrInit: no HdfDeviceObject attached!");
        return HDF_ERR_INVALID_OBJECT;
    }
    iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (iface == NULL || iface->GetUint32 == NULL) {
        HDF_LOGE("HdfPwmInit: iface or GetUint32 is null!");
        return HDF_FAILURE;
    }

    cntlr->hdfDevObj = obj;
    cntlr->num = num;
    cntlr->device.number = (int32_t)cntlr->deviceIndex;
    cntlr->device.hdfDev = cntlr->hdfDevObj;
    obj->priv = (void *)cntlr;
    ret = EdpAdapterInit(cntlr);
    return ret;
}

static int32_t HdfEdpBind(struct HdfDeviceObject *obj)
{
    (void)obj;
    return HDF_SUCCESS;
}

static int32_t HdfEdpInit(struct HdfDeviceObject *obj)
{
    int32_t ret = -1;
    struct EdpCntlr *cntlr = NULL;

    HDF_LOGI("%s: entry", __func__);
    if (obj == NULL) {
        HDF_LOGE("%s: obj is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = (struct EdpCntlr *)OsalMemCalloc(sizeof(struct EdpCntlr));
    if (cntlr == NULL) {
        HDF_LOGE("%s: OsalMemCalloc cntlr error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = OsalMutexInit(&cntlr->mutex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: mutex init fail!", __func__);
        return ret;
    }

    cntlr->state = EDP_CNTLR_STATE_NONE;
    cntlr->service.Dispatch = &EdpIoDispatch;
    ret = EdpCntlrInit(cntlr, obj);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: error probe, ret is %d", __func__, ret);
        OsalMemFree(cntlr);
    }
    ret = EdpCntlrAdd(obj, cntlr);

    return ret;
}

static int32_t EdpAdapterDeInit(struct EdpCntlr *cntlr)
{
    int32_t ret = -1;
    HdfEdpAdapterData *edpAdapterData = EdpAdapterPrivToEdpData(cntlr);

    if (NULL == edpAdapterData) {
        HDF_LOGE("edp adapter data is NULL\n");
        return ret;
    }

    LinuxEdpAdapterDeInit(edpAdapterData);
    OsalMemFree(edpAdapterData);

    ret = 0;
    return ret;
}

static void EdpCntlrDeinit(struct EdpCntlr *cntlr)
{
    if (cntlr != NULL) {
        EdpAdapterDeInit(cntlr);
        EdpAdapterOpsDeInit(cntlr);
        (void)OsalMutexDestroy(&cntlr->mutex);
    }
}

static void HdfEdpRelease(struct HdfDeviceObject *obj)
{
    HDF_LOGD("%s: entry", __func__);
    struct EdpCntlr *cntlr = (struct EdpCntlr *)obj->priv;
    EdpCntlrDeinit(cntlr);
}

struct HdfDriverEntry g_hdfEdpEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_EDP",
    .Bind = HdfEdpBind,
    .Init = HdfEdpInit,
    .Release = HdfEdpRelease,
};

HDF_INIT(g_hdfEdpEntry);