#ifndef __EDP_DISPATCH_H__
#define __EDP_DISPATCH_H__

#include "hdf_base.h"
#include "devsvc_manager_clnt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum EdpIoCmd {
    EDP_CMD_OPEN,
    EDP_CMD_CLOSE,
    EDP_CMD_START,
    EDP_CMD_STOP,
    EDP_CMD_AVMUTE_SET,
    EDP_CMD_DEEP_COLOR_SET,
    EDP_CMD_DEEP_COLOR_GET,
    EDP_CMD_VIDEO_ATTR_SET,
    EDP_CMD_AUDIO_ATTR_SET,
    EDP_CMD_HDR_ATTR_SET,
    EDP_CMD_READ_SINK_EDID,
    EDP_CMD_INFOFRAME_SET,
    EDP_CMD_INFOFRAME_GET,
    EDP_CMD_REGISTER_HPD_CALLBACK_FUNC,
    EDP_CMD_UNREGISTER_HPD_CALLBACK_FUNC,
    EDP_CMD_BUTT,
};

int32_t EdpIoDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //__EDP_DISPATCH_H__