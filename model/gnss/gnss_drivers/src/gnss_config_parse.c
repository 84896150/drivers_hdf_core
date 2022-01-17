/*
 * Copyright (C) 2021 HiHope Open Source Organization.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <securec.h>
#include "device_resource_if.h"
#include "osal_mem.h"
#include "gnss_driver.h"

static int32_t ParseGnssInfo(struct DeviceResourceIface *parser, const struct DeviceResourceNode *infoNode,
    struct GnssCfgData *config)
{
    int32_t ret;
    const char *name = NULL;

    ret = parser->GetString(infoNode, "gnssName", &name, NULL);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("get gnssName failed");
        return HDF_FAILURE;
    }
    if (strcpy_s(config->gnssInfo.gnssName, GNSS_INFO_NAME_MAX_LEN, name) != EOK) {
        GNSS_LOG_ERR("copy gnssName failed!");
        return HDF_FAILURE;
    }
    GNSS_LOG_ERR("config->gnssInfo.gnssName = %s", config->gnssInfo.gnssName);
    ret = parser->GetString(infoNode, "vendorName", &name, NULL);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("get vendname failed");
        return HDF_FAILURE;
    }
    if (strcpy_s(config->gnssInfo.vendorName, GNSS_INFO_NAME_MAX_LEN, name) != EOK) {
        GNSS_LOG_ERR("copy vendorName failed!");
        return HDF_FAILURE;
    }
    GNSS_LOG_ERR("config->gnssInfo.vendorName = %s", config->gnssInfo.vendorName);
    ret = parser->GetUint32(infoNode, "gnssDeviceId", &config->gnssInfo.gnssDeviceId, 0);
     if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("get config->gnssInfo.gnssDeviceId failed");
        return HDF_FAILURE;
    }
    GNSS_LOG_ERR("config->gnssInfo.gnssDeviceId = %d", config->gnssInfo.gnssDeviceId);
    return ret;
}

static int32_t ParseGnssBus(struct DeviceResourceIface *parser, const struct DeviceResourceNode *busNode,
    struct GnssCfgData *config)
{
    int32_t ret;

    ret = parser->GetUint8(busNode, "busType", &config->busCfg.busType, 0);
    if (ret != HDF_SUCCESS) {
        GNSS_LOG_ERR("get config->busCfg.busType failed");
        return HDF_FAILURE;
    }

    if (config->busCfg.busType == GNSS_BUS_UART) {
        ret = parser->GetUint32(busNode, "busNum", &config->busCfg.uartCfg.busNum, 0);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("get busCfg.uartCfg.busNum failed");
            return HDF_FAILURE;
        }
        GNSS_LOG_ERR("config->busCfg.uartCfg.busNum = %d", config->busCfg.uartCfg.busNum);
        ret = parser->GetUint32(busNode, "busSpeed", &config->busCfg.uartCfg.busSpeed, 0);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("get busCfg.uartCfg.busSpeed failed");
            return HDF_FAILURE;
        }
        GNSS_LOG_ERR("config->busCfg.uartCfg.busSpeed = %d", config->busCfg.uartCfg.busSpeed);
        ret = parser->GetUint32(busNode, "busAddr", &config->busCfg.uartCfg.reserved, 0);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("get busCfg.uartCfg.reserved failed");
            return HDF_FAILURE;
        }
        GNSS_LOG_ERR("config->busCfg.uartCfg.busAddr = %d", config->busCfg.uartCfg.reserved);

    } else if (config->busCfg.busType == GNSS_BUS_I2C) {
        ret = parser->GetUint32(busNode, "busNum", &config->busCfg.i2cCfg.busNum, 0);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("get busCfg.i2cCfg.busNum failed");
            return HDF_FAILURE;
        }
        ret = parser->GetUint32(busNode, "busSpeed", &config->busCfg.i2cCfg.busSpeed, 0);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("get busCfg.i2cCfg.busSpeed failed");
            return HDF_FAILURE;
        }
        ret = parser->GetUint32(busNode, "busAddr", &config->busCfg.i2cCfg.devAddr, 0);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("get busCfg.i2cCfg.devAddr failed");
            return HDF_FAILURE;
        }
    } else if (config->busCfg.busType == GNSS_BUS_SPI) {
        ret = parser->GetUint32(busNode, "busNum", &config->busCfg.spiCfg.busNum, 0);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("get busCfg.spiCfg.busNum failed");
            return HDF_FAILURE;
        }
        ret = parser->GetUint32(busNode, "busSpeed", &config->busCfg.spiCfg.busSpeed, 0);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("get busCfg.spiCfg.busSpeed failed");
            return HDF_FAILURE;
        }
        ret = parser->GetUint32(busNode, "busAddr", &config->busCfg.spiCfg.csNum, 0);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("get busCfg.spiCfg.csNum failed");
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}


int32_t GetGnssConfigData(const struct DeviceResourceNode *node, struct GnssCfgData *config)
{
    int32_t ret;
    struct DeviceResourceIface *parser = NULL;
    const struct DeviceResourceNode *infoNode = NULL;
    const struct DeviceResourceNode *busNode = NULL;

    if (node == NULL) {
        GNSS_LOG_ERR("input param node check failed");
        return HDF_FAILURE;
    }

    if (config == NULL) {
        GNSS_LOG_ERR("input param node check failed");
        return HDF_FAILURE;
    }

    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (parser == NULL) {
        GNSS_LOG_ERR("DeviceResourceGetIfaceInstance failed");
        return HDF_ERR_INVALID_PARAM;
    }
    config->root = node;

    if (parser->GetChildNode == NULL) {
        GNSS_LOG_ERR("parser->GetChildNode is NULL");
        return HDF_ERR_INVALID_PARAM;
    }
    infoNode = parser->GetChildNode(node, "gnssInfo");
    if (infoNode != NULL) {
        ret = ParseGnssInfo(parser, infoNode, config);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("ParseGnssInfo failed");
            return HDF_FAILURE;     
        }
    }

    busNode = parser->GetChildNode(node, "gnssBusConfig");
    if (busNode != NULL) {
        ret = ParseGnssBus(parser, busNode, config);
        if (ret != HDF_SUCCESS) {
            GNSS_LOG_ERR("ParseGnssBus failed");
            return HDF_FAILURE;     
        }
    }

    return HDF_SUCCESS;
}
