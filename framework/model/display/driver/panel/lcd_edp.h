#ifndef __LCD_EDP_H__
#define __LCD_EDP_H__

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
    uint32_t prepare_delay;
    uint32_t hpd_absent_delay;
    uint32_t enable_delay;
    uint32_t disable_delay;
    uint32_t unprepare_delay;
    uint32_t reset_delay;
    uint32_t init_delay;
};

struct panel_edp_dev {
    bool power_invert;
    struct PanelData panel;
    struct device *edpDev;
    struct regulator *supply;
    struct gpio_desc *enable_gpio;
    struct gpio_desc *reset_gpio;
    struct gpio_desc *hpd_gpio;
    struct panel_edp_hw_delay hw_delay;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif //__LCD_EDP_H__