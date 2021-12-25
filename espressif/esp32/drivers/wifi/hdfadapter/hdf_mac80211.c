#include "wifi_mac80211_ops.h"
#include "net_adpater.h"
#include "hdf_wlan_utils.h"
#include "wifi_module.h"
#include <net/cfg80211.h>
#include <net/regulatory.h>
#include "osal_mem.h"
#include "hdf_wifi_event.h"

#define HDF_LOG_TAG Esp32WifiDriver

/*--------------------------------------------------------------------------------------------------*/
/* HdfMac80211BaseOps */
/*--------------------------------------------------------------------------------------------------*/
int32_t WalSetMode(NetDevice *netDev, enum WlanWorkMode iftype)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;
}

int32_t WalAddKey(struct NetDevice *netDev, uint8_t keyIndex, bool pairwise, const uint8_t *macAddr,
    struct KeyParams *params)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;
}

int32_t WalDelKey(struct NetDevice *netDev, uint8_t keyIndex, bool pairwise, const uint8_t *macAddr)
{
    int32_t retVal = 0;

    HDF_LOGE("%s: start...", __func__);

    return retVal;
}

int32_t WalSetDefaultKey(struct NetDevice *netDev, uint8_t keyIndex, bool unicast, bool multicas)
{
    int32_t retVal = 0;

    HDF_LOGE("%s: start...", __func__);

    return retVal;
}

int32_t WalGetDeviceMacAddr(NetDevice *netDev, int32_t type, uint8_t *mac, uint8_t len)
{
    (void)len;
    (void)type;
    (void)netDev;
    HDF_LOGE("%s: start...", __func__);

    return HDF_SUCCESS;
}

int32_t WalSetMacAddr(NetDevice *netDev, uint8_t *mac, uint8_t len)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);

    return retVal;
}

int32_t WalSetTxPower(NetDevice *netDev, int32_t power)
{
    int retVal = 0;
    HDF_LOGE("%s: start...", __func__);
        
    return HDF_SUCCESS;
}

int32_t WalGetValidFreqsWithBand(NetDevice *netDev, int32_t band, int32_t *freqs, uint32_t *num)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

int32_t WalGetHwCapability(struct NetDevice *netDev, struct WlanHwCapability **capability)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

int32_t WalRemainOnChannel(struct NetDevice *netDev, WifiOnChannel *onChannel)
{
    HDF_LOGE("%s: start...", __func__);
    return retVal;    
}

int32_t WalCancelRemainOnChannel(struct NetDevice *netDev)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;       
}

int32_t WalProbeReqReport(struct NetDevice *netDev, int32_t report)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;     
}

int32_t WalAddIf(struct NetDevice *netDev, WifiIfAdd *ifAdd)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;              
}

int32_t WalRemoveIf(struct NetDevice *netDev, WifiIfRemove *ifRemove)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;      
}

int32_t WalSetApWpsP2pIe(struct NetDevice *netDev, WifiAppIe *appIe)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;
}

int32_t WalGetDriverFlag(struct NetDevice *netDev, WifiGetDrvFlags **params)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

int32_t WalSendAction(struct NetDevice *netDev, WifiActionData *actionData)
{
    (void)netDev;
    (void)actionData;
    HDF_LOGE("%s: start...", __func__);
    return HDF_ERR_NOT_SUPPORT;     
}

int32_t WalGetIftype(struct NetDevice *netDev, uint8_t *iftype)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}


/*--------------------------------------------------------------------------------------------------*/
/* HdfMac80211STAOps */
/*--------------------------------------------------------------------------------------------------*/
static int32_t WalConnect(NetDevice *netDev, WlanConnectParams *param)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start ...!", __func__);
    return retVal;
}

int32_t WalDisconnect(NetDevice *netDev, uint16_t reasonCode)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...reasonCode:%d", __func__, reasonCode);
    return retVal;
}

int32_t WalStartScan(NetDevice *netDev, struct WlanScanRequest *scanParam)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

int32_t WalAbortScan(NetDevice *netDev)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

int32_t WalSetScanningMacAddress(NetDevice *netDev, unsigned char *mac, uint32_t len)
{
    (void)netDev;
    (void)mac;
    (void)len;
    return HDF_ERR_NOT_SUPPORT;
}

/*--------------------------------------------------------------------------------------------------*/
/* HdfMac80211APOps */
/*--------------------------------------------------------------------------------------------------*/
int32_t WalConfigAp(NetDevice *netDev, struct WlanAPConf *apConf)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

int32_t WalStartAp(NetDevice *netDev)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

int32_t WalStopAp(NetDevice *netDev)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

int32_t WalChangeBeacon(NetDevice *netDev, struct WlanBeaconConf *param)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

int32_t WalDelStation(NetDevice *netDev, const uint8_t *macAddr)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;
}

int32_t WalSetCountryCode(NetDevice *netDev, const char *code, uint32_t len)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;
}

int32_t WalGetAssociatedStasCount(NetDevice *netDev, uint32_t *num)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;       
}

int32_t WalGetAssociatedStasInfo(NetDevice *netDev, WifiStaInfo *staInfo, uint32_t num) 
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;        
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
static struct HdfMac80211BaseOps g_baseOps = 
{
    .SetMode = WalSetMode,
    .AddKey = WalAddKey,
    .DelKey = WalDelKey,
    .SetDefaultKey = WalSetDefaultKey,
    .GetDeviceMacAddr = WalGetDeviceMacAddr,
    .SetMacAddr = WalSetMacAddr,
    .SetTxPower = WalSetTxPower,
    .GetValidFreqsWithBand = WalGetValidFreqsWithBand,
    .GetHwCapability = WalGetHwCapability,
    .RemainOnChannel = WalRemainOnChannel,
    .CancelRemainOnChannel = WalCancelRemainOnChannel,
    .ProbeReqReport = WalProbeReqReport,
    .AddIf = WalAddIf,
    .RemoveIf = WalRemoveIf,
    .SetApWpsP2pIe = WalSetApWpsP2pIe,
    .GetDriverFlag = WalGetDriverFlag,
    .SendAction = WalSendAction,
    .GetIftype = WalGetIftype,
};

static struct HdfMac80211STAOps g_staOps = 
{
    .Connect = WalConnect,
    .Disconnect = WalDisconnect,
    .StartScan = WalStartScan,
    .AbortScan = WalAbortScan,
    .SetScanningMacAddress = WalSetScanningMacAddress,
};

static struct HdfMac80211APOps g_apOps = 
{
    .ConfigAp = WalConfigAp,
    .StartAp = WalStartAp,
    .StopAp = WalStopAp,
    .ConfigBeacon = WalChangeBeacon,
    .DelStation = WalDelStation,
    .SetCountryCode = WalSetCountryCode,
    .GetAssociatedStasCount = WalGetAssociatedStasCount,
    .GetAssociatedStasInfo = WalGetAssociatedStasInfo
};

void Esp32Mac80211Init(struct HdfChipDriver *chipDriver)
{
    HDF_LOGE("%s: start...", __func__);

    if (chipDriver == NULL) {
        HDF_LOGE("%s: input is NULL", __func__);
        return;
    }
    chipDriver->ops = &g_baseOps;
    chipDriver->staOps = &g_staOps;
    chipDriver->apOps = &g_apOps;
}
