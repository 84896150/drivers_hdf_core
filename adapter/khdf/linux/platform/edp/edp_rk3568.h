#ifndef __EDP_RK3568_H__
#define __EDP_RK3568_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct analogix_dp_plat_data;
struct rockchip_drm_sub_dev;

enum link_lane_count_type {
	LANE_COUNT1 = 1,
	LANE_COUNT2 = 2,
	LANE_COUNT4 = 4
};

enum link_training_state {
	START,
	CLOCK_RECOVERY,
	EQUALIZER_TRAINING,
	FINISHED,
	FAILED
};

enum voltage_swing_level {
	VOLTAGE_LEVEL_0,
	VOLTAGE_LEVEL_1,
	VOLTAGE_LEVEL_2,
	VOLTAGE_LEVEL_3,
};

enum pre_emphasis_level {
	PRE_EMPHASIS_LEVEL_0,
	PRE_EMPHASIS_LEVEL_1,
	PRE_EMPHASIS_LEVEL_2,
	PRE_EMPHASIS_LEVEL_3,
};

enum pattern_set {
	PRBS7,
	D10_2,
	TRAINING_PTN1,
	TRAINING_PTN2,
	TRAINING_PTN3,
	DP_NONE
};

enum color_space {
	COLOR_RGB,
	COLOR_YCBCR422,
	COLOR_YCBCR444
};

enum color_depth {
	COLOR_6,
	COLOR_8,
	COLOR_10,
	COLOR_12
};

enum color_coefficient {
	COLOR_YCBCR601,
	COLOR_YCBCR709
};

enum dynamic_range {
	VESA,
	CEA
};

enum pll_status {
	PLL_UNLOCKED,
	PLL_LOCKED
};

enum clock_recovery_m_value_type {
	CALCULATED_M,
	REGISTER_M
};

enum video_timing_recognition_type {
	VIDEO_TIMING_FROM_CAPTURE,
	VIDEO_TIMING_FROM_REGISTER
};

enum analog_power_block {
	AUX_BLOCK,
	CH0_BLOCK,
	CH1_BLOCK,
	CH2_BLOCK,
	CH3_BLOCK,
	ANALOG_TOTAL,
	POWER_ALL
};

enum dp_irq_type {
	DP_IRQ_TYPE_HP_CABLE_IN  = BIT(0),
	DP_IRQ_TYPE_HP_CABLE_OUT = BIT(1),
	DP_IRQ_TYPE_HP_CHANGE    = BIT(2),
	DP_IRQ_TYPE_UNKNOWN      = BIT(3),
};

struct rockchip_drm_sub_dev {
	struct list_head list;
	struct drm_connector *connector;
	struct device_node *of_node;
	void (*loader_protect)(struct drm_encoder *encoder, bool on);
	void (*oob_hotplug_event)(struct drm_connector *connector);
};

struct video_info {
	char *name;
	struct drm_display_mode mode;

	bool h_sync_polarity;
	bool v_sync_polarity;
	bool interlaced;

	enum color_space color_space;
	enum dynamic_range dynamic_range;
	enum color_coefficient ycbcr_coeff;
	enum color_depth color_depth;

	int max_link_rate;
	enum link_lane_count_type max_lane_count;

	bool video_bist_enable;
};

struct link_train {
	int eq_loop;
	int cr_loop[4];

	u8 link_rate;
	u8 lane_count;
	u8 training_lane[4];
	bool ssc;
	bool enhanced_framing;

	enum link_training_state lt_state;
};

struct analogix_dp_device {
	struct drm_encoder	*encoder;
	struct device		*dev;
	struct drm_device	*drm_dev;
	struct drm_connector	connector;
	struct drm_bridge	bridge;
	struct drm_dp_aux       aux;
	struct clk_bulk_data	*clks;
	int			nr_clks;
	unsigned int		irq;
	void __iomem		*reg_base;

	struct video_info	video_info;
	struct link_train	link_train;
	struct phy		*phy;
	bool			phy_enabled;
	int			dpms_mode;
	struct gpio_desc	*hpd_gpiod;
	bool                    force_hpd;
	bool			fast_train_enable;
	bool			psr_supported;

	struct mutex		panel_lock;
	bool			panel_is_prepared;

	struct analogix_dp_plat_data *plat_data;
};

struct rockchip_grf_reg_field {
	unsigned int reg;
	unsigned int lsb;
	unsigned int msb;
	bool valid;
};

/**
 * struct rockchip_dp_chip_data - splite the grf setting of kind of chips
 * @lcdc_sel: grf register field of lcdc_sel
 * @spdif_sel: grf register field of spdif_sel
 * @i2s_sel: grf register field of i2s_sel
 * @edp_mode: grf register field of edp_mode
 * @chip_type: specific chip type
 * @ssc: check if SSC is supported by source
 * @audio: check if audio is supported by source
 * @split_mode: check if split mode is supported
 */
struct rockchip_dp_chip_data {
	const struct rockchip_grf_reg_field lcdc_sel;
	const struct rockchip_grf_reg_field spdif_sel;
	const struct rockchip_grf_reg_field i2s_sel;
	const struct rockchip_grf_reg_field edp_mode;
	u32	chip_type;
	bool	ssc;
	bool	audio;
	bool	split_mode;
};

struct rockchip_dp_device {
	struct drm_device        *drm_dev;
	struct device            *dev;
	struct drm_encoder       encoder;
	struct drm_bridge	 *bridge;
	struct drm_display_mode  mode;

	struct regmap            *grf;
	struct reset_control     *rst;
	struct reset_control     *apb_reset;

	struct platform_device *audio_pdev;
	const struct rockchip_dp_chip_data *data;
	int id;

	struct analogix_dp_device *adp;
	struct analogix_dp_plat_data plat_data;
	struct rockchip_drm_sub_dev sub_dev;
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //__EDP_RK3568_H__