#ifndef __EDP_IF_H__
#define __EDP_IF_H__

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum EdpColorDepth {
  EDP_COLOR_DEPTH_NONE = 0,
  EDP_COLOR_DEPTH_6BIT = 6,
  EDP_COLOR_DEPTH_8BIT = 8,
  EDP_COLOR_DEPTH_10BIT = 10,
  EDP_COLOR_DEPTH_12BIT = 12,
  EDP_COLOR_DEPTH_INVALID,
};

enum EdpColorSpace {
  EDP_COLOR_SPACE_RGB = 0,
  EDP_COLOR_SPACE_YCBCR422 = 1,
  EDP_COLOR_SPACE_YCBCR444 = 2,
};

enum EdpColorCoefficient {
  EDP_COLOR_COEFF_YCBCR601,
  EDP_COLOR_COEFF_YCBCR709,
};

enum EdpDynamicRange {
  EDP_DYNAMIC_RANGE_VESA,
  EDP_DYNAMIC_RANGE_CEA,
};

enum EdpVideoTimingRecognitionType {
	EDP_VIDEO_TIMING_FROM_CAPTURE,
	EDP_VIDEO_TIMING_FROM_REGISTER
};

// enum EdpAnalogPowerBlock {
// 	AUX_BLOCK,
// 	CH0_BLOCK,
// 	CH1_BLOCK,
// 	CH2_BLOCK,
// 	CH3_BLOCK,
// 	ANALOG_TOTAL,
// 	POWER_ALL
// };

/**
 * extracted from drm
 * enum dp_colorimetry - drm DP Colorimetry formats
 *
 * This enum is used to indicate DP VSC SDP Colorimetry formats.
 * It is based on DP 1.4 spec [Table 2-117: VSC SDP Payload for DB16 through
 * DB18] and a name of enum member follows DRM_MODE_COLORIMETRY definition.
 *
 * @DP_COLORIMETRY_DEFAULT: sRGB (IEC 61966-2-1) or
 *                          ITU-R BT.601 colorimetry format
 * @DP_COLORIMETRY_RGB_WIDE_FIXED: RGB wide gamut fixed point colorimetry format
 * @DP_COLORIMETRY_BT709_YCC: ITU-R BT.709 colorimetry format
 * @DP_COLORIMETRY_RGB_WIDE_FLOAT: RGB wide gamut floating point
 *                                 (scRGB (IEC 61966-2-2)) colorimetry format
 * @DP_COLORIMETRY_XVYCC_601: xvYCC601 colorimetry format
 * @DP_COLORIMETRY_OPRGB: OpRGB colorimetry format
 * @DP_COLORIMETRY_XVYCC_709: xvYCC709 colorimetry format
 * @DP_COLORIMETRY_DCI_P3_RGB: DCI-P3 (SMPTE RP 431-2) colorimetry format
 * @DP_COLORIMETRY_SYCC_601: sYCC601 colorimetry format
 * @DP_COLORIMETRY_RGB_CUSTOM: RGB Custom Color Profile colorimetry format
 * @DP_COLORIMETRY_OPYCC_601: opYCC601 colorimetry format
 * @DP_COLORIMETRY_BT2020_RGB: ITU-R BT.2020 R' G' B' colorimetry format
 * @DP_COLORIMETRY_BT2020_CYCC: ITU-R BT.2020 Y'c C'bc C'rc colorimetry format
 * @DP_COLORIMETRY_BT2020_YCC: ITU-R BT.2020 Y' C'b C'r colorimetry format
 */
enum EdpColorimetry {
	EDP_COLORIMETRY_DEFAULT = 0,
	EDP_COLORIMETRY_RGB_WIDE_FIXED = 0x1,
	EDP_COLORIMETRY_BT709_YCC = 0x1,
	EDP_COLORIMETRY_RGB_WIDE_FLOAT = 0x2,
	EDP_COLORIMETRY_XVYCC_601 = 0x2,
	EDP_COLORIMETRY_OPRGB = 0x3,
	EDP_COLORIMETRY_XVYCC_709 = 0x3,
	EDP_COLORIMETRY_DCI_P3_RGB = 0x4,
	EDP_COLORIMETRY_SYCC_601 = 0x4,
	EDP_COLORIMETRY_RGB_CUSTOM = 0x5,
	EDP_COLORIMETRY_OPYCC_601 = 0x5,
	EDP_COLORIMETRY_BT2020_RGB = 0x6,
	EDP_COLORIMETRY_BT2020_CYCC = 0x6,
	EDP_COLORIMETRY_BT2020_YCC = 0x7,
};

enum EdpSampleRate {
    EDP_SAMPLE_RATE_UNKNOWN,
    EDP_SAMPLE_RATE_8K = 8000,
    EDP_SAMPLE_RATE_11K = 11025,
    EDP_SAMPLE_RATE_12K = 12000,
    EDP_SAMPLE_RATE_16K = 16000,
    EDP_SAMPLE_RATE_22K = 22050,
    EDP_SAMPLE_RATE_24K = 24000,
    EDP_SAMPLE_RATE_32K = 32000,
    EDP_SAMPLE_RATE_44K = 44100,
    EDP_SAMPLE_RATE_48K = 48000,
    EDP_SAMPLE_RATE_88K = 88200,
    EDP_SAMPLE_RATE_96K = 96000,
    EDP_SAMPLE_RATE_176K = 176400,
    EDP_SAMPLE_RATE_192K = 192000,
    EDP_SAMPLE_RATE_768K = 768000,
    EDP_SAMPLE_RATE_BUTT,
};

enum EdpAudioBitDepth {
    EDP_AUDIO_BIT_DEPTH_UNKNOWN,
    EDP_AUDIO_BIT_DEPTH_8 = 8,
    EDP_AUDIO_BIT_DEPTH_16 = 16,
    EDP_AUDIO_BIT_DEPTH_18 = 18,
    EDP_AUDIO_BIT_DEPTH_20 = 20,
    EDP_AUDIO_BIT_DEPTH_24 = 24,
    EDP_AUDIO_BIT_DEPTH_32 = 32,
    EDP_AUDIO_BIT_DEPTH_BUTT,
};

enum EdpAudioCodingType {
    EDP_AUDIO_CODING_TYPE_STREAM = 0,
    EDP_AUDIO_CODING_TYPE_LPCM = 1,
    EDP_AUDIO_CODING_TYPE_AC3 = 2,
    EDP_AUDIO_CODING_TYPE_MPEG1 = 3,
    EDP_AUDIO_CODING_TYPE_MP3 = 4,
    EDP_AUDIO_CODING_TYPE_MPEG2 = 5,
    EDP_AUDIO_CODING_TYPE_AAC_LC = 6,
    EDP_AUDIO_CODING_TYPE_DTS = 7,
    EDP_AUDIO_CODING_TYPE_ATRAC = 8,
    EDP_AUDIO_CODING_TYPE_OBA = 9,
    EDP_AUDIO_CODING_TYPE_EAC3 = 10,
    EDP_AUDIO_CODING_TYPE_DTS_HD = 11,
    EDP_AUDIO_CODING_TYPE_MLP = 12,
    EDP_AUDIO_CODING_TYPE_DST = 13,
    EDP_AUDIO_CODING_TYPE_WMA_PRO = 14,
    EDP_AUDIO_CODING_TYPE_CXT = 15,
};

enum EdpVideoTiming {
    EDP_VIDEO_TIMING_NONE = 0,
    EDP_VIDEO_TIMING_640X480P60 = 1,
    EDP_VIDEO_TIMING_720X480P60 = 2,
    EDP_VIDEO_TIMING_1280X720P60 = 3,
    EDP_VIDEO_TIMING_1920X1080P60 = 4,
};

enum EdpAudioInterfaceType {
    EDP_AUDIO_IF_TYPE_I2S = 0,    /* Inter-IC Sound */
    EDP_AUDIO_IF_TYPE_SPDIF = 1,  /* Sony/Philips Digital Interface */
    EDP_AUDIO_IF_TYPE_OTHER,
};

enum EdpAudioFormatChannel {
    EDP_AUDIO_FORMAT_CHANNEL_2 = 2,
    EDP_AUDIO_FORMAT_CHANNEL_3,
    EDP_AUDIO_FORMAT_CHANNEL_4,
    EDP_AUDIO_FORMAT_CHANNEL_5,
    EDP_AUDIO_FORMAT_CHANNEL_6,
    EDP_AUDIO_FORMAT_CHANNEL_7,
    EDP_AUDIO_FORMAT_CHANNEL_8,
    EDP_AUDIO_FORMAT_CHANNEL_BUTT,
};

struct EdpVideoAttr {
    uint32_t tmdsClock;  /* unit: KHz */
    uint32_t pixelClock; /* unit: KHz */
    uint32_t pixelRepeat;
    enum EdpColorSpace colorSpace;
    enum EdpColorimetry colorimetry;
    enum EdpColorDepth colorDepth;
    enum EdpVideoTiming timing;
};

struct EdpAudioAttr {
    enum EdpAudioCodingType codingType;
    enum EdpAudioInterfaceType ifType;
    enum EdpAudioBitDepth bitDepth;
    enum EdpSampleRate sampleRate;
    bool downSample;
    enum EdpAudioFormatChannel channels;
};

struct EdpHpdCallbackInfo {
    void *data;
    void (*callbackFunc)(void *data, bool hdp);
};

/**
 * @brief EdpOpen open Edp device by bus number
 * 
 * @param busNum bus number of the target screen
 * @return DevHandle handler of the target, return NULL while error occurs
 */
DevHandle EdpOpen(uint16_t busNum);

/**
 * @brief release handle resources both in user space and kernel space, 
 * It just release resources which is related to the handle.
 * 
 * @param handle device handler 
 */
void EdpClose(DevHandle handle);

/**
 * @brief EdpAvmuteSet mute edp audio output
 * 
 * @param handle device handler of target
 * @param enable true for muting output/false for turning on output
 * @return int32_t 0 for sucess, other for error
 */
int32_t EdpAvmuteSet(DevHandle handle, bool enable);

/**
 * @brief EdpDeepColorSet setup deep color for screen
 * 
 * @param handle device handler
 * @param color color data for setup
 * @return int32_t 0 for success, other for error
 */
int32_t EdpDeepColorSet(DevHandle handle, enum EdpColorDepth color);  

/**
 * @brief EdpDeepColorGet fetch deep color current data from hdf service
 * 
 * @param handle device handler
 * @param color memory for get the data from hdf service
 * @return int32_t 0 for sucess, other for error
 */
int32_t EdpDeepColorGet(DevHandle handle, enum EdpColorDepth *color);  

/**
 * @brief EdpSetVideoAttribute setup video attribute
 * 
 * @param handle device handler
 * @param attr video attribute details for setup
 * @return int32_t 0 for sucess, other for error
 */
int32_t EdpSetVideoAttribute(DevHandle handle, struct EdpVideoAttr *attr);  

/**
 * @brief EdpSetAudioAttribute setup audio attribute
 * 
 * @param handle device handler
 * @param attr audio attribute details for setup
 * @return int32_t 0 for sucess, other for error
 */
int32_t EdpSetAudioAttribute(DevHandle handle, struct EdpAudioAttr *attr);  

/**
 * @brief EdpRegisterHpdCallbackFunc register callback for hpd event?
 * 
 * I don't think this api can be implement in user space
 * @param handle 
 * @param callback 
 * @return int32_t 
 */
int32_t EdpRegisterHpdCallbackFunc(DevHandle handle, struct EdpHpdCallbackInfo *callback);

/**
 * @brief EdpUnregisterHpdCallbackFunc remove the registered hpd event cb in kernel space
 *  This is a terrible interface to perform this operation from a user request
 * @param handle device handler  
 * @return int32_t 
 */
int32_t EdpUnregisterHpdCallbackFunc(DevHandle handle);

/**
 * @brief EdpBackLightSetup setup backlight for Edp device
 * 
 * @param handle device handler for backlight setup
 * @param value  backlight value to setup
 * @return int32_t 
 */
int32_t EdpBackLightSetup(DevHandle handle, int32_t value);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //__EDP_IF_H__