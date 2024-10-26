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

#ifndef EDP_LINUX_DEF_H
#define EDP_LINUX_DEF_H

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
#ifdef CONFIG_ARCH_ROCKCHIP
    #include <drm/display/drm_dp_helper.h>
#else
    #include <drm/drm_dp_helper.h>
#endif
#include <drm/drm_of.h>
#include <drm/drm_atomic_uapi.h>

#include <drm/drm_crtc.h>
#include <drm/drm_bridge.h>
#include <drm/bridge/analogix_dp.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct HdfEdpAdapterS {
    struct platform_device  *platDev;
    struct device           *dev;
    struct device_node      *node;
    struct drm_device       *drmDev;
    struct drm_bridge       *drmBridge;
    struct drm_panel        *drmPanel;
    struct drm_connector    *connector;
    void                    *driverData;
    void                    *hdfHdl;
}HdfEdpAdapterData;

int32_t LinuxEdpAdapterInit(HdfEdpAdapterData *edpAdapterData);
void LinuxEdpAdapterDeInit(HdfEdpAdapterData *edpAdapterData);
int32_t LinuxEdpEnable(HdfEdpAdapterData *edpAdapterData);
int32_t LinuxEdpDisable(HdfEdpAdapterData *edpAdapterData);
int32_t LinuxEdpDeepColorSet(HdfEdpAdapterData *edpAdapterData, uint32_t color);
int32_t LinuxEdpDeepColorGet(HdfEdpAdapterData *edpAdapterData, uint32_t *color);
int32_t LinuxEdpAvMuteSet(HdfEdpAdapterData *edpAdapterData, bool enable);
int32_t LinuxEdpSetVideoAttribute(HdfEdpAdapterData *edpAdapterData, const void *attr);
int32_t LinuxEdpSetAudioAttribute(HdfEdpAdapterData *edpAdapterData, const void *attr);
int32_t LinuxEdpRegisterHpdCallbackFunc(HdfEdpAdapterData *edpAdapterData, void *hpdcb);
int32_t LinuxEdpUnRegisterHpdCallbackFunc(HdfEdpAdapterData *edpAdapterData);
int32_t LinuxEdpGetConnector(HdfEdpAdapterData *edpAdapterData, void **connector);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //EDP_LINUX_DEF_H