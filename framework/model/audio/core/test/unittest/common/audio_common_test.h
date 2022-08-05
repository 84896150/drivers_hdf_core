/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_COMMON_TEST_H
#define AUDIO_COMMON_TEST_H

#include "hdf_types.h"

const int32_t g_testAudioType = 701; // 701 is TEST_AUDIO_TYPE

enum {
    TEST_AUDIOHOSTCREATEANDBIND = 1,        // 1 audio ADM audio_host
    TEST_GETCARDINSTANCE = 2,               // 2 audio ADM audio_host

    TEST_AUDIOFILLCONFIGDATA = 3,           // 3 audio ADM audio_parse
    TEST_CODECGETREGCONFIG = 4,             // 4 audio ADM audio_parse

    TEST_AUDIODEVICEREADREG = 5,            // 5 audio ADM audio_core
    TEST_AUDIODEVICEWRITEREG,               // 6 audio ADM audio_core
    TEST_AUDIOSOCREGISTERPLATFORM,          // 7 audio ADM audio_core
    TEST_AUDIOSOCREGISTERDAI,               // 8 audio ADM audio_core
    TEST_AUDIOREGISTERDSP,                  // 9 audio ADM audio_core
    TEST_AUDIOREGISTERCODEC,                // 10 audio ADM audio_core
    TEST_AUDIOBINDDAILINK,                  // 11 audio ADM audio_core
    TEST_AUDIOUPDATECODECREGBITS,           // 12 audio ADM audio_core
    TEST_AUDIOUPDATEDAIREGBITS,             // 13 audio ADM audio_core
    TEST_AUDIOKCONTROLGETCPUDAI,            // 14 audio ADM audio_core
    TEST_AUDIOKCONTROLGETCODEC,             // 15 audio ADM audio_core
    TEST_AUDIOADDCONTROLS,                  // 16 audio ADM audio_core
    TEST_AUDIOADDCONTROL,                   // 17 audio ADM audio_core
    TEST_AUDIOGETCTRLOPSRREG,               // 18 audio ADM audio_core
    TEST_AUDIOGETCTRLOPSREG,                // 19 audio ADM audio_core
    TEST_AUDIOSETCTRLOPSREG,                // 20 audio ADM audio_core
    TEST_AUDIOSETCTRLOPSRREG,               // 21 audio ADM audio_core
    TEST_AUDIODAIREADREG,                   // 22 audio ADM audio_core
    TEST_AUDIODAIWRITEREG,                  // 23 audio ADM audio_core
    TEST_AUDIOCODECREADREG,                 // 24 audio ADM audio_core
    TEST_AUDIOCODECWRITEREG,                // 25 audio ADM audio_core
    TEST_AUDIOINFOCTRLOPS,                  // 26 audio ADM audio_core
    TEST_AUDIOCODECGETCTRLOPS,              // 27 audio ADM audio_core
    TEST_AUDIOCODECSETCTRLOPS,              // 28 audio ADM audio_core
    TEST_AUDIOCPUDAISETCTRLOPS,             // 29 audio ADM audio_core

    TEST_AUDIOCPUDAIGETCTRLOPS = 37,        // 37 audio ADM audio_core

    TEST_AUDIOSAPMNEWCOMPONENTS = 38,       // 38 audio ADM audio_sapm
    TEST_AUDIOSAPMADDROUTES,                // 39 audio ADM audio_sapm
    TEST_AUDIOSAPMNEWCONTROLS,              // 40 audio ADM audio_sapm
    TEST_AUDIOSAPMSLEEP,                    // 41 audio ADM audio_sapm
    TEST_AUDIOSAMPPOWERUP,                  // 42 audio ADM audio_sapm
    TEST_AUDIOSAMPSETPOWERMONITOR,          // 43 audio ADM audio_sapm
    TEST_AUDIOCODECSAPMSETCTRLOPS,          // 44 audio ADM audio_sapm
    TEST_AUDIOCODECSAPMGETCTRLOPS,          // 45 audio ADM audio_sapm
};

#endif /* AUDIO_COMMON_TEST_H */
