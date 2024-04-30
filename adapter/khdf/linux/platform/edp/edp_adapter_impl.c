
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

static inline hdf_edp_adapter_t * EdpAdapterPrivToEdpData(struct EdpCntlr *cntlr)
{
    if (cntlr == NULL) {
        return NULL;
    }
    return (hdf_edp_adapter_t *)cntlr->priv;
}

static int32_t EdpAdapterDeepColorSet(struct EdpCntlr *cntlr, enum EdpColorDepth color)
{
    int32_t ret = -1;
    hdf_edp_adapter_t * edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    ret = LinuxEdpDeepColorSet(edpAdapterData, color);
    return ret;
}

static int32_t EdpAdapterDeepColorGet(struct EdpCntlr* cntlr, enum EdpColorDepth *color)
{
    int32_t ret = -1;
    hdf_edp_adapter_t * edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    uint32_t colorGet = 0;
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
    (void)cntlr;
    (void)enable;
    return HDF_SUCCESS;
}

static int32_t EdpAdapterSetVideoAttribute(struct EdpCntlr *cntlr, const struct EdpVideoAttr *attr)
{
    int32_t ret = -1;
    if (cntlr == NULL || attr == NULL) {
        HDF_LOGE("%s, invalid cntlr data\n", __func__);
        return ret;
    }
    hdf_edp_adapter_t * edpAdapterData = EdpAdapterPrivToEdpData(cntlr);
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    ret = LinuxEdpSetVideoAttribute(edpAdapterData, (void *)attr);

    return ret;
}

static int32_t EdpAdapterSetAudioAttribute(struct EdpCntlr * cntlr, const struct EdpAudioAttr *attr)
{
    (void)cntlr;
    (void)attr;
    return HDF_SUCCESS;
}

static int32_t EdpAdapterRegisterHpdCallbackFunc(struct EdpCntlr *cntlr, struct EdpHpdCallbackInfo *callback)
{
    (void)cntlr;
    (void)callback;
    return HDF_SUCCESS;
}

static int32_t EdpAdapterUnregisterHpdCallbackFunc(struct EdpCntlr *cntlr)
{
    (void)cntlr;
    return HDF_SUCCESS;
}


static struct EdpCntlrOps g_edpCntlrOps = {
    .open = NULL,
    .close = NULL,
    .start = NULL,
    .stop = NULL,
    .avmuteSet = EdpAdapterAvmuteSet,
    .deepColorSet = EdpAdapterDeepColorSet,
    .deepColorGet = EdpAdapterDeepColorGet,
    .setVideoAttribute = EdpAdapterSetVideoAttribute,
    .setAudioAttribute = EdpAdapterSetAudioAttribute,
    .registerHpdCallbackFunc = EdpAdapterRegisterHpdCallbackFunc,
    .unregisterHpdCallbackFunc = EdpAdapterUnregisterHpdCallbackFunc,
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
    (void)cntlr;
    return HDF_SUCCESS;
}

static int32_t EdpAdapterInit(struct EdpCntlr *cntlr)
{
    int32_t ret = -1;
    if (cntlr == NULL) {
        HDF_LOGE("%s, device handle is null", __func__);
        return ret;
    }
    hdf_edp_adapter_t *edpAdapterData = NULL;

    edpAdapterData = (hdf_edp_adapter_t *)OsalMemCalloc(sizeof(hdf_edp_adapter_t));
    if (NULL == edpAdapterData) {
        HDF_LOGE("Alloc mem for adapter data failed\n");
        return ret;
    }

    ret = LinuxEdpAdapterInit(edpAdapterData);
    if (HDF_SUCCESS != ret) {
        OsalMemFree(edpAdapterData);
        return ret;
    }
    cntlr->priv = (void *)edpAdapterData;

    enum EdpColorDepth color;
    EdpAdapterDeepColorGet(cntlr, &color);
    HDF_LOGI("Finished, color: %d!!\n", color);
    EdpAdapterDeepColorSet(cntlr, EDP_COLOR_DEPTH_8BIT);
    return ret;
}

static int32_t EdpAdapterDeInit(struct EdpCntlr *cntlr)
{
    int32_t ret = -1;
    hdf_edp_adapter_t * edpAdapterData = EdpAdapterPrivToEdpData(cntlr);

    if (NULL == edpAdapterData)
    {
        HDF_LOGE("edp adapter data is NULL\n");
        return ret;
    }

    LinuxEdpAdapterDeInit(edpAdapterData);
    OsalMemFree(edpAdapterData);

    ret = 0;
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
    // if (iface->GetUint32(obj->property, "num", &num, 0) != HDF_SUCCESS) {
    //     HDF_LOGE("HdfPwmInit: read num fail!");
    //     return HDF_FAILURE;
    // }

    cntlr->hdfDevObj = obj;
    cntlr->num = num;
    cntlr->device.number = (int32_t)cntlr->deviceIndex;
    cntlr->device.hdfDev = cntlr->hdfDevObj;
    ret = EdpAdapterInit(cntlr);
    return ret;
}

static void EdpCntlrDeinit(struct EdpCntlr *cntlr)
{
    if (cntlr != NULL) {
        EdpAdapterDeInit(cntlr);
        // EdpCntlrFreeDev(cntlr);
        (void)OsalMutexDestroy(&cntlr->mutex);
    }
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
    // ret = EdpCntlrDeviceAdd(obj, cntlr);
    ret = EdpCntlrInit(cntlr, obj);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: error probe, ret is %d", __func__, ret);
        OsalMemFree(cntlr);
    }

    return ret;
}

static void HdfEdpRelease(struct HdfDeviceObject *obj)
{
    HDF_LOGI("%s: entry", __func__);
}

struct HdfDriverEntry g_hdfEdpEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_EDP",
    .Bind = HdfEdpBind,
    .Init = HdfEdpInit,
    .Release = HdfEdpRelease,
};

HDF_INIT(g_hdfEdpEntry);