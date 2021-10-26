/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */


#ifndef REGULATOR_IF_H
#define REGULATOR_IF_H

#include "hdf_platform.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Enumerates regulator statuses.
 *
 * To obtain the regulator status, call the {@link RegulatorGetStatus} function.
 *
 * @since 1.0
 */

enum RegulatorDisableMode {
    NORMAL_DISABLE,
    DEFERRED_DISABLE,
    FORCE_DISABLE,
    MAX_DISABLE_MODE,
};

DevHandle RegulatorGet(const char *name);

void RegulatorPut(DevHandle handle);

int32_t RegulatorEnable(DevHandle handle);

int32_t RegulatorDisable(DevHandle handle,int32_t disableMode);

int32_t RegulatorIsEnabled(DevHandle handle);

int32_t RegulatorSetVoltage(DevHandle handle, int32_t voltage);

int32_t RegulatorGetVoltage(DevHandle handle, int32_t *voltage);

int32_t RegulatorSetVoltageRange(DevHandle handle, int32_t vmin, int32_t vmax);

int32_t RegulatorSetCurrent(DevHandle handle, int32_t current);

int32_t RegulatorGetCurrent(DevHandle handle, int32_t *current);

int32_t RegulatorSetCurrentRange(DevHandle handle, int32_t cmin, int32_t cmax);

int32_t RegulatorGetStatus(DevHandle handle, int32_t *status);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* REGULATOR_IF_H */
/** @} */
