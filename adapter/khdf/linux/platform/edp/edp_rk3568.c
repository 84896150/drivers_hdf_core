#include "hdf_base.h"
#include "hdf_log.h"

#include "edp_linux_def.h"
#include "edp_rk3568.h"

#define HDF_LOG_TAG edp_rk3568_hdf

static int32_t EdpBpcToColorDepth(uint32_t bpc, uint32_t*color)
{
    int32_t ret = -1;
    switch(bpc) {
    case COLOR_6:
        *color = 6;
        break;
    case COLOR_8:
        *color = 8;
        break;
    case COLOR_10:
        *color = 10;
        break;
    case COLOR_12:
        *color = 12;
        break;
    default:
        return ret;
    }
    ret = 0;
    return ret;
}

static int32_t EdpColorDepthToBpc(uint32_t *bpc, uint32_t color )
{
    int32_t ret = -1;
    switch(color) {
    case 6:
        *bpc = COLOR_6;
        break;
    case 8:
        *bpc = COLOR_8;
        break;
    case 10:
        *bpc = COLOR_10;
        break;
    case 12:
        *bpc = COLOR_12;
        break;
    default:
        return ret;
    }
    ret = 0;
    return ret;
}

int32_t LinuxEdpAdapterInit(hdf_edp_adapter_t *edpAdapterData)
{
    int32_t ret = -1;
    if (NULL == edpAdapterData) {
        HDF_LOGE("%s null adapter get\n", __func__);
        return ret;
    }
    struct drm_device *edpDrmDev = NULL;
    struct drm_panel *edpDrmPanel = NULL;
    struct drm_bridge *edpDrmBridge = NULL;
    struct device * edpDev = NULL;
    struct drm_mode_config *drmConfig = NULL;
    struct EdpCntlrOps *ops = NULL;
    struct device_node *edpNode = NULL;
    struct platform_device *platDev = NULL;
    struct device_node *dispDevNode = NULL;
    void * privData = NULL;
    // edpNode = of_find_compatible_node(edpNode, NULL, "rockchip,rk3568-edp");
    edpNode = of_find_node_by_path("/edp@fe0c0000");
    if (NULL == edpNode) {
        HDF_LOGE("find edp dt node failed\n");
        return ret;
    }
    HDF_LOGI("find edp dt node!!dev is: %x\n", edpNode->fwnode.dev);
    platDev = of_find_device_by_node(edpNode);
    if (NULL == platDev) {
        HDF_LOGE("find platform device of edp dt node failed\n");
        return ret;
    }
    privData = platform_get_drvdata(platDev);
    edpDrmBridge = &((struct rockchip_dp_device *)privData)->adp->bridge;
    HDF_LOGI("of_find_device_by_node: %x, dev: %x, priv: %x, bridge: %x\n", platDev, platDev->dev.of_node, privData, edpDrmBridge);
    
    dispDevNode = of_find_node_by_path("/display-subsystem");
    if (NULL == dispDevNode) {
        HDF_LOGE("find display-subsystem node failed\n");
        return ret;
    }
    HDF_LOGI("of_find_node_by_path/display-subsystem: %x\n", dispDevNode);
    HDF_LOGI("drm_of_find_panel_or_bridge(%x, %d, %d, %x, %x)!!\n", edpNode, 1, -1, &edpDrmPanel, &edpDrmBridge);
    ret = drm_of_find_panel_or_bridge(edpNode, 1, -1, &edpDrmPanel, &edpDrmBridge);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("drm_of_find_panel_or_bridge failed: %d\n", ret);
        return ret;
    }

    HDF_LOGI("drm_of_find_panel_or_bridge returned: panel: %x bridge: %x!!\n", edpDrmPanel, edpDrmBridge);
    if (!edpDrmBridge) {
        edpDrmBridge = &((struct rockchip_dp_device *)privData)->adp->bridge;
        HDF_LOGI("of_find_device_by_node: %x, dev: %x, priv: %x, bridge: %x\n", platDev, platDev->dev.of_node, privData, edpDrmBridge);
        #if 0
        // edpDrmBridge = of_drm_find_bridge(dispDevNode);
        // if (!edpDrmBridge) {
        ret = drm_of_find_panel_or_bridge(dispDevNode, 1, -1, NULL, &edpDrmBridge);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("of_drm_find_bridge failed %d\n", ret);
            return ret;
        }
        #endif
    }

    HDF_LOGI("drm_of_find_panel_or_bridge returned: panel: %x bridge: %x!!\n", edpDrmPanel, edpDrmBridge);
    if (edpDrmBridge) {
        edpDrmDev = edpDrmBridge->dev;
        edpAdapterData->connector = drm_panel_bridge_connector(edpDrmBridge);
        HDF_LOGI("drm_panel_bridge_connector %x!!\n", edpAdapterData->connector);
        if (!edpAdapterData->connector) {
            HDF_LOGE("drm_panel_bridge_connector failed\n");
            return -1;
        }
    }
    if (edpDrmDev) {
        HDF_LOGI("Assigning adapter edpDrmDev data %x!!\n", edpDrmDev);
        edpDev = edpDrmDev->dev;
    }

    edpAdapterData->platDev = platDev;
    edpAdapterData->dev = edpDev;
    edpAdapterData->node = edpNode;
    edpAdapterData->drmDev = edpDrmDev;
    edpAdapterData->drmPanel = edpDrmPanel;
    edpAdapterData->drmBridge = edpDrmBridge;
    edpAdapterData->connector = &((struct rockchip_dp_device *)privData)->adp->connector;
    // edpAdapterData->driverData = dev_get_drvdata(edpDev);

    ret = HDF_SUCCESS;
    return ret;
}

void LinuxEdpAdapterDeInit(hdf_edp_adapter_t *edpAdapterData)
{
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return;
    }
}

int32_t LinuxEdpDeepColorSet(hdf_edp_adapter_t* edpAdapterData, uint32_t color)
{
    int32_t ret = -1;
    if (edpAdapterData == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }
    struct drm_display_mode mode;
    struct drm_display_mode adjustedMode;
    struct drm_display_info *info = &edpAdapterData->connector->display_info;
    struct drm_atomic_state oldState; 
    HDF_LOGI("drm_mode_copy() %x!!\n", edpAdapterData->connector);
    drm_mode_copy(&mode, &edpAdapterData->connector->encoder->crtc->state->mode);
    drm_mode_copy(&adjustedMode, &mode);

    //modify video bpc to setup color depth by edp driver
    ret = EdpColorDepthToBpc(&info->bpc, color);
    if (ret != 0) {
        return ret;
    }

    HDF_LOGI("drm_atomic_bridge_chain_disable()!!\n");
    // drm_atomic_bridge_chain_disable(e000dpAdapterData->drmBridge, &oldState);
    HDF_LOGI("drm_bridge_chain_mode_set()!!\n");
    drm_bridge_chain_mode_set(edpAdapterData->drmBridge, &mode, &adjustedMode);
    HDF_LOGI("drm_atomic_bridge_chain_enable()!!\n");
    // drm_atomic_bridge_chain_enable(edpAdapterData->drmBridge, &oldState);

    ret = HDF_SUCCESS;
    return ret;
}

int32_t LinuxEdpDeepColorGet(hdf_edp_adapter_t* edpAdapterData, uint32_t* color)
{
    int32_t ret = -1;
    if (edpAdapterData == NULL || color == NULL) {
        HDF_LOGE("%s, edp priv data is null", __func__);
        return ret;
    }
    struct rockchip_dp_device *drvData = platform_get_drvdata(edpAdapterData->platDev);

    if (drvData == NULL) {
        
    }
    HDF_LOGI("drm_display_info %x!!\n", edpAdapterData->connector);
    drvData->plat_data.get_modes(&drvData->plat_data, edpAdapterData->connector);
    struct drm_display_info *info = &edpAdapterData->connector->display_info;
    ret = EdpBpcToColorDepth(info->bpc, color);

    return ret;
}

int32_t LinuxEdpSetVideoAttribute(hdf_edp_adapter_t *edpAdapterData, void *videoAttr)
{
    int32_t ret = -1;
    if (edpAdapterData == NULL || videoAttr == NULL) {
        HDF_LOGE("%s, null parameters got", __func__);
        return ret;
    }

    return ret;
}