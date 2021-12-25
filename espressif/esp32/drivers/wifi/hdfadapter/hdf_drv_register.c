#include "hdf_device_desc.h"
#include "hdf_wifi_product.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "hdf_wlan_chipdriver_manager.h"
#include "securec.h"
#include "wifi_module.h"

static const char * const ESP32_DRIVER_NAME = "esp32";

static struct HdfChipDriver *BuildESP32Driver(struct HdfWlanDevice *device, uint8_t ifIndex) {
    HDF_LOGE("%s: Enter ", __func__);
    return NULL;
}

static void ReleaseESP32Driver(struct HdfChipDriver *chipDriver)
{
    HDF_LOGE("%s: Enter ", __func__);
}

static uint8_t GetESP32GetMaxIFCount(struct HdfChipDriverFactory *factory)
{
    HDF_LOGE("%s: Enter ", __func__);
}

static int32_t HDFWlanRegApDriverFactory(void)
{
    HDF_LOGE("%s: Enter ", __func__);
    return HDF_SUCCESS;
}

static int HdfWlanEsp32DriverBind(struct HdfDeviceObject *dev)
{
    (void)dev;
    HDF_LOGE("%s: Enter ", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfWlanEsp32ChipDriverInit(struct HdfDeviceObject *device)
{
    (void)device;
    HDF_LOGE("%s: Enter ", __func__);
    return HDFWlanRegApDriverFactory();
}

static void HdfWlanEsp32ChipRelease(struct HdfDeviceObject *object)
{
    (void)object;
    HDF_LOGE("%s: Enter ", __func__);
}

struct HdfDriverEntry g_esp32_wifi_chip_entry = {
    .moduleVersion = 1,
    .Bind = HdfWlanEsp32DriverBind,
    .Init = HdfWlanEsp32ChipDriverInit,
    .Release = HdfWlanEsp32ChipRelease,
    .moduleName = "HDF_WLAN_CHIPS"
};

HDF_INIT(g_esp32_wifi_chip_entry);
