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

#include "hdf_base.h"
#include "hdf_log.h"

#include "edp_linux_def.h"
#include <drm/drm_connector.h>
#include <drm/drm_panel.h>
#include "edp_rk35xx.h"
#include "edp_if.h"
#include "edp_core.h"

#define HDF_LOG_TAG edp_rk35xx_hdf

#ifdef CONFIG_ARCH_ROCKCHIP
static int32_t EdpBpcToColorDepth(uint32_t bpc, uint32_t *color)
{
    int32_t ret = HDF_FAILURE;
    switch (bpc) {
        case COLOR_6:
            *color = EDP_COLOR_DEPTH_6BIT;
            break;
        case COLOR_8:
            *color = EDP_COLOR_DEPTH_8BIT;
            break;
        case COLOR_10:
            *color = EDP_COLOR_DEPTH_10BIT;
            break;
        case COLOR_12:
            *color = EDP_COLOR_DEPTH_12BIT;
            break;
        default:
            return ret;
    }
    ret = 0;
    return ret;
}

static int32_t EdpColorDepthToBpc(uint32_t *bpc, uint32_t color)
{
    int32_t ret = HDF_FAILURE;
    switch (color) {
        case EDP_COLOR_DEPTH_6BIT:
            *bpc = COLOR_6;
            break;
        case EDP_COLOR_DEPTH_8BIT:
            *bpc = COLOR_8;
            break;
        case EDP_COLOR_DEPTH_10BIT:
            *bpc = COLOR_10;
            break;
        case EDP_COLOR_DEPTH_12BIT:
            *bpc = COLOR_12;
            break;
        default:
            return ret;
    }
    ret = 0;
    return ret;
}

static int32_t EdpColorSpaceToFormat(uint32_t *colorFormats, uint32_t colorSpace)
{
    int32_t ret = HDF_SUCCESS;
    switch (colorSpace) {
        case EDP_COLOR_SPACE_RGB:
            *colorFormats = DRM_COLOR_FORMAT_RGB444;
            break;
        case EDP_COLOR_SPACE_YCBCR422:
            *colorFormats = DRM_COLOR_FORMAT_YCBCR422;
            break;
        case EDP_COLOR_SPACE_YCBCR444:
            *colorFormats = DRM_COLOR_FORMAT_YCBCR444;
            break;
        default:
            ret = HDF_FAILURE;
            break;
    }

    return ret;
}

static int32_t EdpColorCoeffToYCBCRCoeff(uint32_t *color_coeff, uint32_t colorCoeff)
{
    int32_t ret = HDF_SUCCESS;
    switch (colorCoeff) {
        case EDP_COLOR_COEFF_YCBCR601:
            *color_coeff = COLOR_YCBCR601;
            break;
        case EDP_COLOR_COEFF_YCBCR709:
            *color_coeff = COLOR_YCBCR709;
            break;
        default:
            ret = HDF_FAILURE;
            break;
    }

    return ret;
}

static int32_t EdpSetupAudioHardwareParams(struct AnalogixDpDevice *dp, enum EdpAudioInterfaceType ifType)
{
    int32_t ret = HDF_SUCCESS;
    switch (ifType) {
        case EDP_AUDIO_IF_TYPE_I2S:
            EdpAudioConfigI2s(dp);
            break;
        case EDP_AUDIO_IF_TYPE_SPDIF:
            EdpAudioConfigSpdif(dp);
            break;
        default:
            ret = HDF_FAILURE;
            break;
    }

    return ret;
}

static int32_t EdpVideoAttrCheck(HdfEdpAdapterData *edpAdapterData, const struct EdpVideoAttr *pAttr)
{
    int32_t ret = HDF_FAILURE;
    struct RockchipDpDevice *drvData = platform_get_drvdata(edpAdapterData->platDev);
    struct AnalogixDpDevice *dp = drvData->adp;
    struct VideoInfo *info = &dp->videoInfo;
    struct drm_display_mode *origMode = &info->mode;
    struct drm_display_mode adjMode;
    drm_mode_copy(&adjMode, origMode);

    ret = EdpColorDepthToBpc(&info->colorDepth, pAttr->colorDepth);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s EdpColorDepthToBpc %d\n", __func__, ret);
        return ret;
    }

    ret = EdpColorSpaceToFormat(&info->colorSpace, pAttr->colorSpace);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s EdpColorSpaceToFormat %d\n", __func__, ret);
        return ret;
    }

    ret = EdpColorCoeffToYCBCRCoeff(&info->ycbcrCoeff, pAttr->colorSpace);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s EdpColorCoeffToYCBCRCoeff %d\n", __func__, ret);
        return ret;
    }

    dp->bridge.funcs->mode_set(&dp->bridge, origMode, &adjMode);
    ret = HDF_SUCCESS;
    return ret;
}
#endif

int32_t LinuxEdpAdapterInit(HdfEdpAdapterData *edpAdapterData)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    int32_t ret = HDF_FAILURE;
    if (NULL == edpAdapterData) {
        HDF_LOGE("%s null adapter get\n", __func__);
        return ret;
    }
    struct drm_panel *edpDrmPanel = NULL;
    struct drm_bridge *edpDrmBridge = NULL;
    struct drm_mode_config *drmConfig = NULL;
    struct device_node *edpNode = NULL;
    struct platform_device *platDev = NULL;
    void *privData = NULL;
    edpNode = of_find_node_by_path("/edp@fe0c0000");
    if (NULL == edpNode) {
        HDF_LOGE("find edp dt node failed\n");
        return ret;
    }
    platDev = of_find_device_by_node(edpNode);
    if (NULL == platDev) {
        HDF_LOGE("find platform device of edp dt node failed\n");
        return ret;
    }
    privData = platform_get_drvdata(platDev);
    if (privData == NULL) {
        HDF_LOGE("edp device is not probed correctly");
        return HDF_FAILURE;
    }
    ret = drm_of_find_panel_or_bridge(edpNode, 1, -1, &edpDrmPanel, &edpDrmBridge);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("drm_of_find_panel_or_bridge failed: %d\n", ret);
        return ret;
    }

    if (!edpDrmBridge) {
        edpDrmBridge = &((struct RockchipDpDevice *)privData)->adp->bridge;
    }

    edpAdapterData->platDev = platDev;
    edpAdapterData->node = edpNode;
    edpAdapterData->drmPanel = edpDrmPanel;
    edpAdapterData->drmBridge = edpDrmBridge;
    edpAdapterData->connector = &((struct RockchipDpDevice *)privData)->adp->connector;

    ret = HDF_SUCCESS;
    return ret;
#else
    return HDF_FAILURE;
#endif
}

void LinuxEdpAdapterDeInit(HdfEdpAdapterData *edpAdapterData)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return;
    }
#endif
}

int32_t LinuxEdpDisable(HdfEdpAdapterData* edpAdapterData)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    int32_t ret = HDF_FAILURE;
    if (edpAdapterData->drmPanel == NULL) {
        HDF_LOGE("%s, edp panel is null", __func__);
        return ret;
    }

    ret = drm_panel_disable(edpAdapterData->drmPanel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s, disable panel failed", __func__);
    }

    ret = drm_panel_unprepare(edpAdapterData->drmPanel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s, panel unprepare failed\n");
        return ret;
    }
    return ret;
#else
    return HDF_FAILURE;
#endif
}

int32_t LinuxEdpEnable(HdfEdpAdapterData* edpAdapterData)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    int32_t ret = HDF_FAILURE;
    if (edpAdapterData->drmPanel == NULL) {
        HDF_LOGE("%s, edp panel is null", __func__);
        return ret;
    }

    ret = drm_panel_enable(edpAdapterData->drmPanel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s, enable panel failed", __func__);
    }

    ret = drm_panel_prepare(edpAdapterData->drmPanel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s, panel prepare failed\n");
        return ret;
    }
    return ret;
#else
    return HDF_FAILURE;
#endif
}

int32_t LinuxEdpDeepColorSet(HdfEdpAdapterData* edpAdapterData, uint32_t color)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    int32_t ret = HDF_FAILURE;
    struct RockchipDpDevice *drvData = NULL;
    struct drm_display_info *info = NULL;
    struct drm_display_mode *mode = NULL;

    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    drvData = platform_get_drvdata(edpAdapterData->platDev);
    info = &edpAdapterData->connector->display_info;
    mode = &drvData->adp->videoInfo.mode;
    //modify video bpc to setup color depth by edp driver
    ret = EdpColorDepthToBpc(&info->bpc, color);
    if (ret != 0) {
        HDF_LOGE("EdpColorDepthToBpc failed: %d\n", ret);
        return ret;
    }

    drvData->adp->bridge.funcs->mode_set(&drvData->adp->bridge, mode, mode);
    HDF_LOGI("EdpSetVideoColorFormat()!!\n");
    EdpSetVideoColorFormat(drvData->adp);

    if (ret != 0) {
        HDF_LOGE("%s failed: %d\n", __func__, ret);
    }

    return ret;
#else
    return HDF_FAILURE;
#endif
}

int32_t LinuxEdpDeepColorGet(HdfEdpAdapterData* edpAdapterData, uint32_t *color)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    int32_t ret = HDF_FAILURE;
    struct RockchipDpDevice *drvData = NULL;

    if (edpAdapterData == NULL || color == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    drvData = platform_get_drvdata(edpAdapterData->platDev);
    if (drvData == NULL) {
        HDF_LOGE("%s: drvData is null!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("drm_display_info %x!!\n", edpAdapterData->connector);
    drvData->platData.get_modes(&drvData->platData, edpAdapterData->connector);
    struct drm_display_info *info = &edpAdapterData->connector->display_info;
    ret = EdpBpcToColorDepth(info->bpc, color);

    return ret;
#else
    return HDF_FAILURE;
#endif
}

int32_t LinuxEdpAvMuteSet(HdfEdpAdapterData *edpAdapterData, bool enable)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    //only for DP to HDMI
    int32_t ret = HDF_SUCCESS;
    struct RockchipDpDevice *drvData = NULL;
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    drvData = platform_get_drvdata(edpAdapterData->platDev);
    if (drvData == NULL) {
        HDF_LOGE("%s: drvData is null!", __func__);
        return HDF_FAILURE;
    }
    EdpEnableVideoMute(drvData->adp, enable);
    return ret;
#else
    return HDF_FAILURE;
#endif
}

int32_t LinuxEdpSetVideoAttribute(HdfEdpAdapterData *edpAdapterData, const void *attr)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    int32_t ret = HDF_SUCCESS;
    struct RockchipDpDevice *drvData = NULL;
    const struct EdpVideoAttr *pAttr = NULL;
    struct VideoInfo *info = NULL;

    if (edpAdapterData == NULL || attr == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    drvData = platform_get_drvdata(edpAdapterData->platDev);
    if (drvData == NULL) {
        HDF_LOGE("%s: drvData is null!", __func__);
        return HDF_FAILURE;
    }

    pAttr = (const struct EdpVideoAttr *)attr;
    info = &drvData->adp->videoInfo;
    ret = EdpVideoAttrCheck(edpAdapterData, pAttr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s EdpVideoAttrCheck failed", __func__);
        return ret;
    }
    EdpSetVideoColorFormat(drvData->adp);
    EdpSetVideoFormat(drvData->adp);
    return ret;
#else
    return HDF_FAILURE;
#endif
}

int32_t LinuxEdpSetAudioAttribute(HdfEdpAdapterData *edpAdapterData, const void *attr)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    //For DP to HDMI only
    int32_t ret = HDF_SUCCESS;
    struct RockchipDpDevice *drvData = NULL;
    const struct EdpAudioAttr *pAttr = NULL;

    if (edpAdapterData == NULL || attr == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }

    drvData = platform_get_drvdata(edpAdapterData->platDev);
    if (drvData == NULL) {
        HDF_LOGE("%s: drvData is null!", __func__);
        return HDF_FAILURE;
    }

    pAttr = (const struct EdpAudioAttr*)attr;
    EdpSetupAudioHardwareParams(drvData->adp, pAttr->ifType);

    return ret;
#else
    return HDF_FAILURE;
#endif
}

static void EdpHpdCallback(void *data, enum drm_connector_status status)
{
    HdfEdpAdapterData *edpAdapterData = NULL;
    if (data == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return;
    }
    edpAdapterData = (HdfEdpAdapterData *)data;
    bool hpd = false;
    switch (status) {
        case connector_status_connected:
            hpd = true;
            break;
        case connector_status_disconnected:
        default:
            hpd = false;
            break;
    }
    struct EdpCntlr *cntlr = (struct EdpCntlr *)edpAdapterData->hdfHdl;
    cntlr->hpdCbInfo->callbackFunc(data, hpd);
}

int32_t LinuxEdpRegisterHpdCallbackFunc(HdfEdpAdapterData *edpAdapterData, void *hpdcb)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    int32_t ret = HDF_SUCCESS;
    struct RockchipDpDevice *drvData = NULL;

    if (edpAdapterData == NULL || hpdcb == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return HDF_FAILURE;
    }
    //hpd is not supported for this device
    drvData = platform_get_drvdata(edpAdapterData->platDev);
    if (drvData == NULL) {
        HDF_LOGE("%s: drvData is null!", __func__);
        return HDF_FAILURE;
    }

    drm_bridge_hpd_enable(&drvData->adp->bridge, EdpHpdCallback, (void *)edpAdapterData);
    return ret;
#else
    return HDF_FAILURE;
#endif
}

int32_t LinuxEdpUnRegisterHpdCallbackFunc(HdfEdpAdapterData *edpAdapterData)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    struct RockchipDpDevice *drvData = NULL;
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return HDF_FAILURE;
    }

    drvData = platform_get_drvdata(edpAdapterData->platDev);
    if (drvData == NULL) {
        HDF_LOGE("%s: drvData is null!", __func__);
        return HDF_FAILURE;
    }
    drm_bridge_hpd_disable(&drvData->adp->bridge);
    return HDF_SUCCESS;
#else
    return HDF_FAILURE;
#endif
}

int32_t LinuxEdpGetConnector(HdfEdpAdapterData *edpAdapterData, void **connector)
{
#ifdef CONFIG_ARCH_ROCKCHIP
    int32_t ret = HDF_SUCCESS;
    struct RockchipDpDevice *drvData = NULL;
    if (edpAdapterData == NULL || connector == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return HDF_FAILURE;
    }

    drvData = platform_get_drvdata(edpAdapterData->platDev);
    if (drvData == NULL) {
        HDF_LOGE("%s: drvData is null!", __func__);
        return HDF_FAILURE;
    }

    *connector = (void *)&drvData->adp->connector;
    return ret;
#else
    return HDF_FAILURE;
#endif
}
