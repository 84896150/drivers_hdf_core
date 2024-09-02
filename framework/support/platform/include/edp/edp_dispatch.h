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

#ifndef EDP_DISPATCH_H
#define EDP_DISPATCH_H

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

#endif //EDP_DISPATCH_H