/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "spi_test.h"
#include "device_resource_if.h"
#include "hdf_io_service_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "spi_if.h"
#include "securec.h"


#define HDF_LOG_TAG spi_test_c

#define SPI_TEST_4BITS  4
#define SPI_TEST_8BITS  8
#define SPI_TEST_16BITS 16

static int32_t SpiTestGetConfig(struct SpiTestConfig *config)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    const void *buf = NULL;
    uint32_t len;

    service = HdfIoServiceBind("SPI_TEST");
    if (service == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    reply = HdfSBufObtain(sizeof(*config) + sizeof(uint64_t));
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, 0, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: remote dispatch failed", __func__);
        return ret;
    }

    if (!HdfSbufReadBuffer(reply, &buf, &len)) {
        HDF_LOGE("%s: read buf failed", __func__);
        HdfSBufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (len != sizeof(*config)) {
        HDF_LOGE("%s: config size:%zu, read size:%u", __func__, sizeof(*config), len);
        HdfSBufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (memcpy_s(config, sizeof(*config), buf, sizeof(*config)) != EOK) {
        HDF_LOGE("%s: memcpy buf failed", __func__);
        HdfSBufRecycle(reply);
        return HDF_ERR_IO;
    }

    HdfSBufRecycle(reply);
    HDF_LOGD("%s: done", __func__);
    return HDF_SUCCESS;
}

struct SpiTester *SpiTesterGet(void)
{
    int32_t ret;
    static struct SpiTester tester;
    struct SpiDevInfo info;

    ret = SpiTestGetConfig(&tester.config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed:%d", __func__, ret);
        return NULL;
    }
    info.busNum = tester.config.bus;
    info.csNum = tester.config.cs;
    tester.handle = SpiOpen(&info);
    if (tester.handle == NULL) {
        HDF_LOGE("%s: open spi: %u, cs: %u failed!", __func__, tester.config.bus, tester.config.cs);
        return NULL;
    }
    return &tester;
}

void SpiTesterPut(struct SpiTester *tester)
{
    SpiClose(tester);
}

#define BITS_PER_WORD_DEFAULT    8
#define BITS_PER_WORD_8BITS      8
#define BITS_PER_WORD_10BITS     10
#define MAX_SPEED_HZ             10000000

static struct SpiCfg g_spiCfg = {
    .mode = SPI_CLK_PHASE | SPI_MODE_LOOP,
    .bitsPerWord = BITS_PER_WORD_DEFAULT,
    .maxSpeedHz = MAX_SPEED_HZ,
    .transferMode = SPI_POLLING_TRANSFER,
};

#define SPI_TEST_ONE_BYTE 1
#define SPI_TEST_TWO_BYTE 2

static int32_t SpiCmpMemByBits(uint8_t *wbuf, uint8_t *rbuf, uint32_t len, uint8_t bits)
{
    uint32_t i;
    uint16_t vw;
    uint16_t vr;

    if (bits < SPI_TEST_4BITS) {
        bits = SPI_TEST_4BITS;
    } else if (bits > SPI_TEST_16BITS) {
        bits = SPI_TEST_16BITS;
    }

    for (i = 0; i < len;) {
        if (bits <= SPI_TEST_8BITS) {
            vw = *((uint8_t *)(wbuf + i)) & (~(0xFFFF << bits));
            vr = *((uint8_t *)(rbuf + i)) & (~(0xFFFF << bits));
        } else {
            vw = *((uint16_t *)(wbuf + i)) & (~(0xFFFF << bits));
            vr = *((uint16_t *)(rbuf + i)) & (~(0xFFFF << bits));
        }
        if (vw != vr) {
            HDF_LOGE("%s: compare mem fail(i=%d, vw=%u, vr=%u, bits = %u, len=%u)",
                __func__, i, vw, vr, bits, len);
            return HDF_FAILURE;
        }
        i += (bits <= SPI_TEST_8BITS) ? SPI_TEST_ONE_BYTE : SPI_TEST_TWO_BYTE;
    }
    HDF_LOGE("%s: mem size(%u) compare success", __func__, len);
    return HDF_SUCCESS;
}

static int32_t SpiDoTransferTest(struct SpiTester *tester, struct SpiCfg *cfg, struct SpiMsg *msg)
{
    int32_t ret;

    ret = SpiSetCfg(tester->handle, cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set config fail", __func__);
        return ret;
    }

    ret = SpiTransfer(tester->handle, msg, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: spi transfer err", __func__);
        return ret;
    }

    ret = SpiCmpMemByBits(msg->wbuf, msg->rbuf, msg->len, g_spiCfg.bitsPerWord);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    ret = SpiWrite(tester->handle, msg->wbuf, msg->len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: spi write err", __func__);
        return ret;
    }

    ret = SpiRead(tester->handle, msg->rbuf, msg->len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: spi read err", __func__);
        return ret;
    }

    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t SpiTransferTest(struct SpiTester *tester)
{
    int32_t ret;
    struct SpiMsg msg;

    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    g_spiCfg.bitsPerWord = BITS_PER_WORD_8BITS;
    g_spiCfg.transferMode = SPI_POLLING_TRANSFER;

    msg.rbuf = tester->config.rbuf;
    msg.wbuf = tester->config.wbuf;
    msg.len = tester->config.len;
    msg.csChange = 1; // switch off the CS after transfer
    msg.delayUs = 0;
    msg.speed = 0;    // use default speed

    ret = SpiDoTransferTest(tester, &g_spiCfg, &msg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail, bitsPerWord = %u, ret = %d", __func__, g_spiCfg.bitsPerWord, ret);
        return ret;
    }

    g_spiCfg.bitsPerWord = BITS_PER_WORD_10BITS;
    ret = SpiDoTransferTest(tester, &g_spiCfg, &msg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail, bitsPerWord = %u, ret = %d", __func__, g_spiCfg.bitsPerWord, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

#define DMA_TRANSFER_SINGLE_MAX   (1024 * 64 - 1)
#define DMA_TRANSFER_SIZE_TOTAL   (DMA_TRANSFER_SINGLE_MAX * 2 + 65532)
#define DMA_TRANSFER_BUF_SEED     0x5A
#define DMA_ALIGN_SIZE            64

static int32_t SpiSetDmaIntMsg(struct SpiMsg *msg, uint32_t len)
{
    uint32_t i;
    uint8_t *wbuf = NULL;
    uint8_t *rbuf = NULL;

    msg->wbuf = msg->rbuf = NULL;

    wbuf = (uint8_t *)OsalMemAllocAlign(DMA_ALIGN_SIZE, len);
    if (wbuf == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }
    rbuf = (uint8_t *)OsalMemAllocAlign(DMA_ALIGN_SIZE, len);
    if (rbuf == NULL) {
        OsalMemFree(wbuf);
        return HDF_ERR_MALLOC_FAIL;
    }

    wbuf[0] = DMA_TRANSFER_BUF_SEED;
    for (i = 1; i < len; i++) {
        wbuf[i] = wbuf[i - 1] + 1;
        rbuf[i] = 0;
    }
    msg->wbuf = wbuf;
    msg->rbuf = rbuf;
    msg->len = len;
    msg->csChange = 1;
    msg->delayUs = 0,  // switch off the CS after transfer
    msg->speed = 0;    // using default speed
    return HDF_SUCCESS;
}

static void SpiUnsetDmaIntMsg(struct SpiMsg *msg)
{
    if (msg != NULL) {
        OsalMemFree(msg->wbuf);
        OsalMemFree(msg->rbuf);
        msg->wbuf = NULL;
        msg->rbuf = NULL;
    }
}

static int32_t SpiDmaTransferTest(struct SpiTester *tester)
{
    int32_t ret;
    struct SpiMsg msg;

    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    g_spiCfg.transferMode = SPI_DMA_TRANSFER;
    g_spiCfg.bitsPerWord = BITS_PER_WORD_8BITS;

    if (tester->config.testDma == 0) {
        HDF_LOGI("%s: testDma not set", __func__);
        return HDF_SUCCESS;
    }

    ret = SpiSetDmaIntMsg(&msg, DMA_TRANSFER_SIZE_TOTAL);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    ret = SpiDoTransferTest(tester, &g_spiCfg, &msg);
    if (ret != HDF_SUCCESS) {
        SpiUnsetDmaIntMsg(&msg);
        HDF_LOGE("%s: fail, bitsPerWord = %u, ret = %d", __func__, g_spiCfg.bitsPerWord, ret);
        return ret;
    }

    g_spiCfg.bitsPerWord = BITS_PER_WORD_10BITS;
    ret = SpiDoTransferTest(tester, &g_spiCfg, &msg);
    if (ret != HDF_SUCCESS) {
        SpiUnsetDmaIntMsg(&msg);
        HDF_LOGE("%s: fail, bitsPerWord = %u, ret = %d", __func__, g_spiCfg.bitsPerWord, ret);
        return ret;
    }

    SpiUnsetDmaIntMsg(&msg);
    return HDF_SUCCESS;
}

static int32_t SpiIntTransferTest(struct SpiTester *tester)
{
    int32_t ret;
    struct SpiMsg msg;

    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    g_spiCfg.transferMode = SPI_INTERRUPT_TRANSFER;
    g_spiCfg.bitsPerWord = BITS_PER_WORD_8BITS;

    ret = SpiSetDmaIntMsg(&msg, DMA_TRANSFER_SIZE_TOTAL);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    ret = SpiDoTransferTest(tester, &g_spiCfg, &msg);
    if (ret != HDF_SUCCESS) {
        SpiUnsetDmaIntMsg(&msg);
        HDF_LOGE("%s: fail, bitsPerWord = %u, ret = %d", __func__, g_spiCfg.bitsPerWord, ret);
        return ret;
    }

    g_spiCfg.bitsPerWord = BITS_PER_WORD_10BITS;
    ret = SpiDoTransferTest(tester, &g_spiCfg, &msg);
    if (ret != HDF_SUCCESS) {
        SpiUnsetDmaIntMsg(&msg);
        HDF_LOGE("%s: fail, bitsPerWord = %u, ret = %d", __func__, g_spiCfg.bitsPerWord, ret);
        return ret;
    }

    SpiUnsetDmaIntMsg(&msg);
    return HDF_SUCCESS;
}

static int32_t SpiReliabilityTest(struct SpiTester *tester)
{
    struct SpiCfg cfg = {0};
    struct SpiMsg msg = {0};

    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)SpiSetCfg(tester->handle, &cfg);
    (void)SpiSetCfg(tester->handle, NULL);
    (void)SpiTransfer(tester->handle, &msg, 1);
    (void)SpiTransfer(tester->handle, NULL, -1);
    (void)SpiWrite(tester->handle, tester->config.wbuf, tester->config.len);
    (void)SpiWrite(tester->handle, NULL, -1);
    (void)SpiRead(tester->handle, tester->config.rbuf, tester->config.len);
    (void)SpiRead(tester->handle, NULL, -1);

    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

struct SpiTestEntry {
    int cmd;
    int32_t (*func)(struct SpiTester *tester);
    const char *name;
};

static struct SpiTestEntry g_spiTestEntry[] = {
    {SPI_TRANSFER_TEST, SpiTransferTest, "SpiTransferTest"},
    {SPI_DMA_TRANSFER_TEST, SpiDmaTransferTest, "SpiDmaTransferTest"},
    {SPI_INT_TRANSFER_TEST, SpiIntTransferTest, "SpiIntTransferTest"},
    {SPI_RELIABILITY_TEST, SpiReliabilityTest, "SpiIntTransferTest"},
    {SPI_PERFORMANCE_TEST, NULL, "SpiPerformanceTest"},
};

int32_t SpiTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret= HDF_ERR_NOT_SUPPORT;
    struct SpiTester *tester = NULL;

    tester = SpiTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (cmd > SPI_TEST_CMD_MAX) {
        HDF_LOGE("%s: invalid cmd:%d", __func__, cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        goto __EXIT__;
    }

    for (i = 0; i < sizeof(g_spiTestEntry) / sizeof(g_spiTestEntry[0]); i++) {
        if (g_spiTestEntry[i].cmd != cmd || g_spiTestEntry[i].func == NULL) {
            continue;
        }
        ret = g_spiTestEntry[i].func(tester);
        break;
    }

__EXIT__:
    HDF_LOGE("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
    SpiTesterPut(tester);

    return ret;    
}

void SpiTestExecuteAll(void)
{
    int32_t i;
    int32_t ret;
    int32_t fails = 0;

    for (i = 0; i < SPI_TEST_CMD_MAX - 1; i++) {
        ret = SpiTestExecute(i);
        fails += (ret != HDF_SUCCESS) ? 1 : 0;
    }

    HDF_LOGI("%s: **********PASS:%d  FAIL:%d************\n\n",
        __func__, SPI_TEST_CMD_MAX - fails, fails);
}