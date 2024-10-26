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

#include "edp_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "edp_core.h"
#include "securec.h"

#define HDF_LOG_TAG "edp_if"

static void *EdpGetDevByNum(uint16_t busNum)
{
    char *pSrvcName = NULL;
    void *pHdl = NULL;

    pSrvcName = (char *)OsalMemCalloc(EDP_NAME_BUF_LEN);
    if (pSrvcName == NULL) {
        HDF_LOGE("Edp service name malloc fail.");
        return NULL;
    }
    if (snprintf_s(pSrvcName, (EDP_NAME_BUF_LEN),
        EDP_NAME_BUF_LEN -1, "HDF_PLATFORM_EDP_%u", busNum) < 0) {
        HDF_LOGE("Generate Edp service name fail.");
        OsalMemFree(pSrvcName);
        return NULL;
    }

#ifdef __USER__
    pHdl = (void *)HdfIoServiceBind(serviceName);
#else
    pHdl = EdpCntlrGetByBusNum(busNum);
#endif
    OsalMemFree(pSrvcName);

    return pHdl;
}

DevHandle EdpOpen(uint16_t busNum)
{
    return EdpCntlrOpen(busNum);
}

int32_t EdpStart(DevHandle handle)
{
    return HDF_SUCCESS;
}

int32_t EdpStop(DevHandle handle)
{
    return HDF_SUCCESS;
}

int32_t EdpAvmuteSet(DevHandle handle, bool enable)
{
    return EdpCntlrAvmuteSet((struct EdpCntlr *)handle, enable);
}

int32_t EdpDeepColorSet(DevHandle handle, enum EdpColorDepth color)
{
    return EdpCntlrDeepColorSet((struct EdpCntlr *)handle, color);
}

int32_t EdpDeepColorGet(DevHandle handle, enum EdpColorDepth *color)
{
    return EdpCntlrDeepColorGet((struct EdpCntlr *)handle, color);
}

int32_t EdpSetVideoAttribute(DevHandle handle, struct EdpVideoAttr *attr)
{
    return EdpCntlrSetVideoAttribute((struct EdpCntlr *)handle, attr);
}

int32_t EdpSetAudioAttribute(DevHandle handle, struct EdpAudioAttr *attr)
{
    return EdpCntlrSetAudioAttribute((struct EdpCntlr *)handle, attr);
}

int32_t EdpRegisterHpdCallbackFunc(
    DevHandle handle,
    struct EdpHpdCallbackInfo *callback)
{
    return EdpCntlrRegisterHpdCallbackFunc((struct EdpCntlr *)handle, callback);
}

int32_t EdpUnregisterHpdCallbackFunc(DevHandle handle)
{
    return EdpCntlrUnregisterHpdCallbackFunc((struct EdpCntlr *)handle);
}

void EdpClose(DevHandle handle)
{
    EdpCntlrClose((struct EdpCntlr *)handle);
}

int32_t EdpGetConnector(DevHandle handle, DevHandle *connector)
{
    return EdpCntlrGetConnector((struct EdpCntlr *)handle, connector);
}

int32_t EdpDisable(DevHandle handle)
{
    return EdpCntlrDisable((struct EdpCntlr *)handle);
}

int32_t EdpEnable(DevHandle handle)
{
    return EdpCntlrEnable((struct EdpCntlr *)handle);
}