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

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/gpio/consumer.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/phy/phy.h>

#include <drm/bridge/analogix_dp.h>
#include <drm/drm_probe_helper.h>

#include "edp_linux_def.h"
#include "edp_rk35xx.h"
#include "edp_analogix_reg.h"

static void AnalogixDpWrite(struct AnalogixDpDevice *dp, u32 reg, u32 val)
{
    if (dp->platData) {
        readl(dp->regBase);
        writel(val, dp->regBase + reg);
    }

    writel(val, dp->regBase + reg);
}

static u32 AnalogixDpRead(struct AnalogixDpDevice *dp, u32 reg)
{
    if (dp->platData)
        readl(dp->regBase + reg);

    return readl(dp->regBase + reg);
}

void EdpEnableVideoMute(struct AnalogixDpDevice *dp, bool enable)
{
    u32 reg;

    if (enable) {
        reg = AnalogixDpRead(dp, ANALOGIX_DP_VIDEO_CTL_1);
        reg |= HDCP_VIDEO_MUTE;
        AnalogixDpWrite(dp, ANALOGIX_DP_VIDEO_CTL_1, reg);
    } else {
        reg = AnalogixDpRead(dp, ANALOGIX_DP_VIDEO_CTL_1);
        reg &= ~HDCP_VIDEO_MUTE;
        AnalogixDpWrite(dp, ANALOGIX_DP_VIDEO_CTL_1, reg);
    }
}

void EdpSetVideoColorFormat(struct AnalogixDpDevice *dp)
{
    u32 reg;

    /* Configure the input color depth, color space, dynamic range */
    reg = (dp->videoInfo.dynamicRange << IN_D_RANGE_SHIFT) |
        (dp->videoInfo.colorDepth << IN_BPC_SHIFT) |
        (dp->videoInfo.colorSpace << IN_COLOR_F_SHIFT);
    AnalogixDpWrite(dp, ANALOGIX_DP_VIDEO_CTL_2, reg);

    /* Set Input Color YCbCr Coefficients to ITU601 or ITU709 */
    reg = AnalogixDpRead(dp, ANALOGIX_DP_VIDEO_CTL_3);
    reg &= ~IN_YC_COEFFI_MASK;
    if (dp->videoInfo.ycbcrCoeff)
        reg |= IN_YC_COEFFI_ITU709;
    else
        reg |= IN_YC_COEFFI_ITU601;
    AnalogixDpWrite(dp, ANALOGIX_DP_VIDEO_CTL_3, reg);
}

void EdpSetVideoTimingMode(struct AnalogixDpDevice *dp, u32 type)
{
    u32 reg;

    if (type == VIDEO_TIMING_FROM_CAPTURE) {
        reg = AnalogixDpRead(dp, ANALOGIX_DP_VIDEO_CTL_10);
        reg &= ~FORMAT_SEL;
        AnalogixDpWrite(dp, ANALOGIX_DP_VIDEO_CTL_10, reg);
    } else {
        reg = AnalogixDpRead(dp, ANALOGIX_DP_VIDEO_CTL_10);
        reg |= FORMAT_SEL;
        AnalogixDpWrite(dp, ANALOGIX_DP_VIDEO_CTL_10, reg);
    }
}

void EdpSetVideoFormat(struct AnalogixDpDevice *dp)
{
    struct VideoInfo *video = &dp->videoInfo;
    const struct drm_display_mode *mode = &video->mode;
    unsigned int hsw, hfp, hbp, vsw, vfp, vbp;

    hsw = mode->hsync_end - mode->hsync_start;
    hfp = mode->hsync_start - mode->hdisplay;
    hbp = mode->htotal - mode->hsync_end;
    vsw = mode->vsync_end - mode->vsync_start;
    vfp = mode->vsync_start - mode->vdisplay;
    vbp = mode->vtotal - mode->vsync_end;

    /* Set Video Format Parameters */
    AnalogixDpWrite(dp, ANALOGIX_DP_TOTAL_LINE_CFG_L,
                    TOTAL_LINE_CFG_L(mode->vtotal));
    AnalogixDpWrite(dp, ANALOGIX_DP_TOTAL_LINE_CFG_H,
                    TOTAL_LINE_CFG_H(mode->vtotal >> ANALOGIX_REG_BIT_8));
    AnalogixDpWrite(dp, ANALOGIX_DP_ACTIVE_LINE_CFG_L,
                    ACTIVE_LINE_CFG_L(mode->vdisplay));
    AnalogixDpWrite(dp, ANALOGIX_DP_ACTIVE_LINE_CFG_H,
                    ACTIVE_LINE_CFG_H(mode->vdisplay >> ANALOGIX_REG_BIT_8));
    AnalogixDpWrite(dp, ANALOGIX_DP_V_F_PORCH_CFG,
                    V_F_PORCH_CFG(vfp));
    AnalogixDpWrite(dp, ANALOGIX_DP_V_SYNC_WIDTH_CFG,
                    V_SYNC_WIDTH_CFG(vsw));
    AnalogixDpWrite(dp, ANALOGIX_DP_V_B_PORCH_CFG,
                    V_B_PORCH_CFG(vbp));
    AnalogixDpWrite(dp, ANALOGIX_DP_TOTAL_PIXEL_CFG_L,
                    TOTAL_PIXEL_CFG_L(mode->htotal));
    AnalogixDpWrite(dp, ANALOGIX_DP_TOTAL_PIXEL_CFG_H,
                    TOTAL_PIXEL_CFG_H(mode->htotal >> ANALOGIX_REG_BIT_8));
    AnalogixDpWrite(dp, ANALOGIX_DP_ACTIVE_PIXEL_CFG_L,
                    ACTIVE_PIXEL_CFG_L(mode->hdisplay));
    AnalogixDpWrite(dp, ANALOGIX_DP_ACTIVE_PIXEL_CFG_H,
                    ACTIVE_PIXEL_CFG_H(mode->hdisplay >> ANALOGIX_REG_BIT_8));
    AnalogixDpWrite(dp, ANALOGIX_DP_H_F_PORCH_CFG_L,
                    H_F_PORCH_CFG_L(hfp));
    AnalogixDpWrite(dp, ANALOGIX_DP_H_F_PORCH_CFG_H,
                    H_F_PORCH_CFG_H(hfp >> ANALOGIX_REG_BIT_8));
    AnalogixDpWrite(dp, ANALOGIX_DP_H_SYNC_CFG_L,
                    H_SYNC_CFG_L(hsw));
    AnalogixDpWrite(dp, ANALOGIX_DP_H_SYNC_CFG_H,
                    H_SYNC_CFG_H(hsw >> ANALOGIX_REG_BIT_8));
    AnalogixDpWrite(dp, ANALOGIX_DP_H_B_PORCH_CFG_L,
                    H_B_PORCH_CFG_L(hbp));
    AnalogixDpWrite(dp, ANALOGIX_DP_H_B_PORCH_CFG_H,
                    H_B_PORCH_CFG_H(hbp >> ANALOGIX_REG_BIT_8));
}

void EdpAudioConfigI2s(struct AnalogixDpDevice *dp)
{
    u32 reg;

    reg = AnalogixDpRead(dp, ANALOGIX_DP_SYS_CTL_4);
    reg &= ~FIX_M_AUD;
    AnalogixDpWrite(dp, ANALOGIX_DP_SYS_CTL_4, reg);

    reg = AnalogixDpRead(dp, ANALOGIX_DP_I2S_CTRL);
    reg |= I2S_EN;
    AnalogixDpWrite(dp, ANALOGIX_DP_I2S_CTRL, reg);
}

void EdpAudioConfigSpdif(struct AnalogixDpDevice *dp)
{
    u32 reg;

    reg = AnalogixDpRead(dp, ANALOGIX_DP_SYS_CTL_4);
    reg &= ~FIX_M_AUD;
    AnalogixDpWrite(dp, ANALOGIX_DP_SYS_CTL_4, reg);

    reg = AnalogixDpRead(dp, ANALOGIX_DP_SPDIF_AUDIO_CTL_0);
    reg |= AUD_SPDIF_EN;
    AnalogixDpWrite(dp, ANALOGIX_DP_SPDIF_AUDIO_CTL_0, reg);
}

void EdpAudioEnable(struct AnalogixDpDevice *dp)
{
    u32 reg;

    reg = AnalogixDpRead(dp, ANALOGIX_DP_FUNC_EN_1);
    reg &= ~(AUD_FIFO_FUNC_EN_N | AUD_FUNC_EN_N);
    AnalogixDpWrite(dp, ANALOGIX_DP_FUNC_EN_1, reg);

    reg = AnalogixDpRead(dp, ANALOGIX_DP_AUD_CTL);
    reg |= MISC_CTRL_RESET | DP_AUDIO_EN;
    AnalogixDpWrite(dp, ANALOGIX_DP_AUD_CTL, reg);
}

void EdpAudioDisable(struct AnalogixDpDevice *dp)
{
    u32 reg;

    AnalogixDpWrite(dp, ANALOGIX_DP_AUD_CTL, 0);

    reg = AnalogixDpRead(dp, ANALOGIX_DP_FUNC_EN_1);
    reg |= AUD_FIFO_FUNC_EN_N | AUD_FUNC_EN_N;
    AnalogixDpWrite(dp, ANALOGIX_DP_FUNC_EN_1, reg);
}
