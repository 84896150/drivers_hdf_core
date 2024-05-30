#ifndef __EDP_LINUX_DEF_H__
#define __EDP_LINUX_DEF_H__

#include <drm/drm_device.h>
#include <drm/drm_atomic_helper.h>
#include <linux/backlight.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/pm_runtime.h>
#include <linux/device.h>
#include <linux/regulator/consumer.h>
#include <drm/drm_connector.h>
#include <drm/drm_dp_helper.h>
#include <drm/drm_of.h>

#include <drm/drm_crtc.h>
#include <drm/drm_bridge.h>
#include <drm/drm_dp_helper.h>
#include <drm/bridge/analogix_dp.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct hdf_edp_adapter_s {
    struct platform_device  *platDev;
    struct device           *dev;
    struct device_node      *node;
    struct drm_device       *drmDev;
    struct drm_bridge       *drmBridge;
    struct drm_panel        *drmPanel;
    struct drm_connector    *connector;
    void                    *driverData;
}hdf_edp_adapter_t;

int32_t LinuxEdpAdapterInit(hdf_edp_adapter_t *);
void LinuxEdpAdapterDeInit(hdf_edp_adapter_t *);
int32_t LinuxEdpDeepColorSet(hdf_edp_adapter_t*, uint32_t);
int32_t LinuxEdpDeepColorGet(hdf_edp_adapter_t*, uint32_t*);
int32_t LinuxEdpSetVideoAttribute(hdf_edp_adapter_t*, void*);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //__EDP_LINUX_DEF_H__