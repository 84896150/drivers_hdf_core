#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "pwm_core.h"
#include "pwm_esp32.h"
#include "pwm_if.h"
#include "ledc.h"

#define PULSE_IO 9
#define PCNT_INPUT_IO 4
#define PCNT_CTRL_FLOATING_IO 5
#define HIGHEST_LIMIT 10000
#define LOWEST_LIMIT -10000

static ledc_channel_config_t initialize_channel_config(unsigned char num, uint32_t duty)
{
    ledc_channel_config_t config;
    memset(&config, 0, sizeof(ledc_channel_config_t));
    config.gpio_num = PULSE_IO;
    config.speed_mode = LEDC_LOW_SPEED_MODE;
    config.channel  = num;
    config.intr_type = LEDC_INTR_DISABLE;
    config.timer_sel = LEDC_TIMER_0;
    config.duty = duty;
    config.hpoint = 0;
    return config;
}

static ledc_timer_config_t create_default_timer_config(uint32_t freq)
{
    ledc_timer_config_t ledc_time_config;
    memset(&ledc_time_config, 0, sizeof(ledc_timer_config_t));
    ledc_time_config.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_time_config.duty_resolution = LEDC_TIMER_13_BIT;
    ledc_time_config.timer_num = LEDC_TIMER_0;
    ledc_time_config.freq_hz = 2000;
    ledc_time_config.clk_cfg = LEDC_USE_APB_CLK;
    return ledc_time_config;
}

#define HDF_LOG_TAG pwm_esp32

int32_t Esp32PwmOpen(struct PwmDev *pwm)
{
    if (pwm == NULL) {
        HDF_LOGE("%s: pwm is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ledc_channel_config_t ledc_ch_config = initialize_channel_config(pwm->num, 4000);
    ledc_timer_config_t ledc_time_config = create_default_timer_config(2000);

    ledc_timer_config(&ledc_time_config);
    ledc_channel_config(&ledc_ch_config);

    pwm->cfg.period = ledc_time_config.freq_hz;
    pwm->cfg.duty = ledc_ch_config.duty;
    pwm->cfg.polarity = PWM_NORMAL_POLARITY;
    pwm->cfg.status = PWM_ENABLE_STATUS;

    return HDF_SUCCESS;
}

int32_t Esp32PwmClose(struct PwmDev *pwm)
{
    if (pwm == NULL) {
        HDF_LOGE("%s: pwm is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ledc_stop(LEDC_LOW_SPEED_MODE, pwm->num, 0);

    return HDF_SUCCESS;
}

int32_t Esp32PwmSetConfig(struct PwmDev *pwm, struct PwmConfig *config)
{
    if (pwm == NULL || config == NULL) {
        HDF_LOGE("%s: pwm | config is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (config->status == PWM_DISABLE_STATUS) {
        ledc_stop(LEDC_LOW_SPEED_MODE, pwm->num, 0);
    } else {
        ledc_channel_config_t ledc_ch_config = initialize_channel_config(pwm->num, config->duty);
        ledc_timer_config_t ledc_time_config = create_default_timer_config(config->period);

        ledc_timer_config(&ledc_time_config);
        ledc_channel_config(&ledc_ch_config);
    }
    return HDF_SUCCESS;
}

struct PwmMethod g_pwm_method = {
    .setConfig = Esp32PwmSetConfig,
    .open = Esp32PwmOpen,
    .close = Esp32PwmClose,
};

/*-------------------------------------------------------------*/
/* Init and DeInit Function */
/*-------------------------------------------------------------*/
static int32_t Esp32PwmBind(struct HdfDeviceObject *obj)
{
    (void)obj;
    return HDF_SUCCESS;
}

static int32_t Esp32PwmInit(struct HdfDeviceObject *obj)
{
    int32_t ret;
    uint32_t num;
    struct PwmDev *pwm = NULL;
    struct DeviceResourceIface *iface = NULL;

    HDF_LOGI("%s: entry", __func__);
    if (obj == NULL) {
        HDF_LOGE("%s: obj is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (iface == NULL || iface->GetUint32 == NULL) {
        HDF_LOGE("%s: face is invalid", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint32(obj->property, "num", &num, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read num fail", __func__);
        return HDF_FAILURE;
    }
    pwm = (struct PwmDev *)OsalMemCalloc(sizeof(*pwm));
    if (pwm == NULL) {
        HDF_LOGE("%s: OsalMemCalloc pwm error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    pwm->cfg.number = 0;
    pwm->num = num;
    pwm->method = &g_pwm_method;
    pwm->busy = false;
    ret = PwmDeviceAdd(obj, pwm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: error probe, ret is %d", __func__, ret);
        OsalMemFree(pwm);
    }
    HDF_LOGE("%s: end", __func__);
    return ret;
}

static void Esp32PwmRelease(struct HdfDeviceObject *obj)
{
    struct PwmDev *pwm = NULL;

    HDF_LOGI("%s: entry", __func__);
    if (obj == NULL) {
        HDF_LOGE("%s: obj is null", __func__);
        return;
    }
    pwm = (struct PwmDev *)obj->service;
    if (pwm == NULL) {
        HDF_LOGE("%s: pwm is null", __func__);
        return;
    }
    PwmDeviceRemove(obj, pwm);
    OsalMemFree(pwm);
}

struct HdfDriverEntry g_pwm_driver_entry = {
    .moduleVersion = 1,
    .Bind = Esp32PwmBind,
    .Init = Esp32PwmInit,
    .Release = Esp32PwmRelease,
    .moduleName = "esp32_pwm_driver",
};

HDF_INIT(g_pwm_driver_entry);