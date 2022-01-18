/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "uart_test.h"
#include "device_resource_if.h"
#include "hdf_base.h"
//#include "hdf_device_desc.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "uart_if.h"

static int32_t UartTestGetConfig(struct UartTestConfig *config)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    const void *buf = NULL;
    uint32_t len;

    service = HdfIoServiceBind("UART_TEST");
    if (service == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    reply = HdfSBufObtain(sizeof(*config) + sizeof(uint64_t));
    if (reply == NULL) {
        HDF_LOGE("%s: Failed to obtain reply", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, 0, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Remote dispatch failed", __func__);
        return ret;
    }

    if (!HdfSbufReadBuffer(reply, &buf, &len)) {
        HDF_LOGE("%s: Read buf failed", __func__);
        HdfSBufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (len != sizeof(*config)) {
        HDF_LOGE("%s: Config size:%zu, read size:%u", __func__, sizeof(*config), len);
        HdfSBufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (memcpy_s(config, sizeof(*config), buf, sizeof(*config)) != EOK) {
        HDF_LOGE("%s: Memcpy buf failed", __func__);
        HdfSBufRecycle(reply);
        return HDF_ERR_IO;
    }

    HdfSBufRecycle(reply);
    HDF_LOGD("%s: Done", __func__);
    return HDF_SUCCESS;
}

struct UartTester *UartTesterGet(void)
{
    int32_t ret;
    static struct UartTester tester;
    static bool hasInit = false;

    if (hasInit) {
        return &tester;
    }
    ret = UartTestGetConfig(&tester.config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed:%d", __func__, ret);
        return NULL;
    }
	tester.handle = UartOpen(tester.config.port);
    if (tester.handle == NULL) {
        HDF_LOGE("%s: open uart port:%u fail!", __func__, tester.config.port);
        return NULL;
    }
    hasInit = true;
    return &tester;
}

static int32_t UartWriteTest(void)
{
    struct UartTester *tester = NULL;
    int32_t ret;

    tester = UartTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = UartWrite(tester->handle, tester->config.wbuf, tester->config.len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: write failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t UartReadTest(void)
{	
    struct UartTester *tester = NULL;
    int32_t ret;

    tester = UartTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
	ret = UartSetTransMode(tester->handle, UART_MODE_RD_NONBLOCK);
    if (UartSetTransMode(tester->handle, UART_MODE_RD_NONBLOCK) != HDF_SUCCESS) {
        HDF_LOGE("%s: transmode error.", __func__);
        return HDF_FAILURE;
    }
    ret = UartRead(tester->handle, tester->config.rbuf, tester->config.len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

#define BAUD_921600 921600
static int32_t UartSetBaudTest(void)
{
    struct UartTester *tester = NULL;
    int32_t ret;

    tester = UartTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = UartSetBaud(tester->handle, BAUD_921600);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set baud failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t UartGetBaudTest(void)
{
    struct UartTester *tester = NULL;
    int32_t ret;
    uint32_t baud;

    tester = UartTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = UartGetBaud(tester->handle, &baud);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get baud failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: baud %u success", __func__, baud);
    return HDF_SUCCESS;
}

static int32_t UartSetAttributeTest(void)
{
    struct UartTester *tester = NULL;
    struct UartAttribute attribute;
    int32_t ret;

    tester = UartTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    attribute.dataBits = UART_ATTR_DATABIT_7;
    attribute.parity = UART_ATTR_PARITY_NONE;
    attribute.stopBits = UART_ATTR_STOPBIT_1;
    attribute.rts = UART_ATTR_RTS_DIS;
    attribute.cts = UART_ATTR_CTS_DIS;
    attribute.fifoRxEn = UART_ATTR_RX_FIFO_EN;
    attribute.fifoTxEn = UART_ATTR_TX_FIFO_EN;
    ret = UartSetAttribute(tester->handle, &attribute);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set attribute failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t UartGetAttributeTest(void)
{
    struct UartTester *tester = NULL;
    struct UartAttribute attribute;
    int32_t ret;

    tester = UartTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = UartGetAttribute(tester->handle, &attribute);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get attribute failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("dataBits %u", attribute.dataBits);
    HDF_LOGD("parity %u", attribute.parity);
    HDF_LOGD("stopBits %u", attribute.stopBits);
    HDF_LOGD("rts %u", attribute.rts);
    HDF_LOGD("cts %u", attribute.cts);
    HDF_LOGD("fifoRxEn %u", attribute.fifoRxEn);
    HDF_LOGD("fifoTxEn %u", attribute.fifoTxEn);
    HDF_LOGD("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t UartSetTransModeTest(void)
{
    struct UartTester *tester;
    int32_t ret;

    tester = UartTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
	ret = UartSetTransMode(tester->handle, UART_MODE_RD_NONBLOCK);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set transmode failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t UartReliabilityTest(void)
{
    uint32_t baud;
    struct UartTester *tester = NULL;
    struct UartAttribute attribute = {0};

    tester = UartTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)UartSetTransMode(tester->handle, UART_MODE_RD_NONBLOCK);
    (void)UartSetTransMode(tester->handle, -1);
    (void)UartWrite(tester->handle, tester->config.wbuf, tester->config.len);
    (void)UartWrite(tester->handle, NULL, -1);
    (void)UartRead(tester->handle, tester->config.rbuf, tester->config.len);
    (void)UartRead(tester->handle, NULL, -1);
    (void)UartSetBaud(tester->handle, BAUD_921600);
    (void)UartSetBaud(tester->handle, -1);
    (void)UartGetBaud(tester->handle, &baud);
    (void)UartGetBaud(tester->handle, NULL);
    (void)UartSetAttribute(tester->handle, &attribute);
    (void)UartSetAttribute(tester->handle, NULL);
    (void)UartGetAttribute(tester->handle, &attribute);
    (void)UartGetAttribute(tester->handle, NULL);
    HDF_LOGD("%s: success", __func__);

    return HDF_SUCCESS;
}

struct UartTestEntry {
    int cmd;
    int32_t (*func)(void);
    const char *name;
};

static struct UartTestEntry g_entry[] = {
    { UART_TEST_CMD_WRITE, UartWriteTest, "UartWriteTest" },
    { UART_TEST_CMD_READ, UartReadTest, "UartReadTest" },
    { UART_TEST_CMD_SET_BAUD, UartSetBaudTest, "UartSetBaudTest" },
    { UART_TEST_CMD_GET_BAUD, UartGetBaudTest, "UartGetBaudTest" },
    { UART_TEST_CMD_SET_ATTRIBUTE, UartSetAttributeTest, "UartSetAttributeTest" },
    { UART_TEST_CMD_GET_ATTRIBUTE, UartGetAttributeTest, "UartGetAttributeTest" },
    { UART_TEST_CMD_SET_TRANSMODE, UartSetTransModeTest, "UartSetTransModeTest" },
    { UART_TEST_CMD_RELIABILITY, UartReliabilityTest, "UartReliabilityTest" },
    { UART_TEST_CMD_PERFORMANCE, NULL, "NULL" },
};

int32_t UartTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;
    
    if (cmd > UART_TEST_CMD_MAX) {
        HDF_LOGE("%s: invalid cmd:%d", __func__, cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        goto __EXIT__;
    }

    for (i = 0; i < sizeof(g_entry) / sizeof(g_entry[0]); i++) {
        if (g_entry[i].cmd != cmd || g_entry[i].func == NULL) {
            continue;
        }
        ret = g_entry[i].func();
        break;
    }

__EXIT__:
    HDF_LOGE("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
    return ret;    
}

void UartTestExecuteAll(void)
{
    int32_t i;
    int32_t ret;
    int32_t fails = 0;

    for (i = 0; i < UART_TEST_CMD_MAX - 1; i++) {
        ret = UartTestExecute(i);
        fails += (ret != HDF_SUCCESS) ? 1 : 0;
    }

    HDF_LOGI("%s: **********PASS:%d  FAIL:%d************\n\n",
        __func__, UART_TEST_CMD_PERFORMANCE + 1 - fails, fails);
}
