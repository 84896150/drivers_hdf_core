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

enum HDFAudioTestCommonCmd {
    TESTI2CREADWRITE = 48,        // 48
    TESTREGBITSREAD,              // 49
    TESTREGBITSUPDATE,            // 50
    TESTDEVICEFREQUENCYPARSE,     // 51
    TESTDAIPARAMSUPDATE,          // 52
    TESTDEVICECFGGET,             // 53
    TESTDEVICECTRLREGINIT,        // 54
    TESTDEVICEREGREAD,            // 55
    TESTDEVICEREGWRITE,           // 56
    TESTAGETCONFIGINFO,           // 57
    TESTGETSERVICENAME,           // 58
    TESTGETDAINAME,               // 59
    TESTGETCONFIGINFO,            // 60
    TESTSETCONFIGINFO,            // 61
    TESTSETCTLFUNC,               // 62
    TESTDAIDATAFROMCARD,          // 63
    TESTDAIGETCONFIGINFO,         // 64
    TESTDAICHECKSAMPLERATE,       // 65
    TESTDAISETCONFIGINFO,         // 66
    TESTDMABUFALLOC,              // 67
    TESTDMABUFFREE,               // 68
    TESTDMAREQUESTCHANNEL,        // 69
    TESTDMACONFIGCHANNEL,         // 70
    TESTDMAPREP,                  // 71
    TESTDMASUBMIT,                // 72
    TESTDMAPENDING,               // 73
    TESTDMAPAUSE,                 // 74
    TESTDMARESUME,                // 75
    TESTDMAPOINTER,               // 76
    TESTDSPGETSERVICENAME,        // 77
    TESTDSPGETDAINAME,            // 78
    TESTDATAFROMCARD,             // 79
    TESTBYTESTOFRAMES,            // 80
    TESTDATABIGENDIANCHANGE,      // 81
    TESTFRAMATTOBITWIDTH,         // 82
    TESTSETPCMINFO,               // 83
    TESTSETRENDERBUFINFO,         // 84
    TESTSETCAPTUREBUFINFO,        // 85
    TESTPCMWRITE,                 // 86
    TESTPCMREAD,                  // 87
    TESTPCMMMAPWRITE,             // 88
    TESTPCMMMAPREAD,              // 89
    TESTRENDEROPEN,               // 90
    TESTCAPTUREOPEN,              // 91
    TESTRENDERCLOSE,              // 92
    TESTPCMPOINTER,               // 93
    TESTCAPTURECLOSE,             // 94
    TESTHWPARAMS,                 // 95
    TESTRENDERPREPARE,            // 96
    TESTCAPTUREPREPARE,           // 97
    TESTRENDERTRIGGER,            // 98
    TESTCAPTURETRIGGER,           // 99

    TESTCODECDEVREADREG,          // 100
    TESTCODECDEVWRITEREG,         // 101
    TESTCODECDAIREGI2CREAD,       // 102
    TESTCODECDAIREGI2CWRITE,      // 103
    TESTCODECDEVREGI2CREAD,       // 104
    TESTCODECDEVREGI2CWRITE,      // 105
    TESTCODECDEVINITREGCONFIG,    // 106
    TESTCODECDAIDEVSTARTUPREGCONFIG   // 107
};

#endif /* AUDIO_COMMON_TEST_H */
