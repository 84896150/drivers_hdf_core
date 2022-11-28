/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_usb_codec_ops.h"
#include "audio_driver_log.h"
#include "audio_sapm.h"
#include "audio_usb_linux.h"
#include "audio_usb_mixer.h"

#define HDF_LOG_TAG HDF_AUDIO_USB

static const struct AudioSapmRoute g_audioRoutes[] = {
    {"SPKL",     NULL,                "SPKL PGA"},
    {"HPL",      NULL,                "HPL PGA" },
    {"HPR",      NULL,                "HPR PGA" },
    {"SPKL PGA", "Speaker1 Switch",   "DAC1"    },
    {"HPL PGA",  "Headphone1 Switch", "DAC2"    },
    {"HPR PGA",  "Headphone2 Switch", "DAC3"    },

    {"ADCL",     NULL,                "LPGA"    },
    {"ADCR",     NULL,                "RPGA"    },
    {"LPGA",     "LPGA MIC Switch",   "MIC1"    },
    {"RPGA",     "RPGA MIC Switch",   "MIC2"    },
};

int32_t AudioUsbCodecDeviceInit(struct AudioCard *audioCard, const struct CodecDevice *device)
{
    struct AudioUsbDriver *audioUsbDriver = NULL;

    if ((audioCard == NULL) || (device == NULL) || device->devData == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    audioUsbDriver = GetLinuxAudioUsb();
    if (audioUsbDriver == NULL) {
        AUDIO_DRIVER_LOG_ERR("audioUsbDriver is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    audioUsbDriver->audioCard = audioCard;

    if (AudioUsbCreateMixer(audioUsbDriver, audioUsbDriver->ifNum, 0) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add controls failed.");
        return HDF_FAILURE;
    }

    if (AudioSapmNewComponents(audioCard, device->devData->sapmComponents, device->devData->numSapmComponent) !=
        HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("new components failed.");
        return HDF_FAILURE;
    }

    if (AudioSapmAddRoutes(audioCard, g_audioRoutes, HDF_ARRAY_SIZE(g_audioRoutes)) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add route failed.");
        return HDF_FAILURE;
    }

    if (AudioSapmNewControls(audioCard) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add sapm controls failed.");
        return HDF_FAILURE;
    }

    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t AudioUsbCodecDaiDeviceInit(struct AudioCard *card, const struct DaiDevice *device)
{
    struct AudioUsbDriver *audioUsbDriver = NULL;

    (void)card;

    audioUsbDriver = GetLinuxAudioUsb();
    if (audioUsbDriver == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    device->devData->privateParam = audioUsbDriver;

    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t AudioUsbCodecReadReg(const struct CodecDevice *codec, uint32_t reg, uint32_t *val)
{
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t AudioUsbCodecWriteReg(const struct CodecDevice *codec, uint32_t reg, uint32_t value)
{
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t AudioUsbCodecDaiReadReg(const struct DaiDevice *dai, uint32_t reg, uint32_t *value)
{
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t AudioUsbCodecDaiWriteReg(const struct DaiDevice *dai, uint32_t reg, uint32_t value)
{
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}
