/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_wlan_log.h"

#include <stdarg.h>
#include <securec.h> 

#define LOG_NAME_MAX_LEN 5
#ifndef HDF_WLAN_DEBUG
#define LOG_PRINT_MAX_LEN 256
#else
#define LOG_PRINT_MAX_LEN 512
#endif

static int32_t g_logLevel;

void HdfWlanLog(HdfWlanLogLevel level, const char *fmt, ...)
{
    uint32_t ulPos;
    char buf[LOG_PRINT_MAX_LEN] = {0};
    va_list arg;
    int32_t ret;

    if (level >= HDF_WLAN_LOG_LEVEL_MAX) {
        HDF_LOGE("%s:hdf wlan log type error, level=%d.", __func__, level);
        return;
    }

    if ((int32_t)level < g_logLevel) {
        return;
    }

    ulPos = strlen(buf);
    (void)memset_s(&arg, sizeof(va_list), 0, sizeof(va_list));
    va_start(arg, fmt);
    ret = vsprintf_s(&buf[ulPos], sizeof(buf) - ulPos, fmt, arg);
    va_end(arg);
    if (ret < 0) {
        HDF_LOGW("%s:hdf wlan log len error, ret=%d.", __func__, ret);
        return;
    }

    switch (level) {
        case HDF_WLAN_LOG_DBG:
            HDF_LOGD("%{public}s", buf);
            break;
        case HDF_WLAN_LOG_INFO:
            HDF_LOGI("%{public}s", buf);
            break;
        case HDF_WLAN_LOG_WARN:
            HDF_LOGW("%{public}s", buf);
            break;
        case HDF_WLAN_LOG_ERROR:
            HDF_LOGE("%{public}s", buf);
            break;
        default:
            break;
    }
}
