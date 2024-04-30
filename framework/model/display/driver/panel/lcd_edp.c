/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "lcd_edp.h"
#include "gpio_if.h"
#include "hdf_bl.h"
#include "hdf_disp.h"
#include "osal.h"

struct panel_edp_dev *g_edp_panel_dev = NULL;

static struct panel_edp_dev *ToPanelSimpleDev(const struct PanelData *panel)
{
    return (struct panel_edp_dev *)panel->object->priv;
}

static int32_t PanelSimpleRegulatorEnable(void)
{
    int32_t err;
    if (g_edp_panel_dev == NULL) {
        return -1;
    }
    err = regulator_enable(g_edp_panel_dev->supply);
    if (err < 0) {
        HDF_LOGE("regulator_enable failed");
        return err;
    }
    return 0;
}
static int32_t PanelSimpleRegulatorDIsable(void)
{
    if (g_edp_panel_dev == NULL) {
        return -1;
    }
    regulator_disable(g_edp_panel_dev->supply);
    return 0;
}

static int32_t PanelOn(struct PanelData *panel)
{
    struct panel_edp_dev *panel_dev = NULL;

    panel_dev = ToPanelSimpleDev(panel);
    if (panel_dev->hw_delay.enable_delay) {
        OsalMSleep(panel_dev->hw_delay.enable_delay);
    }
    return HDF_SUCCESS;
}

static int32_t PanelOff(struct PanelData *panel)
{
    struct panel_edp_dev *panel_dev = NULL;

    panel_dev = ToPanelSimpleDev(panel);
    if (panel_dev->hw_delay.disable_delay) {
        OsalMSleep(panel_dev->hw_delay.disable_delay);
    }
    return HDF_SUCCESS;
}

static int32_t PanelPrepare(struct PanelData *panel)
{
    int32_t ret;
    struct panel_edp_dev *panel_dev = NULL;

    panel_dev = ToPanelSimpleDev(panel);
    ret = regulator_enable(panel_dev->supply);
    if (ret < 0) {
        HDF_LOGE("failed to enable supply: %d\n", ret);
        return ret;
    }
    gpiod_set_value_cansleep(panel_dev->enable_gpio, 1);

    if (panel_dev->hw_delay.reset_delay > 0) {
        OsalMSleep(panel_dev->hw_delay.reset_delay);
    }

    gpiod_set_value_cansleep(panel_dev->reset_gpio, 1);

    if (panel_dev->hw_delay.reset_delay > 0) {
        OsalMSleep(panel_dev->hw_delay.reset_delay);
    }

    gpiod_set_value_cansleep(panel_dev->reset_gpio, 0);

    if (panel_dev->hw_delay.prepare_delay > 0) {
        OsalMSleep(panel_dev->hw_delay.prepare_delay);
    }

    if (panel_dev->hw_delay.init_delay > 0) {
        OsalMSleep(panel_dev->hw_delay.init_delay);
    }

    return HDF_SUCCESS;
}

static int32_t PanelUnprepare(struct PanelData *panel)
{
    int32_t ret;
    struct panel_edp_dev *panel_dev = NULL;

    panel_dev = ToPanelSimpleDev(panel);

    gpiod_set_value_cansleep(panel_dev->reset_gpio, 1);
    gpiod_set_value_cansleep(panel_dev->enable_gpio, 0);
    regulator_disable(panel_dev->supply);

    if (panel_dev->hw_delay.unprepare_delay) {
        OsalMSleep(panel_dev->hw_delay.unprepare_delay);
    }

    return HDF_SUCCESS;
}

static int32_t PanelRegulatorEnable(struct PanelData *panel)
{
    int32_t ret;
    struct panel_edp_dev *panel_dev = NULL;

    panel_dev = ToPanelSimpleDev(panel);
    ret = regulator_enable(panel_dev->supply);
    if (ret < 0) {
        HDF_LOGE("failed to enable supply: %d\n", ret);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t PanelInit(struct PanelData *panel)
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
    .width = 1920,           /* width */
    .height = 1080,          /* height */
    .hbp = 48,               /* horizontal back porch */
    .hfp = 88,               /* horizontal front porch */
    .hsw = 44,               /* horizontal sync width */
    .vbp = 36,               /* vertical back porch */
    .vfp = 4,                /* vertical front porch */
    .vsw = 5,                /* vertical sync width */
    .clockFreq = 70600000,   /* clock */
    .pWidth = 298,           /* physical width */
    .pHeight = 168,          /* physical height */
    .connectorType = DRM_MODE_CONNECTOR_DPI,   /* DRM_MODE_CONNECTOR_DPI=17 */
    .blk = { BLK_PWM, MIN_LEVEL, MAX_LEVEL, DEFAULT_LEVEL },
};

static void PanelResInit(struct panel_edp_dev *panel_dev)
{
    panel_dev->panel.info = &g_panelInfo;
    panel_dev->panel.init = PanelInit;
    panel_dev->panel.on = PanelOn;
    panel_dev->panel.off = PanelOff;
    panel_dev->panel.prepare = PanelPrepare;
    panel_dev->panel.unprepare = PanelUnprepare;
    panel_dev->panel.panelRegulatorEnable = PanelRegulatorEnable;
    panel_dev->panel.prepared = false;
    panel_dev->panel.priv = panel_dev->edpDev;
    panel_dev->hw_delay.disable_delay = 50; /* 50:disable_delay time */
    panel_dev->hw_delay.enable_delay = 120; /* 120:enable_delay */
    panel_dev->hw_delay.init_delay = 20; /* 20:init_delay */
    panel_dev->hw_delay.prepare_delay = 20; /* 20:prepare_delay */
    panel_dev->hw_delay.reset_delay = 20;  /* 20:reset_delay */
    panel_dev->hw_delay.unprepare_delay = 20;  /* 20:unprepare_delay */
}

static int32_t PanelEDPEntryInit(struct HdfDeviceObject *object)
{
    HDF_LOGW("%s Performing", __func__);
    struct device_node *panelNode = NULL;
    struct panel_edp_dev *panel_dev = NULL;
    struct platform_device *edp_platformDev;

    panel_dev = (struct panel_edp_dev *)OsalMemCalloc(sizeof(struct panel_edp_dev));
    if (panel_dev == NULL) {
        HDF_LOGE("%s panel_dev malloc fail", __func__);
        return HDF_FAILURE;
    }
    g_edp_panel_dev = panel_dev;
    panelNode = of_find_compatible_node(NULL, NULL, "simple-panel");
    if (panelNode == NULL) {
        HDF_LOGE("%s of_find_compatible_node fail", __func__);
        goto FAIL;
    }

    edp_platformDev = of_find_device_by_node(panelNode);
    panel_dev->edpDev = &edp_platformDev->dev;
    if (panel_dev->edpDev == NULL) {
        HDF_LOGE("%s of_find_device_by_node fail", __func__);
        goto FAIL;
    }

    panel_dev->supply = devm_regulator_get(panel_dev->edpDev, "power");
    if (panel_dev->supply == NULL) {
        HDF_LOGE("%s Get regulator fail", __func__);
        goto FAIL;
    }

    panel_dev->enable_gpio = devm_gpiod_get_optional(panel_dev->edpDev, "enable", GPIOD_ASIS);
    if (IS_ERR(panel_dev->enable_gpio)) {
        HDF_LOGE("%s get enable_gpio fail", __func__);
        goto FAIL;
    }
    panel_dev->hpd_gpio = devm_gpiod_get_optional(panel_dev->edpDev, "hpd", GPIOD_IN);
    if (IS_ERR(panel_dev->hpd_gpio)) {
        HDF_LOGE("%s get hpd_gpio fail", __func__);
        goto FAIL;
    }
    panel_dev->reset_gpio = devm_gpiod_get_optional(panel_dev->edpDev, "reset", GPIOD_ASIS);
    if (IS_ERR(panel_dev->reset_gpio)) {
        HDF_LOGE("%s get reset_gpio fail", __func__);
        goto FAIL;
    }

    PanelResInit(panel_dev);
    panel_dev->panel.object = object;
    object->priv = panel_dev;

    panel_dev->panel.panelType = LCD_EDP;

    if (RegisterPanel(&panel_dev->panel) != HDF_SUCCESS) {
        HDF_LOGE("%s RegisterPanel fail", __func__);
        goto FAIL;
    }
    HDF_LOGI("%s success", __func__);
    return HDF_SUCCESS;
FAIL:
    OsalMemFree(panel_dev);
    return HDF_FAILURE;
}

struct HdfDriverEntry PanelEDPDevEntry = {
    .moduleVersion = 1,
    .moduleName = "LCD_EDP",
    .Init = PanelEDPEntryInit,
};

HDF_INIT(PanelEDPDevEntry);
