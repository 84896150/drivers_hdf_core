/*
 * Copyright (c) 2023-2024 ArcherMind Technology (Nanjing) Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
 
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <linux/etherdevice.h>

#include <linux/ethtool.h>
#include <linux/rtnetlink.h>
#include <linux/version.h>
#if defined(CONFIG_DRIVERS_HDF_IMX8MM_ETHERNET)
#include <linux/phy.h>
#include <linux/of_mdio.h>
#endif
#include <net/ip.h>  
#include <stddef.h>
#include "osal_mem.h"
#include "securec.h"

#include "hdf_device_desc.h"
#include "usb_net_adapter.h"

#define HDF_LOG_TAG         UsbnetAdapter
#define TX_TIMEOUT_JIFFIES    (5*HZ)
#define RNDIS_MSG_PACKET    0x00000001    /* 1-N packets */

// between wakeups
#define UNLINK_TIMEOUT_MS    3
#define    TX_QLEN(dev)        ((dev)->usbnetInfo.txQlen)

/* use ethtool to change the level for any given device */
static int msg_level = -1;
module_param (msg_level, int, 0);
MODULE_PARM_DESC (msg_level, "Override default message level");

static int printf_char_buffer(char *buff, int size, bool isPrint)
{
    if (isPrint)
    {
        int i = 0;
        printk("===-harch-=== printf_char_buffer begin\n");
        for (i = 0; i < size; i++)
        {
            printk(KERN_CONT"%02x ", buff[i]);
            if ((i+1) % 32 == 0)
            {
                printk(KERN_CONT"\n");
            }
        }
        printk("===-harch-=== printf_char_buffer end\n");
    }
    return 0;
}

static void analysis_skb(struct sk_buff *skb) {
    
    struct net_device *dev = skb->dev; 
    struct ethhdr *ethh = eth_hdr(skb);
    struct iphdr *ip = ip_hdr(skb);
    
    unsigned int *src_ip = &ip->saddr;
    unsigned int *dst_ip = &ip->daddr;
    unsigned char *src_mac = ethh->h_source;
    unsigned char *dst_mac = ethh->h_dest;

    printk(" source ip:%pI4\n",src_ip);
    printk(" dest   ip:%pI4\n",dst_ip);
    printk(" destination eth addr:%x:%x:%x:%x:%x:%x\n",
        ethh->h_dest[0],ethh->h_dest[1],ethh->h_dest[2],ethh->h_dest[3],ethh->h_dest[4],ethh->h_dest[5]);
    printk(" source      eth addr:%x:%x:%x:%x:%x:%x\n",
        ethh->h_source[0],ethh->h_source[1],ethh->h_source[2],ethh->h_source[3],ethh->h_source[4],ethh->h_source[5]);
}

int32_t UsbnetAdapterSendBufToUsb(const struct HdfDeviceObject *device, uint32_t id,  
                                            const void *buf, uint32_t writeSize)
{
    HARCH_NET_INFO_PRINT("writeSize: %d", writeSize);    
    int32_t ret = HDF_SUCCESS;
    if ((device == NULL) || (buf == NULL)) {
        HDF_LOGE("%s param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct HdfSBuf *data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("fail to obtain sbuf data");
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteBuffer(data, buf, writeSize)) {
        HDF_LOGE("fail to write sbuf");
        ret = HDF_FAILURE;
        goto out;
    }

    if (HdfDeviceSendEvent(device, id, data) != HDF_SUCCESS) {
        HDF_LOGE("%s: send sensor data event failed", __func__);
        ret = HDF_FAILURE;
        goto out;
    }
    ret = HDF_SUCCESS;

out:
    HdfSbufRecycle(data);
    return ret;
}


static void rx_complete(struct work_struct *work){
    
    unsigned long flags = 0;
    struct UsbnetAdapter *usbNet = container_of(work, struct UsbnetAdapter, RxCompleteWorkqueue);
    struct pcpu_sw_netstats *stats64 = this_cpu_ptr(usbNet->stats64);

    HARCH_NET_INFO_PRINT("rx_complete stats64->rx_packets = %lu,usbNet stats64->rx_bytes = %lu",
    stats64->rx_packets, stats64->rx_bytes);

    HARCH_NET_INFO_PRINT("rx_complete stats64->tx_packets = %lu,usbNet stats64->tx_bytes = %lu",
    stats64->tx_packets,stats64->tx_bytes);
    
    flags = u64_stats_update_begin_irqsave(&stats64->syncp);
    stats64->rx_packets++;
    stats64->rx_bytes += usbNet->RxLen;
    u64_stats_update_end_irqrestore(&stats64->syncp, flags);

    HARCH_NET_INFO_PRINT("rx_complete stats64->rx_packets = %lu,usbNet stats64->rx_bytes = %lu",
    stats64->rx_packets, stats64->rx_bytes);

    HARCH_NET_INFO_PRINT("rx_complete stats64->tx_packets = %lu,usbNet stats64->tx_bytes = %lu",
    stats64->tx_packets,stats64->tx_bytes);

}

/* Passes this packet up the stack, updating its accounting.
 * Some link protocols batch packets, so their rx_fixup paths
 * can return clones as well as just modify the original skb.
 */
void UsbnetAdapterSkbReturn (struct UsbnetAdapter *usbNet, struct sk_buff *skb)
{
    
    if (test_bit(EVENT_RX_PAUSED, &usbNet->flags)) {
        skb_queue_tail(&usbNet->rxq_pause, skb);
        return;
    }

    /* only update if unset to allow minidriver rx_fixup override */
    if (skb->protocol == 0)
        skb->protocol = eth_type_trans (skb, usbNet->net);
    
    usbNet->RxLen = skb->len;
    schedule_work_on(0,&usbNet->RxCompleteWorkqueue);
    HARCH_NET_INFO_PRINT("< rx, len %zu, type 0x%x\n", skb->len + sizeof (struct ethhdr), skb->protocol);
    (void)memset_s(skb->cb, sizeof (struct skb_data), 0, sizeof (struct skb_data));

    if (skb_defer_rx_timestamp(skb))
        return;

    HARCH_NET_INFO_PRINT("-------rx before netif_rx");
    printf_char_buffer(skb->data, skb->len, false);
 
    int    status = NET_RX_SUCCESS;
    status = netif_rx (skb);
    
    HARCH_NET_INFO_PRINT("netif_rx status %d\n", status);
    if (status != NET_RX_SUCCESS)
        HARCH_NET_INFO_PRINT("netif_rx status %d\n", status);
}


/*
 * DATA -- host must not write zlps
 */
static int rndisRxFixup(struct UsbnetAdapter *usbNet, struct sk_buff *skb)
{
    /* This check is no longer done by usbnet */
    if (skb->len < usbNet->net->hard_header_len)
        return 0;

    /* peripheral may have batched packets to us... */
    while (likely(skb->len)) {
        struct rndis_data_hdr    *hdr = (void *)skb->data;
        struct sk_buff        *skb2;
        u32            msg_type, msg_len, data_offset, data_len;

        msg_type = le32_to_cpu(hdr->msg_type);
        msg_len = le32_to_cpu(hdr->msg_len);
        data_offset = le32_to_cpu(hdr->data_offset);
        data_len = le32_to_cpu(hdr->data_len);

        /* don't choke if we see oob, per-packet data, etc */
        if (unlikely(msg_type != RNDIS_MSG_PACKET || skb->len < msg_len
                || (data_offset + data_len + 8) > msg_len)) {
            usbNet->net->stats.rx_frame_errors++;
            netdev_dbg(usbNet->net, "bad rndis message %d/%d/%d/%d, len %d\n",
                   le32_to_cpu(hdr->msg_type),
                   msg_len, data_offset, data_len, skb->len);
            return 0;
        }
        skb_pull(skb, 8 + data_offset);

        /* at most one packet left? */
        if (likely((data_len - skb->len) <= sizeof *hdr)) {
            skb_trim(skb, data_len);
            break;
        }

        /* try to return all the packets in the batch */
        skb2 = skb_clone(skb, GFP_ATOMIC);
        if (unlikely(!skb2))
            break;
        skb_pull(skb, msg_len - sizeof *hdr);
        skb_trim(skb2, data_len);
        UsbnetAdapterSkbReturn(usbNet, skb2);
    }
    return 1;
}

static inline void UsbnetAdapterRxProcess (struct UsbnetAdapter *usbNet, struct sk_buff *skb)
{
    HARCH_NET_INFO_PRINT("-------rx before fix");
    printf_char_buffer(skb->data, skb->len, false);

    if (!rndisRxFixup(usbNet, skb)) {
        /* With RX_ASSEMBLE, rx_fixup() must update counters */
        if (!(usbNet->usbnetInfo.usbFlags & FLAG_RX_ASSEMBLE))
            usbNet->net->stats.rx_errors++;
        goto done;
    }
    // else network stack removes extra byte if we forced a short packet
    HARCH_NET_INFO_PRINT("--------rx after fix");
    printf_char_buffer(skb->data, skb->len, false);

    /* all data was already cloned from skb inside the driver */
    if (usbNet->usbnetInfo.usbFlags & FLAG_MULTI_PACKET)
    {
        HARCH_NET_INFO_PRINT("usbNet->driver_info->flags = %d",usbNet->usbnetInfo.usbFlags);
        goto done;
    }

    if (skb->len < ETH_HLEN) {
        usbNet->net->stats.rx_errors++;
        usbNet->net->stats.rx_length_errors++;
        HARCH_NET_INFO_PRINT("rx length %d\n", skb->len);
    } else {
        HARCH_NET_INFO_PRINT("UsbnetAdapterSkbReturn");
        UsbnetAdapterSkbReturn(usbNet, skb);
        return;
    }

done:
    skb_queue_tail(&usbNet->done, skb);
}

static void UsbnetAdapterBh (struct timer_list *t)
{
    HARCH_NET_INFO_PRINT("begin");
    struct UsbnetAdapter *usbNet = from_timer(usbNet, t, delay);
    
    struct sk_buff    *skb = NULL;
    struct skb_data    *entry = NULL;

    while ((skb = skb_dequeue (&usbNet->done))) {
        entry = (struct skb_data *) skb->cb;
        HARCH_NET_INFO_PRINT("entry->state = %d",entry->state);

        switch (entry->state) {
        case rx_done: 
            HARCH_NET_INFO_PRINT("rx_done");
            entry->state = rx_cleanup;
            UsbnetAdapterRxProcess (usbNet, skb);
            continue;
        case tx_done:
            HARCH_NET_INFO_PRINT("tx_done");
            /* fall-through */
            fallthrough;
        case rx_cleanup:
            HARCH_NET_INFO_PRINT("rx_cleanup");
            dev_kfree_skb (skb);
            continue;
        default:
            HARCH_NET_INFO_PRINT("bogus skb state %d\n", entry->state);
            /* fall-through */
        }
    }
    HARCH_NET_INFO_PRINT();
    /* restart RX again after disabling due to high error rate */
    clear_bit(EVENT_RX_KILL, &usbNet->flags);

    /* waiting for all pending urbs to complete?
     * only then can we forgo submitting anew
     */
    if (waitqueue_active(&usbNet->wait)) {
        HARCH_NET_INFO_PRINT("waitqueue_active");
        if (usbNet->rxq.qlen + usbNet->txq.qlen + usbNet->done.qlen == 0)
        {
            HARCH_NET_INFO_PRINT();
            wake_up_all(&usbNet->wait);
        }
    }

    if (usbNet->txq.qlen < TX_QLEN (usbNet))
    {
        netif_wake_queue (usbNet->net);
        HARCH_NET_INFO_PRINT("usbNet->txq.qlen = %d, TX_QLEN (usbNet) = %d",usbNet->txq.qlen,TX_QLEN (usbNet));
    }
    HARCH_NET_INFO_PRINT("end");
}

static void UsbnetAdapterBhTasklet(unsigned long data)
{
    HARCH_NET_INFO_PRINT("begin");
    struct timer_list *t = (struct timer_list *)data;

    UsbnetAdapterBh(t);
    HARCH_NET_INFO_PRINT("end");
}

/* The caller must hold list->lock */
static void UsbnetAdapterQueueSkb(struct sk_buff_head *list,
            struct sk_buff *newsk, enum skb_state state)
{
    struct skb_data *entry = (struct skb_data *) newsk->cb;

    __skb_queue_tail(list, newsk);
    entry->state = state;
}

static enum skb_state UsbnetAdapterDeferBh(struct UsbnetAdapter *usbNet, struct sk_buff *skb,
        struct sk_buff_head *list, enum skb_state state)
{
    unsigned long flags = 0;
    enum skb_state old_state = illegal;
    struct skb_data *entry = (struct skb_data *) skb->cb;

    spin_lock_irqsave(&list->lock, flags);
    old_state = entry->state;
    entry->state = state;
    __skb_unlink(skb, list);

    /* UsbnetAdapterDeferBh() is never called with list == &usbNet->done.
     * spin_lock_nested() tells lockdep that it is OK to take
     * usbNet->done.lock here with list->lock held.
     */
    spin_lock_nested(&usbNet->done.lock, SINGLE_DEPTH_NESTING);

    __skb_queue_tail(&usbNet->done, skb);
    if (usbNet->done.qlen == 1)
        tasklet_schedule(&usbNet->bh);

    spin_unlock(&usbNet->done.lock);
    spin_unlock_irqrestore(&list->lock, flags);

    return old_state;
}

int32_t UsbnetAdapterOpen(struct net_device *net)
{
    HARCH_NET_INFO_PRINT("begin");
    if (NULL == net) 
    {
        HDF_LOGE("net device object is invalid");
        return HDF_FAILURE;
    }
    struct UsbnetAdapter  *usbNet = netdev_priv(net);
    if (NULL == usbNet) 
    {
        HDF_LOGE("usb net adapter object is invalid");
        return HDF_FAILURE;
    }

    set_bit(EVENT_DEV_OPEN, &usbNet->flags);

    netif_start_queue (net);

    /* set device param, if find usb net host need, then it should send to it */
    /* reset rx error state */
    usbNet->pkt_cnt = 0;
    usbNet->pkt_err = 0;

    clear_bit(EVENT_RX_KILL, &usbNet->flags);

    /* send info to usb start usb function */
    /* need transform usb_net_host change about usbNet->driverInfo->flags and set it on usb_net_host */
    OsalMutexLock(&usbNet->sendSkbClock);
    int32_t ret = UsbnetAdapterSendBufToUsb(usbNet->deviceObject, USB_NET_OPEN_USB, (const void *)&usbNet->flags, sizeof(usbNet->flags));
    if (ret != HDF_SUCCESS)
    {
        HDF_LOGE("fail to UsbnetAdapterSendBufToUsb SendEvent!");
        OsalMutexUnlock(&usbNet->sendSkbClock);
        return HDF_FAILURE;
    }
    OsalMutexUnlock(&usbNet->sendSkbClock);
    
    tasklet_schedule (&usbNet->bh);

    return HDF_SUCCESS;
}

static void waitSkbQueueEmpty(struct sk_buff_head *q)
{
    unsigned long flags = 0;
    HARCH_NET_INFO_PRINT();
    spin_lock_irqsave(&q->lock, flags);
    while (!skb_queue_empty(q)) {
        spin_unlock_irqrestore(&q->lock, flags);
        schedule_timeout(msecs_to_jiffies(UNLINK_TIMEOUT_MS));
        set_current_state(TASK_UNINTERRUPTIBLE);
        spin_lock_irqsave(&q->lock, flags);
    }
    spin_unlock_irqrestore(&q->lock, flags);
}

// precondition: never called in_interrupt
static void UsbnetAdapterTerminateUrbs(struct UsbnetAdapter *usbNet)
{
    HARCH_NET_INFO_PRINT();

    DECLARE_WAITQUEUE(wait, current);
    /* ensure there are no more active urbs */
    add_wait_queue(&usbNet->wait, &wait);
    set_current_state(TASK_UNINTERRUPTIBLE);
    /* maybe wait for deletions to finish. */
    waitSkbQueueEmpty(&usbNet->rxq);
    waitSkbQueueEmpty(&usbNet->txq);

    waitSkbQueueEmpty(&usbNet->done);
    set_current_state(TASK_RUNNING);
    remove_wait_queue(&usbNet->wait, &wait);
}

int32_t UsbnetAdapterStop(struct net_device *net)
{
    HARCH_NET_INFO_PRINT("begin");
    if (NULL == net) 
    {
        HDF_LOGE("net device object is invalid");
        return HDF_FAILURE;
    }

    struct UsbnetAdapter  *usbNet = netdev_priv(net);
    if (NULL == usbNet) 
    {
        HDF_LOGE("usb net adapter object is invalid");
        return HDF_FAILURE;
    }

    struct pcpu_sw_netstats *stats64 = this_cpu_ptr(usbNet->stats64);

    HARCH_NET_INFO_PRINT("usbNet stats64->rx_packets = %lu,usbNet stats64->rx_bytes = %lu",
    stats64->rx_packets, stats64->rx_bytes);

    HARCH_NET_INFO_PRINT("usbNet stats64->tx_packets = %lu,usbNet stats64->tx_bytes = %lu",
    stats64->tx_packets,stats64->tx_bytes);

    /* 1. clear_bit(EVENT_DEV_OPEN, &dev->flags); */
    clear_bit(EVENT_DEV_OPEN, &usbNet->flags);
    /* 2. netif_stop_queue (net); */
    netif_stop_queue (net);
    HARCH_NET_INFO_PRINT("stop stats: rx/tx %lu/%lu, errs %lu/%lu\n",
           net->stats.rx_packets, net->stats.tx_packets,
           net->stats.rx_errors, net->stats.tx_errors);
    
    /* 3. pm = usb_autopm_get_interface(dev->intf); do nothing */
    /* 4. if (info->stop) { no stop interface impl in rndis driver info */
    /* 5. if (!(info->flags & FLAG_AVOID_UNLINK_URBS)) do nothing*/
    if (!(usbNet->usbnetInfo.usbFlags & FLAG_AVOID_UNLINK_URBS)){
        UsbnetAdapterTerminateUrbs(usbNet);
    }
    
    /* 6. usbnet_status_stop(dev); none */
    /* 7. usbnet_purge_paused_rxq(dev); none */
    skb_queue_purge(&usbNet->rxq_pause);
    /* 8. mpn = !test_and_clear_bit(EVENT_NO_RUNTIME_PM, &dev->flags); if (info->manage_power && mpn) 
    no manage_power interface impl in rndis driver info*/
    /* 9. dev->flags = 0; */
    usbNet->flags = 0;
    OsalMutexLock(&usbNet->sendSkbClock);
    int32_t ret = UsbnetAdapterSendBufToUsb(usbNet->deviceObject, USB_NET_CLOSE_USB, (const void *)&usbNet->flags, sizeof(usbNet->flags));
    if (ret != HDF_SUCCESS)
    {
        HDF_LOGE("fail to UsbnetAdapterSendBufToUsb SendEvent!");
        OsalMutexUnlock(&usbNet->sendSkbClock);
        return HDF_FAILURE;
    }
    OsalMutexUnlock(&usbNet->sendSkbClock);
    /* 10. del_timer_sync (&dev->delay); */
    del_timer_sync (&usbNet->delay);
    /* 11. tasklet_kill (&dev->bh); */
    tasklet_kill (&usbNet->bh);
    /* 12. cancel_work_sync(&dev->kevent); no kevent, do nothing */
    //cancel_work_sync(&usbNet->kevent);
    cancel_work_sync(&usbNet->TxCompleteWorkqueue);
    cancel_work_sync(&usbNet->RxCompleteWorkqueue);
    return HDF_SUCCESS;
}

struct sk_buff *RndisTxFixup(struct sk_buff *skb, gfp_t flags)
{
    struct rndis_data_hdr *hdr = NULL;
    struct sk_buff *skb2 = NULL;
    unsigned len = skb->len;

    if (likely(!skb_cloned(skb))) {
        int    room = skb_headroom(skb);

        /* enough head room as-is? */
        if (unlikely((sizeof *hdr) <= room))
            goto fill;

        /* enough room, but needs to be readjusted? */
        room += skb_tailroom(skb);
        if (likely((sizeof *hdr) <= room)) {
            skb->data = memmove(skb->head + sizeof *hdr,
                        skb->data, len);
            skb_set_tail_pointer(skb, len);
            goto fill;
        }
    }

    /* create a new skb, with the correct size (and tailpad) */
    skb2 = skb_copy_expand(skb, sizeof *hdr, 1, flags);
    dev_kfree_skb_any(skb);
    if (unlikely(!skb2))
        return skb2;
    skb = skb2;

    /* fill out the RNDIS header.  we won't bother trying to batch
     * packets; Linux minimizes wasted bandwidth through tx queues.
     */
fill:
    HARCH_NET_INFO_PRINT("%s:%d fill skb by rndis host", __func__, __LINE__);
    hdr = __skb_push(skb, sizeof *hdr);
    memset(hdr, 0, sizeof *hdr);
    hdr->msg_type = cpu_to_le32(RNDIS_MSG_PACKET);
    hdr->msg_len = cpu_to_le32(skb->len);
    hdr->data_offset = cpu_to_le32(sizeof(*hdr) - 8);
    hdr->data_len = cpu_to_le32(len);

    /* FIXME make the last packet always be short ... */
    return skb;
}

static void tx_complete(struct work_struct *work)
{
    unsigned long flags = 0;
    struct UsbnetAdapter *usbNet = container_of(work, struct UsbnetAdapter, TxCompleteWorkqueue);
    struct pcpu_sw_netstats *stats64 = this_cpu_ptr(usbNet->stats64);

    printk("tx_complete dump_stack start usbNet->usbnetInfo.mtu = %d\n",usbNet->usbnetInfo.mtu);
    dump_stack();
    printk("tx_complete dump_stack end\n");

    HARCH_NET_INFO_PRINT("tx_complete stats64->rx_packets = %lu,usbNet stats64->rx_bytes = %lu",
    stats64->rx_packets, stats64->rx_bytes);

    HARCH_NET_INFO_PRINT("tx_complete stats64->tx_packets = %lu,usbNet stats64->tx_bytes = %lu",
    stats64->tx_packets,stats64->tx_bytes);

    flags = u64_stats_update_begin_irqsave(&stats64->syncp);
    stats64->tx_packets++;
    stats64->tx_bytes += usbNet->TxLen;
    u64_stats_update_end_irqrestore(&stats64->syncp, flags);

    HARCH_NET_INFO_PRINT("tx_complete stats64->rx_packets = %lu,usbNet stats64->rx_bytes = %lu",
    stats64->rx_packets, stats64->rx_bytes);

    HARCH_NET_INFO_PRINT("tx_complete stats64->tx_packets = %lu,usbNet stats64->tx_bytes = %lu",
    stats64->tx_packets,stats64->tx_bytes);
}

netdev_tx_t UsbnetAdapterStartXmit(struct sk_buff *skb, struct net_device *net)
{
    analysis_skb(skb);
    HARCH_NET_INFO_PRINT("netif_running  = %d, netif_device_present = %d" , netif_running (net),netif_device_present (net));
    HARCH_NET_INFO_PRINT("netif_carrier_ok  = %d" , netif_carrier_ok (net));

    /* print org skb data info */
    HARCH_NET_INFO_PRINT("UsbnetAdapterStartXmit skb->data = %x, len = %d",skb->data, skb->len);
    printf_char_buffer(skb->data, skb->len, false);
    struct UsbnetAdapter *usbNet = netdev_priv(net);
    
    //1.time tamp
    if (skb)
        skb_tx_timestamp(skb);
    //2.fix up
    struct sk_buff* skb_fixup = NULL;

    /* rndis fixup sk_buf */
    skb_fixup = RndisTxFixup(skb, GFP_ATOMIC);
    if (NULL == skb_fixup)
    {
        if (usbNet->usbnetInfo.usbFlags & FLAG_MULTI_PACKET){
            goto not_drop;
        }
        HDF_LOGE("fail to tx fixup by rndis host");
        goto drop;
    }

    HARCH_NET_INFO_PRINT("UsbnetAdapterStartXmit skb_fixup->data = %x, len = %d",skb_fixup->data, skb_fixup->len);
    /* print org skb data info */
    printf_char_buffer(skb_fixup->data, skb_fixup->len, false);
    //3.send to usb
    
    // send msg to usb
    unsigned long flags = 0;
    spin_lock_irqsave(&usbNet->txq.lock, flags);
       if (netif_queue_stopped(usbNet->net)) {
        goto drop;
    }

    int32_t ret = UsbnetAdapterSendBufToUsb(usbNet->deviceObject, USB_NET_SEND_DATA_TO_USB,  
                        (unsigned char *)(skb_fixup->data), skb_fixup->len);

    if (ret != HDF_SUCCESS) {
        HDF_LOGE("fail to UsbnetHost SendEvent!");
drop:
        net->stats.tx_dropped++;
not_drop:
        spin_unlock_irqrestore(&usbNet->txq.lock, flags);
        if (skb_fixup) {
            dev_kfree_skb_any (skb_fixup);
        }
    } else {
        netif_trans_update(usbNet->net);
        UsbnetAdapterQueueSkb(&usbNet->txq, skb_fixup, tx_start);

        if (usbNet->txq.qlen >= TX_QLEN (usbNet))
        {
            HARCH_NET_INFO_PRINT("usbNet->txq.qlen = %d, TX_QLEN (usbNet) = %d",usbNet->txq.qlen,TX_QLEN (usbNet));
            netif_stop_queue (usbNet->net);
        }

        spin_unlock_irqrestore(&usbNet->txq.lock, flags);

        usbNet->TxLen = skb_fixup->len;
        schedule_work_on(0,&usbNet->TxCompleteWorkqueue);
        
        enum skb_state state = UsbnetAdapterDeferBh(usbNet, skb_fixup, &usbNet->txq, tx_done);
        HARCH_NET_INFO_PRINT("state= %d", state);

        HARCH_NET_INFO_PRINT("========print tosend skb data info========");
    }
    return NETDEV_TX_OK;
}

static void UsbnetAdapterTXTimeout(struct net_device *net, unsigned int txqueue)
{
    //send to device
    HARCH_NET_INFO_PRINT("begin");
    if ( NULL == net) {
        HDF_LOGE("%s net device object is invalid", __func__);
        return;
    }
    struct UsbnetAdapter  *usbNet = netdev_priv(net);
    if ( NULL == usbNet) {
        HDF_LOGE("%s UsbnetAdapter object is invalid", __func__);
        return;
    }
    /* 1. unlink_urbs (dev, &dev->txq); maybe later be used */

    /* 2. tasklet_schedule (&dev->bh) */
    tasklet_schedule (&usbNet->bh);

    /* 3. if (dev->driver_info->recover) none, rndis host no recover impl */
    
    return;
}

/* some work can't be done in tasklets, so we use keventd
 *
 * NOTE:  annoying asymmetry:  if it's active, schedule_work() fails,
 * but tasklet_schedule() doesn't.  hope the failure is rare.
 */
void UsbnetAdapter_defer_kevent (struct UsbnetAdapter *usbNet, int work)
{
    HARCH_NET_INFO_PRINT("begin");
    if ( NULL == usbNet) {
        HDF_LOGE("%s UsbnetAdapter object is invalid", __func__);
        return;
    }
    set_bit (work, &usbNet->flags);
    /* need update flags to usb_net_host */
    OsalMutexLock(&usbNet->sendSkbClock);
    int32_t ret = UsbnetAdapterSendBufToUsb(usbNet->deviceObject, USB_NET_UPDATE_FLAGS, (const void *)&usbNet->flags, sizeof(usbNet->flags));
    if (ret != HDF_SUCCESS)
    {
        HDF_LOGE("fail to UsbnetAdapterSendBufToUsb SendEvent!");
        OsalMutexUnlock(&usbNet->sendSkbClock);
        return;
    }
    OsalMutexUnlock(&usbNet->sendSkbClock);
    
    if (!schedule_work (&usbNet->kevent))
        HARCH_NET_INFO_PRINT("kevent %d may have been dropped\n", work);
    else
        HARCH_NET_INFO_PRINT("kevent %d scheduled\n", work);
}

void UsbnetAdapterSetRxMode(struct net_device *net)
{
    //send to device
    HARCH_NET_INFO_PRINT("begin");
    return;

    if ( NULL == net) {
        HDF_LOGE("%s net device object is invalid", __func__);
        return;
    }
    struct UsbnetAdapter  *usbNet = netdev_priv(net);
    if ( NULL == usbNet) {
        HDF_LOGE("%s UsbnetAdapter object is invalid", __func__);
        return;
    }
    /* usbnet_defer_kevent(dev, EVENT_SET_RX_MODE); */
    UsbnetAdapter_defer_kevent(usbNet, EVENT_SET_RX_MODE);
    return;
}

void UsbnetAdapter_pause_rx(struct UsbnetAdapter *usbNet)
{
    HARCH_NET_INFO_PRINT("begin");
    if ( NULL == usbNet) {
        HDF_LOGE("%s UsbnetAdapter object is invalid", __func__);
        return;
    }
    set_bit(EVENT_RX_PAUSED, &usbNet->flags);
    /* need update flags to usb_net_host */
    OsalMutexLock(&usbNet->sendSkbClock);
    int32_t ret = UsbnetAdapterSendBufToUsb(usbNet->deviceObject, USB_NET_UPDATE_FLAGS, (const void *)&usbNet->flags, sizeof(usbNet->flags));
    if (ret != HDF_SUCCESS)
    {
        HDF_LOGE("fail to UsbnetAdapterSendBufToUsb SendEvent!");
        OsalMutexUnlock(&usbNet->sendSkbClock);
        return;
    }
    OsalMutexUnlock(&usbNet->sendSkbClock);

    HARCH_NET_INFO_PRINT("paused rx queue enabled\n");
}

void UsbnetAdapter_unlink_rx_urbs(struct UsbnetAdapter *usbNet)
{
    HARCH_NET_INFO_PRINT("begin");
    if ( NULL == usbNet) {
        HDF_LOGE("%s UsbnetAdapter object is invalid", __func__);
        return;
    }
    
    if (netif_running(usbNet->net)) {
        /* (void) unlink_urbs (usbNet, &usbNet->rxq); */
        tasklet_schedule(&usbNet->bh);
    }
}

void UsbnetAdapter_resume_rx(struct UsbnetAdapter *usbNet)
{
    HARCH_NET_INFO_PRINT("begin");
    int num = 0;
    struct sk_buff *skb = NULL;
    if ( NULL == usbNet) {
        HDF_LOGE("%s UsbnetAdapter object is invalid", __func__);
        return;
    }
    
    clear_bit(EVENT_RX_PAUSED, &usbNet->flags);
    /* need update flags to usb_net_host */
    OsalMutexLock(&usbNet->sendSkbClock);
    int32_t ret = UsbnetAdapterSendBufToUsb(usbNet->deviceObject, USB_NET_UPDATE_FLAGS, (const void *)&usbNet->flags, sizeof(usbNet->flags));
    if (ret != HDF_SUCCESS)
    {
        HDF_LOGE("fail to UsbnetAdapterSendBufToUsb SendEvent!");
        OsalMutexUnlock(&usbNet->sendSkbClock);
        return;
    }
    OsalMutexUnlock(&usbNet->sendSkbClock);

    while ((skb = skb_dequeue(&usbNet->rxq_pause)) != NULL) {
        UsbnetAdapterSkbReturn(usbNet, skb);
        num++;
    }

    tasklet_schedule(&usbNet->bh);
    HARCH_NET_INFO_PRINT("paused rx queue disabled, %d skbs requeued\n", num);
}


int32_t UsbnetAdapterChangeMtu(struct net_device *net, int newMtu)
{
    //send to device
    HARCH_NET_INFO_PRINT("begin");
    if ( NULL == net) {
        HDF_LOGE("%s net device object is invalid", __func__);
        return HDF_FAILURE;
    }
    
    struct UsbnetAdapter  *usbNet = netdev_priv(net);
    if ( NULL == usbNet) {
        HDF_LOGE("%s UsbnetAdapter object is invalid", __func__);
        return HDF_FAILURE;
    }

       /* 1. int     ll_mtu = new_mtu + net->hard_header_len; 
        int     old_hard_mtu = dev->hard_mtu;
        int     old_rx_urb_size = dev->rx_urb_size;
    */
    HARCH_NET_INFO_PRINT("newMtu:%d", newMtu);
    int ll_mtu = newMtu + net->hard_header_len; 
    int old_hard_mtu = usbNet->usbnetInfo.hardMtu;
    int old_rx_urb_size = usbNet->usbnetInfo.rxUrbSize;

    /* 2. no second zero-length packet read wanted after mtu-sized packets */
    if ((ll_mtu % usbNet->usbnetInfo.maxpacket) == 0) {
        HARCH_NET_INFO_PRINT("");
        return -EDOM;
    }

    /* 3. set usbNet->hard_mtu */
    net->mtu = newMtu;
    usbNet->usbnetInfo.hardMtu = net->mtu + net->hard_header_len;
    
    /* 4. pause and resume usbnet */
    if (usbNet->usbnetInfo.rxUrbSize == old_hard_mtu) {
        usbNet->usbnetInfo.rxUrbSize = usbNet->usbnetInfo.hardMtu;
        if (usbNet->usbnetInfo.rxUrbSize > old_rx_urb_size) {
            UsbnetAdapter_pause_rx(usbNet);
            UsbnetAdapter_unlink_rx_urbs(usbNet);
            UsbnetAdapter_resume_rx(usbNet);
        }
    }

    /* need transmit hard_mtu to usb net host, UsbnetHostUpdateMaxQlen need this param */
    HARCH_NET_INFO_PRINT("name = %s, flags = %d, usbFlags = %x mtu = %d, hardHeaderLen = %d, link = %d needReset = %d, hardMtu = %d, rxUrbSize = %d, maxpacket =%d",
            usbNet->usbnetInfo.name,usbNet->usbnetInfo.flags, usbNet->usbnetInfo.usbFlags,
            usbNet->usbnetInfo.mtu, usbNet->usbnetInfo.hardHeaderLen, usbNet->usbnetInfo.link,
            usbNet->usbnetInfo.needReset, usbNet->usbnetInfo.hardMtu, usbNet->usbnetInfo.rxUrbSize, 
            usbNet->usbnetInfo.maxpacket
        );
    /* 5. max qlen depend on hard_mtu and rx_urb_size */
    //UsbnetHostUpdateMaxQlen(usbNet);
    /* need update flags to usb_net_host */
    OsalMutexLock(&usbNet->sendSkbClock);
    int32_t ret = UsbnetAdapterSendBufToUsb(usbNet->deviceObject, USB_NET_UPDATE_MAXQLEN, (const void *)&usbNet->usbnetInfo, sizeof(usbNet->usbnetInfo));
    if (ret != HDF_SUCCESS)
    {
        HDF_LOGE("fail to UsbnetAdapterSendBufToUsb SendEvent!");
        OsalMutexUnlock(&usbNet->sendSkbClock);
        return ret;
    }
    OsalMutexUnlock(&usbNet->sendSkbClock);
    return HDF_SUCCESS;
}

void UsbnetAdapterGetStats64(struct net_device *net,  struct rtnl_link_stats64 *stats)
{
    //send to device
    HARCH_NET_INFO_PRINT("begin");

    struct UsbnetAdapter *usbNet = netdev_priv(net);
    netdev_stats_to_stats64(stats, &net->stats);
    dev_fetch_sw_netstats(stats, usbNet->stats64);

    struct pcpu_sw_netstats *stats64 = this_cpu_ptr(usbNet->stats64);

    HARCH_NET_INFO_PRINT("usbNet stats64->rx_packets = %lu,usbNet stats64->rx_bytes = %lu",
    stats64->rx_packets, stats64->rx_bytes);

    HARCH_NET_INFO_PRINT("usbNet stats64->tx_packets = %lu,usbNet stats64->tx_bytes = %lu",
    stats64->tx_packets,stats64->tx_bytes);

    HARCH_NET_INFO_PRINT("end");

    return;
}

static struct net_device_ops g_UsbnetAdapterDeviceOps = {
    .ndo_open = UsbnetAdapterOpen,
    .ndo_stop = UsbnetAdapterStop,
    .ndo_start_xmit = UsbnetAdapterStartXmit,
     .ndo_tx_timeout        = UsbnetAdapterTXTimeout,
    .ndo_set_rx_mode    = UsbnetAdapterSetRxMode,
    .ndo_change_mtu        = UsbnetAdapterChangeMtu,
    .ndo_get_stats64    = UsbnetAdapterGetStats64,
    .ndo_set_mac_address     = eth_mac_addr,
    .ndo_validate_addr    = eth_validate_addr,
};

/* drivers may override default ethtool_ops in their bind() routine */
static const struct ethtool_ops g_UsbnetAdapterEthtoolOps = {
};

void UsbnetAdapterLinkChange(struct UsbnetAdapter *usbNet, bool link, bool need_reset)
{
    HARCH_NET_INFO_PRINT("begin");
    /* update link after link is reseted */
    if (link && !need_reset)
        netif_carrier_on(usbNet->net);
    else
        netif_carrier_off(usbNet->net);
}

static int32_t UsbnetAdapterRxComplete(struct HdfDeviceObject *device, struct HdfSBuf *data, gfp_t flags)
{
    HARCH_NET_INFO_PRINT("begin");
    unsigned char *buff = NULL;   
    uint32_t infoSize    = 0;
    int ret = HDF_SUCCESS;
    
    bool flag = HdfSbufReadBuffer(data, (const void **)(&(buff)), &infoSize);
    if ((!flag) || buff == NULL) {
        HDF_LOGE("%s: fail to read infoTable in event data, flag = %d", __func__, flag);
        return  HDF_ERR_INVALID_PARAM;
    }

    HARCH_NET_INFO_PRINT("infoSize= %d", infoSize);
    struct net_device *net = (struct net_device *)device->priv;
    if (net == NULL)
        return HDF_ERR_INVALID_PARAM;

    HARCH_NET_INFO_PRINT("recv msg buff");
    printf_char_buffer(buff, infoSize, false);


    struct UsbnetAdapter *usbNet = netdev_priv(net);
    if (usbNet == NULL)
        return HDF_ERR_INVALID_PARAM;

    struct sk_buff    *skb   = NULL;
    struct skb_data    *entry = NULL;
    uint32_t skb_size =  sizeof(struct iphdr) + sizeof(struct udphdr) + infoSize;
    size_t size = (skb_size > usbNet->usbnetInfo.rxUrbSize ? usbNet->usbnetInfo.rxUrbSize : skb_size );
    HARCH_NET_INFO_PRINT("size = %d", size);

    if (test_bit(EVENT_NO_IP_ALIGN, &usbNet->flags))
        skb = __netdev_alloc_skb(usbNet->net, size, flags);
    else
        skb = __netdev_alloc_skb_ip_align(usbNet->net, size, flags);

    if (!skb) {
        HARCH_NET_INFO_PRINT("no rx skb\n");
        UsbnetAdapter_defer_kevent(usbNet, EVENT_RX_MEMORY);
        return HDF_DEV_ERR_NO_MEMORY;
    }

    entry = (struct skb_data *) skb->cb;
    entry->length = 0;
    skb_put (skb, infoSize);
    if (memcpy_s(skb->data, infoSize, buff, infoSize) != EOK) {
        HARCH_NET_INFO_PRINT("memcpy_s error ");
        return  HDF_ERR_INVALID_PARAM;
    }

    HARCH_NET_INFO_PRINT("skb->len = %d",skb->len);
    HARCH_NET_INFO_PRINT("recv msg skbBuff");
    printf_char_buffer(skb->data, skb->len, false);

    // success recv 
    unsigned long lockflags = 0;
    spin_lock_irqsave (&usbNet->rxq.lock, lockflags);
    UsbnetAdapterQueueSkb(&usbNet->rxq, skb, rx_start);
    spin_unlock_irqrestore (&usbNet->rxq.lock, lockflags);

    struct pcpu_sw_netstats *stats64 = this_cpu_ptr(usbNet->stats64);
    HARCH_NET_INFO_PRINT("usbNet stats64->rx_packets = %lu,usbNet stats64->rx_bytes = %lu",
    stats64->rx_packets, stats64->rx_bytes);

    // rx complete
    enum skb_state    state = rx_done;
    /* stop rx if packet error rate is high */
    if (++usbNet->pkt_cnt > 30) {
        HARCH_NET_INFO_PRINT("usbNet->pkt_cnt = %d,pkt_err = %d",usbNet->pkt_cnt,usbNet->pkt_err);
        usbNet->pkt_cnt = 0;
        usbNet->pkt_err = 0;
    } else {
        HARCH_NET_INFO_PRINT("usbNet->pkt_cnt = %d,pkt_err = %d",usbNet->pkt_cnt,usbNet->pkt_err);
        if (state == rx_cleanup)
            usbNet->pkt_err++;

        if (usbNet->pkt_err > 20)
            set_bit(EVENT_RX_KILL, &usbNet->flags);
    }
    HARCH_NET_INFO_PRINT("usbNet->pkt_cnt = %d,pkt_err = %d",usbNet->pkt_cnt,usbNet->pkt_err);
    state = UsbnetAdapterDeferBh(usbNet, skb, &usbNet->rxq, state);
    HARCH_NET_INFO_PRINT("state= %d", state);

    HARCH_NET_INFO_PRINT("end");
    return HDF_SUCCESS;
}

// precondition: never called in_interrupt
static struct device_type wlanType = {
    .name    = "wlan",
};

static struct device_type wwanType = {
    .name    = "wwan",
};

int32_t UsbnetAdapterProbe(struct HdfDeviceObject *device, struct HdfSBuf *data)
{
    HARCH_NET_INFO_PRINT("begin");
    int    ret = HDF_SUCCESS;
    uint32_t infoSize = 0;
    struct UsbnetTransInfo *uNetTransInfo = NULL;   
    
    bool flag = HdfSbufReadBuffer(data, (const void **)(&(uNetTransInfo)), &infoSize);
    if ((!flag) || uNetTransInfo == NULL) {
        ret = HDF_ERR_INVALID_PARAM;
        HDF_LOGE("%s: fail to read infoTable in event data, flag = %d", __func__, flag);
        return ret;
    }

    struct net_device *net = alloc_etherdev(sizeof(struct UsbnetAdapter));
    if (net == NULL) {
        HARCH_NET_INFO_PRINT();
        goto out;
    }

    device->priv = net;
    HARCH_NET_INFO_PRINT();
    struct UsbnetAdapter *usbNet = netdev_priv(net);
    usbNet->deviceObject = device;

    //info from usb
    if (memcpy_s(&(usbNet->usbnetInfo), sizeof(struct UsbnetTransInfo), 
            uNetTransInfo, sizeof(struct UsbnetTransInfo)) != EOK) {
        HARCH_NET_INFO_PRINT("memcpy_s error ");
        goto out0;
    }

    HARCH_NET_INFO_PRINT("name = %s, \
                         flags = %d, \
                         usbFlags = %x, \
                         mtu = %d, \
                         hardHeaderLen = %d, \                         
                         link = %d, \
                         needReset = %d, \
                         hardMtu = %d, \
                         rxUrbSize = %d, \
                         maxpacket = %d",
                        usbNet->usbnetInfo.name,
                        usbNet->usbnetInfo.flags,
                        usbNet->usbnetInfo.usbFlags,
                        usbNet->usbnetInfo.mtu, 
                        usbNet->usbnetInfo.hardHeaderLen, 
                        usbNet->usbnetInfo.link,
                        usbNet->usbnetInfo.needReset, 
                        usbNet->usbnetInfo.hardMtu,
                        usbNet->usbnetInfo.rxUrbSize, 
                        usbNet->usbnetInfo.maxpacket);
    
    usbNet->stats64 = netdev_alloc_pcpu_stats(struct pcpu_sw_netstats);
    if (!usbNet->stats64)
        goto out0;

    usbNet->msg_enable = netif_msg_init (msg_level, NETIF_MSG_DRV
           | NETIF_MSG_PROBE | NETIF_MSG_LINK| NETIF_MSG_RX_ERR|NETIF_MSG_TX_ERR);
    
    OsalMutexInit(&usbNet->sendSkbClock);

    init_waitqueue_head(&usbNet->wait);
    skb_queue_head_init (&usbNet->rxq);
    skb_queue_head_init (&usbNet->txq);
    skb_queue_head_init (&usbNet->done);
    skb_queue_head_init(&usbNet->rxq_pause);

    // init work for tx_complete rx_complete
    INIT_WORK(&usbNet->TxCompleteWorkqueue, tx_complete);
    INIT_WORK(&usbNet->RxCompleteWorkqueue, rx_complete);

    usbNet->bh.func = (void (*)(unsigned long))UsbnetAdapterBhTasklet;
    usbNet->bh.data = (unsigned long)&usbNet->delay;    //struct timer_list    delay;
   
    timer_setup(&usbNet->delay, UsbnetAdapterBh, 0);

    usbNet->net = net;
    // 1.addr
    static u8 node_id[ETH_ALEN];
    if (usbNet->usbnetInfo.isGetmacAddr)
    {
        if (memcpy_s(node_id, ETH_ALEN, usbNet->usbnetInfo.macAddr, ETH_ALEN) != EOK) {
            goto out0;
        }
    }else
    {
        eth_random_addr(node_id);
    }
    memcpy(net->dev_addr, node_id, sizeof(node_id));
    HARCH_NET_INFO_PRINT("macAddr  addr %pM", net->dev_addr);

    // 2. mtu
    net->min_mtu = 0;
    net->max_mtu = ETH_MAX_MTU;

    net->netdev_ops = &g_UsbnetAdapterDeviceOps;
    net->watchdog_timeo = TX_TIMEOUT_JIFFIES;
    net->ethtool_ops = &g_UsbnetAdapterEthtoolOps;

    //3. name and mtu
    strcpy (net->name, "usb%d");
    if (usbNet->usbnetInfo.isBindDevice)
    {
        /* heuristic:  "usb%d" for links we know are two-host,
           else "eth%d" when there's reasonable doubt.  userspace
           can rename the link if it knows better.
        */
        if ((usbNet->usbnetInfo.usbFlags & FLAG_ETHER) != 0 &&
            ((usbNet->usbnetInfo.usbFlags & FLAG_POINTTOPOINT) == 0 ||
            (net->dev_addr[0] & 0x02) == 0))
        {
            HARCH_NET_INFO_PRINT();
            strcpy (net->name, "eth%d");
        }
        
        /* WLAN devices should always be named "wlan%d" */
        if ((usbNet->usbnetInfo.usbFlags & FLAG_WLAN) != 0)
        {
            HARCH_NET_INFO_PRINT();
            strcpy(net->name, "wlan%d");
        }
        /* WWAN devices should always be named "wwan%d" */
        if ((usbNet->usbnetInfo.usbFlags & FLAG_WWAN) != 0)
        {
            HARCH_NET_INFO_PRINT();
            strcpy(net->name, "wwan%d");
        }

        /* devices that cannot do ARP */
        if ((usbNet->usbnetInfo.usbFlags & FLAG_NOARP) != 0)
        {
            net->flags |= IFF_NOARP;
        }

        HARCH_NET_INFO_PRINT("usbNet->usbnetInfo.mtu = %d",usbNet->usbnetInfo.mtu);
        /* maybe the remote can't receive an Ethernet MTU */
        net->mtu = usbNet->usbnetInfo.mtu;
        net->hard_header_len = usbNet->usbnetInfo.hardHeaderLen;

        HARCH_NET_INFO_PRINT("net->mtu= %d",net->mtu);
    }
    
    HARCH_NET_INFO_PRINT("net->name= %s",net->name);   
    /* let userspace know we have a random address */
    if (ether_addr_equal(net->dev_addr, node_id))
        net->addr_assign_type = NET_ADDR_RANDOM;

    if ((usbNet->usbnetInfo.usbFlags & FLAG_WLAN) != 0)
        SET_NETDEV_DEVTYPE(net, &wlanType);

    if (usbNet->usbnetInfo.usbFlags & FLAG_WWAN != 0)
        SET_NETDEV_DEVTYPE(net, &wwanType);

    ret = register_netdev(net);
    if (ret)
        goto out0;

    HARCH_NET_INFO_PRINT( "register net %pM\n",  net->dev_addr);

    netif_device_attach(net);

    HARCH_NET_INFO_PRINT( "netif_device_attach net %pM\n",  net->dev_addr);

    if (usbNet->usbnetInfo.usbFlags & FLAG_LINK_INTR)
    {
        HARCH_NET_INFO_PRINT();
        UsbnetAdapterLinkChange(usbNet, 0, 0);
    }
    return HDF_SUCCESS;

out0:
    cancel_work_sync(&usbNet->TxCompleteWorkqueue);
    cancel_work_sync(&usbNet->RxCompleteWorkqueue);
    free_netdev(net);
out:
    return ret;
}

static int32_t UsbnetAdapterDisconnect(struct HdfDeviceObject *device)
{
    HARCH_NET_INFO_PRINT("begin");
    if (device == NULL) {
        HDF_LOGI("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    //free net
    struct net_device *net = (struct net_device *)device->priv;
    unregister_netdev (net);

    struct UsbnetAdapter *usbNet = netdev_priv(net);
    if (usbNet->stats64)
    {
        free_percpu(usbNet->stats64);
    }
    
    UsbnetAdapterTerminateUrbs(usbNet);
    skb_queue_purge(&usbNet->rxq_pause);
    OsalMutexDestroy(&usbNet->sendSkbClock);
   
    free_netdev(net);
    return HDF_SUCCESS;
}

static int32_t UsbnetAdapterDispatch(
    struct HdfDeviceIoClient *client, int32_t cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HARCH_NET_INFO_PRINT("begin, received cmd = %d", cmd);

    int32_t ret = HDF_ERR_INVALID_PARAM;
    switch (cmd)
    {
        case USB_NET_REGISTER_NET:
        {
            //add new usbnet device
            ret = UsbnetAdapterProbe(client->device, data);
        } 
            break;
        case USB_NET_CLOSE_NET:
        {
            ret = UsbnetAdapterDisconnect(client->device);
        }
                break;
        case USB_NET_RECIVE_DATA_FROM_USB:
        {
            ret = UsbnetAdapterRxComplete(client->device, data, GFP_ATOMIC);
        }
            break;
        default:
            HDF_LOGI("%s: no this cmd: %d", __func__, cmd);
            break;
    }

    if (!HdfSbufWriteInt32(reply, ret)) {
        HDF_LOGE("%s: reply int32 fail", __func__);
    }
    
    return ret;
}

static int32_t UsbnetAdapterBind(struct HdfDeviceObject *device)
{
    HARCH_NET_INFO_PRINT("begin");
    static struct IDeviceIoService UsbnetAdapterService = {
        .Dispatch = UsbnetAdapterDispatch,
    };

    if (device == NULL) {
        HARCH_NET_INFO_PRINT("device is NULL!");
        return HDF_ERR_INVALID_OBJECT;
    }
    device->service = &UsbnetAdapterService;
    return HDF_SUCCESS;
}

static int32_t UsbnetAdapterInit(struct HdfDeviceObject *device)
{
    HARCH_NET_INFO_PRINT("begin");
    if (device == NULL) {
        HARCH_NET_INFO_PRINT("device is null!");
        return HDF_FAILURE;
    }

    HARCH_NET_INFO_PRINT("UsbnetAdapterInit Init success");
    return HDF_SUCCESS;
}

static void UsbnetAdapterRelease(struct HdfDeviceObject *device)
{
    HARCH_NET_INFO_PRINT("begin");
    return;
}

struct HdfDriverEntry g_UsbnetAdapterEntry = {
    .moduleVersion = 1,
    .Bind = UsbnetAdapterBind,
    .Init = UsbnetAdapterInit,
    .Release = UsbnetAdapterRelease,
    .moduleName = "HDF_USB_NET",
};

HDF_INIT(g_UsbnetAdapterEntry);
