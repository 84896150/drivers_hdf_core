/* ****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "net_adpater.h"
#include "hdf_base.h"
#include "net_device.h"
#include <linux/netdevice.h>
#include <net/cfg80211.h>
#include "eapol.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 全局变量声明
**************************************************************************** */
NetDevice *g_gphdf_netdev;
extern struct wiphy* wrap_get_wiphy(void);
extern struct net_device *get_krn_netdev(void);
extern struct NetDeviceInterFace *wal_get_net_dev_ops(void);
extern int dhd_netdev_changemtu_wrapper(struct net_device *netdev, int mtu);
extern struct wireless_dev *wl_cfg80211_add_monitor_if_wrap(struct net_device * netdev,
                                                            struct wiphy *wiphy,
                                                            const char *name);
/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */

/* ****************************************************************************
  3 函数实现
**************************************************************************** */
int32_t hdf_netdev_init(struct NetDevice *netDev)
{
    HDF_LOGE("%s: start...", __func__);
    return HDF_SUCCESS;
}

void hdf_netdev_deinit(struct NetDevice *netDev)
{
    HDF_LOGE("%s: start...", __func__);
}

int32_t hdf_netdev_open(struct NetDevice *netDev)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;
}

int32_t hdf_netdev_stop(struct NetDevice *netDev)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;
}

int32_t hdf_netdev_xmit(struct NetDevice *netDev, NetBuf *netBuff)
{
    int32_t retVal = 0;
    HDF_LOGI("%s: start...", __func__);
    return retVal;
}

int32_t hdf_netdev_ioctl(struct NetDevice *netDev, IfReq *req, int32_t cmd)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;
}

int32_t hdf_netdev_setmacaddr(struct NetDevice *netDev, void *addr)
{
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);
    return retVal;
}

struct NetDevStats *hdf_netdev_getstats(struct NetDevice *netDev)
{
    static struct NetDevStats devStat = {0};
    HDF_LOGE("%s: start...", __func__);
    return &devStat;
}

void hdf_netdev_setnetifstats(struct NetDevice *netDev, NetIfStatus status)
{
    HDF_LOGE("%s: start...", __func__);
    (void)netDev;
    (void)status;
}

uint16_t hdf_netdev_selectqueue(struct NetDevice *netDev, NetBuf *netBuff)
{
    HDF_LOGE("%s: start...", __func__);
    (void)netDev;
    (void)netBuff;
    return HDF_SUCCESS;
}

uint32_t hdf_netdev_netifnotify(struct NetDevice *netDev, NetDevNotify *notify)
{
    HDF_LOGE("%s: start...", __func__);
    (void)netDev;
    (void)notify;
    return HDF_SUCCESS;
}

int32_t hdf_netdev_changemtu(struct NetDevice *netDev, int32_t mtu)
{   
    int32_t retVal = 0;
    HDF_LOGE("%s: start...", __func__);

    return retVal;
}

void hdf_netdev_linkstatuschanged(struct NetDevice *netDev)
{
    HDF_LOGE("%s: start...", __func__);
    (void)netDev;
}

ProcessingResult hdf_netdev_specialethertypeprocess(const struct NetDevice *netDev, NetBuf *buff)
{
    struct EtherHeader *header = NULL;
    uint16_t etherType;
    const struct Eapol *eapolInstance = NULL;
    int ret = 0;
    uint16_t protocol;

    HDF_LOGE("%s: start...", __func__);

    if (netDev == NULL || buff == NULL) {
        return PROCESSING_ERROR;
    }

    header = (struct EtherHeader *)NetBufGetAddress(buff, E_DATA_BUF);

    protocol = (buff->data[12] << 8) | buff->data[13];
    if (protocol != ETHER_TYPE_PAE) {
        HDF_LOGE("%s: return PROCESSING_CONTINUE", __func__);
        return PROCESSING_CONTINUE;
    }
    if (netDev->specialProcPriv == NULL) {
        HDF_LOGE("%s: return PROCESSING_ERROR", __func__);
        return PROCESSING_ERROR;
    }

    eapolInstance = EapolGetInstance();
    ret = eapolInstance->eapolOp->writeEapolToQueue(netDev, buff);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: writeEapolToQueue failed", __func__);
        NetBufFree(buff);
    }
    return PROCESSING_COMPLETE;
}

/*****************************************************************************
  net_device上挂接的net_device_ops函数
**************************************************************************** */
struct NetDeviceInterFace g_wal_net_dev_ops = 
{
    .init       = hdf_netdev_init,
    .deInit     = hdf_netdev_deinit,
    .open       = hdf_netdev_open,
    .stop       = hdf_netdev_stop,
    .xmit       = hdf_netdev_xmit,
    .ioctl      = hdf_netdev_ioctl,
    .setMacAddr = hdf_netdev_setmacaddr,
    .getStats   = hdf_netdev_getstats,
    .setNetIfStatus     = hdf_netdev_setnetifstats,
    .selectQueue        = hdf_netdev_selectqueue,
    .netifNotify        = hdf_netdev_netifnotify,
    .changeMtu          = hdf_netdev_changemtu,
    .linkStatusChanged  = hdf_netdev_linkstatuschanged,
    .specialEtherTypeProcess  = hdf_netdev_specialethertypeprocess,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
