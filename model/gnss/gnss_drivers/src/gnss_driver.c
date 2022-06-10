/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "osal_time.h"
#include "gnss_driver.h"

static struct GnssDrvData *g_gnssDrvData = NULL;
static struct GnssBusCfg *g_busCfg = NULL;
struct GnssDeviceInfo g_gnssDeviceInfo;

struct GnssDrvData *GetGnssDrvData(void)
{
    return g_gnssDrvData;
}

struct GnssBusCfg *GetGnssBusCfg(void)
{
    return g_busCfg;
}

int32_t InitGnssBusHandle(struct GnssBusCfg *busCfg)
{
    uint32_t busSpeed;
    if (busCfg == NULL) {
        GNSS_LOG_ERR("input param check failed");
        return HDF_ERR_INVALID_PARAM;
    }
    if (busCfg->busType == GNSS_BUS_UART) {
        int16_t busNum = busCfg->uartCfg.busNum;
        busCfg->uartCfg.handle = UartOpen(busNum);
        if (busCfg->uartCfg.handle == NULL) {
            GNSS_LOG_ERR("try to open uart %d first", busNum);
            return HDF_SUCCESS;
        }

        busSpeed = busCfg->uartCfg.busSpeed;
        if (UartSetBaud(busCfg->uartCfg.handle, busSpeed) != HDF_SUCCESS) {
            GNSS_LOG_ERR("UartSetBaud failed");
            return HDF_SUCCESS;
        }
    } else if (busCfg->busType == GNSS_BUS_I2C) {
        int16_t busNum = busCfg->i2cCfg.busNum;
        busCfg->i2cCfg.handle = I2cOpen(busNum);
        if (busCfg->i2cCfg.handle == NULL) {
            GNSS_LOG_ERR("Failed to open i2c%d", busNum);
            return HDF_FAILURE;
        }
    } else if (busCfg->busType == GNSS_BUS_SPI) {
        /*struct SpiDevInfo spiDevinfo;
        struct SpiCfg cfg;
        int32_t ret;
        int32_t busSpeed = busCfg->spiCfg.busSpeed;

        spiDevinfo.busNum = busCfg->spiCfg.busNum;
        spiDevinfo.csNum = busCfg->spiCfg.csNum;
        busCfg->spiCfg.handle = SpiOpen(&spiDevinfo);

        cfg.mode = SPI_CLK_PHASE | SPI_CLK_POLARITY;
        cfg.bitsPerWord = 8;
        cfg.maxSpeedHz = busSpeed;
        ret = SpiSetCfg(busCfg->spiCfg.handle, &cfg);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("SpiSetCfg failed");
            SpiClose(busCfg->spiCfg.handle);
            return ret;
        }*/
    }
    return HDF_SUCCESS;
}

static int32_t gnssDriverDispatch(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t GnssDriverOpen(struct GnssDeviceInfo *deviceInfo)
{
    struct GnssBusCfg *busCfg = GetGnssBusCfg();
    if (busCfg == NULL) {
        GNSS_LOG_ERR("GetGnssBusCfg return is NULL");
        return HDF_ERR_INVALID_PARAM;
    }

    switch (busCfg->busType) {
        case GNSS_BUS_UART:
            if (busCfg->uartCfg.handle == NULL) {
                GNSS_LOG_INFO("uart handle is null, reinitialize");
                InitGnssBusHandle(busCfg);
            } else {
                GNSS_LOG_INFO("GNSS_BUS_UART OK");
            }
            break;
        case GNSS_BUS_SPI:
            if (busCfg->spiCfg.handle == NULL) {
                GNSS_LOG_INFO("spi handle is null, reinitialize");
                InitGnssBusHandle(busCfg);
            } else {
                GNSS_LOG_INFO("GNSS_BUS_SPI OK");
            }
            break;
        case GNSS_BUS_I2C:
            if (busCfg->i2cCfg.handle == NULL) {
                GNSS_LOG_INFO("i2c handle is null, reinitialize");
                InitGnssBusHandle(busCfg);
            } else {
                GNSS_LOG_INFO("GNSS_BUS_I2C OK");
            }
            break;
        default:
            GNSS_LOG_ERR("unknown error");
            return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t ReleaseGnssBusHandle(struct GnssBusCfg *busCfg)
{
    if (busCfg == NULL) {
        return HDF_SUCCESS;
    }

    if (busCfg->busType == GNSS_BUS_UART && busCfg->uartCfg.handle != NULL) {
        UartClose(busCfg->uartCfg.handle);
        // I2cClose(busCfg->uartCfg.handle);
	    busCfg->uartCfg.handle = NULL;
    } else if (busCfg->busType == GNSS_BUS_SPI) {
        //SpiClose(busCfg->spiCfg.handle);
        busCfg->spiCfg.handle = NULL;
    } else if (busCfg->busType == GNSS_BUS_I2C) {
        I2cClose(busCfg->i2cCfg.handle);
        busCfg->i2cCfg.handle = NULL;
    }
    return HDF_SUCCESS;
}

void GnssReleaseCfgData(struct GnssCfgData *config)
{
    if (config == NULL) {
        GNSS_LOG_ERR("config no need release.");
        return;
    }

    (void)ReleaseGnssBusHandle(&config->busCfg);

    config->root = NULL;
    (void)memset_s(&config->gnssInfo, sizeof(struct GnssBasicInfo), 0, sizeof(struct GnssBasicInfo));
    (void)memset_s(&config->busCfg, sizeof(struct GnssBusCfg), 0, sizeof(struct GnssBusCfg));
}

int32_t GnssDriverClose(struct GnssDeviceInfo *deviceInfo)
{
    struct GnssBusCfg *busCfg = GetGnssBusCfg();
    if (busCfg == NULL) {
        GNSS_LOG_ERR("GetGnssBusCfg return is NULL");
        return HDF_ERR_INVALID_PARAM;
    }
    
    return ReleaseGnssBusHandle(busCfg);
}

int32_t GnssDriverReadUart(struct GnssBusCfg *busCfg, char *readData)
{
    int readDataLen = 0;
    int offset = 0;
    int statesNum= 0;
    int i = 0;
    int ret = 0;

    // 保证读数据完整性：读满1000字节，或者读到6个"$GP"字符串, 超时时间5秒
    uint64_t timeBefor = OsalGetSysTimeMs();
    uint64_t timeAfter = 0;

    while (1) {
        ret = UartRead(busCfg->uartCfg.handle, readData + offset, READ_GNSS_INFO_MAX_DATA_LEN);
        if (ret <= 0) {
            timeAfter = OsalGetSysTimeMs();
            if ((timeAfter - timeBefor) >= 5000) {
                return readDataLen;
            }
            continue;
        }
        readDataLen += ret;
        offset = readDataLen;
        if (readDataLen > READ_GNSS_INFO_MAX_DATA_LEN) {
            readDataLen = READ_GNSS_INFO_MAX_DATA_LEN;
            readData[readDataLen] = '\0';
            return readDataLen;
        }

        for (i = 0; i < readDataLen; i++) {
            if (strncmp(readData + i, GNSS_DATA_FLAG, 3) == 0) {
                statesNum++;
            }
            if (statesNum >= 6) {
                return readDataLen;
            }
        }
    }

    return readDataLen;
}

int32_t GnssDriverRead(struct GnssDeviceInfo *deviceInfo, char *resp, uint32_t dataLen, int32_t *replyLen)
{
    struct GnssBusCfg *busCfg = NULL;
    char readData[GNSS_ALL_INFO_MAX_DATA_LEN / 2 + 1] = {0};
    int32_t readDataLen = 0;

    busCfg = GetGnssBusCfg();
    if (busCfg == NULL) {
        GNSS_LOG_ERR("GetGnssBusCfg return is NULL");
        return HDF_ERR_INVALID_PARAM;
    }

    if (busCfg->busType == GNSS_BUS_UART && busCfg->uartCfg.handle != NULL) {
        readDataLen = GnssDriverReadUart(busCfg, readData);
        if (readDataLen <= 0) {
            GNSS_LOG_ERR("UartRead failed.");
            return HDF_FAILURE;
        }

        memcpy_s(resp, dataLen, readData, readDataLen);
        *replyLen = readDataLen;
        return HDF_SUCCESS;
    } else if (busCfg->busType == GNSS_BUS_SPI && busCfg->spiCfg.handle != NULL) {
        /*
        if (SpiRead(busCfg->spiCfg.handle, resp, dataLen) != HDF_SUCCESS) {
            GNSS_LOG_ERR("SpiRead failed.");
            return HDF_FAILURE;
        }
        strncpy_s(resp, dataLen, g_gpsData, strlen(g_gpsData));
	    *replyLen = strlen(g_gpsData);
        */
        return HDF_SUCCESS;
    } else if (busCfg->busType == GNSS_BUS_I2C && busCfg->i2cCfg.handle != NULL) {
        /*
        unsigned char regBuf[GNSS_I2C_BUS_REG_BUF_LEN] = {0};
        struct I2cMsg msg[GNSS_I2C_BUS_READ_MSG_NUM];
        int32_t index = 0;
        uint32_t regAddr = busCfg->i2cCfg.devAddr;
        // I2C addr buf prepare
        regBuf[index++] = (regAddr >> GNSS_I2C_BUS_BYTE_OFFSET) & GNSS_I2C_BUS_BYTE_MASK;
        regBuf[index++] = regAddr & GNSS_I2C_BUS_BYTE_MASK;
        
        // I2C addr msg info
        msg[GNSS_I2C_BUS_READ_MSG_ADDR_IDX].addr = busCfg->i2cCfg.devAddr;
        msg[GNSS_I2C_BUS_READ_MSG_ADDR_IDX].flags = 0;
        msg[GNSS_I2C_BUS_READ_MSG_ADDR_IDX].len = 2;
        msg[GNSS_I2C_BUS_READ_MSG_ADDR_IDX].buf = regBuf;

        // I2C addr data info
        msg[GNSS_I2C_BUS_READ_MSG_VALUE_IDX].addr = busCfg->i2cCfg.devAddr;
        msg[GNSS_I2C_BUS_READ_MSG_VALUE_IDX].flags = I2C_FLAG_READ;
        msg[GNSS_I2C_BUS_READ_MSG_VALUE_IDX].len = dataLen;
        msg[GNSS_I2C_BUS_READ_MSG_VALUE_IDX].buf = resp;

        if (I2cTransfer(busCfg->i2cCfg.handle, msg, GNSS_I2C_BUS_READ_MSG_NUM) != GNSS_I2C_BUS_READ_MSG_NUM) {
            GNSS_LOG_ERR("I2c[%u]Transfer  read failed", busCfg->i2cCfg.busNum);
            return HDF_FAILURE;
        }
        *replyLen = strlen(msg);
        */
        return HDF_SUCCESS;
    }
    return HDF_FAILURE;
}

int32_t GnssDriverWrite(struct GnssDeviceInfo *deviceInfo, char *req, uint32_t len)
{
    struct GnssBusCfg *busCfg = GetGnssBusCfg();
    if (busCfg == NULL) {
        GNSS_LOG_ERR("GetGnssBusCfg return is NULL");
        return HDF_ERR_INVALID_PARAM;
    } 
    if (busCfg->busType == GNSS_BUS_UART && busCfg->uartCfg.handle != NULL) {
        if (UartWrite(busCfg->uartCfg.handle, req, len) != 0) {
            GNSS_LOG_ERR("UartRead failed.");
            return HDF_FAILURE;
        }
        return HDF_SUCCESS;
    } else if (busCfg->busType == GNSS_BUS_SPI && busCfg->spiCfg.handle != NULL) {
        /*if (SpiWrite(busCfg->spiCfg.handle, req, len) != HDF_SUCCESS) {
            GNSS_LOG_ERR("SpiRead failed.");
            return HDF_FAILURE;
        }*/
        return HDF_SUCCESS;
    } else if (busCfg->busType == GNSS_BUS_I2C && busCfg->i2cCfg.handle != NULL) {
        unsigned char regBuf[GNSS_I2C_BUS_REG_BUF_LEN] = {0};
        struct I2cMsg msg[GNSS_I2C_BUS_WRITE_MSG_NUM];
        int32_t index = 0;
        uint32_t regAddr = busCfg->i2cCfg.devAddr;
        // I2C addr buf prepare
        regBuf[index++] = (regAddr >> GNSS_I2C_BUS_BYTE_OFFSET) & GNSS_I2C_BUS_BYTE_MASK;
        regBuf[index++] = regAddr & GNSS_I2C_BUS_BYTE_MASK;
        
        // I2C addr msg info
        msg[GNSS_I2C_BUS_WRITE_MSG_ADDR_IDX].addr = busCfg->i2cCfg.devAddr;
        msg[GNSS_I2C_BUS_WRITE_MSG_ADDR_IDX].flags = 0;
        msg[GNSS_I2C_BUS_WRITE_MSG_ADDR_IDX].len = 2;
        msg[GNSS_I2C_BUS_WRITE_MSG_ADDR_IDX].buf = regBuf;

        // I2C addr data info
        msg[GNSS_I2C_BUS_WRITE_MSG_VALUE_IDX].addr = busCfg->i2cCfg.devAddr;
        msg[GNSS_I2C_BUS_WRITE_MSG_VALUE_IDX].flags |= ~I2C_FLAG_READ;
        msg[GNSS_I2C_BUS_WRITE_MSG_VALUE_IDX].len = len;
        msg[GNSS_I2C_BUS_WRITE_MSG_VALUE_IDX].buf = req;

        if (I2cTransfer(busCfg->i2cCfg.handle, msg, GNSS_I2C_BUS_WRITE_MSG_NUM) != GNSS_I2C_BUS_WRITE_MSG_NUM) {
            GNSS_LOG_ERR("I2c[%u]Transfer  write failed", busCfg->i2cCfg.busNum);
            return HDF_FAILURE;
        }
        return HDF_SUCCESS;
    }
    return HDF_FAILURE;
}

int32_t GnssDriverSetPowerMode(struct GnssDeviceInfo *deviceInfo, uint32_t status)
{
    (void)deviceInfo;
    return HDF_SUCCESS;
}

static int32_t InitGnssOps(struct GnssCfgData *config, struct GnssDeviceInfo *deviceInfo)
{
    deviceInfo->Open = GnssDriverOpen;
    deviceInfo->Read = GnssDriverRead;
    deviceInfo->Write = GnssDriverWrite;
    deviceInfo->SetPowerMode = GnssDriverSetPowerMode;
    deviceInfo->Close = GnssDriverClose;
    GNSS_LOG_INFO("config->gnssInfo.gnssName = %s", config->gnssInfo.gnssName);
    GNSS_LOG_INFO("config->gnssInfo.vendorName = %s", config->gnssInfo.vendorName);
    GNSS_LOG_INFO("config->gnssInfo.gnssDeviceId = %d", config->gnssInfo.gnssDeviceId);
    if (memcpy_s(&deviceInfo->gnssInfo, sizeof(deviceInfo->gnssInfo),
        &config->gnssInfo, sizeof(config->gnssInfo)) != EOK) {
        GNSS_LOG_ERR("Copy gnss info failed");
        return HDF_FAILURE;
    }
    GNSS_LOG_INFO("deviceInfo->gnssInfo.gnssName = %s", deviceInfo->gnssInfo.gnssName);
    GNSS_LOG_INFO("deviceInfo->gnssInfo.vendorName = %s", deviceInfo->gnssInfo.vendorName);
    GNSS_LOG_INFO("deviceInfo->gnssInfo.gnssDeviceId = %d", deviceInfo->gnssInfo.gnssDeviceId);
    return HDF_SUCCESS;
}

int32_t InitGnssDevice(struct GnssCfgData *gnssCfg)
{   
    int32_t ret;
    g_busCfg = &(gnssCfg->busCfg);
    if (g_busCfg == NULL) {
        GNSS_LOG_ERR("gnssCfg->busCfg is NULL.");
        return HDF_ERR_INVALID_PARAM;
    }
    ret = InitGnssBusHandle(g_busCfg);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("GetGnssBusHandle failed.");
        return HDF_FAILURE;
    }

    ret = InitGnssOps(gnssCfg, &g_gnssDeviceInfo);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("InitGnssOps failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t GnssDriverBind(struct HdfDeviceObject *device)
{
    struct GnssDrvData *drvData;
    if (device == NULL) {
        GNSS_LOG_ERR("input param check failed");
        return HDF_ERR_INVALID_PARAM;
    }
    drvData = (struct GnssDrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        GNSS_LOG_ERR("malloc GnssDrvData  failed");
        return HDF_ERR_MALLOC_FAIL;
    }

    
    drvData->device = device;
    device->service = &(drvData->ioService);
    drvData->ioService.Dispatch = gnssDriverDispatch;
    g_gnssDrvData = drvData;

    return HDF_SUCCESS;
}

int32_t GnssDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct GnssDrvData *drvData = NULL;
    if (device == NULL) {
        GNSS_LOG_ERR("input param check failed");
        return HDF_ERR_INVALID_PARAM;
    }
    drvData = (struct GnssDrvData *)device->service;
    if (drvData == NULL) {
        GNSS_LOG_ERR("get drvData address failed");
        return HDF_ERR_INVALID_PARAM;
    }
    
    drvData->gnssCfg = (struct GnssCfgData *)OsalMemCalloc(sizeof(struct GnssCfgData));
    if (drvData->gnssCfg == NULL) {
        GNSS_LOG_ERR("malloc drvData->gnssCfg  failed");
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = GetGnssConfigData(device->property, drvData->gnssCfg);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("GetGnssConfigData failed (ret != HDF_FAILURE)");
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = InitGnssDevice(drvData->gnssCfg);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("Init gnss driver failed");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

void GnssDriverRelease(struct HdfDeviceObject *device)
{
    struct GnssDrvData *drvData = (struct GnssDrvData *)device->service;

    if (drvData->gnssCfg != NULL) { 
        GnssReleaseCfgData(drvData->gnssCfg);
        OsalMemFree(drvData->gnssCfg);
        drvData->gnssCfg = NULL;
    }
    OsalMemFree(drvData);
}

/* g_gnssDriverEntry definitions */
struct HdfDriverEntry g_gnssDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_GNSS_DEVICE",
    .Bind = GnssDriverBind,
    .Init = GnssDriverInit,
    .Release = GnssDriverRelease,
};
HDF_INIT(g_gnssDriverEntry);
