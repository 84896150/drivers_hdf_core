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

#include "lcd_edp.h"
#include "gpio_if.h"
#include "hdf_bl.h"
#include "hdf_disp.h"
#include "osal.h"
#include "hcs_tree_if.h"

struct panel_edp_dev *g_edpPanelDev = NULL;

static struct panel_edp_dev *ToPanelSimpleDev(const struct PanelData *panel)
{
    return (struct panel_edp_dev *)panel->object->priv;
}

static int32_t PanelOn(struct PanelData *panel)
{
    struct panel_edp_dev *panelDev = NULL;

    panelDev = ToPanelSimpleDev(panel);
    if (panelDev->hwDelay.enableDelay) {
        OsalMSleep(panelDev->hwDelay.enableDelay);
    }
    return HDF_SUCCESS;
}

static int32_t PanelOff(struct PanelData *panel)
{
    struct panel_edp_dev *panelDev = NULL;

    panelDev = ToPanelSimpleDev(panel);
    if (panelDev->hwDelay.disableDelay) {
        OsalMSleep(panelDev->hwDelay.disableDelay);
    }
    return HDF_SUCCESS;
}

static int32_t PanelPrepare(struct PanelData *panel)
{
    int32_t ret;
    struct panel_edp_dev *panelDev = NULL;

    panelDev = ToPanelSimpleDev(panel);
    ret = regulator_enable(panelDev->supply);
    if (ret < 0) {
        HDF_LOGE("failed to enable supply: %d\n", ret);
        return ret;
    }
    gpiod_set_value_cansleep(panelDev->enableGpio, 1);

    if (panelDev->hwDelay.resetDelay > 0) {
        OsalMSleep(panelDev->hwDelay.resetDelay);
    }

    gpiod_set_value_cansleep(panelDev->resetGpio, 1);

    if (panelDev->hwDelay.resetDelay > 0) {
        OsalMSleep(panelDev->hwDelay.resetDelay);
    }

    gpiod_set_value_cansleep(panelDev->resetGpio, 0);

    if (panelDev->hwDelay.prepareDelay > 0) {
        OsalMSleep(panelDev->hwDelay.prepareDelay);
    }

    if (panelDev->hwDelay.initDelay > 0) {
        OsalMSleep(panelDev->hwDelay.initDelay);
    }

    return HDF_SUCCESS;
}

static int32_t PanelUnprepare(struct PanelData *panel)
{
    struct panel_edp_dev *panelDev = NULL;

    panelDev = ToPanelSimpleDev(panel);

    gpiod_set_value_cansleep(panelDev->resetGpio, 1);
    gpiod_set_value_cansleep(panelDev->enableGpio, 0);
    regulator_disable(panelDev->supply);

    if (panelDev->hwDelay.unprepareDelay) {
        OsalMSleep(panelDev->hwDelay.unprepareDelay);
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

static void PanelResInit(struct panel_edp_dev *panelDev)
{
    panelDev->panel.info = &g_panelInfo;
    panelDev->panel.init = PanelInit;
    panelDev->panel.on = PanelOn;
    panelDev->panel.off = PanelOff;
    panelDev->panel.prepare = PanelPrepare;
    panelDev->panel.unprepare = PanelUnprepare;
    panelDev->panel.priv = panelDev->edpDev;
    panelDev->hwDelay.disableDelay = 50; /* 50:disableDelay time */
    panelDev->hwDelay.enableDelay = 120; /* 120:enableDelay */
    panelDev->hwDelay.initDelay = 20; /* 20:initDelay */
    panelDev->hwDelay.prepareDelay = 20; /* 20:prepareDelay */
    panelDev->hwDelay.resetDelay = 20;  /* 20:resetDelay */
    panelDev->hwDelay.unprepareDelay = 20;  /* 20:unprepareDelay */
}

static int32_t PanelEDPDevInit(struct panel_edp_dev *panelDev)
{
    panelDev->supply = devm_regulator_get(panelDev->edpDev, "power");
    if (panelDev->supply == NULL) {
        HDF_LOGE("%s Get regulator fail", __func__);
        goto FAIL;
    }

    panelDev->enableGpio = devm_gpiod_get_optional(panelDev->edpDev, "enable", GPIOD_ASIS);
    if (IS_ERR(panelDev->enableGpio)) {
        HDF_LOGE("%s get enableGpio fail", __func__);
        goto FAIL;
    }
    panelDev->hpdGpio = devm_gpiod_get_optional(panelDev->edpDev, "hpd", GPIOD_IN);
    if (IS_ERR(panelDev->hpdGpio)) {
        HDF_LOGE("%s get hpdGpio fail", __func__);
        goto FAIL;
    }
    panelDev->resetGpio = devm_gpiod_get_optional(panelDev->edpDev, "reset", GPIOD_ASIS);
    if (IS_ERR(panelDev->resetGpio)) {
        HDF_LOGE("%s get resetGpio fail", __func__);
        goto FAIL;
    }

    return HDF_SUCCESS;
FAIL:
    return HDF_FAILURE;
}

static int32_t PanelEDPEntryInit(struct HdfDeviceObject *object)
{
    struct device_node *panelNode = NULL;
    struct panel_edp_dev *panelDev = NULL;
    struct platform_device *edpPlatformDev;

    panelDev = (struct panel_edp_dev *)OsalMemCalloc(sizeof(struct panel_edp_dev));
    if (panelDev == NULL) {
        HDF_LOGE("%s panelDev malloc fail", __func__);
        return HDF_FAILURE;
    }
    g_edpPanelDev = panelDev;
    panelNode = of_find_compatible_node(NULL, NULL, "simple-panel");
    if (panelNode == NULL) {
        HDF_LOGE("%s of_find_compatible_node fail", __func__);
        goto FAIL;
    }

    edpPlatformDev = of_find_device_by_node(panelNode);
    panelDev->edpDev = &edpPlatformDev->dev;
    if (panelDev->edpDev == NULL) {
        HDF_LOGE("%s of_find_device_by_node fail", __func__);
        goto FAIL;
    }

    if (PanelEDPDevInit(panelDev) != HDF_SUCCESS) {
        goto FAIL;
    }

    PanelResInit(panelDev);
    panelDev->panel.object = object;
    object->priv = panelDev;

    if (RegisterPanel(&panelDev->panel) != HDF_SUCCESS) {
        HDF_LOGE("%s RegisterPanel fail", __func__);
        goto FAIL;
    }
    HDF_LOGI("%s success", __func__);
    return HDF_SUCCESS;
FAIL:
    OsalMemFree(panelDev);
    return HDF_FAILURE;
}

struct HdfDriverEntry PanelEDPDevEntry = {
    .moduleVersion = 1,
    .moduleName = "LCD_EDP",
    .Init = PanelEDPEntryInit,
};

HDF_INIT(PanelEDPDevEntry);
