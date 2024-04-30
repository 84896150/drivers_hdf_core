#include "device_resource_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"

#include "edp_if.h"
#include "edp_core.h"

#define HDF_LOG_TAG edp_core

static void *EdpCntlrObjGet(uint16_t busNum)
{
    char *serviceName = NULL;
    void *obj = NULL;

    serviceName = (char *)OsalMemCalloc(EDP_SERVICE_NAME_LEN + 1);
    if (serviceName == NULL) {
        HDF_LOGE("EDP service name malloc fail.");
        return NULL;
    }
    if (snprintf_s(serviceName, (EDP_SERVICE_NAME_LEN + 1),
        EDP_SERVICE_NAME_LEN, "HDF_PLATFORM_EDP_%u", busNum) < 0) {
        HDF_LOGE("get EDP service name fail.");
        OsalMemFree(serviceName);
        return obj;
    }
    obj = (void *)EdpCntlrGetByBusNum(busNum);
    OsalMemFree(serviceName);
    return obj;
}

static int32_t EdpCntlrParseVideoCaps(
    struct EdpCntlr *cntlr, struct DeviceResourceIface *drsOps, const struct DeviceResourceNode *node)
{
    int32_t ret;

    ret = drsOps->GetUint32(node, "maxTmdsClock", &(cntlr->cap.maxTmdsClock), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParseVideoCaps: read maxTmdsClock fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "defTmdsClock", &(cntlr->cap.defTmdsClock), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParseVideoCaps: read defTmdsClock fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "maxFrlRate", &(cntlr->cap.maxFrlRate), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParseVideoCaps: read maxFrlRate fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "videoTiming", &(cntlr->cap.videoTiming), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParseVideoCaps: read videoTiming fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "quantization", &(cntlr->cap.quantization), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParseVideoCaps: read quantization fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "colorSpace", &(cntlr->cap.colorSpace), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParseVideoCaps: read colorSpace fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "colorimetry", &(cntlr->cap.colorimetry), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParseVideoCaps: read colorimetry fail!");
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t EdpCntlrParseAudioCaps(
    struct EdpCntlr *cntlr, struct DeviceResourceIface *drsOps, const struct DeviceResourceNode *node)
{
    int32_t ret;

    ret = drsOps->GetUint32(node, "audioIfType", &(cntlr->cap.audioIfType), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParseAudioCaps: read audioIfType fail!");
        return ret;
    }
    HDF_LOGD("EdpCntlrParseAudioCaps: audioIfType = %d", cntlr->cap.audioIfType);

    ret = drsOps->GetUint32(node, "audioBitDepth", &(cntlr->cap.audioBitDepth), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParseAudioCaps: read audioBitDepth fail!");
        return ret;
    }
    HDF_LOGD("EdpCntlrParseAudioCaps: audioBitDepth = %d", cntlr->cap.audioBitDepth);

    ret = drsOps->GetUint32(node, "audioSampleRate", &(cntlr->cap.audioSampleRate), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParseAudioCaps: read audioSampleRate fail!");
        return ret;
    }
    HDF_LOGD("EdpCntlrParseAudioCaps: audioSampleRate = %d", cntlr->cap.audioSampleRate);

    ret = drsOps->GetUint32(node, "audioChannels", &(cntlr->cap.audioChannels), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParseAudioCaps: read audioChannels fail!");
        return ret;
    }
    HDF_LOGD("EdpCntlrParseAudioCaps: audioChannels = %d", cntlr->cap.audioChannels);
    return HDF_SUCCESS;
}

int32_t EdpCntlrParse(struct EdpCntlr *cntlr, struct HdfDeviceObject *obj)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret = -1;

    if (obj == NULL || cntlr == NULL) {
        HDF_LOGE("EdpCntlrParse: input param is NULL.");
        return HDF_FAILURE;
    }

    node = obj->property;
    if (node == NULL) {
        HDF_LOGE("EdpCntlrParse: drs node is NULL.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("EdpCntlrParse: invalid drs ops fail!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "index", &(cntlr->deviceIndex), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParse: read hostId fail!");
        return ret;
    }

    ret = drsOps->GetUint32(node, "cap", &(cntlr->cap.baseCap.data), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParse: read baseCap fail!");
        return ret;
    }

    ret = EdpCntlrParseVideoCaps(cntlr, drsOps, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParse: read video caps fail!");
        return ret;
    }

    ret = EdpCntlrParseAudioCaps(cntlr, drsOps, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("EdpCntlrParse: read audio caps fail!");
        return ret;
    }
    return HDF_SUCCESS;
}

int32_t EdpCntlrAdd(struct HdfDeviceObject *obj, struct EdpCntlr *cntlr)
{
    int32_t ret = -1;
    if (obj == NULL || cntlr == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->ops == NULL) {
        HDF_LOGE("%s: edp ops is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (OsalSpinInit(&(cntlr->lock)) != HDF_SUCCESS) {
        HDF_LOGE("%s: init spinlock fail", __func__);
        return HDF_FAILURE;
    }
    cntlr->device.manager = PlatformManagerGet(PLATFORM_MODULE_EDP);
    ret = PlatformDeviceAdd(&cntlr->device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: device add fail!", __func__);
        return ret;
    }
    cntlr->hdfDevObj = obj;
    cntlr->hdfDevObj->service = &(cntlr->service);
    cntlr->device.hdfDev = obj;
    cntlr->device.service = &cntlr->service;
    return HDF_SUCCESS;
}

DevHandle EdpCntlrOpen(uint16_t busNum)
{
    DevHandle hdl = EdpCntlrObjGet(busNum);
    return hdl;
}

int32_t EdpCntlrDeepColorSet(struct EdpCntlr *cntlr, enum EdpColorDepth color)
{
    int32_t ret = -1;
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->deepColorSet == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = cntlr->ops->deepColorSet(cntlr, color);
    return ret;
}

int32_t EdpCntlrDeepColorGet(struct EdpCntlr *cntlr, enum EdpColorDepth *color)
{
    int32_t ret = -1;
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->deepColorGet == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = cntlr->ops->deepColorGet(cntlr, color);
    return ret;
}

int32_t EdpCntlrSetVideoAttribute(struct EdpCntlr *cntlr, const struct EdpVideoAttr * attr)
{
    int32_t ret = HDF_FAILURE;
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->setVideoAttribute == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = cntlr->ops->setVideoAttribute(cntlr, attr);

    return ret;
}

int32_t EdpCntlrGetVideoAttribute(struct EdpCntlr *cntlr, struct EdpVideoAttr * attr)
{
    int32_t ret = HDF_FAILURE;
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->getVideoAttribute == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = cntlr->ops->getVideoAttribute(cntlr, attr);

    return ret;
}

int32_t EdpCntlrSetAudioAttribute(struct EdpCntlr * cntlr, const struct EdpAudioAttr *attr)
{
    int32_t ret = -1;
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->setAudioAttribute == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = cntlr->ops->setAudioAttribute(cntlr, attr);

    return ret;
}

void EdpCntlrClose(struct EdpCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->close == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return;
    }
    cntlr->ops->close(cntlr);
}

int32_t EdpCntlrRegisterHpdCallbackFunc(struct EdpCntlr *cntlr, struct EdpHpdCallbackInfo *callback)
{
    int32_t ret = -1;
    if (cntlr == NULL || callback == NULL || cntlr->ops == NULL || cntlr->ops->registerHpdCallbackFunc == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return ret;
    }
    return cntlr->ops->registerHpdCallbackFunc(cntlr, callback);
}

int32_t EdpCntlrUnregisterHpdCallbackFunc(struct EdpCntlr *cntlr)
{
    int32_t ret = -1;
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->unregisterHpdCallbackFunc == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return ret;
    }
    return cntlr->ops->unregisterHpdCallbackFunc(cntlr);
}

int32_t EdpCntlrAvmuteSet(struct EdpCntlr *cntlr, bool enable)
{
    int32_t ret = -1;
    if (cntlr == NULL || cntlr->ops == NULL || cntlr->ops->avmuteSet == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return ret;
    }

    EdpCntlrLock(cntlr);
    ret = cntlr->ops->avmuteSet(cntlr, enable);
    EdpCntlrUnlock(cntlr);
    return ret;
}