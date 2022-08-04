/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_AUDIO_TEST_H
#define HDF_AUDIO_TEST_H

#include "hdf_main_test.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    AUDIO_ADM_TEST_AUDIOHOSTCREATEANDBIND = 1,        // 1 audio ADM audio_host
    AUDIO_ADM_TEST_GETCARDINSTANCE = 2,               // 2 audio ADM audio_host
    AUDIO_ADM_TEST_AUDIOFILLCONFIGDATA = 3,           // 3 audio ADM audio_parse
    AUDIO_ADM_TEST_CODECGETREGCONFIG = 4,             // 4 audio ADM audio_parse
    AUDIO_ADM_TEST_AUDIODEVICEREADREG = 5,            // 5 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIODEVICEWRITEREG,               // 6 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOSOCREGISTERPLATFORM,          // 7 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOSOCREGISTERDAI,               // 8 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOREGISTERDSP,                  // 9 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOREGISTERCODEC,                // 10 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOBINDDAILINK,                  // 11 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOUPDATECODECREGBITS,           // 12 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOUPDATEDAIREGBITS,             // 13 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOKCONTROLGETCPUDAI,            // 14 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOKCONTROLGETCODEC,             // 15 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOADDCONTROLS,                  // 16 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOADDCONTROL,                   // 17 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOGETCTRLOPSRREG,               // 18 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOGETCTRLOPSREG,                // 19 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOSETCTRLOPSREG,                // 20 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOSETCTRLOPSRREG,               // 21 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIODAIREADREG,                   // 22 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIODAIWRITEREG,                  // 23 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOCODECREADREG,                 // 24 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOCODECWRITEREG,                // 25 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOINFOCTRLOPS,                  // 26 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOCODECGETCTRLOPS,              // 27 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOCODECSETCTRLOPS,              // 28 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOCPUDAISETCTRLOPS,             // 29 audio ADM audio_core

    AUDIO_ADM_TEST_AUDIOCPUDAIGETCTRLOPS = 37,        // 37 audio ADM audio_core
    AUDIO_ADM_TEST_AUDIOSAPMNEWCOMPONENTS = 38,       // 38 audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOSAPMADDROUTES,                // 39 audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOSAPMNEWCONTROLS,              // 40 audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOSAPMSLEEP,                    // 41 audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOSAMPPOWERUP,                  // 42 audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOSAMPSETPOWERMONITOR,          // 43 audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOCODECSAPMSETCTRLOPS,          // 44 audio ADM audio_sapm
    AUDIO_ADM_TEST_AUDIOCODECSAPMGETCTRLOPS,          // 45 audio ADM audio_sapm

    AUDIO_ADM_TEST_I2CREADWRITE = 48,                 // 48 audio ADM audio_codec_base
    AUDIO_ADM_TEST_REGBITSREAD,                       // 49
    AUDIO_ADM_TEST_REGBITSUPDATE,                     // 50
    AUDIO_ADM_TEST_DEVICEFREQUENCYPARSE,              // 51
    AUDIO_ADM_TEST_DAIPARAMSUPDATE,                   // 52
    AUDIO_ADM_TEST_DEVICECFGGET,                      // 53
    AUDIO_ADM_TEST_DEVICECTRLREGINIT,                 // 54
    AUDIO_ADM_TEST_DEVICEREGREAD,                     // 55
    AUDIO_ADM_TEST_DEVICEREGWRITE,                    // 56
    AUDIO_ADM_TEST_AGETCONFIGINFO,                    // 57

    AUDIO_ADM_TEST_GETSERVICENAME,                    // 58 audio ADM audio_codec_base
    AUDIO_ADM_TEST_GETDAINAME,                        // 59
    AUDIO_ADM_TEST_GETCONFIGINFO,                     // 60
    AUDIO_ADM_TEST_SETCONFIGINFO,                     // 61
    AUDIO_ADM_TEST_SETCTLFUNC,                        // 62
    AUDIO_ADM_TEST_DAIDATAFROMCARD,                   // 63 audio ADM audio_dai_base
    AUDIO_ADM_TEST_DAIGETCONFIGINFO,                  // 64
    AUDIO_ADM_TEST_DAICHECKSAMPLERATE,                // 65
    AUDIO_ADM_TEST_DAISETCONFIGINFO,                  // 66

    AUDIO_ADM_TEST_DMABUFALLOC,                       // 67 audio ADM audio_dma_base
    AUDIO_ADM_TEST_DMABUFFREE,                        // 68
    AUDIO_ADM_TEST_DMAREQUESTCHANNEL,                 // 69
    AUDIO_ADM_TEST_DMACONFIGCHANNEL,                  // 70
    AUDIO_ADM_TEST_DMAPREP,                           // 71
    AUDIO_ADM_TEST_DMASUBMIT,                         // 72
    AUDIO_ADM_TEST_DMAPENDING,                        // 73
    AUDIO_ADM_TEST_DMAPAUSE,                          // 74
    AUDIO_ADM_TEST_DMARESUME,                         // 75
    AUDIO_ADM_TEST_DMAPOINTER,                        // 76

    AUDIO_ADM_TEST_DSPGETSERVICENAME,                 // 77 audio ADM audio_dsp_base
    AUDIO_ADM_TEST_DSPGETDAINAME,                     // 78
    AUDIO_ADM_TEST_DATAFROMCARD,                      // 79 audio ADM audio_platform_base
    AUDIO_ADM_TEST_BYTESTOFRAMES,                     // 80
    AUDIO_ADM_TEST_DATABIGENDIANCHANGE,               // 81
    AUDIO_ADM_TEST_FRAMATTOBITWIDTH,                  // 82
    AUDIO_ADM_TEST_SETPCMINFO,                        // 83
    AUDIO_ADM_TEST_SETRENDERBUFINFO,                  // 84
    AUDIO_ADM_TEST_SETCAPTUREBUFINFO,                 // 85
    AUDIO_ADM_TEST_PCMWRITE,                          // 86
    AUDIO_ADM_TEST_PCMREAD,                           // 87
    AUDIO_ADM_TEST_PCMMMAPWRITE,                      // 88
    AUDIO_ADM_TEST_PCMMMAPREAD,                       // 89
    AUDIO_ADM_TEST_RENDEROPEN,                        // 90
    AUDIO_ADM_TEST_CAPTUREOPEN,                       // 91
    AUDIO_ADM_TEST_RENDERCLOSE,                       // 92
    AUDIO_ADM_TEST_PCMPOINTER,                        // 93
    AUDIO_ADM_TEST_CAPTURECLOSE,                      // 94
    AUDIO_ADM_TEST_HWPARAMS,                          // 95
    AUDIO_ADM_TEST_RENDERPREPARE,                     // 96
    AUDIO_ADM_TEST_CAPTUREPREPARE,                    // 97
    AUDIO_ADM_TEST_RENDERTRIGGER,                     // 98
    AUDIO_ADM_TEST_CAPTURETRIGGER,                    // 99

    AUDIO_ADM_TEST_CODECDEVREADREG,                   // 100
    AUDIO_ADM_TEST_CODECDEVWRITEREG,                  // 101
    AUDIO_ADM_TEST_CODECDAIREGI2CREAD,                // 102
    AUDIO_ADM_TEST_CODECDAIREGI2CWRITE,               // 103
    AUDIO_ADM_TEST_CODECDEVREGI2CREAD,                // 104
    AUDIO_ADM_TEST_CODECDEVREGI2CWRITE,               // 105
    AUDIO_ADM_TEST_CODECDEVINITREGCONFIG,             // 106
    AUDIO_ADM_TEST_CODECDAIDEVSTARTUPREGCONFIG,       // 107
} HdfAudioTestCaseCmd;

int32_t HdfAudioEntry(HdfTestMsg *msg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDF_AUDIO_TEST_H */
