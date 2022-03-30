/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mipi_dsi_panel_simple.h"
#include "gpio_if.h"
#include "hdf_bl.h"
#include "hdf_disp.h"
#include "osal.h"

struct PanelSimpleDev *g_panelSimpleDev = NULL;

static struct PanelSimpleDev *ToPanelSimpleDev(const struct PanelData *panel)
{
    return (struct PanelSimpleDev *)panel->object->priv;
}

static int panel_simple_regulator_enable(void)
{
    int err;
    if (g_panelSimpleDev == NULL)
        return -1;
    err = regulator_enable(g_panelSimpleDev->supply);
    if (err < 0) {
        HDF_LOGE("regulator_enable failed");
        return err;
    }
    return 0;
}
static int panel_simple_regulator_disable(void)
{
    int err;
    if (g_panelSimpleDev == NULL)
        return -1;
    regulator_disable(g_panelSimpleDev->supply);
    return 0;
}

int panel_simple_loader_protect(struct drm_panel *panel)
{
    int err;
    err = panel_simple_regulator_enable();
    if (err < 0) {
        HDF_LOGE("failed to enable supply: %d\n", err);
        return err;
    }
    return 0;
}
EXPORT_SYMBOL(panel_simple_loader_protect);

static int PanelSimpleSendCmds(struct mipi_dsi_device *dsi,
    const struct DsiCmdDesc *cmds, int size)
{
    int32_t i = 0;
    HDF_LOGI("%s() %d", __func__, __LINE__);
    if (dsi == NULL) {
        HDF_LOGE("error: dis is NULL\n");
        return -EINVAL;
    }
    for (i = 0; i < size; i++) {
        mipi_dsi_dcs_write_buffer(dsi, cmds[i].payload, cmds[i].dataLen);
        if (cmds[i].delay) {
            OsalMSleep(cmds[i].delay);
        }
    }
    HDF_LOGI("%s() %d", __func__, __LINE__);
    return HDF_SUCCESS;
}

static int32_t PanelSimpleOn(struct PanelData *panel)
{
    struct PanelSimpleDev *panelSimpleDev = NULL;

    HDF_LOGE("%s() %d", __func__, __LINE__);
    panelSimpleDev = ToPanelSimpleDev(panel);
    if (panelSimpleDev->panelhwdelay.enable_delay) {
        OsalMSleep(panelSimpleDev->panelhwdelay.enable_delay);
    }
    return HDF_SUCCESS;
}

static int32_t PanelSimpleOff(struct PanelData *panel)
{
    struct PanelSimpleDev *panelSimpleDev = NULL;

    HDF_LOGE("%s() %d", __func__, __LINE__);
    panelSimpleDev = ToPanelSimpleDev(panel);
    if (panelSimpleDev->panelhwdelay.disable_delay) {
        OsalMSleep(panelSimpleDev->panelhwdelay.disable_delay);
    }
    return HDF_SUCCESS;
}

static int32_t PanelSimplePrepare(struct PanelData *panel)
{
    int32_t ret;
    struct PanelSimpleDev *panelSimpleDev = NULL;

    HDF_LOGE("%s() %d", __func__, __LINE__);
    panelSimpleDev = ToPanelSimpleDev(panel);

    ret = regulator_enable(panelSimpleDev->supply);
    if (ret < 0) {
        HDF_LOGE("failed to enable supply: %d\n", ret);
        return ret;
    }
    gpiod_set_value_cansleep(panelSimpleDev->enable_gpio, 1);

    if (panelSimpleDev->panelhwdelay.reset_delay) {
        OsalMSleep(panelSimpleDev->panelhwdelay.reset_delay);
    }

    gpiod_set_value_cansleep(panelSimpleDev->reset_gpio, 1);

    if (panelSimpleDev->panelhwdelay.reset_delay) {
        OsalMSleep(panelSimpleDev->panelhwdelay.reset_delay);
    }

    gpiod_set_value_cansleep(panelSimpleDev->reset_gpio, 0);

    if (panelSimpleDev->panelhwdelay.prepare_delay) {
        OsalMSleep(panelSimpleDev->panelhwdelay.prepare_delay);
    }

    ret = PanelSimpleSendCmds(panelSimpleDev->dsiDev, g_panelOnCode,
                              sizeof(g_panelOnCode) / sizeof(g_panelOnCode[0]));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s PanelSimpleSendCmds failed", __func__);
        return HDF_FAILURE;
    }
    if (panelSimpleDev->panelhwdelay.init_delay) {
        OsalMSleep(panelSimpleDev->panelhwdelay.init_delay);
    }

    HDF_LOGI("%s() %d", __func__, __LINE__);
    return HDF_SUCCESS;
}

static int32_t PanelSimpleUnprepare(struct PanelData *panel)
{
    int32_t ret;
    struct PanelSimpleDev *panelSimpleDev = NULL;

    panelSimpleDev = ToPanelSimpleDev(panel);
    HDF_LOGE(" %s line = %d", __func__, __LINE__);

    ret = PanelSimpleSendCmds(panelSimpleDev->dsiDev, g_panelOffCode,
                              sizeof(g_panelOffCode) / sizeof(g_panelOffCode[0]));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s PanelSimpleSendCmds failed", __func__);
        return HDF_FAILURE;
    }
    gpiod_set_value_cansleep(panelSimpleDev->reset_gpio, 1);
    gpiod_set_value_cansleep(panelSimpleDev->enable_gpio, 1);
    regulator_disable(panelSimpleDev->supply);

    if (panelSimpleDev->panelhwdelay.unprepare_delay) {
        OsalMSleep(panelSimpleDev->panelhwdelay.unprepare_delay);
    }

    return HDF_SUCCESS;
}

static int32_t PanelSimpleInit(struct PanelData *panel)
{
    return 0;
}

#define BLK_PWM_INDEX             2
#define PWM_MAX_PERIOD            40000
/* backlight setting */
#define MIN_LEVEL                 0
#define MAX_LEVEL                 255
#define DEFAULT_LEVEL             127

static struct PanelInfo g_panelInfo = {
    .width = 720,          /* width */
    .height = 1280,          /* height */
    .hbp = 40,             /* horizontal back porch */
    .hfp = 40,         /* horizontal front porch */
    .hsw = 10,              /* horizontal sync width */
    .vbp = 15,              /* vertical back porch */
    .vfp = 10,              /* vertical front porch */
    .vsw = 36,               /* vertical sync width */
    .clockFreq = 75000000,  /* clock */
    .pWidth = 68,           /* physical width */
    .pHeight = 121,         /* physical height */
    .connector_type = DRM_MODE_CONNECTOR_DPI,   /* DRM_MODE_CONNECTOR_DPI=17 */
    .blk = { BLK_PWM, MIN_LEVEL, MAX_LEVEL, DEFAULT_LEVEL },
};

static void PanelSimpleResInit(struct PanelSimpleDev *panelSimpleDev)
{
    panelSimpleDev->dsiDev->lanes = 4;  /* 4: dsi,lanes ,number of active data lanes */
    panelSimpleDev->dsiDev->format = MIPI_DSI_FMT_RGB888; // dsi,format pixel format for video mode MIPI_DSI_FMT_RGB888
    panelSimpleDev->dsiDev->mode_flags = (MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST | MIPI_DSI_MODE_LPM \
                                          | MIPI_DSI_MODE_EOT_PACKET);
    panelSimpleDev->panel.info = &g_panelInfo;
    panelSimpleDev->panel.init = PanelSimpleInit;
    panelSimpleDev->panel.on = PanelSimpleOn;
    panelSimpleDev->panel.off = PanelSimpleOff;
    panelSimpleDev->panel.prepare = PanelSimplePrepare;
    panelSimpleDev->panel.unprepare = PanelSimpleUnprepare;
    panelSimpleDev->panel.priv = panelSimpleDev->dsiDev;
    panelSimpleDev->panelhwdelay.disable_delay = 50; /* disable_delay time */
    panelSimpleDev->panelhwdelay.enable_delay = 120; /* enable_delay */
    panelSimpleDev->panelhwdelay.init_delay = 20; /* init_delay */
    panelSimpleDev->panelhwdelay.prepare_delay = 2; /* prepare_delay */
    panelSimpleDev->panelhwdelay.reset_delay = 100;  /* reset_delay */
    panelSimpleDev->panelhwdelay.unprepare_delay = 20;  /* unprepare_delay */
}

int32_t PanelSimpleEntryInit(struct HdfDeviceObject *object)
{
    struct device_node *panelNode = NULL;
    struct PanelSimpleDev *panelSimpleDev = NULL;

    panelSimpleDev = (struct PanelSimpleDev *)OsalMemCalloc(sizeof(struct PanelSimpleDev));
    if (panelSimpleDev == NULL) {
        HDF_LOGE("%s panelSimpleDev malloc fail", __func__);
        return HDF_FAILURE;
    }
    g_panelSimpleDev = panelSimpleDev;
    panelNode = of_find_compatible_node(NULL, NULL, "simple-panel-dsi");
    if (panelNode == NULL) {
        HDF_LOGE("%s of_find_compatible_node fail", __func__);
        goto FAIL;
    }
    panelSimpleDev->dsiDev = of_find_mipi_dsi_device_by_node(panelNode);
    if (panelSimpleDev->dsiDev == NULL) {
        HDF_LOGE("%s of_find_mipi_dsi_device_by_node fail", __func__);
        goto FAIL;
    }
    panelSimpleDev->supply = devm_regulator_get(&panelSimpleDev->dsiDev->dev, "power");
    if (panelSimpleDev->supply == NULL) {
        HDF_LOGE("Get regulator fail");
        goto FAIL;
    }

    panelSimpleDev->enable_gpio = devm_gpiod_get_optional(&panelSimpleDev->dsiDev->dev, "enable", GPIOD_ASIS);
    if (IS_ERR(panelSimpleDev->enable_gpio)) {
        HDF_LOGI("get enable_gpio fail");
        return PTR_ERR(panelSimpleDev->enable_gpio);
    }
    panelSimpleDev->hpd_gpio = devm_gpiod_get_optional(&panelSimpleDev->dsiDev->dev, "hpd", GPIOD_IN);
    if (IS_ERR(panelSimpleDev->hpd_gpio)) {
        HDF_LOGI("get hpd_gpio fail");
        return PTR_ERR(panelSimpleDev->hpd_gpio);
    }
    panelSimpleDev->reset_gpio = devm_gpiod_get_optional(&panelSimpleDev->dsiDev->dev, "reset", GPIOD_ASIS);
    if (IS_ERR(panelSimpleDev->reset_gpio)) {
        HDF_LOGI("get reset_gpio fail");
        return PTR_ERR(panelSimpleDev->reset_gpio);
    }

    PanelSimpleResInit(panelSimpleDev);
    panelSimpleDev->panel.blDev = GetBacklightDev("hdf_pwm");
    if (panelSimpleDev->panel.blDev == NULL) {
        HDF_LOGE("%s GetBacklightDev fail", __func__);
        goto FAIL;
    }

    panelSimpleDev->panel.object = object;
    object->priv = panelSimpleDev;

    if (RegisterPanel(&panelSimpleDev->panel) != HDF_SUCCESS) {
        HDF_LOGI("RegisterPanel fail");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s success", __func__);
    return HDF_SUCCESS;
FAIL:
    OsalMemFree(panelSimpleDev);
    return HDF_FAILURE;
}

struct HdfDriverEntry PanelSimpleDevEntry = {
    .moduleVersion = 1,
    .moduleName = "LCD_ILI9881CBOE",
    .Init = PanelSimpleEntryInit,
};

HDF_INIT(PanelSimpleDevEntry);