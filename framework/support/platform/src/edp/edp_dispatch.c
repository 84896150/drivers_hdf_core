#include "edp_dispatch.h"
#include "hdf_log.h"
#include "edp_core.h"
#include "edp_if.h"

#define HDF_LOG_TAG   edp_dispatch

struct EdpDispatchFunc {
    uint32_t cmd;
    int32_t (*func)(struct EdpCntlr * cntlr, struct HdfSBuf *data, struct HdfSBuf *reply);
};

static int32_t EdpCmdOpen(struct EdpCntlr * cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;
    return EdpCntlrOpen(cntlr);
}

static int32_t EdpCmdClose(struct EdpCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;

    EdpCntlrClose(cntlr);
    return HDF_SUCCESS;
}
#if 0
static int32_t EdpCmdStart(struct EdpCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;

    return EdpCntlrStart(cntlr);
}

static int32_t EdpCmdStop(struct EdpCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;

    return EdpCntlrStop(cntlr);
}
#endif
static int32_t EdpCmdAvmuteSet(struct EdpCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    bool *enable = NULL;
    uint32_t size;
    (void)reply;

    if (!HdfSbufReadBuffer(data, (const void **)&enable, &size)) {
        HDF_LOGE("EdpCmdAvmuteSet: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    EdpCntlrAvmuteSet(cntlr, *enable);
    return HDF_SUCCESS;
}

static int32_t EdpCmdDeepColorSet(struct EdpCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    enum EdpColorDepth *color = NULL;
    uint32_t size;
    (void)reply;

    if (!HdfSbufReadBuffer(data, (const void **)&color, &size)) {
        HDF_LOGE("EdpCmdDeepColorSet: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    return EdpCntlrDeepColorSet(cntlr, *color);
}

static int32_t EdpCmdDeepColorGet(struct EdpCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    enum EdpColorDepth color;
    (void)data;

    if (reply == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = EdpCntlrDeepColorGet(cntlr, &color);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (HdfSbufWriteBuffer(reply, &color, sizeof(color)) == false) {
        HDF_LOGE("EdpCmdDeepColorGet: write back color fail!");
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t EdpCmdVideoInfoSet(struct EdpCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct EdpVideoAttr *attr = NULL;
    uint32_t size;
    (void)reply;

    if (!HdfSbufReadBuffer(data, (const void **)&attr, &size)) {
        HDF_LOGE("EdpCmdVideoInfoSet: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    return EdpCntlrSetVideoAttribute(cntlr, attr);
}

static int32_t EdpCmdAudioInfoSet(struct EdpCntlr *cntlr, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct EdpAudioAttr *attr = NULL;
    uint32_t size;
    (void)reply;

    if (!HdfSbufReadBuffer(data, (const void **)&attr, &size)) {
        HDF_LOGE("EdpCmdAudioInfoSet: sbuf read buffer failed");
        return HDF_ERR_IO;
    }
    return EdpCntlrSetAudioAttribute(cntlr, attr);
}

int32_t EdpIoDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct EdpCntlr *cntlr = NULL;
    uint32_t i;
    uint32_t len;
    struct EdpDispatchFunc dispatchFunc[] = {
        { EDP_CMD_OPEN, EdpCmdOpen },
        { EDP_CMD_CLOSE, EdpCmdClose },
        #if 0
        { EDP_CMD_START, EdpCmdStart },
        { EDP_CMD_STOP, EdpCmdStop },
        #endif
        { EDP_CMD_AVMUTE_SET, EdpCmdAvmuteSet },
        { EDP_CMD_DEEP_COLOR_SET, EdpCmdDeepColorSet },
        { EDP_CMD_DEEP_COLOR_GET, EdpCmdDeepColorGet },
        { EDP_CMD_VIDEO_ATTR_SET, EdpCmdVideoInfoSet },
        { EDP_CMD_AUDIO_ATTR_SET, EdpCmdAudioInfoSet },
    };

    if (client == NULL || client->device == NULL) {
        HDF_LOGE("EdpIoDispatch: client or hdf dev obj is NULL");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = (struct EdpCntlr *)client->device->service;
    if (cntlr == NULL) {
        HDF_LOGE("EdpIoDispatch: service is NULL");
        return HDF_ERR_INVALID_OBJECT;
    }

    len = sizeof(dispatchFunc) / sizeof(dispatchFunc[0]);
    for (i = 0; i < len; i++) {
        if (dispatchFunc[i].cmd == cmd) {
        return dispatchFunc[i].func(cntlr, data, reply);
        }
    }

    HDF_LOGE("EdpIoDispatch: cmd %d is not support", cmd);
    return HDF_ERR_NOT_SUPPORT;
}
