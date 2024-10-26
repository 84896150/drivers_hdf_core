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

#ifndef __EDP_CORE_H__
#define __EDP_CORE_H__

// HDF common header files
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_mutex.h"
#include "platform_core.h"
#include "osal_spinlock.h"

#include "edp_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define EDP_ONE_BYTE_MARK 0xFF
#define EDP_ONE_BYTE_SHIFT 8
#define EDP_TWO_BYTES_SHIFT 16
#define EDP_SERVICE_NAME_LEN 32
#define EDP_NAME_BUF_LEN 32

struct EdpPhyCfg {
    uint32_t pixelClk;
    uint32_t tmdsClk;               /* TMDS clock, KHz */
    enum EdpColorDepth deepColor;        /* deep color(color depth) */
    enum EdpColorSpace colorSpace;
};

struct EdpCntlr;

union EdpCap {
    uint32_t data;
};

enum EdpAudioFormat {
    EDP_AFMT_I2S = 0,
    EDP_AFMT_SPDIF = 1,
    EDP_AFMT_UNUSED,
};

struct EdpAudioInfo {
    enum EdpAudioFormat format;
    int32_t sampleRate;
    int32_t channels;
    int32_t sampleWidth;
};

struct EdpCntlrCap {
    union EdpCap baseCap;
    uint32_t maxTmdsClock;
    uint32_t defTmdsClock; /* unit: MHz */
    uint32_t maxFrlRate;
    uint32_t videoTiming;
    uint32_t quantization;
    uint32_t colorSpace;
    uint32_t colorimetry;
    uint32_t audioIfType;
    uint32_t audioBitDepth;
    uint32_t audioSampleRate;
    uint32_t audioChannels;
    uint32_t hdrColorimetry;
    uint32_t hdrUserMode;
};

struct EdpAudioConfigInfo {
    bool enable;
    bool downSample;
    uint32_t tmdsClock;
    uint32_t pixelRepeat;
    enum EdpSampleRate sampleRate;
};

struct EdpCntlrOps {
    int32_t (*open)(struct EdpCntlr *cntlr);
    int32_t (*close)(struct EdpCntlr *cntlr);
    int32_t (*enable)(struct EdpCntlr *cntlr);
    int32_t (*disable)(struct EdpCntlr *cntlr);
    int32_t (*avmuteSet)(struct EdpCntlr *cntlr, bool enable);
    int32_t (*deepColorSet)(struct EdpCntlr *cntlr, enum EdpColorDepth color);
    int32_t (*deepColorGet)(struct EdpCntlr *cntlr, enum EdpColorDepth *color);
    int32_t (*setVideoAttribute)(struct EdpCntlr *cntlr, const struct EdpVideoAttr *attr);
    int32_t (*getVideoAttribute)(struct EdpCntlr *cntlr, struct EdpVideoAttr *attr);
    int32_t (*setAudioAttribute)(struct EdpCntlr *cntlr, const struct EdpAudioAttr *attr);
    int32_t (*getAudioAttribute)(struct EdpCntlr *cntlr, struct EdpAudioAttr *attr);
    int32_t (*registerHpdCallbackFunc)(struct EdpCntlr *cntlr, struct EdpHpdCallbackInfo *callback);
    int32_t (*unregisterHpdCallbackFunc)(struct EdpCntlr *cntlr);
    int32_t (*getConnector)(struct EdpCntlr *cntlr, DevHandle *connector);
};

enum EdpCntlrState {
    EDP_CNTLR_STATE_NONE = 0,
    EDP_CNTLR_STATE_OPEN = (1 << 0),
    EDP_CNTLR_STATE_START = (1 << 1),
    EDP_CNTLR_STATE_STOP = (1 << 2),
    EDP_CNTLR_STATE_CLOSE = (1 << 3),
};

enum EdpEventType {
    EDP_EVENT_TYPE_NONE,
};

struct EdpDevice {
    struct EdpCntlr *cntlr;
    void *priv;
};

struct EdpAttr {
    struct EdpAudioInfo audioAttr;
    struct EdpVideoAttr videoAttr;
};

struct EdpCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *hdfDevObj;
    struct PlatformDevice device;
    struct OsalMutex mutex;
    struct EdpCntlrCap cap;
    struct EdpAttr attr;
    struct EdpCntlrOps *ops;
    struct PlatformQueue *msgQueue;
    struct EdpHpdCallbackInfo *hpdCbInfo;
    uint32_t num;
    uint32_t deviceIndex;
    enum EdpCntlrState state;                 /* cntlr state. */
    OsalSpinlock lock;
    void *priv;
};

static inline void EdpCntlrLock(struct EdpCntlr *cntlr)
{
    if (cntlr != NULL) {
        (void)OsalMutexLock(&cntlr->mutex);
    }
}

static inline void EdpCntlrUnlock(struct EdpCntlr *cntlr)
{
    if (cntlr != NULL) {
        (void)OsalMutexUnlock(&cntlr->mutex);
    }
}

static inline struct EdpCntlr *EdpCntlrGetByBusNum(uint16_t num)
{
    struct PlatformDevice *device = PlatformManagerGetDeviceByNumber(PlatformManagerGet(PLATFORM_MODULE_EDP), num);

    if (device == NULL) {
        return NULL;
    }
    return CONTAINER_OF(device, struct EdpCntlr, device);
}

DevHandle EdpCntlrOpen(uint16_t busNum);
void EdpCntlrClose(struct EdpCntlr *cntlr);
int32_t EdpCntlrDeepColorGet(struct EdpCntlr *cntlr, enum EdpColorDepth *color);
int32_t EdpCntlrDeepColorSet(struct EdpCntlr *cntlr, enum EdpColorDepth color);
int32_t EdpCntlrSetVideoAttribute(struct EdpCntlr *cntlr, const struct EdpVideoAttr *attr);
int32_t EdpCntlrGetVideoAttribute(struct EdpCntlr *cntlr, struct EdpVideoAttr *attr);
int32_t EdpCntlrSetAudioAttribute(struct EdpCntlr *cntlr, const struct EdpAudioAttr *attr);
int32_t EdpCntlrAvmuteSet(struct EdpCntlr *cntlr, bool enable);
int32_t EdpEventHandle(struct EdpCntlr *cntlr, enum EdpEventType event, void *data);
int32_t EdpCntlrDeviceAdd(struct HdfDeviceObject *obj, struct EdpCntlr *cntlr);
int32_t EdpCntlrRegisterHpdCallbackFunc(struct EdpCntlr *cntlr, struct EdpHpdCallbackInfo *callback);
int32_t EdpCntlrUnregisterHpdCallbackFunc(struct EdpCntlr *cntlr);
int32_t EdpCntlrGetConnector(struct EdpCntlr *cntlr, DevHandle *connector);
int32_t EdpCntlrAdd(struct HdfDeviceObject *obj, struct EdpCntlr *cntlr);
int32_t EdpCntlrDisable(struct EdpCntlr *cntlr);
int32_t EdpCntlrEnable(struct EdpCntlr *cntlr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //__EDP_CORE_H__