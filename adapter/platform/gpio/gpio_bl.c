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
#include "device_resource_if.h"
#include "osal_irq.h"
#include "hdf_log.h"
#include "bl602_common.h"
#include "bl602_gpio.h"
#include "bl602_glb.h"
#include "bl_gpio.h"

static struct GpioCntlr g_gpioCntlr;
struct GpioResource {
    uint32_t pinNum;
};

struct GpioDevice {
    uint8_t port; /* gpio port */
    struct GpioResource resource;
};

static void gpio_irq_callback(gpio_ctx_t *pstnode)
{
//重新打开中断
    bl_gpio_intmask(pstnode->gpioPin,0);

    printf("gpio_irq_callback\r\n");
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
struct GpioMethod g_GpioCntlrMethod = {
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
    BL_Err_Type ret = GLB_GPIO_Write((GLB_GPIO_Type)gpio, (uint32_t)val);
    if(ret != SUCCESS) {
        HDF_LOGE("%s gpio_write failed", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

static int32_t GpioDevRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
    (void)cntlr;
    uint32_t value = GLB_GPIO_Read((GLB_GPIO_Type)gpio);
    *val = value;
    return HDF_SUCCESS;
}

static int32_t GpioDevSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
    (void)cntlr;
    int32_t ret;
    if(dir == GPIO_DIR_OUT) {
        ret = bl_gpio_enable_output((uint8_t)gpio, 0, 0);
    } else if(dir == GPIO_DIR_IN){
        ret = bl_gpio_enable_input((uint8_t)gpio, 0, 0);
    } else {
        return HDF_ERR_NOT_SUPPORT;
    }

    if(ret != 0) {
        HDF_LOGE("%s gpiosetdir failed", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

#define GPIP_INT_STATE_OFFSET    (0x1a8)
static int check_gpio_is_interrupt(int gpioPin)
{
    int bitcount = 0;
    int reg_val = 0;

    bitcount = 1 << gpioPin;
    reg_val = *(int32_t *)(GLB_BASE + GPIP_INT_STATE_OFFSET);

    if ((bitcount & reg_val) == bitcount) {
        return 0;
    }
    return -1;
}

static int exec_gpio_handler(gpio_ctx_t *pstnode)
{
    bl_gpio_intmask(pstnode->gpioPin, 1);

    if (pstnode->gpio_handler) {
        pstnode->gpio_handler(pstnode);
        return 0;
    }

    return -1;
}

static void gpio_interrupt_entry(gpio_ctx_t *pstnode)
{
    int ret;

    while (pstnode) {
        ret = check_gpio_is_interrupt(pstnode->gpioPin);
        if (ret == 0) {
            exec_gpio_handler(pstnode);
        }

        pstnode = pstnode->next;
    }
    return;
}

static int32_t GpioDevSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode)
{
    (void)cntlr;

    gpio_ctx_t pstnode;
    switch(mode) {
        case OSAL_IRQF_TRIGGER_RISING:
           pstnode.intTrgMod = GLB_GPIO_INT_TRIG_POS_LEVEL;  //上升沿触发
           break;
        case OSAL_IRQF_TRIGGER_FALLING:
           pstnode.intTrgMod = GLB_GPIO_INT_TRIG_NEG_LEVEL;  //下降沿触发
           break;
         default:
           break;
    }
    pstnode.gpioPin = gpio;
    pstnode.intCtrlMod = GLB_GPIO_INT_CONTROL_ASYNC;
    pstnode.gpio_handler = (void *)&gpio_irq_callback;
    pstnode.arg = NULL;
    pstnode.next = NULL;

    bl_gpio_intmask(&pstnode.gpioPin, 1);
    bl_set_gpio_intmod(&pstnode.gpioPin, &pstnode.intCtrlMod, &pstnode.intTrgMod);
    bl_irq_register_with_ctx(GPIO_INT0_IRQn, gpio_interrupt_entry, &pstnode);
    bl_gpio_intmask(&pstnode.gpioPin, 0);

    return HDF_SUCCESS;
}

static int32_t GpioDevUnSetIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    bl_gpio_int_clear(gpio, SET);
    bl_irq_unregister(GPIO_INT0_IRQn, (void *)&gpio_irq_callback);
    return HDF_SUCCESS;
}

static int32_t GpioDevEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    bl_irq_enable(GPIO_INT0_IRQn);
    return HDF_SUCCESS;
}

static int32_t GpioDevDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    bl_irq_disable(GPIO_INT0_IRQn);
    return HDF_SUCCESS;
}

static uint32_t GetGpioDeviceResource(struct GpioDevice *device,
    const struct DeviceResourceNode *resourceNode)
{
    printf("%s: entry!\n", __func__);
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
        HDF_LOGE("DeviceResourceIface is invalid");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (dri->GetUint32(resourceNode, "pinNum", &resource->pinNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("gpio config read pinNum fail");
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: resource->pinNum %d\n", __func__, resource->pinNum);
    printf("%s: HDF_SUCCESS!\n", __func__);
    return HDF_SUCCESS;
}

static int32_t AttachGpioDevice(struct GpioCntlr *gpioCntlr, struct HdfDeviceObject *device)
{
    printf("%s: entry!\n", __func__);
    int32_t ret;

    struct GpioDevice *gpioDevice = NULL;

    if (device == NULL || device->property == NULL) {

        HDF_LOGE("%s: property is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    gpioDevice = (struct GpioDevice *)OsalMemAlloc(sizeof(struct GpioDevice));
    if (gpioDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc gpioDevice error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = GetGpioDeviceResource(gpioDevice, device->property);
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
    printf("%s: entry!\n", __func__);
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

    printf("%s: GpioCntlrFromHdfDev!\n", __func__);
    gpioCntlr = GpioCntlrFromHdfDev(device);
    if (gpioCntlr == NULL) {
        HDF_LOGE("GpioCntlrFromHdfDev fail\r\n");
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }
    printf("%s: AttachGpioDevice!\n", __func__);
    ret = AttachGpioDevice(gpioCntlr, device); // GpioCntlr add GpioDevice to priv
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("AttachGpioDevice fail\r\n");
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }

    gpioCntlr->ops = &g_GpioCntlrMethod; // register callback
    printf("%s: GpioCntlrAdd!\n", __func__);
    ret = GpioCntlrAdd(gpioCntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioCntlrAdd fail %d\r\n", gpioCntlr->start);
        return HDF_FAILURE;
    }
    printf("%s: HDF_SUCCESS!\n", __func__);	
    return HDF_SUCCESS;
}

static int32_t GpioDriverBind(struct HdfDeviceObject *device)
{
    printf("%s: entry!\n", __func__);
    (void *)device;
    return HDF_SUCCESS;
}

static void GpioDriverRelease(struct HdfDeviceObject *device)
{
    struct GpioCntlr *gpioCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return;
    }

    gpioCntlr = GpioCntlrFromHdfDev(device);
    if (gpioCntlr == NULL) {
        HDF_LOGE("%s: host is NULL", __func__);
        return;
    }

    gpioCntlr->ops = NULL;
    OsalMemFree(gpioCntlr);
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_GpioDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "BL_GPIO_MODULE_HDF",
    .Bind = GpioDriverBind,
    .Init = GpioDriverInit,
    .Release = GpioDriverRelease,
};
HDF_INIT(g_GpioDriverEntry);
