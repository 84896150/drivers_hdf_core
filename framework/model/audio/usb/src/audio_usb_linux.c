/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_usb_linux.h"
#include <linux/bitops.h>
#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/usb.h>
#include <linux/usb/audio-v2.h>
#include <linux/usb/audio-v3.h>
#include <linux/usb/audio.h>

#include "audio_driver_log.h"
#include "audio_usb_endpoints.h"
#include "audio_usb_parse_interface.h"
#include "audio_usb_validate_desc.h"
#include "devmgr_service_clnt.h"
#include "devsvc_manager_clnt.h"
#include "hdf_device_object.h"
#include "osal_time.h"

#define HDF_LOG_TAG HDF_AUDIO_USB

#define HDF_AUDIO_USB_CODEC_NAME       "hdf_audio_codec_usb_dev0"
#define HDF_AUDIO_USB_PNP_SRV_NAME     "audio_usb_service_0"
#define HDF_AUDIO_USB_DMA_NAME         "usb_dma_service_0"
#define WAIT_FOR_HDF_START_MILLISECOND 500

#define USB_SHIFT_SIZE_16 16

static struct AudioUsbDriver g_hdfAudioUsbDriver;

struct AudioUsbDriver *GetLinuxAudioUsb(void)
{
    return &g_hdfAudioUsbDriver;
}

uint32_t AudioUsbGetUsbId(uint32_t vendor, uint32_t product)
{
    return ((vendor) << USB_SHIFT_SIZE_16) | (product);
}

static int32_t AudioUsbDriverLoad(const char *serviceName)
{
    struct AudioUsbDriver *usbDriver = NULL;
    struct DevmgrServiceClnt *hdf = NULL;
    int32_t ret;

    if (serviceName == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    usbDriver = GetLinuxAudioUsb();
    if (usbDriver == NULL) {
        ADM_LOG_ERR("get linux audio usb driver fail!");
        return HDF_ERR_INVALID_OBJECT;
    }

    hdf = DevmgrServiceClntGetInstance();
    if ((hdf == NULL || hdf->devMgrSvcIf == NULL || hdf->devMgrSvcIf->LoadDevice == NULL)) {
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = hdf->devMgrSvcIf->LoadDevice(hdf->devMgrSvcIf, serviceName);
    if (ret == HDF_SUCCESS) {
        ADM_LOG_INFO("serviceName[%s] load success", serviceName);
        usbDriver->pnpFlag = 1;
    } else if (ret == HDF_ERR_DEVICE_BUSY) {
        ADM_LOG_INFO("serviceName[%s] has been loaded", serviceName);
    } else {
        ADM_LOG_ERR("serviceName[%s] load fail", serviceName);
        usbDriver->pnpFlag = 0;
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t AudioUsbDriverInit(
    struct AudioUsbDriver *audioUsbDriver, struct usb_interface *usbIf, const struct usb_device_id *usbDevId)
{
    struct usb_device *usbDev = interface_to_usbdev(usbIf);
    struct usb_host_interface *alts;
    uint32_t i, ifnum, protocol;
    struct usb_host_interface *hostIface;
    struct usb_interface_descriptor *altsd;
    struct uac1_ac_header_descriptor *h1;

    alts = &usbIf->altsetting[0];
    ifnum = AUDIO_USB_GET_IFACE_DESC(alts)->bInterfaceNumber;
    hostIface = &usb_ifnum_to_if(usbDev, ifnum)->altsetting[0];
    altsd = AUDIO_USB_GET_IFACE_DESC(hostIface);
    protocol = altsd->bInterfaceProtocol;

    audioUsbDriver->pnpFlag = -1;
    audioUsbDriver->usbDevId = usbDevId;
    audioUsbDriver->usbIf = usbIf;
    audioUsbDriver->usb_id =
        AudioUsbGetUsbId(le16_to_cpu(usbDev->descriptor.idVendor), le16_to_cpu(usbDev->descriptor.idProduct));
    audioUsbDriver->sampleRateReadError = 0;
    audioUsbDriver->ctrlIntf = alts;
    audioUsbDriver->setup = 0;
    audioUsbDriver->dev = usbDev;
    audioUsbDriver->ifNum = ifnum;
    init_waitqueue_head(&audioUsbDriver->shutdownWait);

    spin_lock_init(&audioUsbDriver->lock);

    atomic_set(&audioUsbDriver->active, 1); /* avoid autopm during probing */
    atomic_set(&audioUsbDriver->usageCount, 0);
    atomic_set(&audioUsbDriver->shutdown, 0);

    DListHeadInit(&audioUsbDriver->renderUsbFormatList);
    DListHeadInit(&audioUsbDriver->captureUsbFormatList);

    switch (protocol) {
        case UAC_VERSION_1: {
            h1 = AudioUsbFindCsintDesc(alts->extra, alts->extralen, NULL, UAC_HEADER);
            if (!h1 || h1->bLength < sizeof(*h1)) {
                ADM_LOG_ERR("cannot find UAC_HEADER\n");
                return -EINVAL;
            }

            for (i = 0; i < h1->bInCollection; i++) {
                AudioUsbParseAudioInterface(audioUsbDriver, h1->baInterfaceNr[i]);
            }
            break;
        }
        default:
            ADM_LOG_ERR("usb version is not support");
            return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

static int32_t LinuxAudioUsbProbe(struct usb_interface *usbIf, const struct usb_device_id *usbDevId)
{
    int32_t ret;
    ADM_LOG_INFO("entry");

    (void)memset_s(&g_hdfAudioUsbDriver, sizeof(struct AudioUsbDriver), 0, sizeof(struct AudioUsbDriver));
    ret = AudioUsbDriverInit(&g_hdfAudioUsbDriver, usbIf, usbDevId);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("AudioUsbDriverInit is failed");
        return HDF_FAILURE;
    }

    while (g_hdfAudioUsbDriver.pnpFlag != 1) { // 1 is hdf audio usb load success
        OsalMSleep(WAIT_FOR_HDF_START_MILLISECOND);
        if (AudioUsbDriverLoad(HDF_AUDIO_USB_PNP_SRV_NAME) == 0 && AudioUsbDriverLoad(HDF_AUDIO_USB_DMA_NAME) == 0 &&
            AudioUsbDriverLoad(HDF_AUDIO_USB_CODEC_NAME) == 0) {
            break;
        }
    }

    ADM_LOG_INFO("success");
    return 0;
}

static void LinuxAudioUsbDisconnect(struct usb_interface *usbIf)
{
    struct HdfDeviceObject *usbService = NULL;

    wait_event(g_hdfAudioUsbDriver.shutdownWait, !atomic_read(&g_hdfAudioUsbDriver.usageCount));

    usbService = DevSvcManagerClntGetDeviceObject(HDF_AUDIO_USB_DMA_NAME);
    HdfDeviceObjectRelease(usbService);
    usbService = DevSvcManagerClntGetDeviceObject(HDF_AUDIO_USB_CODEC_NAME);
    HdfDeviceObjectRelease(usbService);
    usbService = DevSvcManagerClntGetDeviceObject(HDF_AUDIO_USB_PNP_SRV_NAME);
    HdfDeviceObjectRelease(usbService);
    g_hdfAudioUsbDriver.pnpFlag = 0;
}

static int32_t LinuxAudioUsbSuspend(struct usb_interface *usbIf, pm_message_t message)
{
    ADM_LOG_INFO("[%s][%s]:[%d]\n", __FILE__, __func__, __LINE__);
    return 0;
}

static int32_t LinuxAudioUsbResume(struct usb_interface *usbIf)
{
    ADM_LOG_INFO("[%s][%s]:[%d]\n", __FILE__, __func__, __LINE__);
    return 0;
}

static int32_t LinuxAudioUsbResetResume(struct usb_interface *usbIf)
{
    ADM_LOG_INFO("[%s][%s]:[%d]\n", __FILE__, __func__, __LINE__);
    return 0;
}

/* Element 2 to Terminating */
static const struct usb_device_id g_linuxAudioUsbIds[2] = {
    {.bInterfaceClass = USB_CLASS_AUDIO,
     .match_flags = (USB_DEVICE_ID_MATCH_INT_CLASS | USB_DEVICE_ID_MATCH_INT_SUBCLASS),
     .bInterfaceSubClass = USB_SUBCLASS_AUDIOCONTROL},
};
MODULE_DEVICE_TABLE(usb, g_linuxAudioUsbIds);

static struct usb_driver g_linuxAudioUsbDriver = {
    .name = "hdf-audio-usb",
    .probe = LinuxAudioUsbProbe,
    .disconnect = LinuxAudioUsbDisconnect,
    .suspend = LinuxAudioUsbSuspend,
    .resume = LinuxAudioUsbResume,
    .reset_resume = LinuxAudioUsbResetResume,
    .id_table = g_linuxAudioUsbIds,
    .supports_autosuspend = 1,
};
module_usb_driver(g_linuxAudioUsbDriver);
