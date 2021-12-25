#include "hdf_wifi_product.h"
#include "wifi_mac80211_ops.h"
#include "hdf_wlan_utils.h"

#define HDF_LOG_TAG Esp32WifiDriver

int32_t InitEsp32WifiChip(struct HdfWlanDevice *device)
{    
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

int32_t DeinitEsp32WifiChip(struct HdfWlanDevice *device)
{
    int32_t ret = 0;

    (void)device;
    HDF_LOGE("%s: start...", __func__);
    return ret;
}

int32_t Esp32WifiInit(struct HdfChipDriver *chipDriver, struct NetDevice *netDevice)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

int32_t Esp32WifiDeinit(struct HdfChipDriver *chipDriver, struct NetDevice *netDevice)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}