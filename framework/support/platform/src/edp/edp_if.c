#include "edp_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "edp_core.h"
#include "securec.h"

#define HDF_LOG_TAG "edp_if"

static void * EdpGetDevByNum(uint16_t busNum)
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
    DevHandle devHdl = (DevHandle)EdpGetDevByNum(busNum);
    if (NULL == devHdl){
        HDF_LOGE(" Edp handle get fail.");
        return NULL;
    }

    return devHdl;
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
  // not support yet
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