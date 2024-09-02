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

#ifndef LCD_EDP_H
#define LCD_EDP_H

#include <drm/drm_mipi_dsi.h>
#include <uapi/drm/drm_mode.h>
#include <drm/drm_modes.h>
#include <linux/backlight.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio/consumer.h>
#include "hdf_disp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

struct panel_edp_hw_delay {
    uint32_t prepareDelay;
    uint32_t hpdAbsentDelay;
    uint32_t enableDelay;
    uint32_t disableDelay;
    uint32_t unprepareDelay;
    uint32_t resetDelay;
    uint32_t initDelay;
};

struct panel_edp_dev {
    bool powerInvert;
    struct PanelData panel;
    struct device *edpDev;
    struct regulator *supply;
    struct gpio_desc *enableGpio;
    struct gpio_desc *resetGpio;
    struct gpio_desc *hpdGpio;
    struct panel_edp_hw_delay hwDelay;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif //LCD_EDP_H