/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_WLAN_LOG_H
#define HDF_WLAN_LOG_H

#include <stdint.h>
#include <inttypes.h>
#include "hdf_log.h"

typedef enum {
    HDF_WLAN_LOG_DBG,
    HDF_WLAN_LOG_INFO,
    HDF_WLAN_LOG_WARN,
    HDF_WLAN_LOG_ERROR,
    HDF_WLAN_LOG_LEVEL_MAX,
} HdfWlanLogLevel;

void HdfWlanLog(HdfWlanLogLevel level, const char *fmt, ...);

#endif /* HDF_WLAN_LOG_H */
