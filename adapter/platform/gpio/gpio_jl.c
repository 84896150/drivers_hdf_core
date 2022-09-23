/*
 * Copyright (c) 2022 Jiangsu Hoperun Software Co., Ltd.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include "gpio_core.h"
#include "gpio_if.h"
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#else
#include "device_resource_if.h"
#endif
#include "osal_irq.h"
#include "hdf_log.h"
#include "gpio.h"

#define JL_GROUP_MAX 4
#define JL_BIT_MAX   16
#define JL_IO_MAX_GPIO_PIN_NUM 48
#define JL_GPIO_PRI  3
#define JL_IOMAP_CON2  0b111111

static uint32_t g_userRealPin = -1;

struct GpioResource {
    uint32_t groupNum;
    uint32_t iogroupNum;
    uint32_t pinNum;
    uint32_t realPin;
};

struct GpioDevice {
    uint8_t port; /* gpio port */
    struct GpioResource resource;
};

static struct GpioCntlr g_gpioCntlr;

___interrupt

GpioIrqFunc GpioIrqHdl()
{
    if (JL_WAKEUP->CON3 & BIT(0)) {
        JL_WAKEUP->CON2 |= BIT(0) ;
        GpioCntlrIrqCallback(&g_gpioCntlr, g_userRealPin);
    }
}

/* GpioMethod method definitions */
static int32_t GpioDevWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val);
static int32_t GpioDevRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val);
static int32_t GpioDevSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir);
static int32_t GpioDevSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode);
static int32_t GpioDevUnSetIrq(struct GpioCntlr *cntlr, uint16_t gpio);
static int32_t GpioDevEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio);
static int32_t GpioDevDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio);

/* GpioMethod definitions */
static struct GpioMethod g_GpioCntlrMethod = {
    .request = NULL,
    .release = NULL,
    .write = GpioDevWrite,
    .read = GpioDevRead,
    .setDir = GpioDevSetDir,
    .getDir = NULL,
    .toIrq = NULL,
    .setIrq = GpioDevSetIrq,
    .unsetIrq = GpioDevUnSetIrq,
    .enableIrq = GpioDevEnableIrq,
    .disableIrq = GpioDevDisableIrq,
};

/* dev api */
static int32_t GpioDevWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val)
{
    (void)cntlr;
    uint32_t ret = gpio_direction_output(gpio, (uint32_t)val);
    if (ret != 0) {
        HDF_LOGE("%s gpio_write failed", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

static int32_t GpioDevRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
    (void)cntlr;
    if (val == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint16_t value = gpio_read(gpio);
    *val = value;
    return HDF_SUCCESS;
}

static int32_t GpioDevSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
    (void)cntlr;
    int ret;
    int32_t direction;
    switch (dir) {
        case GPIO_DIR_OUT:
            direction = 0;
            break;
        case GPIO_DIR_IN:
            direction = 1;
            break;
        default:
            direction = -1;
            break;
    }
    ret = gpio_set_direction(gpio, direction);
    if (ret != 0) {
        HDF_LOGE("%s gpio_set_direction failed", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode)
{
    (void)cntlr;
    uint8_t trigger_edge;

    switch (mode) {
        case OSAL_IRQF_TRIGGER_RISING:
            trigger_edge = 0;
            break;
        case OSAL_IRQF_TRIGGER_FALLING:
            trigger_edge = 1;
            break;
        default:
            trigger_edge = -1;
            break;
    }

    JL_WAKEUP->CON0 &= ~BIT(0);
    gpio_set_die(gpio, 1);
    gpio_set_direction(gpio, 1);
    if (trigger_edge == 0) {
        JL_WAKEUP->CON1 &= ~BIT(0);
        gpio_set_pull_up(gpio, 0);
        gpio_set_pull_down(gpio, 1);
    } else {
        JL_WAKEUP->CON1 |= BIT(0);
        gpio_set_pull_up(gpio, 1);
        gpio_set_pull_down(gpio, 0);
    }

    g_userRealPin = gpio;
    request_irq(IRQ_PORT_IDX, JL_GPIO_PRI, (void *)GpioIrqHdl, 0);
    JL_IOMAP->CON2 &= ~(JL_IOMAP_CON2 << 0);
    JL_IOMAP->CON2 |= (gpio << 0);
    JL_WAKEUP->CON2 |= BIT(0);
    JL_WAKEUP->CON0 |= BIT(0);
    return HDF_SUCCESS;
}

static int32_t GpioDevUnSetIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;

    JL_WAKEUP->CON0 &= ~BIT(0);
    gpio_set_die(gpio, 0);
    gpio_set_direction(gpio, 1);
    gpio_set_pull_up(gpio, 0);
    gpio_set_pull_down(gpio, 0);
    return HDF_SUCCESS;
}

static int32_t GpioDevEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    (void)gpio;
    irq_enable(IRQ_PORT_IDX);
    return HDF_SUCCESS;
}

static int32_t GpioDevDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    (void)gpio;
    irq_disable(IRQ_PORT_IDX);
    return HDF_SUCCESS;
}

#define root_device_info_match_attr "hdf_manager"

#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#define PLATFORM_GPIO_CONFIG HCS_NODE(HCS_NODE(HCS_ROOT, platform), gpio_config)
static uint32_t GetGpioDeviceResource(struct GpioDevice *device)
{
    struct GpioResource *resource = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s: resource is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    HDF_LOGD("%s", HCS_PROP(HCS_NODE(HCS_ROOT, device_info), match_attr));
    return HDF_SUCCESS;
}
#else
static uint32_t GetGpioDeviceResource(struct GpioDevice *device,
                                      const struct DeviceResourceNode *resourceNode)
{
    struct GpioResource *resource = NULL;
    struct DeviceResourceIface *dri = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s: resource is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("%s: DeviceResourceIface is invalid", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (dri->GetUint32(resourceNode, "groupNum", &resource->groupNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: gpio config read groupNum fail", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: resource->groupNum %u\n", __func__, resource->groupNum);

    if (dri->GetUint32(resourceNode, "iogroupNum", &resource->iogroupNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: gpio config read iogroupNum fail", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: resource->iogroupNum %u\n", __func__, resource->iogroupNum);

    if (dri->GetUint32(resourceNode, "pinNum", &resource->pinNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: gpio config read pinNum fail", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: resource->pinNum %u\n", __func__, resource->pinNum);

    return HDF_SUCCESS;
}
#endif

static int32_t AttachGpioDevice(struct GpioCntlr *gpioCntlr, struct HdfDeviceObject *device)
{
    HDF_LOGD("%s: entry!\n", __func__);
    int32_t ret;

    struct GpioDevice *gpioDevice = NULL;
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    if (device == NULL) {
#else
    if (device == NULL || device->property == NULL) {
#endif
        HDF_LOGE("%s: property is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    gpioDevice = (struct GpioDevice *)OsalMemAlloc(sizeof(struct GpioDevice));
    if (gpioDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc gpioDevice error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    ret = GetGpioDeviceResource(gpioDevice);
#else
    ret = GetGpioDeviceResource(gpioDevice, device->property);
#endif
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(gpioDevice);
        return HDF_FAILURE;
    }

    gpioCntlr->count = gpioDevice->resource.pinNum;
    gpioCntlr->priv = (void *)gpioDevice;
    return HDF_SUCCESS;
}

static int32_t GpioDriverInit(struct HdfDeviceObject *device)
{
    HDF_LOGD("%s: entry!\n", __func__);

    int32_t ret;
    struct GpioCntlr *gpioCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = PlatformDeviceBind(&g_gpioCntlr.device, device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: bind hdf device failed:%d", __func__, ret);
        return ret;
    }

    gpioCntlr = GpioCntlrFromHdfDev(device);
    if (gpioCntlr == NULL) {
        HDF_LOGE("%s: GpioCntlrFromHdfDev fail\r\n", __func__);
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }

    ret = AttachGpioDevice(gpioCntlr, device); // GpioCntlr add GpioDevice to priv
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AttachGpioDevice fail\r\n", __func__);
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }

    gpioCntlr->ops = &g_GpioCntlrMethod; // register callback
    ret = GpioCntlrAdd(gpioCntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: GpioCntlrAdd fail %hu\r\n", __func__, gpioCntlr->start);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDriverBind(struct HdfDeviceObject *device)
{
    (void *)device;
    return HDF_SUCCESS;
}

static void GpioDriverRelease(struct HdfDeviceObject *device)
{
    HDF_LOGD("%s: entry!\n", __func__);
    struct GpioCntlr *gpioCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return;
    }

    gpioCntlr = GpioCntlrFromHdfDev(device);
    if (gpioCntlr == NULL) {
        HDF_LOGE("%s: gpioCntlr is NULL", __func__);
        return;
    }

    gpioCntlr->ops = NULL;
    OsalMemFree(gpioCntlr);
}

/* HdfDriverEntry definitions */
static struct HdfDriverEntry g_GpioDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "JL_HDF_PLATFORM_GPIO",
    .Init = GpioDriverInit,
    .Bind = GpioDriverBind,
    .Release = GpioDriverRelease,
};
HDF_INIT(g_GpioDriverEntry);
