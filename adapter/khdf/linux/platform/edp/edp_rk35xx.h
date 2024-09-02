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

#ifndef EDP_RK35XX_H
#define EDP_RK35XX_H

#include <drm/bridge/analogix_dp.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct RockchipDrmSubDev;

enum LinkLaneCountType {
    LANE_COUNT1 = 1,
    LANE_COUNT2 = 2,
    LANE_COUNT4 = 4
};

enum LinkTrainingState {
    START,
    CLOCK_RECOVERY,
    EQUALIZER_TRAINING,
    FINISHED,
    FAILED
};

enum ColorSpace {
    COLOR_RGB,
    COLOR_YCBCR422,
    COLOR_YCBCR444
};

enum ColorDepth {
    COLOR_6,
    COLOR_8,
    COLOR_10,
    COLOR_12
};

enum ColorCoefficient {
    COLOR_YCBCR601,
    COLOR_YCBCR709
};

enum DynamicRange {
    VESA,
    CEA
};

enum VideoTimingRecognitionType {
    VIDEO_TIMING_FROM_CAPTURE,
    VIDEO_TIMING_FROM_REGISTER
};

struct RockchipDrmSubDev {
    struct list_head list;
    struct drm_connector *connector;
    struct device_node *of_node;
    void (*loaderProtect)(struct drm_encoder *encoder, bool on);
    void (*oobHotplugEvent)(struct drm_connector *connector);
};

struct VideoInfo {
    char *name;
    struct drm_display_mode mode;
    bool hSyncPolarity;
    bool vSyncPolarity;
    bool interlaced;
    enum ColorSpace colorSpace;
    enum DynamicRange dynamicRange;
    enum ColorCoefficient ycbcrCoeff;
    enum ColorDepth colorDepth;
    int maxLinkRate;
    enum LinkLaneCountType maxLaneCount;
    bool videoBistEnable;
};

struct LinkTrain {
    int eqLoop;
    int crLoop[4];
    u8 linkRate;
    u8 laneCount;
    u8 trainingLane[4];
    bool ssc;
    bool enhancedFraming;
    enum LinkTrainingState ltState;
};

struct AnalogixDpDevice {
    struct drm_encoder    *encoder;
    struct device        *dev;
    struct drm_device    *drmDev;
    struct drm_connector    connector;
    struct drm_bridge    bridge;
    struct drm_dp_aux       aux;
    struct clk_bulk_data    *clks;
    int            nrClks;
    unsigned int        irq;
    void __iomem        *regBase;
    struct VideoInfo    videoInfo;
    struct LinkTrain    linkTrain;
    struct phy        *phy;
    bool            phyEnabled;
    int            dpmsMode;
    struct gpio_desc    *hpdGpiod;
    bool                    forceHpd;
    bool            fastTrainEnable;
    bool            psrSupported;
    struct mutex        panelLock;
    bool            panelIsPrepared;
    struct analogix_dp_plat_data *platData;
};

struct RockchipGrfRegField {
    unsigned int reg;
    unsigned int lsb;
    unsigned int msb;
    bool valid;
};

/**
 * struct RockchipDpChipData - splite the grf setting of kind of chips
 * @lcdcSel: grf register field of lcdcSel
 * @spdifSel: grf register field of spdifSel
 * @i2sSel: grf register field of i2sSel
 * @edpMode: grf register field of edpMode
 * @chipType: specific chip type
 * @ssc: check if SSC is supported by source
 * @audio: check if audio is supported by source
 * @splitMode: check if split mode is supported
 */
struct RockchipDpChipData {
    const struct RockchipGrfRegField lcdcSel;
    const struct RockchipGrfRegField spdifSel;
    const struct RockchipGrfRegField i2sSel;
    const struct RockchipGrfRegField edpMode;
    u32    chipType;
    bool    ssc;
    bool    audio;
    bool    splitMode;
};

struct RockchipDpDevice {
    struct drm_device        *drmDev;
    struct device            *dev;
    struct drm_encoder       encoder;
    struct drm_bridge     *bridge;
    struct drm_display_mode  mode;
    struct regmap            *grf;
    struct reset_control     *rst;
    struct reset_control     *apbReset;
    struct platform_device *audioPdev;
    const struct RockchipDpChipData *data;
    int id;
    struct AnalogixDpDevice *adp;
    struct analogix_dp_plat_data platData;
    struct RockchipDrmSubDev subDev;
};

#ifndef DP_RECEIVER_CAP_SIZE
#define DP_RECEIVER_CAP_SIZE        0xf
#endif
#define MAX_PHY        2

enum AudioFormat {
    AFMT_I2S = 0,
    AFMT_SPDIF = 1,
    AFMT_UNUSED,
};

struct AudioInfo {
    enum AudioFormat format;
    int sampleRate;
    int channels;
    int sampleWidth;
};

enum VicPxlEncodingFormat {
    PXL_RGB = 0x1,
    YCBCR_4_4_4 = 0x2,
    YCBCR_4_2_2 = 0x4,
    YCBCR_4_2_0 = 0x8,
    Y_ONLY = 0x10,
};

struct VideoInfos {
    bool hSyncPolarity;
    bool vSyncPolarity;
    bool interlaced;
    int colorDepth;
    enum VicPxlEncodingFormat colorFmt;
};

struct CdnDpPort {
    struct CdnDpDevice *dp;
    struct phy *phy;
    u8 lanes;
    bool phyEnabled;
    u8 id;
};

struct CdnDpDevice {
    struct device *dev;
    struct drm_device *drmDev;
    struct drm_connector connector;
    struct drm_encoder encoder;
    struct drm_display_mode mode;
    struct platform_device *audioPdev;
    struct delayed_work eventWork;
    struct edid *edid;
    struct RockchipDrmSubDev subDev;
    struct mutex lock;
    bool connected;
    bool active;
    bool suspended;
    const struct firmware *fw;    /* cdn dp firmware */
    unsigned int fwVersion;    /* cdn fw version */
    bool fwLoaded;
    void __iomem *regs;
    struct regmap *grf;
    struct clk *coreClk;
    struct clk *pclk;
    struct clk *spdifClk;
    struct clk *grfClk;
    struct reset_control *spdifRst;
    struct reset_control *dptxRst;
    struct reset_control *apbRst;
    struct reset_control *coreRst;
    struct AudioInfo audioInfo;
    struct VideoInfos videoInfo;
    struct CdnDpPort *port[MAX_PHY];
    u8 ports;
    u8 maxLanes;
    unsigned int maxRate;
    u8 lanes;
    int activePort;
    u8 dpcd[DP_RECEIVER_CAP_SIZE];
    bool sinkHasAudio;
};

extern void EdpSetVideoFormat(struct AnalogixDpDevice *dp);
extern void EdpEnableVideoMute(struct AnalogixDpDevice *dp, bool enable);
extern void EdpSetVideoColorFormat(struct AnalogixDpDevice *dp);
extern void EdpAudioConfigSpdif(struct AnalogixDpDevice *dp);
extern void EdpAudioConfigI2s(struct AnalogixDpDevice *dp);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //EDP_RK35XX_H