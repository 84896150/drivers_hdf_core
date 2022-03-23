/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_drm_panel.h"
#include <drm/drm_device.h>
#include <drm/drm_atomic_helper.h>
#include <linux/backlight.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <video/mipi_display.h>
#include <video/of_display_timing.h>
#include <video/videomode.h>
#include <uapi/drm/drm_mode.h>
#include "osal_mem.h"
#include "osal.h"

static inline struct HdfDrmPanel *ToHdfDrmPanel(const struct drm_panel *panel)
{
    return container_of(panel, struct HdfDrmPanel, panel);
}

static int HdfDrmPanelUnprepare(struct drm_panel *panel)
{
    pr_err("==debug %s %d\n",__FUNCTION__,__LINE__);
    return HDF_SUCCESS;
}
 
static int HdfDrmPanelPrepare(struct drm_panel *panel)
{
    pr_err("==debug %s %d\n",__FUNCTION__,__LINE__);
    return HDF_SUCCESS;
}
 
static int HdfDrmPanelDisable(struct drm_panel *panel)
{
    pr_err("==debug %s %d\n",__FUNCTION__,__LINE__);
    return HDF_SUCCESS;
}
 
static int HdfDrmPanelEnable(struct drm_panel *panel)
{
    pr_err("==debug %s %d\n",__FUNCTION__,__LINE__);
    return HDF_SUCCESS;
}

static int HdfDrmPanelGetModes(struct drm_panel *panel,struct drm_connector *connector)
{
    struct HdfDrmPanel *hdfDrmPanel = NULL;
    struct drm_display_mode *mode = NULL;
    struct PanelInfo *panelInfo = NULL;

    if (panel == NULL) {
       HDF_LOGE("failed:panel = NULL");
       return 0;
    }
    if (connector == NULL) {
        HDF_LOGE("failed:connector = NULL");
       return 0;
    }
    pr_err("==debug %s %d\n",__FUNCTION__,__LINE__);
    HDF_LOGD("HdfDrmPanelGetModes");
    hdfDrmPanel = ToHdfDrmPanel(panel);
    pr_err("==debug %s %d clockFreq %d\n",__FUNCTION__,__LINE__,hdfDrmPanel->mode.clock);
    pr_err("==debug %s %d debug %d\n",__FUNCTION__,__LINE__,hdfDrmPanel->mode.debug);
    panelInfo = hdfDrmPanel->manager->panelManager->panel[hdfDrmPanel->index]->info;
	pr_err("==debug %s %d clockFreq %d\n",__FUNCTION__,__LINE__,panelInfo->clockFreq);
	mode = drm_mode_duplicate(connector->dev, &hdfDrmPanel->mode);
    if (!mode) {
        HDF_LOGE("failed to add mode %ux%ux@%u",
            hdfDrmPanel->mode.hdisplay, hdfDrmPanel->mode.vdisplay,
            drm_mode_vrefresh(mode));
        return -ENOMEM;
    }
    pr_err("==debug %s %d debug %d\n",__FUNCTION__,__LINE__,hdfDrmPanel->mode.debug);
    drm_mode_set_name(mode);
    mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
    connector->display_info.width_mm = panelInfo->pWidth;
    connector->display_info.height_mm = panelInfo->pHeight;
	drm_mode_probed_add(connector, mode);
    return 1;
}
 
static struct drm_panel_funcs g_hdfDrmPanelFuncs = {
    .get_modes = HdfDrmPanelGetModes,
    .enable = HdfDrmPanelEnable,
    .disable = HdfDrmPanelDisable,
    .prepare = HdfDrmPanelPrepare,
    .unprepare = HdfDrmPanelUnprepare,
};



static void CreateDrmMode(struct HdfDrmPanel *hdfDrmPanel)
{
    uint32_t panelIndex;
    struct PanelInfo *panelInfo = NULL;
     pr_err("==debug %s %d hdfDrmPanel->index %d\n",__FUNCTION__,__LINE__,hdfDrmPanel->index);
    panelIndex = hdfDrmPanel->index;
    panelInfo = hdfDrmPanel->manager->panelManager->panel[panelIndex]->info;
    pr_err("==debug %s %d clockFreq %d\n",__FUNCTION__,__LINE__,panelInfo->clockFreq);
    hdfDrmPanel->mode.clock = panelInfo->clockFreq / 1000;
    pr_err("==debug %s %d clockFreq %d\n",__FUNCTION__,__LINE__,hdfDrmPanel->mode.clock);
    hdfDrmPanel->mode.hdisplay = panelInfo->width;
    hdfDrmPanel->mode.hsync_start = panelInfo->width + panelInfo->hfp;
    hdfDrmPanel->mode.hsync_end = panelInfo->width + panelInfo->hfp + panelInfo->hsw;
    hdfDrmPanel->mode.htotal = panelInfo->width + panelInfo->hfp + panelInfo->hsw + panelInfo->hbp;
    hdfDrmPanel->mode.vdisplay = panelInfo->height;
    hdfDrmPanel->mode.vsync_start = panelInfo->height + panelInfo->vfp;
    hdfDrmPanel->mode.vsync_end = panelInfo->height + panelInfo->vfp + panelInfo->vsw;
    hdfDrmPanel->mode.vtotal = panelInfo->height + panelInfo->vfp + panelInfo->vsw + panelInfo->vbp;
    hdfDrmPanel->mode.flags = 10;
    hdfDrmPanel->mode.debug = 66;
}

static struct HdfDrmPanel *InstanceHdfDrmPanel(struct DispManager *manager, int32_t index)
{
    struct HdfDrmPanel *hdfDrmPanel = NULL;

    hdfDrmPanel = (struct HdfDrmPanel *)OsalMemCalloc(sizeof(struct HdfDrmPanel));
    if (hdfDrmPanel == NULL) {
        HDF_LOGE("%s hdfDrmPanel malloc fail", __func__);
        return NULL;
    }
    hdfDrmPanel->index = index;
    hdfDrmPanel->manager = manager;
    CreateDrmMode(hdfDrmPanel);
    return hdfDrmPanel;
}

int32_t HdfDrmPanelEntryInit(struct HdfDeviceObject *object)
{
    uint32_t ret,i,j;
    uint32_t panelNum;
    struct HdfDrmPanel *hdfDrmPanel = NULL;
    struct mipi_dsi_device *dsiDev = NULL;
    struct DispManager *manager = NULL;
    struct PanelInfo *panelInfo = NULL;

    manager = GetDispManager();
    if (manager == NULL) {
        HDF_LOGE("%s manager is null", __func__);
        return HDF_FAILURE;
    }
    panelNum = manager->panelManager->panelNum;
    for (i = 0; i < panelNum; i++) {
        hdfDrmPanel = InstanceHdfDrmPanel(manager, i);
        if (hdfDrmPanel == NULL) {
            return HDF_FAILURE;
        }
        panelInfo = hdfDrmPanel->manager->panelManager->panel[i]->info;
        pr_err("==debug %s %d clockFreq--- %d\n",__FUNCTION__,__LINE__,panelInfo->clockFreq);
        pr_err("==debug %s %d clockFreq %d\n",__FUNCTION__,__LINE__,hdfDrmPanel->mode.clock);
         pr_err("==debug %s %d debug %d\n",__FUNCTION__,__LINE__,hdfDrmPanel->mode.debug);

        dsiDev = (struct mipi_dsi_device *)manager->panelManager->panel[i]->priv;
        hdfDrmPanel->panel.dev = &dsiDev->dev;
        drm_panel_init(&hdfDrmPanel->panel,&dsiDev->dev,&g_hdfDrmPanelFuncs,panelInfo->connector_type);
        drm_panel_add(&hdfDrmPanel->panel);
        ret = mipi_dsi_attach(dsiDev);
        if (ret) {
            HDF_LOGE("%s mipi_dsi_attach failed", __func__);
            drm_panel_remove(&hdfDrmPanel->panel);
            return ret;
        }
        mipi_dsi_set_drvdata(dsiDev, hdfDrmPanel);
        /*for (j = 0; j < ATTR_NUM; j++) {
            if (device_create_file(&dsiDev->dev, g_panelAttrs[j]) != 0) {
                HDF_LOGE("%s line = %d device_create_file fail", __func__, __LINE__);
            }
        }*/
        HDF_LOGI("%s panel[%d] registered success", __func__,i );
    }
    HDF_LOGI("%s success", __func__);
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_hdfDrmPanelEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_DRMPANEL",
    .Init = HdfDrmPanelEntryInit,
};

HDF_INIT(g_hdfDrmPanelEntry);
