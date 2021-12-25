#include "device_resource_if.h"
#include "gpio_core.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_irq.h"
#include "osal_mem.h"
#include "osal_spinlock.h"
#include "plat_log.h"
#include "gpio.h"
#include "gpio_if.h"
#include "gpio_hal.h"
#include "gpio_types.h"
#include "portmacro.h"

#define HDF_LOG_TAG gpio_esp32
#define GPIO_ISR_CORE_ID_UNINIT    (3)

struct Esp32GpioGroup {
    volatile unsigned char *regBase;
    unsigned int index;
    unsigned int irq;
    OsalIRQHandle irqFunc;
    OsalSpinlock lock;
};

struct Esp32GpioCntlr {
    struct GpioCntlr cntlr;
    volatile unsigned char *regBase;
    uint32_t phyBase;
    uint32_t regStep;
    uint32_t irqStart;
    uint16_t groupNum;
    uint16_t bitNum;
    uint8_t irqShare;
    struct Esp32GpioGroup *groups;
};
static struct Esp32GpioCntlr g_esp32_gpio;

static unsigned char gpio_direct[22];
/*-------------------------------------------------------------*/
/* Hdi Interface */
/*-------------------------------------------------------------*/

typedef struct {
    gpio_isr_t fn;   /*!< isr function */
    void *args;      /*!< isr function args */
} gpio_isr_func_t;

typedef struct {
    gpio_hal_context_t *gpio_hal;
    portMUX_TYPE gpio_spinlock;
    uint32_t isr_core_id;
    gpio_isr_func_t *gpio_isr_func;
    gpio_isr_handle_t gpio_isr_handle;
} gpio_context_t;

static gpio_hal_context_t _gpio_hal = {
    .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
};

static gpio_context_t gpio_context = {
    .gpio_hal = &_gpio_hal,
    .gpio_spinlock = portMUX_INITIALIZER_UNLOCKED,
    .isr_core_id = GPIO_ISR_CORE_ID_UNINIT,
    .gpio_isr_func = NULL,
};

static int32_t Esp32GpioWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val) 
{
    if (cntlr == NULL || cntlr->priv == NULL) {
        HDF_LOGE("%s: cntlr or priv is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    gpio_set_level(gpio,val);

    return HDF_SUCCESS;
}

static int32_t Esp32GpioRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
    int valCur;

    if (cntlr == NULL || cntlr->priv == NULL) {
        HDF_LOGE("%s: cntlr or priv is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    valCur = gpio_get_level(gpio);
    if (valCur == 1) {
        *val = GPIO_VAL_HIGH;
    } else if (valCur == 0 ){
        *val = GPIO_VAL_LOW;
    }

    return HDF_SUCCESS;
}

static int32_t Esp32GpioSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
    int mode ;

    HDF_LOGE("%s: start", __func__);
    if (cntlr == NULL || cntlr->priv == NULL) {
        HDF_LOGE("%s: cntlr or priv is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
     if (dir == GPIO_DIR_IN) {
        mode = GPIO_MODE_INPUT;
    } else if (dir == GPIO_DIR_OUT ){
        mode = GPIO_MODE_OUTPUT;
    }
    gpio_direct[gpio] = dir;
    gpio_set_direction(gpio, mode);
    HDF_LOGE("%s: end", __func__);

    return HDF_SUCCESS;
}

static int32_t Esp32GpioGetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir) 
{
    unsigned char direct;

    HDF_LOGE("%s: start", __func__);
    if (cntlr == NULL || cntlr->priv == NULL) {
        HDF_LOGE("%s: cntlr or priv is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    *dir = gpio_direct[gpio];
    HDF_LOGE("%s: end", __func__);

    return HDF_SUCCESS;
}

struct GpioTester {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*doTest)(struct GpioTester *tester, int32_t cmd);
    uint16_t gpio;
    uint16_t gpioIrq;
    uint16_t oldDir;
    uint16_t oldVal;
    uint16_t irqCnt;
    uint16_t total;
    uint16_t fails;
    uint32_t irqTimeout;
};

static int32_t TestESP32GpioIrqHandler(void *data)
{
    struct GpioTester *tester = (struct GpioTester *)data;

    if (tester != NULL) {
        tester->irqCnt++;
        return 0;
    }

    return HDF_FAILURE;
}

static int32_t Esp32GpioSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode,
    GpioIrqFunc func, void *arg)
{
    (void)func;
    int ret;

    if (cntlr == NULL || cntlr->priv == NULL) {
        HDF_LOGE("%s: cntlr or priv is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (mode == GPIO_IRQ_TRIGGER_RISING) {
        ret = GPIO_INTR_POSEDGE;
    } else if (mode == GPIO_IRQ_TRIGGER_FALLING) {
        ret = GPIO_INTR_NEGEDGE;
    } else if (mode == GPIO_IRQ_TRIGGER_HIGH) {
        ret = GPIO_INTR_HIGH_LEVEL;
    } else if (mode == GPIO_IRQ_TRIGGER_LOW) {
        ret = GPIO_INTR_LOW_LEVEL;
    } else if (mode == GPIO_IRQ_TRIGGER_FALLING | GPIO_IRQ_TRIGGER_RISING | GPIO_IRQ_USING_THREAD){
        ret = GPIO_INTR_ANYEDGE;
    }
    gpio_set_intr_type(gpio, ret);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio, TestESP32GpioIrqHandler, arg);

    return HDF_SUCCESS; 
}

static int32_t Esp32GpioUnsetIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    uint32_t gpio_intr_status;

    if (cntlr == NULL || cntlr->priv == NULL) {
        HDF_LOGE("%s: cntlr or priv is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (gpio_context.gpio_isr_func == NULL) {
        return;
    }
    gpio_intr_disable(gpio);
    gpio_hal_clear_intr_status(gpio_context.gpio_hal, gpio_intr_status);

    return HDF_SUCCESS;
}

static int32_t Esp32GpioEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    if (cntlr == NULL || cntlr->priv == NULL) {
        HDF_LOGE("%s: cntlr or priv is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    gpio_intr_enable(gpio);

    return HDF_SUCCESS;        
}

static int32_t Esp32GpioDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    if (cntlr == NULL || cntlr->priv == NULL) {
        HDF_LOGE("%s: cntlr or priv is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    gpio_intr_disable(gpio);

    return HDF_SUCCESS;     
}

static struct GpioMethod g_GpioMethod = {
    .request = NULL,
    .release = NULL,
    .write = Esp32GpioWrite,
    .read = Esp32GpioRead,
    .setDir = Esp32GpioSetDir,
    .getDir = Esp32GpioGetDir,
    .toIrq = NULL,
    .setIrq = Esp32GpioSetIrq,
    .unsetIrq = Esp32GpioUnsetIrq,
    .enableIrq = Esp32GpioEnableIrq,
    .disableIrq = Esp32GpioDisableIrq,
};

/*-------------------------------------------------------------*/
/* Init and DeInit Function */
/*-------------------------------------------------------------*/
static int32_t Esp32GpioBind(struct HdfDeviceObject *device)
{ 
    HDF_LOGE("Esp32GpioBind start!!!!!!!!!!");
    (void)device;

    return HDF_SUCCESS;
}

static void Esp32GpioRleaseCntlrMem(struct Esp32GpioCntlr *esp32)
{
    uint16_t i;

    if (esp32 == NULL) {
        return;
    }
    if (esp32->groups != NULL) {
        for (i = 0; i < esp32->groupNum; i++) {
            (void)OsalSpinDestroy(&esp32->groups[i].lock);
        }
        OsalMemFree(esp32->groups);
        esp32->groups = NULL;
    }
}

static int32_t Esp32GpioReadDrs(struct Esp32GpioCntlr *esp32, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint16(node, "groupNum", &esp32->groupNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read groupNum fail!", __func__);
        return ret;
    }

    ret = drsOps->GetUint16(node, "bitNum", &esp32->bitNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bitNum fail!", __func__);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t Esp32GpioInit(struct HdfDeviceObject *device)
{
    // Binding g_method and g_lockOps
    int32_t ret;
    struct Esp32GpioCntlr *esp32 = &g_esp32_gpio;

    HDF_LOGI("%s: Enter", __func__);
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = Esp32GpioReadDrs(esp32, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read drs fail:%d", __func__, ret);
        return ret;
    }
    esp32->cntlr.count = esp32->groupNum * esp32->bitNum;;
    esp32->cntlr.priv = (void *)device->property;
    esp32->cntlr.ops = &g_GpioMethod;
    esp32->cntlr.device = device;
    ret = GpioCntlrAdd(&esp32->cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: err add controller:%d", __func__, ret);
       
        return ret;
    }
    HDF_LOGI("%s: dev service:%s init success!", __func__, HdfDeviceGetServiceName(device));
    
    return HDF_SUCCESS;
}

static void Esp32GpioRelease(struct HdfDeviceObject *device)
{
    struct GpioCntlr *cntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null!", __func__);
        return;
    }

    cntlr = GpioCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("%s: no service binded!", __func__);
        return;
    }

    GpioCntlrRemove(cntlr);
    
}

struct HdfDriverEntry g_gpioDriverEntry = {
    .moduleVersion = 1,
    .Bind = Esp32GpioBind,
    .Init = Esp32GpioInit,
    .Release = Esp32GpioRelease,
    .moduleName = "esp32_gpio_driver",
};
HDF_INIT(g_gpioDriverEntry);
