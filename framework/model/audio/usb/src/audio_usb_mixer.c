/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_usb_mixer.h"
#include <linux/bitops.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/log2.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/usb.h>
#include <linux/usb/audio-v2.h>
#include <linux/usb/audio-v3.h>
#include <linux/usb/audio.h>

#include "audio_control.h"
#include "audio_core.h"
#include "audio_driver_log.h"
#include "audio_host.h"
#include "audio_usb_linux.h"
#include "audio_usb_validate_desc.h"

#define HDF_LOG_TAG HDF_AUDIO_USB

#define MAX_ID_ELEMS      256
#define KCTL_NAME_LEN     64
#define MAX_ITEM_NAME_LEN 64
#define HDR_BLENGTH_MAX   4

#define AUDIO_USB_CTL_ELEM_TYPE_NONE       0 /* invalid */
#define AUDIO_USB_CTL_ELEM_TYPE_BOOLEAN    1 /* boolean type */
#define AUDIO_USB_CTL_ELEM_TYPE_INTEGER    2 /* integer type */
#define AUDIO_USB_CTL_ELEM_TYPE_ENUMERATED 3 /* enumerated type */

#define AUDIO_USB_CTL_ELEM_IFACE_CARD  0 /* global control */
#define AUDIO_USB_CTL_ELEM_IFACE_HWDEP 1 /* hardware dependent device */
#define AUDIO_USB_CTL_ELEM_IFACE_MIXER 2 /* virtual mixer device */

struct UsbMixerElemList {
    struct UsbMixerInterface *mixer;
    struct UsbMixerElemList *nextIdElem; /* list of controls with same id */
    uint32_t id;
    bool isStdInfo;
};

struct UsbMixerInterface {
    struct AudioUsbDriver *audioUsbDriver;
    struct usb_host_interface *hostIf;
    struct list_head list;
    uint32_t ignoreCtlError;
    struct urb *urb;
    struct UsbMixerElemList **idElems; /* array[MAX_ID_ELEMS], indexed by unit id */
    struct usb_ctrlrequest *rcSetupPacket;
    bool disconnected;
    void (*PrivateFree)(struct UsbMixerInterface *mixer);
    int32_t protocol; /* the usb audio specification version this interface complies to */
    struct urb *rcUrb;
};

enum AudioUsbMixerType {
    USB_MIXER_BOOLEAN = 0,
    USB_MIXER_INV_BOOLEAN,
    USB_MIXER_S8,
    USB_MIXER_U8,
    USB_MIXER_S16,
    USB_MIXER_U16,
    USB_MIXER_S32,
    USB_MIXER_U32,
    USB_MIXER_BESPOKEN, /* non-standard type */
};

#define MAX_CHANNELS  16 /* max logical channels */
#define USB_VAL_LEN_1 1
#define USB_VAL_LEN_2 2
#define USB_VAL_LEN_3 3
#define USB_VAL_LEN_4 4

#define USB_SHIFT_SIZE_3  3
#define USB_SHIFT_SIZE_4  4
#define USB_SHIFT_SIZE_8  8
#define USB_SHIFT_SIZE_16 16
#define USB_SHIFT_SIZE_24 24

#define USB_TIMEOUT 10

#define OVERF_LOWDIVISOR_NUM 8
#define VOLUME_RANGE         2

#define USB_DESCRIPTIONS_CONTAIN_100DB  100
#define USB_DESCRIPTIONS_CONTAIN_256DB  256
#define USB_DESCRIPTIONS_CONTAIN_9600DB 9600

#define CHANNEL_HDR_BLENGTH 7

#define ZERO_BASED_INTEGER 0xff

#define ARRAY_INDEX_0 0
#define ARRAY_INDEX_1 1
#define ARRAY_INDEX_2 2
#define ARRAY_INDEX_3 3
#define ARRAY_INDEX_4 4

struct UsbMixerElemInfo {
    struct UsbMixerElemList head;
    uint32_t control; /* CS or ICN (high byte) */
    uint32_t cmask;   /* channel mask bitmap: 0 = master */
    uint32_t idxOff;  /* Control index offset */
    uint32_t chReadonly;
    uint32_t masterReadonly;
    int32_t channels;
    int32_t valType;
    int32_t min, max, res; /* volume range */
    int32_t dBmin, dBmax;
    int32_t cached;
    int32_t cacheVal[MAX_CHANNELS];
    uint8_t initialized;
    uint8_t minMute;
    void *privateData;
};

struct UsbMixerDBMap {
    uint32_t min;
    uint32_t max;
};

struct UsbMixerNameMap {
    int32_t id;
    const char *name;
    int32_t control;
    const struct UsbMixerDBMap *dB;
};

struct UsbMixerSelectorMap {
    int32_t id;
    int32_t count;
    const char **names;
};

struct UsbAudioTerm {
    int32_t id;
    int32_t type;
    int32_t channels;
    uint32_t chconfig;
    int32_t name;
};

struct UsbMixerBuild {
    struct AudioUsbDriver *audioUsbDriver;
    struct UsbMixerInterface *mixer;
    uint8_t *buffer;
    uint32_t bufLen;
    DECLARE_BITMAP(unitbitmap, MAX_ID_ELEMS);
    DECLARE_BITMAP(termbitmap, MAX_ID_ELEMS);
    struct UsbAudioTerm oterm;
    const struct UsbMixerNameMap *map;
    const struct UsbMixerSelectorMap *selectorMap;
};

struct AudioUsbItermNameCombo {
    int32_t type;
    char *name;
};

struct AudioUsbFeatureControl {
    uint32_t ctlMask;
    int32_t control;
    int32_t unitId;
    uint32_t readOnlyMask;
    int32_t nameid;
};

struct AudioUsbFeatureParam {
    uint32_t masterBits;
    uint32_t channels;
    uint32_t csize;
    uint8_t *bmaControls;
};

struct MixerUnitCtlParam {
    int32_t numOuts;
    int32_t itemChannels;
};

static void AudioUsbMixerElemInitStd(struct UsbMixerElemList *list, struct UsbMixerInterface *mixer, int32_t unitId)
{
    if (list == NULL) {
        AUDIO_DRIVER_LOG_ERR("param is null");
        return;
    }

    list->mixer = mixer;
    list->id = unitId;
}

static int32_t AudioUsbAutoResume(struct AudioUsbDriver *audioUsbDriver)
{
    int32_t i, err;

    if (atomic_read(&audioUsbDriver->shutdown) != 0) {
        AUDIO_DRIVER_LOG_ERR("atomic_read fail");
        return HDF_FAILURE;
    }

    if (atomic_inc_return(&audioUsbDriver->active) != 1) {
        return HDF_SUCCESS;
    }

    for (i = 0; i < 1; i++) {
        err = usb_autopm_get_interface(audioUsbDriver->usbIf);
        if (err < 0) {
            /* rollback */
            while (--i >= 0) {
                usb_autopm_put_interface(audioUsbDriver->usbIf);
            }
            atomic_dec(&audioUsbDriver->active);
            return err;
        }
    }

    return 0;
}

/* lock the shutdown (disconnect) task and autoresume */
static int32_t AudioUsbLockShutdown(struct AudioUsbDriver *audioUsbDriver)
{
    int32_t err;

    atomic_inc(&audioUsbDriver->usageCount);
    if (atomic_read(&audioUsbDriver->shutdown) != 0) {
        AUDIO_DRIVER_LOG_ERR("atomic_read fail");
        err = HDF_FAILURE;
        if (atomic_dec_and_test(&audioUsbDriver->usageCount) != 0) {
            AUDIO_DRIVER_LOG_ERR("atomic_dec_and_test fail");
            wake_up(&audioUsbDriver->shutdownWait);
        }
        return err;
    }
    err = AudioUsbAutoResume(audioUsbDriver);
    if (err < 0) {
        if (atomic_dec_and_test(&audioUsbDriver->usageCount) != 0) {
            AUDIO_DRIVER_LOG_ERR("atomic_dec_and_test fail");
            wake_up(&audioUsbDriver->shutdownWait);
        }
        return err;
    }
    return 0;
}

static void AudioUsbAutoSuspend(struct AudioUsbDriver *audioUsbDriver)
{
    if (atomic_read(&audioUsbDriver->shutdown) != 0) {
        return;
    }

    if (!atomic_dec_and_test(&audioUsbDriver->active)) {
        return;
    }

    usb_autopm_put_interface(audioUsbDriver->usbIf);
}

/* autosuspend and unlock the shutdown */
static void AudioUsbUnlockShutdown(struct AudioUsbDriver *audioUsbDriver)
{
    AudioUsbAutoSuspend(audioUsbDriver);
    if (atomic_dec_and_test(&audioUsbDriver->usageCount) != 0) {
        wake_up(&audioUsbDriver->shutdownWait);
    }
}

static const struct UsbMixerNameMap *AudioUsbFindMap(
    const struct UsbMixerNameMap *usbMixerNameMap, int32_t unitId, int32_t control)
{
    if (usbMixerNameMap == NULL) {
        AUDIO_DRIVER_LOG_DEBUG("param is null");
        return NULL;
    }

    for (; usbMixerNameMap->id; usbMixerNameMap++) {
        if (usbMixerNameMap->id == unitId &&
            (control == 0 || usbMixerNameMap->control == 0 || control == usbMixerNameMap->control)) {
            return usbMixerNameMap;
        }
    }

    return NULL;
}

/* get the mapped name if the unit matches */
static int32_t AudioUsbCheckMappedName(const struct UsbMixerNameMap *usbMixerNameMap, char *buf, int32_t bufLen)
{
    if (usbMixerNameMap == NULL || usbMixerNameMap->name == NULL) {
        return 0;
    }

    bufLen--;
    return strlcpy(buf, usbMixerNameMap->name, bufLen);
}

/* ignore the error value if ignoreCtlError flag is set */
static inline int32_t AudioUsbFilterError(struct UsbMixerElemInfo *mixElemInfo, int32_t err)
{
    return (mixElemInfo->head.mixer->ignoreCtlError != 0) ? 0 : err;
}

/* check whether the control should be ignored */
static inline bool AudioUsbCheckIgnoredCtl(const struct UsbMixerNameMap *usbMixerNameMap)
{
    if (usbMixerNameMap == NULL || usbMixerNameMap->name != NULL || usbMixerNameMap->dB != NULL) {
        return false;
    }
    AUDIO_DRIVER_LOG_ERR("param is not null");
    return true;
}

/* get the mapped selector source name */
static int32_t AudioUsbCheckMappedSelectorName(
    struct UsbMixerBuild *state, int32_t unitId, int32_t index, char *buf, int32_t bufLen)
{
    const struct UsbMixerSelectorMap *usbMixerSelectorMap = NULL;
    (void)bufLen;

    if (state->selectorMap == NULL) {
        AUDIO_DRIVER_LOG_ERR("param is null");
        return HDF_FAILURE;
    }

    for (usbMixerSelectorMap = state->selectorMap; usbMixerSelectorMap->id != 0; usbMixerSelectorMap++) {
        if (usbMixerSelectorMap->id == unitId && index < usbMixerSelectorMap->count) {
            return strlcpy(buf, usbMixerSelectorMap->names[index], bufLen);
        }
    }

    return HDF_FAILURE;
}

/* find an audio control unit with the given unit id */
static void *AudioUsbFindAudioControlUnit(struct UsbMixerBuild *state, unsigned char unit)
{
    /* we just parse the header */
    struct uac_feature_unit_descriptor *featureUnitDesc = NULL;

    while ((featureUnitDesc = AudioUsbFindDesc(state->buffer, state->bufLen, featureUnitDesc, USB_DT_CS_INTERFACE)) !=
        NULL) {
        if (featureUnitDesc->bLength >= HDR_BLENGTH_MAX && featureUnitDesc->bDescriptorSubtype >= UAC_INPUT_TERMINAL &&
            featureUnitDesc->bDescriptorSubtype <= UAC3_SAMPLE_RATE_CONVERTER && featureUnitDesc->bUnitID == unit) {
            return featureUnitDesc;
        }
    }

    return NULL;
}

/* copy a string with the given id */
static uint32_t AudioUsbCopyStringDesc(struct AudioUsbDriver *audioUsbDriver, int32_t index, char *buf, int32_t maxLen)
{
    int32_t len = usb_string(audioUsbDriver->dev, index, buf, maxLen - 1);
    if (len < 0) {
        AUDIO_DRIVER_LOG_ERR("len error, len = %d", len);
        return 0;
    }

    buf[len] = 0;
    return len;
}

/* convert from the byte/word on usb descriptor to the zero-based integer */
static int32_t AudioUsbConvertSignedValue(struct UsbMixerElemInfo *mixElemInfo, int32_t value)
{
    bool temp;
    switch (mixElemInfo->valType) {
        case USB_MIXER_BOOLEAN:
            temp = !!value;
            return temp;
        case USB_MIXER_INV_BOOLEAN:
            temp = !value;
            return temp;
        case USB_MIXER_S8:
            value &= ZERO_BASED_INTEGER;
            if (value >= 0x80) {
                value -= 0x100;
            }
            break;
        case USB_MIXER_U8:
            value &= ZERO_BASED_INTEGER;
            break;
        case USB_MIXER_S16:
            value &= 0xffff;
            if (value >= 0x8000) {
                value -= 0x10000;
            }
            break;
        case USB_MIXER_U16:
            value &= 0xffff;
            break;
    }
    return value;
}

/* convert from the zero-based int32_t to the byte/word for usb descriptor */
static int32_t AudioUsbConvertBytesValue(struct UsbMixerElemInfo *mixElemInfo, int32_t value)
{
    switch (mixElemInfo->valType) {
        case USB_MIXER_BOOLEAN:
            return !!value;
        case USB_MIXER_INV_BOOLEAN:
            return !value;
        case USB_MIXER_S16:
        case USB_MIXER_U16:
            return value & 0xffff;
        case USB_MIXER_S8:
        case USB_MIXER_U8:
            return value & ZERO_BASED_INTEGER;
    }
    return 0; /* not reached */
}

static int32_t AudioUsbGetRelativeValue(struct UsbMixerElemInfo *mixElemInfo, int32_t value)
{
    if (mixElemInfo->res == 0) {
        mixElemInfo->res = 1;
    }
    if (value < mixElemInfo->min) {
        return 0;
    } else if (value >= mixElemInfo->max) {
        return (mixElemInfo->max - mixElemInfo->min + mixElemInfo->res - 1) / mixElemInfo->res;
    } else {
        return (value - mixElemInfo->min) / mixElemInfo->res;
    }
}

static int32_t AudioUsbGetAbsValue(struct UsbMixerElemInfo *mixElemInfo, int32_t value)
{
    if (value < 0) {
        return mixElemInfo->min;
    }
    if (!mixElemInfo->res) {
        mixElemInfo->res = 1;
    }
    value *= mixElemInfo->res;
    value += mixElemInfo->min;
    if (value > mixElemInfo->max) {
        return mixElemInfo->max;
    }
    return value;
}

static int32_t AudioUsbCtlValueSize(int32_t valType)
{
    switch (valType) {
        case USB_MIXER_S32:
        case USB_MIXER_U32:
            return USB_MIXER_S16;
        case USB_MIXER_S16:
        case USB_MIXER_U16:
            return USB_MIXER_S8;
        default:
            return USB_MIXER_INV_BOOLEAN;
    }
    return 0; /* unreachable */
}

/* retrieve a mixer value */
static inline int32_t AudioUsbMixerCtrlIntf(struct UsbMixerInterface *mixer)
{
    return AUDIO_USB_GET_IFACE_DESC(mixer->hostIf)->bInterfaceNumber;
}

static int32_t AudioUsbGetCtlValueV1(
    struct UsbMixerElemInfo *mixElemInfo, int32_t request, int32_t validx, int32_t *value)
{
    struct AudioUsbDriver *audioUsbDriver = mixElemInfo->head.mixer->audioUsbDriver;
    uint8_t buf[USB_VAL_LEN_2] = {0};
    int32_t valLen = mixElemInfo->valType >= USB_MIXER_S16 ? USB_VAL_LEN_2 : USB_VAL_LEN_1;
    int32_t timeout = USB_TIMEOUT;
    int32_t idx = 0, err;
    struct AudioUsbCtlMsgParam usbCtlMsgParam;

    err = AudioUsbLockShutdown(audioUsbDriver);
    if (err < 0) {
        return -EIO;
    }

    while (timeout-- > 0) {
        idx = AudioUsbMixerCtrlIntf(mixElemInfo->head.mixer) | (mixElemInfo->head.id << USB_SHIFT_SIZE_8);

        usbCtlMsgParam.pipe = usb_rcvctrlpipe(audioUsbDriver->dev, 0);
        usbCtlMsgParam.request = request;
        usbCtlMsgParam.requestType = USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_IN;
        usbCtlMsgParam.value = validx;
        usbCtlMsgParam.index = idx;

        err = AudioUsbCtlMsg(audioUsbDriver->dev, &usbCtlMsgParam, buf, valLen);
        if (err >= valLen) {
            *value = AudioUsbConvertSignedValue(mixElemInfo, AudioUsbCombineBytes(buf, valLen));
            err = 0;
            AudioUsbUnlockShutdown(audioUsbDriver);
            return err;
        } else if (err == -ETIMEDOUT) {
            AudioUsbUnlockShutdown(audioUsbDriver);
            return err;
        }
    }
    AUDIO_DRIVER_LOG_DEBUG("cannot get ctl value: req = %#x, wValue = %#x, wIndex = %#x, type = %d\n", request, validx,
        idx, mixElemInfo->valType);
    err = -EINVAL;
    AudioUsbUnlockShutdown(audioUsbDriver);
    return err;
}

static int32_t AudioUsbGetCtlValueV2(
    struct UsbMixerElemInfo *mixElemInfo, int32_t request, int32_t validx, int32_t *value)
{
    struct AudioUsbDriver *audioUsbDriver = mixElemInfo->head.mixer->audioUsbDriver;
    /* enough space for one range */
    uint8_t buf[sizeof(uint16_t) + USB_VAL_LEN_3 * sizeof(uint32_t)];
    uint8_t *valTemp;
    int32_t ret, valSize, size;
    struct AudioUsbCtlMsgParam usbCtlMsgParam;

    valSize = AudioUsbCtlValueSize(mixElemInfo->valType);

    if (request == UAC_GET_CUR) {
        usbCtlMsgParam.request = UAC2_CS_CUR;
        size = valSize;
    } else {
        usbCtlMsgParam.request = UAC2_CS_RANGE;
        size = sizeof(uint16_t) + USB_VAL_LEN_3 * valSize;
    }

    (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));

    ret = AudioUsbLockShutdown(audioUsbDriver) ? -EIO : 0;
    if (ret != 0) {
        AUDIO_DRIVER_LOG_ERR(
            "cannot get ctl value: req = %#x, wValue = %#x, type = %d\n", request, validx, mixElemInfo->valType);
        return ret;
    }

    usbCtlMsgParam.index = AudioUsbMixerCtrlIntf(mixElemInfo->head.mixer) | (mixElemInfo->head.id << USB_SHIFT_SIZE_8);

    usbCtlMsgParam.pipe = usb_rcvctrlpipe(audioUsbDriver->dev, 0);
    usbCtlMsgParam.requestType = USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_IN;
    usbCtlMsgParam.value = validx;

    ret = AudioUsbCtlMsg(audioUsbDriver->dev, &usbCtlMsgParam, buf, size);
    AudioUsbUnlockShutdown(audioUsbDriver);

    if (ret < 0) {
        AUDIO_DRIVER_LOG_ERR("cannot get ctl value: req = %#x, wValue = %#x, wIndex = %#x, type = %d\n", request,
            validx, usbCtlMsgParam.index, mixElemInfo->valType);
        return ret;
    }

    switch (request) {
        case UAC_GET_CUR:
            valTemp = buf;
            break;
        case UAC_GET_RES:
            valTemp = buf + sizeof(uint16_t) + valSize * USB_VAL_LEN_2;
            break;
        case UAC_GET_MIN:
            valTemp = buf + sizeof(uint16_t);
            break;
        case UAC_GET_MAX:
            valTemp = buf + sizeof(uint16_t) + valSize;
            break;

        default:
            return -EINVAL;
    }

    *value = AudioUsbConvertSignedValue(mixElemInfo, AudioUsbCombineBytes(valTemp, valSize));

    return 0;
}

static int32_t AudioUsbGetCtlValue(
    struct UsbMixerElemInfo *mixElemInfo, int32_t request, int32_t validx, int32_t *value)
{
    validx += mixElemInfo->idxOff;

    return (mixElemInfo->head.mixer->protocol == UAC_VERSION_1) ?
        AudioUsbGetCtlValueV1(mixElemInfo, request, validx, value) :
        AudioUsbGetCtlValueV2(mixElemInfo, request, validx, value);
}

static int32_t AudioUsbGetCurCtlValue(struct UsbMixerElemInfo *mixElemInfo, int32_t validx, int32_t *value)
{
    return AudioUsbGetCtlValue(mixElemInfo, UAC_GET_CUR, validx, value);
}

/* channel = 0: master, 1 = first channel */
static inline int32_t AudioUsbGetCurMixRaw(struct UsbMixerElemInfo *mixElemInfo, int32_t channel, int32_t *value)
{
    return AudioUsbGetCtlValue(mixElemInfo, UAC_GET_CUR, (mixElemInfo->control << USB_SHIFT_SIZE_8) | channel, value);
}

int32_t AudioUsbGetCurMixValue(struct UsbMixerElemInfo *mixElemInfo, int32_t channel, int32_t index, int32_t *value)
{
    int32_t err;

    if (mixElemInfo->cached & (1 << channel)) {
        *value = mixElemInfo->cacheVal[index];
        return 0;
    }
    err = AudioUsbGetCurMixRaw(mixElemInfo, channel, value);
    if (err < 0) {
        if (!mixElemInfo->head.mixer->ignoreCtlError) {
            AUDIO_DRIVER_LOG_ERR(
                "cannot get current value for control %d ch %d: err = %d\n", mixElemInfo->control, channel, err);
        }
        return err;
    }
    mixElemInfo->cached |= 1 << channel;
    mixElemInfo->cacheVal[index] = *value;
    return 0;
}

/* set a mixer value */
int32_t AudioUsbMixerSetCtlValue(
    struct UsbMixerElemInfo *mixElemInfo, int32_t request, int32_t validx, int32_t valueSet)
{
    struct AudioUsbDriver *audioUsbDriver = mixElemInfo->head.mixer->audioUsbDriver;
    uint8_t buf[USB_VAL_LEN_4] = {0};
    int32_t valLen, err, timeout = USB_TIMEOUT;
    struct AudioUsbCtlMsgParam usbCtlMsgParam;

    validx += mixElemInfo->idxOff;

    if (mixElemInfo->head.mixer->protocol == UAC_VERSION_1) {
        valLen = mixElemInfo->valType >= USB_MIXER_S16 ? USB_VAL_LEN_2 : USB_VAL_LEN_1;
    } else { /* UAC_VERSION_2/3 */
        valLen = AudioUsbCtlValueSize(mixElemInfo->valType);
        if (request != UAC_SET_CUR) {
            AUDIO_DEVICE_LOG_INFO("RANGE setting not yet supported\n");
            return -EINVAL;
        }
        request = UAC2_CS_CUR;
    }

    valueSet = AudioUsbConvertBytesValue(mixElemInfo, valueSet);
    buf[ARRAY_INDEX_0] = valueSet & ZERO_BASED_INTEGER;
    buf[ARRAY_INDEX_1] = (valueSet >> USB_SHIFT_SIZE_8) & ZERO_BASED_INTEGER;
    buf[ARRAY_INDEX_2] = (valueSet >> USB_SHIFT_SIZE_16) & ZERO_BASED_INTEGER;
    buf[ARRAY_INDEX_3] = (valueSet >> USB_SHIFT_SIZE_24) & ZERO_BASED_INTEGER;

    err = AudioUsbLockShutdown(audioUsbDriver);
    if (err < 0) {
        AUDIO_DEVICE_LOG_INFO("AudioUsbLockShutdown failed\n");
        return -EIO;
    }
    while (timeout-- > 0) {
        usbCtlMsgParam.index =
            AudioUsbMixerCtrlIntf(mixElemInfo->head.mixer) | (mixElemInfo->head.id << USB_SHIFT_SIZE_8);

        usbCtlMsgParam.pipe = usb_sndctrlpipe(audioUsbDriver->dev, 0);
        usbCtlMsgParam.request = request;
        usbCtlMsgParam.requestType = USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_OUT;
        usbCtlMsgParam.value = validx;

        err = AudioUsbCtlMsg(audioUsbDriver->dev, &usbCtlMsgParam, buf, valLen);
        if (err >= 0) {
            err = 0;
            AudioUsbUnlockShutdown(audioUsbDriver);
            return err;
        } else if (err == -ETIMEDOUT) {
            AudioUsbUnlockShutdown(audioUsbDriver);
            return err;
        }
    }
    AudioUsbUnlockShutdown(audioUsbDriver);
    return -EINVAL;
}

static int32_t AudioUsbSetCurCtlValue(struct UsbMixerElemInfo *mixElemInfo, int32_t validx, int32_t value)
{
    return AudioUsbMixerSetCtlValue(mixElemInfo, UAC_SET_CUR, validx, value);
}

int32_t AudioUsbSetCurMixValue(struct UsbMixerElemInfo *mixElemInfo, int32_t channel, int32_t index, int32_t value)
{
    int32_t err;
    uint32_t readOnly = (channel == 0) ? mixElemInfo->masterReadonly : mixElemInfo->chReadonly & (1 << (channel - 1));

    if (readOnly != 0) {
        AUDIO_DEVICE_LOG_DEBUG("%s(): channel %d of control %d is readOnly\n", __func__, channel, mixElemInfo->control);
        return 0;
    }

    err =
        AudioUsbMixerSetCtlValue(mixElemInfo, UAC_SET_CUR, (mixElemInfo->control << USB_SHIFT_SIZE_8) | channel, value);
    if (err < 0) {
        AUDIO_DEVICE_LOG_ERR("AudioUsbMixerSetCtlValue failed %d.", err);
        return err;
    }
    mixElemInfo->cached |= 1 << channel;
    mixElemInfo->cacheVal[index] = value;
    return 0;
}

static int32_t AudioUsbParseUnit(struct UsbMixerBuild *state, int32_t unitid);

static int32_t AudioUsbCheckMatrixBitmap(uint8_t *bmap, int32_t ich, int32_t och, int32_t numOuts)
{
    int32_t idx = ich * numOuts + och;
    return bmap[idx >> USB_SHIFT_SIZE_3] & (0x80 >> (idx & CHANNEL_HDR_BLENGTH));
}

static int32_t AudioUsbGetTermNameSub(struct UsbAudioTerm *iterm, uint8_t *name)
{
    char *itemName = "Main";

    switch (iterm->type) {
        case 0x0300 ... 0x0307:
        case 0x0600 ... 0x0607:
        case 0x0700 ... 0x0713:
            strcpy_s(name, KCTL_NAME_LEN, itemName);
            return strlen(itemName);
        default:
            return 0;
    }
}

static int32_t AudioUsbGetTermName(
    struct AudioUsbDriver *audioUsbDriver, struct UsbAudioTerm *iterm, uint8_t *name, int32_t maxLen, int32_t termOnly)
{
    int32_t len;
    if (iterm->name) {
        len = AudioUsbCopyStringDesc(audioUsbDriver, iterm->name, name, maxLen);
        if (len != 0) {
            return len;
        }
    }

    /* virtual type - not a real terminal */
    if (iterm->type >> USB_SHIFT_SIZE_16) {
        if (termOnly != 0) {
            return 0;
        }
        switch (iterm->type >> USB_SHIFT_SIZE_16) {
            case UAC3_SELECTOR_UNIT:
                (void)strcpy_s(name, KCTL_NAME_LEN, "Selector");
                return strlen("Selector");
            case UAC3_PROCESSING_UNIT:
                (void)strcpy_s(name, KCTL_NAME_LEN, "Process Unit");
                return strlen("Process Unit");
            case UAC3_EXTENSION_UNIT:
                (void)strcpy_s(name, KCTL_NAME_LEN, "Ext Unit");
                return strlen("Ext Unit");
            case UAC3_MIXER_UNIT:
                (void)strcpy_s(name, KCTL_NAME_LEN, "Mixer");
                return strlen("Mixer");
            default:
                return sprintf(name, "Unit %d", iterm->id);
        }
    }

    switch (iterm->type & 0xff00) {
        case 0x0100:
            (void)strcpy_s(name, KCTL_NAME_LEN, "Main");
            return strlen("Main");
        case 0x0200:
            (void)strcpy_s(name, KCTL_NAME_LEN, "Mic");
            return strlen("Mic");
        case 0x0400:
            (void)strcpy_s(name, KCTL_NAME_LEN, "Headset");
            return strlen("Headset");
        case 0x0500:
            (void)strcpy_s(name, KCTL_NAME_LEN, "Phone");
            return strlen("Phone");
    }

    return AudioUsbGetTermNameSub(iterm, name);
}

static inline int32_t AudioUsbCtrlIntf(struct AudioUsbDriver *audioUsbDriver)
{
    return AUDIO_USB_GET_IFACE_DESC(audioUsbDriver->ctrlIntf)->bInterfaceNumber;
}

/* Get logical cluster information for UAC3 devices */
static int32_t AudioUsbGetClusterChannelsV3(struct UsbMixerBuild *state, uint32_t clusterId)
{
    struct uac3_cluster_header_descriptor uacHeader;
    int32_t err;
    struct AudioUsbCtlMsgParam usbCtlMsgParam;

    usbCtlMsgParam.pipe = usb_rcvctrlpipe(state->audioUsbDriver->dev, 0);
    usbCtlMsgParam.request = UAC3_CS_REQ_HIGH_CAPABILITY_DESCRIPTOR;
    usbCtlMsgParam.requestType = USB_RECIP_INTERFACE | USB_TYPE_CLASS | USB_DIR_IN;
    usbCtlMsgParam.value = clusterId;
    usbCtlMsgParam.index = AudioUsbCtrlIntf(state->audioUsbDriver);

    err = AudioUsbCtlMsg(state->audioUsbDriver->dev, &usbCtlMsgParam, &uacHeader, sizeof(uacHeader));
    if (err < 0) {
        AUDIO_DRIVER_LOG_ERR("cannot request logical cluster ID: %d (err: %d)\n", clusterId, err);
        return err;
    }
    if (err != sizeof(uacHeader)) {
        err = -EIO;
        AUDIO_DRIVER_LOG_ERR("cannot request logical cluster ID: %d (err: %d)\n", clusterId, err);
        return err;
    }

    return uacHeader.bNrChannels;
}

/* Get number of channels for a Mixer Unit */
static int32_t AudioUsbMixerUnitGetChannels(struct UsbMixerBuild *state, struct uac_mixer_unit_descriptor *desc)
{
    int32_t muChannels;
    uint32_t temp;

    switch (state->mixer->protocol) {
        case UAC_VERSION_1:
        case UAC_VERSION_2:
        default:
            temp = sizeof(*desc) + desc->bNrInPins + 1;
            if (desc->bLength < temp) {
                return 0;
            }
            muChannels = uac_mixer_unit_bNrChannels(desc);
            break;
        case UAC_VERSION_3:
            muChannels = AudioUsbGetClusterChannelsV3(state, uac3_mixer_unit_wClusterDescrID(desc));
            break;
    }

    return muChannels;
}

/* Parse Input Terminal Unit */
static int32_t AudioUsbCheckInputTermSub(struct UsbMixerBuild *state, int32_t id, struct UsbAudioTerm *term);

static int32_t AudioUsbParseTermUac1ItermUnit(
    struct UsbMixerBuild *state, struct UsbAudioTerm *term, void *unitDesc, int32_t id)
{
    struct uac_input_terminal_descriptor *inptTermDesc = unitDesc;

    term->type = le16_to_cpu(inptTermDesc->wTerminalType);
    term->channels = inptTermDesc->bNrChannels;
    term->chconfig = le16_to_cpu(inptTermDesc->wChannelConfig);
    term->name = inptTermDesc->iTerminal;
    return 0;
}

static int32_t AudioUsbParseTermMixerUnit(
    struct UsbMixerBuild *state, struct UsbAudioTerm *term, void *unitDesc, int32_t id)
{
    struct uac_mixer_unit_descriptor *mixUnitDesc = unitDesc;
    int32_t protocol = state->mixer->protocol;
    int32_t err;

    err = AudioUsbMixerUnitGetChannels(state, mixUnitDesc);
    if (err <= 0) {
        return err;
    }

    term->type = UAC3_MIXER_UNIT << USB_SHIFT_SIZE_16; /* virtual type */
    term->channels = err;
    if (protocol != UAC_VERSION_3) {
        term->chconfig = uac_mixer_unit_wChannelConfig(mixUnitDesc, protocol);
        term->name = uac_mixer_unit_iMixer(mixUnitDesc);
    }
    return 0;
}

static int32_t AudioUsbParseTermSelectorUnit(
    struct UsbMixerBuild *state, struct UsbAudioTerm *term, void *unitDesc, int32_t id)
{
    struct uac_selector_unit_descriptor *selUnitDesc = unitDesc;
    int32_t err;

    /* call recursively to retrieve the channel info */
    err = AudioUsbCheckInputTermSub(state, selUnitDesc->baSourceID[0], term);
    if (err < 0)
        return err;
    term->type = UAC3_SELECTOR_UNIT << USB_SHIFT_SIZE_16; /* virtual type */
    term->id = id;
    if (state->mixer->protocol != UAC_VERSION_3)
        term->name = uac_selector_unit_iSelector(selUnitDesc);
    return 0;
}

static int32_t AudioUsbParseTermProcUnit(
    struct UsbMixerBuild *state, struct UsbAudioTerm *term, void *unitDesc, int32_t id, int32_t vtype)
{
    struct uac_processing_unit_descriptor *procUnitDesc = unitDesc;
    int32_t protocol = state->mixer->protocol;
    int32_t err;
    AUDIO_DRIVER_LOG_DEBUG("entry");

    if (procUnitDesc->bNrInPins) {
        err = AudioUsbCheckInputTermSub(state, procUnitDesc->baSourceID[0], term);
        if (err < 0) {
            AUDIO_DRIVER_LOG_ERR("AudioUsbCheckInputTermSub if failed");
            return err;
        }
    }

    term->type = vtype << USB_SHIFT_SIZE_16;
    term->id = id;

    if (protocol == UAC_VERSION_3) {
        AUDIO_DRIVER_LOG_INFO("UAC_VERSION_3 is not support");
        return 0;
    }
    if (!term->channels) {
        term->channels = uac_processing_unit_bNrChannels(procUnitDesc);
        term->chconfig = uac_processing_unit_wChannelConfig(procUnitDesc, protocol);
    }
    term->name = uac_processing_unit_iProcessing(procUnitDesc, protocol);
    return 0;
}

static int32_t AudioUsbCheckInputTermSub(struct UsbMixerBuild *state, int32_t id, struct UsbAudioTerm *term)
{
    int32_t protocol = state->mixer->protocol;
    void *hdrRet = NULL;
    uint8_t *hdr = NULL;

    for (;;) {
        /* a loop in the terminal chain? */
        if (test_and_set_bit(id, state->termbitmap)) {
            return -EINVAL;
        }

        hdrRet = AudioUsbFindAudioControlUnit(state, id);
        if (hdrRet == NULL || !AudioUsbValidateAudioDesc(hdrRet, protocol)) {
            break; /* bad descriptor */
        }

        hdr = hdrRet;
        term->id = id;

        switch (((protocol << USB_SHIFT_SIZE_8) | hdr[USB_VAL_LEN_2])) {
            case 0x0006:
            case 0x2006:
            case 0x3007:
                id = ((struct uac_feature_unit_descriptor *)hdrRet)->bSourceID;
                break; /* continue to parse */
            case 0x0002:
                return AudioUsbParseTermUac1ItermUnit(state, term, hdrRet, id);
            case 0x0004:
            case 0x2004:
            case 0x3005:
                return AudioUsbParseTermMixerUnit(state, term, hdrRet, id);
            case 0x0005:
            case 0x2005:
            case 0x200b:
            case 0x3006:
            case 0x300c:
                return AudioUsbParseTermSelectorUnit(state, term, hdrRet, id);
            case 0x0007:
            case 0x2008:
            case 0x3009:
                return AudioUsbParseTermProcUnit(state, term, hdrRet, id, UAC3_PROCESSING_UNIT);
            case 0x0008:
            case 0x2009:
            case 0x300a:
                return AudioUsbParseTermProcUnit(state, term, hdrRet, id, UAC3_EXTENSION_UNIT);
            default:
                return -ENODEV;
        }
    }
    return -ENODEV;
}

static int32_t AudioUsbCheckInputTerm(struct UsbMixerBuild *state, int32_t id, struct UsbAudioTerm *term)
{
    (void)memset_s(term, sizeof(*term), 0, sizeof(*term));
    (void)memset_s(state->termbitmap, sizeof(state->termbitmap), 0, sizeof(state->termbitmap));
    return AudioUsbCheckInputTermSub(state, id, term);
}

/* feature unit control information */
struct AudioUsbFeatureControlInfo {
    int32_t control;
    const char *name;
    int32_t type;     /* data type for uac1 */
    int32_t typeUac2; /* data type for uac2 if different from uac1, else -1 */
};

#define USB_MIX_TYPE_ERROR (-1)

static const struct AudioUsbFeatureControlInfo g_audioFeatureInfo[] = {
    {UAC_FU_MUTE,              "Mute",                   USB_MIXER_INV_BOOLEAN, USB_MIX_TYPE_ERROR},
    {UAC_FU_VOLUME,            "Volume",                 USB_MIXER_S16,         USB_MIX_TYPE_ERROR},
    {UAC_FU_BASS,              "Tone Control - Bass",    USB_MIXER_S8,          USB_MIX_TYPE_ERROR},
    {UAC_FU_MID,               "Tone Control - Mid",     USB_MIXER_S8,          USB_MIX_TYPE_ERROR},
    {UAC_FU_TREBLE,            "Tone Control - Treble",  USB_MIXER_S8,          USB_MIX_TYPE_ERROR},
    {UAC_FU_GRAPHIC_EQUALIZER, "Graphic Equalizer",      USB_MIXER_S8,          USB_MIX_TYPE_ERROR},
    {UAC_FU_AUTOMATIC_GAIN,    "Auto Gain Control",      USB_MIXER_BOOLEAN,     USB_MIX_TYPE_ERROR},
    {UAC_FU_DELAY,             "Delay Control",          USB_MIXER_U16,         USB_MIXER_U32     },
    {UAC_FU_BASS_BOOST,        "Bass Boost",             USB_MIXER_BOOLEAN,     USB_MIX_TYPE_ERROR},
    {UAC_FU_LOUDNESS,          "Loudness",               USB_MIXER_BOOLEAN,     USB_MIX_TYPE_ERROR},
    {UAC2_FU_INPUT_GAIN,       "Input Gain Control",     USB_MIXER_S16,         USB_MIX_TYPE_ERROR},
    {UAC2_FU_INPUT_GAIN_PAD,   "Input Gain Pad Control", USB_MIXER_S16,         USB_MIX_TYPE_ERROR},
    {UAC2_FU_PHASE_INVERTER,   "Phase Inverter Control", USB_MIXER_BOOLEAN,     USB_MIX_TYPE_ERROR},
};

static void AudioUsbMixerElemInfoFree(struct UsbMixerElemInfo *mixElemInfo)
{
    OsalMemFree(mixElemInfo);
}

static void AudioUsbCtlGetMaxVal(struct UsbMixerElemInfo *mixElemInfo, int32_t minchn)
{
    int32_t saved, temp, check;

    if (AudioUsbGetCurMixRaw(mixElemInfo, minchn, &saved) < 0) {
        mixElemInfo->initialized = 1;
        return;
    }
    for (;;) {
        temp = saved;
        if (temp < mixElemInfo->max) {
            temp += mixElemInfo->res;
        } else {
            temp -= mixElemInfo->res;
        }
        if (temp < mixElemInfo->min || temp > mixElemInfo->max ||
            AudioUsbSetCurMixValue(mixElemInfo, minchn, 0, temp) || AudioUsbGetCurMixRaw(mixElemInfo, minchn, &check)) {
            mixElemInfo->res = mixElemInfo->res;
            break;
        }
        if (temp == check) {
            break;
        }
        mixElemInfo->res *= VOLUME_RANGE;
    }
    AudioUsbSetCurMixValue(mixElemInfo, minchn, 0, saved);
}

static int32_t AudioUsbCtlGetMinMaxValSub(struct UsbMixerElemInfo *mixElemInfo)
{
    int32_t minchn = 0;
    int32_t i;
    int32_t request = (mixElemInfo->control << USB_SHIFT_SIZE_8) | minchn;
    if (mixElemInfo->cmask) {
        for (i = 0; i < MAX_CHANNELS; i++) {
            if (mixElemInfo->cmask & (1 << i)) {
                minchn = i + 1;
                break;
            }
        }
    }
    if (AudioUsbGetCtlValue(mixElemInfo, UAC_GET_MAX, request, &mixElemInfo->max) < 0 ||
        AudioUsbGetCtlValue(mixElemInfo, UAC_GET_MIN, request, &mixElemInfo->min) < 0) {
        AUDIO_DRIVER_LOG_ERR("%d:%d: cannot get min/max values for control %d (id %d)\n", mixElemInfo->head.id,
            AudioUsbMixerCtrlIntf(mixElemInfo->head.mixer), mixElemInfo->control, mixElemInfo->head.id);
        return -EINVAL;
    }

    if (AudioUsbGetCtlValue(mixElemInfo, UAC_GET_RES, request, &mixElemInfo->res) < 0) {
        mixElemInfo->res = 1;
    } else {
        int32_t lastValidRes = mixElemInfo->res;
        while (mixElemInfo->res > 1) {
            if (AudioUsbMixerSetCtlValue(mixElemInfo, UAC_SET_RES, request, mixElemInfo->res / VOLUME_RANGE) < 0) {
                break;
            }
            mixElemInfo->res /= VOLUME_RANGE;
        }
        if (AudioUsbGetCtlValue(mixElemInfo, UAC_GET_RES, request, &mixElemInfo->res) < 0) {
            mixElemInfo->res = lastValidRes;
        }
    }
    if (mixElemInfo->res == 0) {
        mixElemInfo->res = 1;
    }

    if (mixElemInfo->min + mixElemInfo->res < mixElemInfo->max) {
        AudioUsbCtlGetMaxVal(mixElemInfo, minchn);
    }

    return 0;
}

static int32_t AudioUsbCtlGetMinMaxVal(
    struct UsbMixerElemInfo *mixElemInfo, int32_t default_min, struct AudioKcontrol *kctl)
{
    /* for failsafe */
    mixElemInfo->min = default_min;
    mixElemInfo->max = mixElemInfo->min + 1;
    mixElemInfo->res = 1;
    mixElemInfo->dBmin = mixElemInfo->dBmax = 0;

    if (mixElemInfo->valType == USB_MIXER_BOOLEAN || mixElemInfo->valType == USB_MIXER_INV_BOOLEAN) {
        mixElemInfo->initialized = 1;
    } else {
        AudioUsbCtlGetMinMaxValSub(mixElemInfo);
    }

    mixElemInfo->dBmin = (AudioUsbConvertSignedValue(mixElemInfo, mixElemInfo->min) * USB_DESCRIPTIONS_CONTAIN_100DB) /
        USB_DESCRIPTIONS_CONTAIN_256DB;
    mixElemInfo->dBmax = (AudioUsbConvertSignedValue(mixElemInfo, mixElemInfo->max) * USB_DESCRIPTIONS_CONTAIN_100DB) /
        USB_DESCRIPTIONS_CONTAIN_256DB;
    if (mixElemInfo->dBmin > mixElemInfo->dBmax) {
        /* something is wrong; assume it's either from/to 0dB */
        if (mixElemInfo->dBmin < 0) {
            mixElemInfo->dBmax = 0;
        } else if (mixElemInfo->dBmin > 0) {
            mixElemInfo->dBmin = 0;
        }
        if (mixElemInfo->dBmin > mixElemInfo->dBmax) {
            /* totally crap, return an error */
            AUDIO_DRIVER_LOG_ERR("mixElemInfo is invalid.");
            return HDF_FAILURE;
        }
    } else {
        /* if the max volume is too low, it's likely a bogus range;
         * here we use -96dB as the threshold
         */
        if (mixElemInfo->dBmax <= -USB_DESCRIPTIONS_CONTAIN_9600DB) {
            AUDIO_DRIVER_LOG_INFO("%d:%d: bogus dB values (%d/%d), disabling dB reporting\n", mixElemInfo->head.id,
                AudioUsbMixerCtrlIntf(mixElemInfo->head.mixer), mixElemInfo->dBmin, mixElemInfo->dBmax);
            mixElemInfo->dBmin = mixElemInfo->dBmax = 0;
        }
    }

    return HDF_SUCCESS;
}

/* get a feature/mixer unit info */
static int32_t AudioUsbMixerCtlFeatureInfo(const struct AudioKcontrol *kcontrol, struct AudioCtrlElemInfo *uinfo)
{
    struct UsbMixerElemInfo *mixElemInfo = (struct UsbMixerElemInfo *)((volatile uintptr_t)kcontrol->privateValue);
    AUDIO_DEVICE_LOG_INFO("entry.");

    if (mixElemInfo->valType == USB_MIXER_BOOLEAN || mixElemInfo->valType == USB_MIXER_INV_BOOLEAN) {
        uinfo->type = AUDIO_USB_CTL_ELEM_TYPE_BOOLEAN;
    } else {
        uinfo->type = AUDIO_USB_CTL_ELEM_TYPE_INTEGER;
    }
    uinfo->count = mixElemInfo->channels;
    if (mixElemInfo->valType == USB_MIXER_BOOLEAN || mixElemInfo->valType == USB_MIXER_INV_BOOLEAN) {
        uinfo->min = 0;
        uinfo->max = 1;
    } else {
        uinfo->min = 0;
        uinfo->max = (mixElemInfo->max - mixElemInfo->min + mixElemInfo->res - 1) / mixElemInfo->res;
    }
    return 0;
}

/* get the current value from feature/mixer unit */
static int32_t AudioUsbMixerCtlFeatureGet(const struct AudioKcontrol *kcontrol, struct AudioCtrlElemValue *ucontrol)
{
    struct UsbMixerElemInfo *mixElemInfo = (struct UsbMixerElemInfo *)((volatile uintptr_t)kcontrol->privateValue);
    int32_t value, err;
    AUDIO_DEVICE_LOG_INFO("entry");

    ucontrol->value[0] = mixElemInfo->min;
    ucontrol->value[1] = mixElemInfo->max;

    /* master channel */
    err = AudioUsbGetCurMixValue(mixElemInfo, 0, 0, &value);
    if (err < 0) {
        return AudioUsbFilterError(mixElemInfo, err);
    }
    value = AudioUsbGetRelativeValue(mixElemInfo, value);

    ucontrol->value[0] = value;

    return 0;
}

/* get the current value from feature/mixer unit */
static int32_t AudioUsbMixerCtlFeatureMultGet(const struct AudioKcontrol *kcontrol, struct AudioCtrlElemValue *ucontrol)
{
    struct UsbMixerElemInfo *mixElemInfo = (struct UsbMixerElemInfo *)((volatile uintptr_t)kcontrol->privateValue);
    int32_t value, err;
    bool multStatus;
    ucontrol->value[0] = mixElemInfo->min;
    ucontrol->value[1] = mixElemInfo->max;

    /* master channel */
    err = AudioUsbGetCurMixValue(mixElemInfo, 0, 0, &value);
    if (err < 0) {
        return AudioUsbFilterError(mixElemInfo, err);
    }
    value = AudioUsbGetRelativeValue(mixElemInfo, value);

    multStatus = value > 0 ? false : true;
    ucontrol->value[0] = multStatus;

    return 0;
}

/* put the current value to feature/mixer unit */
static int32_t AudioUsbmixerCtlFeatureSet(
    const struct AudioKcontrol *kcontrol, const struct AudioCtrlElemValue *ucontrol)
{
    struct UsbMixerElemInfo *mixElemInfo = (struct UsbMixerElemInfo *)((volatile uintptr_t)kcontrol->privateValue);
    int32_t value, oval, err;

    /* master channel */
    err = AudioUsbGetCurMixValue(mixElemInfo, 0, 0, &oval);
    if (err < 0) {
        return AudioUsbFilterError(mixElemInfo, err);
    }
    value = ucontrol->value[0];

    value = AudioUsbGetAbsValue(mixElemInfo, value);
    if (value != oval) {
        AudioUsbSetCurMixValue(mixElemInfo, 0, 0, value);
    }

    return 0;
}

/* put the current value to feature/mixer unit */
static int32_t AudioUsbMixerCtlFeatureMultSet(
    const struct AudioKcontrol *kcontrol, const struct AudioCtrlElemValue *ucontrol)
{
    struct UsbMixerElemInfo *mixElemInfo = (struct UsbMixerElemInfo *)((volatile uintptr_t)kcontrol->privateValue);
    int32_t value, oval, err;
    bool multStatus;

    /* master channel */
    err = AudioUsbGetCurMixValue(mixElemInfo, 0, 0, &oval);
    if (err < 0) {
        return AudioUsbFilterError(mixElemInfo, err);
    }
    value = ucontrol->value[0];

    multStatus = value > 0 ? false : true;
    value = AudioUsbGetAbsValue(mixElemInfo, multStatus);
    if (value != oval) {
        AudioUsbSetCurMixValue(mixElemInfo, 0, 0, value);
    }

    return 0;
}

static struct AudioKcontrol g_usbFeatureUnitCtl = {
    .iface = AUDIO_USB_CTL_ELEM_IFACE_MIXER,
    .name = "", /* will be filled later manually */
    .Info = AudioUsbMixerCtlFeatureInfo,
    .Get = AudioUsbMixerCtlFeatureGet,
    .Set = AudioUsbmixerCtlFeatureSet,
};

static struct AudioKcontrol g_usbFeatureUnitCtlMult = {
    .iface = AUDIO_USB_CTL_ELEM_IFACE_MIXER,
    .name = "", /* will be filled later manually */
    .Info = AudioUsbMixerCtlFeatureInfo,
    .Get = AudioUsbMixerCtlFeatureMultGet,
    .Set = AudioUsbMixerCtlFeatureMultSet,
};

/* the read-only variant */
static struct AudioKcontrol g_usbFeatureUnitCtlRo = {
    .iface = AUDIO_USB_CTL_ELEM_IFACE_MIXER,
    .name = "", /* will be filled later manually */
    .Info = AudioUsbMixerCtlFeatureInfo,
    .Get = AudioUsbMixerCtlFeatureGet,
    .Set = NULL,
};

/* build a feature control */
static size_t AudioUsbAppendCtlName(struct AudioKcontrol *kctl, const char *str, int32_t size)
{
    return strlcat(kctl->name, str, size);
}

static const struct AudioUsbFeatureControlInfo *AudioUsbGetFeatureControlInfo(int32_t control)
{
    int32_t i;

    for (i = 0; i < ARRAY_SIZE(g_audioFeatureInfo); ++i) {
        if (g_audioFeatureInfo[i].control == control) {
            return &g_audioFeatureInfo[i];
        }
    }
    return NULL;
}

static int32_t AudioUsbFeatureCtlInit(struct UsbMixerInterface *mixer, struct AudioKcontrol **kctl,
    struct UsbMixerElemInfo *mixElemInfo, struct AudioUsbFeatureControl *featureControl)
{
    const struct AudioUsbFeatureControlInfo *ctlInfo;
    int32_t i, c = 0;

    AudioUsbMixerElemInitStd(&mixElemInfo->head, mixer, featureControl->unitId);
    mixElemInfo->control = featureControl->control;
    mixElemInfo->cmask = featureControl->ctlMask;

    ctlInfo = AudioUsbGetFeatureControlInfo(featureControl->control);
    if (!ctlInfo) {
        AudioUsbMixerElemInfoFree(mixElemInfo);
        AUDIO_DEVICE_LOG_ERR("AudioUsbGetFeatureControlInfo failed.");
        return HDF_FAILURE;
    }

    if (mixer->protocol == UAC_VERSION_1) {
        mixElemInfo->valType = ctlInfo->type;
    } else { /* UAC_VERSION_2 */
        mixElemInfo->valType = ctlInfo->typeUac2 >= 0 ? ctlInfo->typeUac2 : ctlInfo->type;
    }
    if (featureControl->ctlMask == 0) {
        mixElemInfo->channels = 1; /* master channel */
        mixElemInfo->masterReadonly = featureControl->readOnlyMask;
    } else {
        for (i = 0; i < USB_SHIFT_SIZE_16; i++) {
            if (featureControl->ctlMask & (1 << i)) {
                c++;
            }
        }
        mixElemInfo->channels = c;
        mixElemInfo->chReadonly = featureControl->readOnlyMask;
    }

    if (mixElemInfo->channels == featureControl->readOnlyMask) {
        *kctl = &g_usbFeatureUnitCtlRo;
    } else {
        *kctl = &g_usbFeatureUnitCtl;
    }

    if (featureControl->control == UAC_FU_MUTE) {
        *kctl = &g_usbFeatureUnitCtlMult;
    }
    DListHeadInit(&((*kctl)->list));
    return HDF_SUCCESS;
}

static void AudioUsbSetVolumeItemName(struct UsbMixerBuild *state, struct AudioKcontrol *kcontrol,
    struct UsbAudioTerm *iterm, struct AudioUsbFeatureControl *featureControl, const struct UsbMixerNameMap *map)
{
    struct UsbMixerInterface *mixer = state->mixer;
    struct UsbAudioTerm *oterm = &state->oterm;
    uint32_t len = 0;
    int32_t mappedName = 0;

    len = AudioUsbCheckMappedName(map, kcontrol->name, KCTL_NAME_LEN);
    mappedName = len != 0;
    if (len == 0 && featureControl->nameid) {
        len = AudioUsbCopyStringDesc(mixer->audioUsbDriver, featureControl->nameid, kcontrol->name, KCTL_NAME_LEN);
    }

    if (len == 0) {
        if (iterm) {
            len = AudioUsbGetTermName(mixer->audioUsbDriver, iterm, kcontrol->name, KCTL_NAME_LEN, 1);
        }
        if (len == 0 && oterm) {
            len = AudioUsbGetTermName(mixer->audioUsbDriver, oterm, kcontrol->name, KCTL_NAME_LEN, 1);
        }
        if (len == 0) {
            snprintf(kcontrol->name, KCTL_NAME_LEN, "Feature %d", featureControl->unitId);
        }
    }
    if (!mappedName && oterm && !(oterm->type >> USB_SHIFT_SIZE_16)) {
        if ((oterm->type & 0xff00) == 0x0100) {
            AudioUsbAppendCtlName(kcontrol, " Capture", KCTL_NAME_LEN);
        } else {
            AudioUsbAppendCtlName(kcontrol, " Playback", KCTL_NAME_LEN);
        }
    }
    AudioUsbAppendCtlName(kcontrol, featureControl->control == UAC_FU_MUTE ? " Mute" : " Volume", KCTL_NAME_LEN);
}

static int32_t AudioUsbSetKctlItermName(struct UsbMixerBuild *state, struct AudioKcontrol *kctl,
    struct UsbAudioTerm *iterm, struct AudioUsbFeatureControl *featureControl)
{
    uint32_t len = 0;
    bool mappedName = 0;
    struct UsbMixerInterface *mixer = state->mixer;
    const struct UsbMixerNameMap *imap = state->map;
    struct UsbAudioTerm *oterm = &state->oterm;
    const struct UsbMixerNameMap *map;

    map = AudioUsbFindMap(imap, featureControl->unitId, featureControl->control);
    if (AudioUsbCheckIgnoredCtl(map)) {
        AUDIO_DRIVER_LOG_ERR("map is invalid.");
        return HDF_FAILURE;
    }

    len = AudioUsbCheckMappedName(map, kctl->name, KCTL_NAME_LEN);
    mappedName = len != 0;
    if (len == 0 && featureControl->nameid) {
        len = AudioUsbCopyStringDesc(mixer->audioUsbDriver, featureControl->nameid, kctl->name, KCTL_NAME_LEN);
    }

    switch (featureControl->control) {
        case UAC_FU_VOLUME:
            AudioUsbSetVolumeItemName(state, kctl, iterm, featureControl, map);
            break;

        case UAC_FU_MUTE:
            if (!mappedName && oterm && !(oterm->type >> USB_SHIFT_SIZE_16)) {
                if ((oterm->type & 0xff00) == 0x0100) {
                    AudioUsbAppendCtlName(kctl, "Capture", KCTL_NAME_LEN);
                } else {
                    AudioUsbAppendCtlName(kctl, "Playback", KCTL_NAME_LEN);
                }
            }
            AudioUsbAppendCtlName(kctl, featureControl->control == UAC_FU_MUTE ? " Mute" : " Volume", KCTL_NAME_LEN);
            break;
        default:
            if (len == 0) {
                strlcpy(kctl->name, g_audioFeatureInfo[featureControl->control - 1].name, KCTL_NAME_LEN);
            }
            break;
    }
    return HDF_SUCCESS;
}

static void AudioUsbBuildFeatureCtlSub(
    struct UsbMixerBuild *state, struct UsbAudioTerm *iterm, struct AudioUsbFeatureControl *featureControl)
{
    struct AudioKcontrol *kctl = NULL;
    struct AudioKcontrol *kControl;
    struct UsbMixerElemInfo *mixElemInfo = NULL;
    struct AudioCard *audioCard;
    int ret;
    struct UsbMixerInterface *mixer;

    if (featureControl->control == UAC_FU_GRAPHIC_EQUALIZER) {
        AUDIO_DEVICE_LOG_INFO("not supported yet");
        return;
    }
    mixer = state->mixer;
    mixElemInfo = OsalMemCalloc(sizeof(*mixElemInfo));
    if (!mixElemInfo) {
        AUDIO_DEVICE_LOG_ERR("OsalMemCalloc is failed");
        return;
    }

    ret = AudioUsbFeatureCtlInit(mixer, &kctl, mixElemInfo, featureControl);
    if (ret != HDF_SUCCESS || kctl == NULL) {
        AUDIO_DEVICE_LOG_ERR("AudioUsbFeatureCtlInit is failed");
        return;
    }

    kctl->name = (char *)OsalMemCalloc(KCTL_NAME_LEN);
    ret = AudioUsbSetKctlItermName(state, kctl, iterm, featureControl);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(kctl->name);
        return;
    }

    /* get min/max values */
    ret = AudioUsbCtlGetMinMaxVal(mixElemInfo, 0, kctl);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(kctl->name);
        return;
    }

    /* skip a bogus volume range */
    if (mixElemInfo->max <= mixElemInfo->min) {
        OsalMemFree(kctl->name);
        AUDIO_DRIVER_LOG_ERR("mixElemInfo is invalid.");
        return;
    }

    audioCard = mixer->audioUsbDriver->audioCard;
    kctl->privateValue = (unsigned long)(uintptr_t)(void *)mixElemInfo;

    kControl = AudioAddControl(audioCard, kctl);
    if (kControl == NULL) {
        AUDIO_DEVICE_LOG_ERR("Add control fail!");
        return;
    }

    DListInsertHead(&kControl->list, &audioCard->controls);
}

static void AudioUsbBuildFeatureCtl(struct UsbMixerBuild *state, void *rawDesc, struct UsbAudioTerm *iterm,
    struct AudioUsbFeatureControl *featureControl)
{
    struct uac_feature_unit_descriptor *desc = rawDesc;
    featureControl->nameid = uac_feature_unit_iFeature(desc);

    AudioUsbBuildFeatureCtlSub(state, iterm, featureControl);
}

static int32_t AudioUsbParseFeatureDescriptorParam(
    struct UsbMixerBuild *state, void *ftrSub, struct AudioUsbFeatureParam *featureParam)
{
    struct uac_feature_unit_descriptor *hdr = ftrSub;

    if (state->mixer->protocol == UAC_VERSION_1) {
        featureParam->csize = hdr->bControlSize;
        featureParam->channels = (hdr->bLength - CHANNEL_HDR_BLENGTH) / featureParam->csize - 1;
        featureParam->bmaControls = hdr->bmaControls;
    } else { /* UAC_VERSION_3 */
        AUDIO_DEVICE_LOG_ERR("not support!");
        return HDF_FAILURE;
    }

    featureParam->masterBits = AudioUsbCombineBytes(featureParam->bmaControls, featureParam->csize);

    switch (state->audioUsbDriver->usb_id) {
        case 0x08bb2702:
            featureParam->masterBits &= ~UAC_CONTROL_BIT(UAC_FU_VOLUME);
            break;
        case 0x1130f211:
            featureParam->channels = 0;
            break;
    }
    return HDF_SUCCESS;
}

/*
 * parse a feature unit
 *
 * most of controls are defined here.
 */
static int32_t AudioUsbParseFeatureUnit(struct UsbMixerBuild *state, int32_t unitId, void *ftrSub)
{
    int32_t i, j;
    struct UsbAudioTerm iterm;
    int32_t err;
    struct uac_feature_unit_descriptor *hdr = ftrSub;
    struct AudioUsbFeatureControl featureControl;
    struct AudioUsbFeatureParam featureParam;
    uint32_t chBits;
    uint32_t mask;

    err = AudioUsbParseFeatureDescriptorParam(state, ftrSub, &featureParam);
    if (err < 0) {
        return err;
    }

    featureControl.unitId = unitId;
    /* parse the source unit */
    err = AudioUsbParseUnit(state, hdr->bSourceID);
    if (err < 0) {
        return err;
    }

    err = AudioUsbCheckInputTerm(state, hdr->bSourceID, &iterm);
    if (err < 0) {
        return err;
    }

    for (i = 0; i < USB_TIMEOUT; i++) {
        chBits = 0;
        featureControl.control = g_audioFeatureInfo[i].control;

        for (j = 0; j < featureParam.channels; j++) {
            mask = AudioUsbCombineBytes(featureParam.bmaControls + featureParam.csize * (j + 1), featureParam.csize);
            if (mask & (1 << i)) {
                chBits |= (1 << j);
            }
        }
        featureControl.readOnlyMask = 0;

        if (chBits & 1) {
            featureControl.ctlMask = chBits;
            AudioUsbBuildFeatureCtl(state, ftrSub, &iterm, &featureControl);
        }
        if (featureParam.masterBits & (1 << i)) {
            featureControl.ctlMask = 0;
            AudioUsbBuildFeatureCtl(state, ftrSub, &iterm, &featureControl);
        }
    }

    return 0;
}

static bool AudioUsbMixerBitmapOverflow(
    struct uac_mixer_unit_descriptor *desc, int32_t protocol, int32_t numIns, int32_t numOuts)
{
    uint8_t *hdr = (uint8_t *)desc;
    uint8_t *controls = uac_mixer_unit_bmControls(desc, protocol);
    size_t restVal;
    uint32_t numVal;
    uint32_t ctlTem;
    uint32_t hdrTem;

    switch (protocol) {
        case UAC_VERSION_1:
        default:
            restVal = 1; /* 1 is iMixer */
            break;
        case UAC_VERSION_2:
            restVal = 2; /* 2 is bmControls + iMixer */
            break;
        case UAC_VERSION_3:
            restVal = 6; /* 6 is bmControls + wMixerDescrStr */
            break;
    }
    numVal = (numIns * numOuts + CHANNEL_HDR_BLENGTH) / OVERF_LOWDIVISOR_NUM;
    ctlTem = (uintptr_t)(controls + numVal + restVal);
    hdrTem = (uintptr_t)(hdr + hdr[0]);
    return ctlTem > hdrTem;
}

static void AudioUsbBuildMixerUnitCtl(struct UsbMixerBuild *state, struct uac_mixer_unit_descriptor *desc,
    struct MixerUnitCtlParam *mixCtlParam, int32_t unitId, struct UsbAudioTerm *iterm)
{
    struct UsbMixerElemInfo *mixElemInfo;
    uint32_t i, len;
    struct AudioKcontrol *kctl;
    struct AudioKcontrol *kControl;
    uint8_t *controls;

    const struct UsbMixerNameMap *map;
    struct AudioCard *audioCard;

    map = AudioUsbFindMap(state->map, unitId, 0);
    if (AudioUsbCheckIgnoredCtl(map)) {
        return;
    }
    mixElemInfo = OsalMemCalloc(sizeof(*mixElemInfo));
    if (mixElemInfo == NULL) {
        AUDIO_DEVICE_LOG_ERR("OsalMemCalloc failed");
        return;
    }
    AudioUsbMixerElemInitStd(&mixElemInfo->head, state->mixer, unitId);
    mixElemInfo->control = mixCtlParam->itemChannels + 1; /* based on 1 */
    mixElemInfo->valType = USB_MIXER_S16;
    for (i = 0; i < mixCtlParam->numOuts; i++) {
        controls = uac_mixer_unit_bmControls(desc, state->mixer->protocol);
        if (controls != NULL &&
            AudioUsbCheckMatrixBitmap(controls, mixCtlParam->itemChannels, i, mixCtlParam->numOuts)) {
            mixElemInfo->cmask |= (1 << i);
            mixElemInfo->channels++;
        }
    }

    /* get min/max values */
    AudioUsbCtlGetMinMaxVal(mixElemInfo, 0, NULL);

    kctl = &g_usbFeatureUnitCtl;

    kctl->name = (char *)OsalMemCalloc(KCTL_NAME_LEN);

    kctl->privateValue = (unsigned long)(uintptr_t)(void *)mixElemInfo;
    DListHeadInit(&kctl->list);

    len = AudioUsbCheckMappedName(map, kctl->name, KCTL_NAME_LEN);
    if (len == 0) {
        len = AudioUsbGetTermName(state->audioUsbDriver, iterm, kctl->name, KCTL_NAME_LEN, 0);
    }
    if (len == 0) {
        (void)sprintf(kctl->name, "Mixer Source %d", mixCtlParam->itemChannels + 1);
    }

    AudioUsbAppendCtlName(kctl, " Volume", KCTL_NAME_LEN);

    AUDIO_DEVICE_LOG_INFO("[%d] MU [%s] ch = %d, value = %d/%d\n", mixElemInfo->head.id, kctl->name,
        mixElemInfo->channels, mixElemInfo->min, mixElemInfo->max);

    audioCard = state->audioUsbDriver->audioCard;

    kControl = AudioAddControl(audioCard, kctl);
    if (kControl == NULL) {
        AUDIO_DEVICE_LOG_ERR("Add control fail!");
        return;
    }
    DListInsertHead(&kctl->list, &audioCard->controls);
}

static bool AudioUsbParseMixerUnitSub(int32_t ich, struct UsbMixerBuild *state, struct MixerUnitCtlParam mixCtlParam,
    struct uac_mixer_unit_descriptor *desc)
{
    int32_t och;
    uint8_t *control;

    for (och = 0; och < mixCtlParam.numOuts; och++) {
        control = uac_mixer_unit_bmControls(desc, state->mixer->protocol);
        if (control != NULL && AudioUsbCheckMatrixBitmap(control, ich, och, mixCtlParam.numOuts)) {
            return true;
        }
    }

    return false;
}

static int32_t AudioUsbParseMixerUnit(struct UsbMixerBuild *state, int32_t unitId, void *rawDesc)
{
    struct uac_mixer_unit_descriptor *desc = rawDesc;
    struct UsbAudioTerm iterm;
    int32_t inputPins, numIns = 0;
    int32_t pin, ich = 0, err;
    struct MixerUnitCtlParam mixCtlParam;

    err = AudioUsbMixerUnitGetChannels(state, desc);
    if (err < 0) {
        AUDIO_DEVICE_LOG_ERR("invalid MIXER UNIT descriptor %d\n", unitId);
        return err;
    }

    mixCtlParam.numOuts = err;
    inputPins = desc->bNrInPins;

    for (pin = 0; pin < inputPins; pin++) {
        err = AudioUsbParseUnit(state, desc->baSourceID[pin]);
        if (err < 0) {
            continue;
        }

        if (mixCtlParam.numOuts == 0) {
            continue;
        }
        err = AudioUsbCheckInputTerm(state, desc->baSourceID[pin], &iterm);
        if (err < 0) {
            return err;
        }
        numIns += iterm.channels;
        if (AudioUsbMixerBitmapOverflow(desc, state->mixer->protocol, numIns, mixCtlParam.numOuts)) {
            break;
        }
        for (; ich < numIns; ich++) {
            if (AudioUsbParseMixerUnitSub(ich, state, mixCtlParam, desc)) {
                mixCtlParam.itemChannels = ich;
                AudioUsbBuildMixerUnitCtl(state, desc, &mixCtlParam, unitId, &iterm);
            }
        }
    }
    return 0;
}

static int32_t AudioUsbMixerCtlSelectorInfo(const struct AudioKcontrol *kcontrol, struct AudioCtrlElemInfo *uinfo)
{
    (void)kcontrol;
    (void)uinfo;
    return 0;
}

/* get callback for selector unit */
static int32_t AudioUsbMixerCtlSelectorGet(const struct AudioKcontrol *kcontrol, struct AudioCtrlElemValue *ucontrol)
{
    struct UsbMixerElemInfo *mixElemInfo = (struct UsbMixerElemInfo *)((volatile uintptr_t)kcontrol->privateValue);
    int32_t value, err;

    err = AudioUsbGetCurCtlValue(mixElemInfo, mixElemInfo->control << USB_SHIFT_SIZE_8, &value);
    if (err < 0) {
        ucontrol->value[0] = 0;
        return AudioUsbFilterError(mixElemInfo, err);
    }
    value = AudioUsbGetRelativeValue(mixElemInfo, value);
    ucontrol->value[0] = value;
    return 0;
}

/* put callback for selector unit */
static int32_t AudioUsbMixerCtlSelectorSet(
    const struct AudioKcontrol *kcontrol, const struct AudioCtrlElemValue *ucontrol)
{
    struct UsbMixerElemInfo *mixElemInfo = (struct UsbMixerElemInfo *)((volatile uintptr_t)kcontrol->privateValue);
    int32_t value, oval, err;

    err = AudioUsbGetCurCtlValue(mixElemInfo, mixElemInfo->control << USB_SHIFT_SIZE_8, &oval);
    if (err < 0) {
        return AudioUsbFilterError(mixElemInfo, err);
    }
    value = ucontrol->value[0];
    value = AudioUsbGetAbsValue(mixElemInfo, value);
    if (value != oval) {
        AudioUsbSetCurCtlValue(mixElemInfo, mixElemInfo->control << USB_SHIFT_SIZE_8, value);
        return 1;
    }
    return 0;
}

/* alsa control interface for selector unit */
static struct AudioKcontrol g_mixerSelectUnitCtl = {
    .iface = AUDIO_USB_CTL_ELEM_IFACE_MIXER,
    .name = "", /* will be filled later */
    .Info = AudioUsbMixerCtlSelectorInfo,
    .Get = AudioUsbMixerCtlSelectorGet,
    .Set = AudioUsbMixerCtlSelectorSet,
};

static int32_t AudioUsbSetTermName(
    struct UsbMixerBuild *state, void *rawDesc, struct UsbMixerElemInfo *mixElemInfo, int32_t unitId)
{
    struct uac_selector_unit_descriptor *desc = rawDesc;
    char **namelist;
    uint32_t i, len;
    struct UsbAudioTerm iterm;

    AudioUsbMixerElemInitStd(&mixElemInfo->head, state->mixer, unitId);
    mixElemInfo->valType = USB_MIXER_U8;
    mixElemInfo->channels = 1;
    mixElemInfo->min = 1;
    mixElemInfo->max = desc->bNrInPins;
    mixElemInfo->res = 1;
    mixElemInfo->initialized = 1;
    mixElemInfo->control = 0;

    namelist = OsalMemCalloc(desc->bNrInPins * sizeof(char *));
    if (namelist == NULL) {
        AUDIO_DEVICE_LOG_ERR("OsalMemCalloc failed.");
        return -ENOMEM;
    }
    for (i = 0; i < desc->bNrInPins; i++) {
        len = 0;
        namelist[i] = OsalMemCalloc(MAX_ITEM_NAME_LEN);
        if (!namelist[i]) {
            for (i = 0; i < desc->bNrInPins; i++) {
                OsalMemFree(namelist[i]);
            }
            OsalMemFree(namelist);
            return -ENOMEM;
        }
        len = AudioUsbCheckMappedSelectorName(state, unitId, i, namelist[i], MAX_ITEM_NAME_LEN);
        if (len != 0 && AudioUsbCheckInputTerm(state, desc->baSourceID[i], &iterm) >= 0) {
            len = AudioUsbGetTermName(state->audioUsbDriver, &iterm, namelist[i], MAX_ITEM_NAME_LEN, 0);
        }
        if (!len) {
            sprintf(namelist[i], "Input %u", i);
        }
    }
    return 0;
}

static void AudioUsbSetCtlName(struct UsbMixerBuild *state, struct AudioKcontrol *kctl,
    struct uac_selector_unit_descriptor *desc, const struct UsbMixerNameMap *map)
{
    uint32_t len;
    uint32_t nameid;

    /* check the static mapping table at first */
    len = AudioUsbCheckMappedName(map, kctl->name, KCTL_NAME_LEN);
    if (len == 0) {
        /* no mapping ? */
        switch (state->mixer->protocol) {
            case UAC_VERSION_1:
            case UAC_VERSION_2:
            default:
                /* if iSelector is given, use it */
                nameid = (uint32_t)uac_selector_unit_iSelector(desc);
                if (nameid != 0) {
                    len = AudioUsbCopyStringDesc(state->audioUsbDriver, nameid, kctl->name, KCTL_NAME_LEN);
                }
                break;
            case UAC_VERSION_3:
                break;
        }

        /* ... or pick up the terminal name at next */
        if (len == 0) {
            len = AudioUsbGetTermName(state->audioUsbDriver, &state->oterm, kctl->name, KCTL_NAME_LEN, 0);
        }
        /* ... or use the fixed string "USB" as the last resort */
        if (len == 0) {
            strlcpy(kctl->name, "USB", KCTL_NAME_LEN);
        }
        /* and add the proper suffix */
        if (desc->bDescriptorSubtype == UAC2_CLOCK_SELECTOR || desc->bDescriptorSubtype == UAC3_CLOCK_SELECTOR) {
            AudioUsbAppendCtlName(kctl, " Clock Source", KCTL_NAME_LEN);
        } else if ((state->oterm.type & 0xff00) == 0x0100) {
            AudioUsbAppendCtlName(kctl, " Capture Source", KCTL_NAME_LEN);
        } else {
            AudioUsbAppendCtlName(kctl, " Playback Source", KCTL_NAME_LEN);
        }
    }
}

static int32_t AudioUsbParseSelectorUnit(struct UsbMixerBuild *state, int32_t unitId, void *rawDesc)
{
    struct uac_selector_unit_descriptor *desc = rawDesc;
    uint32_t i;
    int32_t err;
    struct UsbMixerElemInfo *mixElemInfo;
    struct AudioKcontrol *kctl;
    struct AudioKcontrol *kControl;

    const struct UsbMixerNameMap *map;
    struct AudioCard *audioCard;

    AUDIO_DEVICE_LOG_INFO("entry");

    for (i = 0; i < desc->bNrInPins; i++) {
        err = AudioUsbParseUnit(state, desc->baSourceID[i]);
        if (err < 0) {
            AUDIO_DEVICE_LOG_ERR("AudioUsbParseUnit is failed");
            return err;
        }
    }

    if (desc->bNrInPins == 1) {
        return 0;
    }
    map = AudioUsbFindMap(state->map, unitId, 0);
    if (AudioUsbCheckIgnoredCtl(map)) {
        return 0;
    }
    mixElemInfo = OsalMemCalloc(sizeof(*mixElemInfo));
    if (!mixElemInfo) {
        AUDIO_DEVICE_LOG_ERR("OsalMemCalloc failed");
        return -ENOMEM;
    }

    err = AudioUsbSetTermName(state, rawDesc, mixElemInfo, unitId);
    if (err != 0) {
        AudioUsbMixerElemInfoFree(mixElemInfo);
        return err;
    }

    kctl = &g_mixerSelectUnitCtl;

    kctl->name = (char *)OsalMemCalloc(KCTL_NAME_LEN);
    DListHeadInit(&kctl->list);

    AudioUsbSetCtlName(state, kctl, desc, map);

    AUDIO_DEVICE_LOG_INFO("[%d] SU [%s] items = %d\n", mixElemInfo->head.id, kctl->name, desc->bNrInPins);

    audioCard = state->audioUsbDriver->audioCard;
    kctl->privateValue = (unsigned long)(uintptr_t)(void *)mixElemInfo;

    kControl = AudioAddControl(audioCard, kctl);
    if (kControl == NULL) {
        AUDIO_DRIVER_LOG_ERR("Add control fail!");
        AudioUsbMixerElemInfoFree(mixElemInfo);
        OsalMemFree(kctl->name);
        return HDF_FAILURE;
    }

    DListInsertHead(&kctl->list, &audioCard->controls);

    return 0;
}

/* parse an audio unit recursively */
static int32_t AudioUsbParseUnit(struct UsbMixerBuild *state, int unitid)
{
    unsigned char *unitDesc;
    AUDIO_DEVICE_LOG_INFO("entry");

    int protocol = state->mixer->protocol;

    if (test_and_set_bit(unitid, state->unitbitmap)) {
        return 0; /* the unit already visited */
    }
    unitDesc = AudioUsbFindAudioControlUnit(state, unitid);
    if (!unitDesc) {
        AUDIO_DRIVER_LOG_ERR("unit %d not found!\n", unitid);
        return -EINVAL;
    }

    if (!AudioUsbValidateAudioDesc(unitDesc, protocol)) {
        AUDIO_DRIVER_LOG_DEBUG("invalid unit %d\n", unitid);
        return 0; /* skip invalid unit */
    }

    switch (((protocol << USB_SHIFT_SIZE_8) | unitDesc[USB_VAL_LEN_2])) {
        case 0x0002:
        case 0x2002:
            return 0;
        case 0x0004:
        case 0x2004:
            return AudioUsbParseMixerUnit(state, unitid, unitDesc);
        case 0x0005:
        case 0x2005:
        case 0x200b:
            return AudioUsbParseSelectorUnit(state, unitid, unitDesc);
        case 0x0006:
        case 0x2006:
            return AudioUsbParseFeatureUnit(state, unitid, unitDesc);
        case 0x0007:
        case 0x2008:
        case 0x0008:
        case 0x2009:
        case 0x2007:
        case 0x200a:
            return 0;
        default:
            AUDIO_DRIVER_LOG_ERR("unit %u: unexpected type 0x%02x\n", unitid, unitDesc[ARRAY_INDEX_2]);
            return -EINVAL;
    }
}

void AudioUsbMixerDisconnect(struct UsbMixerInterface *mixer)
{
    if (mixer->disconnected) {
        return;
    }
    if (mixer->urb != NULL) {
        usb_kill_urb(mixer->urb);
    }
    if (mixer->rcUrb != NULL) {
        usb_kill_urb(mixer->rcUrb);
    }
    if (mixer->PrivateFree != NULL) {
        mixer->PrivateFree(mixer);
    }
    mixer->disconnected = true;
}

static void AudioUsbMixerFree(struct UsbMixerInterface *mixer)
{
    /* kill pending URBs */
    AudioUsbMixerDisconnect(mixer);

    OsalMemFree(mixer->idElems);
    if (mixer->urb != NULL) {
        kfree(mixer->urb->transfer_buffer);
        usb_free_urb(mixer->urb);
    }
    usb_free_urb(mixer->rcUrb);
    OsalMemFree(mixer->rcSetupPacket);
    OsalMemFree(mixer);
}

static int32_t AudioUsbMixerControls(struct UsbMixerInterface *mixer)
{
    struct UsbMixerBuild state;
    int32_t err;
    void *p = NULL;

    (void)memset_s(&state, sizeof(state), 0, sizeof(state));
    state.audioUsbDriver = mixer->audioUsbDriver;
    state.mixer = mixer;
    state.buffer = mixer->hostIf->extra;
    state.bufLen = mixer->hostIf->extralen;

    AUDIO_DEVICE_LOG_INFO("entry");

    while ((p = AudioUsbFindCsintDesc(mixer->hostIf->extra, mixer->hostIf->extralen, p, UAC_OUTPUT_TERMINAL)) != NULL) {
        if (!AudioUsbValidateAudioDesc(p, mixer->protocol)) {
            continue;
        }
        if (mixer->protocol == UAC_VERSION_1) {
            struct uac1_output_terminal_descriptor *desc = p;

            set_bit(desc->bTerminalID, state.unitbitmap);
            state.oterm.id = desc->bTerminalID;
            state.oterm.type = le16_to_cpu(desc->wTerminalType);
            state.oterm.name = desc->iTerminal;
            err = AudioUsbParseUnit(&state, desc->bSourceID);
            if (err < 0 && err != -EINVAL) {
                AUDIO_DEVICE_LOG_ERR("AudioUsbParseUnit is failed");
                return err;
            }
        }
    }

    return 0;
}

int32_t AudioUsbCreateMixer(struct AudioUsbDriver *audioUsbDriver, int32_t ctrlif, int32_t ignore_error)
{
    struct UsbMixerInterface *mixer;
    int32_t err = 0;
    AUDIO_DEVICE_LOG_INFO("entry");

    mixer = OsalMemCalloc(sizeof(*mixer));
    if (!mixer) {
        AUDIO_DEVICE_LOG_ERR("OsalMemCalloc failed");
        return -ENOMEM;
    }
    mixer->audioUsbDriver = audioUsbDriver;
    mixer->ignoreCtlError = ignore_error;
    mixer->idElems = OsalMemCalloc(sizeof(*mixer->idElems) * MAX_ID_ELEMS);
    if (!mixer->idElems) {
        OsalMemFree(mixer);
        return -ENOMEM;
    }

    mixer->hostIf = &usb_ifnum_to_if(audioUsbDriver->dev, ctrlif)->altsetting[0];
    switch (AUDIO_USB_GET_IFACE_DESC(mixer->hostIf)->bInterfaceProtocol) {
        case UAC_VERSION_1:
        default:
            mixer->protocol = UAC_VERSION_1;
            break;
        case UAC_VERSION_2:
            mixer->protocol = UAC_VERSION_2;
            break;
        case UAC_VERSION_3:
            mixer->protocol = UAC_VERSION_3;
            break;
    }

    if (mixer->protocol != UAC_VERSION_3) {
        err = AudioUsbMixerControls(mixer);
        if (err < 0) {
            AudioUsbMixerFree(mixer);
            return err;
        }
    }
    return 0;
}
