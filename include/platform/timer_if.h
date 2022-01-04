/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */


#ifndef TIMER_IF_H
#define TIMER_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines a callback that will be invoked when a timer's interrupt involved.
 *
 */
typedef int32_t (*TimerHandleCb)(void);


/**
 * @brief Gets a timer.
 *
 * This function must be called to get its device handle before operating the timer.
 *
 * @param id Indicates a timer id.
 *
 * @return If the operation is successful, a pointer to the timer device handle is returned.
 *
 * @since 1.0
 */
DevHandle TimerOpen(const uint32_t id);

/**
 * @brief Releases a timer.
 *
 * If you no longer need the timer, call this function to  release it
 *
 * @param handle Represents a pointer to the timer device handle.
 *
 * @since 1.0
 */
void TimerClose(DevHandle handle);

/**
 * @brief start a timer.
 *
 * If you need the timer run, call this function to  start it
 *
 * @param handle Represents a pointer to the timer device handle.
 *
 * @since 1.0
 */
int32_t TimerStart(DevHandle handle);

/**
 * @brief stop a timer.
 *
 * If you no longer need the timer run, call this function to  stop it
 *
 * @param handle Represents a pointer to the timer device handle.
 *
 * @since 1.0
 */
int32_t TimerStop(DevHandle handle);

/**
 * @brief set a period timer.
 *
 * If you need the timer run, call this function to set timer info
 *
 * @param handle Represents a pointer to the timer device handle.
 * @param useconds Represents the timer interval. 
 * @param cb Represents the timer callback function.
 *
 * @since 1.0
 */
int32_t TimerSet(DevHandle handle, uint32_t useconds, TimerHandleCb cb);

/**
 * @brief set a oneshot timer.
 *
 * If you need the timer run, call this function to set timer info 
 * @param useconds Represents the timer interval. 
 * @param cb Represents the timer callback function.
 *
 * @since 1.0
 */
int32_t TimerSetOnce(DevHandle handle, uint32_t useconds, TimerHandleCb cb);

/**
 * @brief get the timer info.
 *
 * If you need the timer info, call this function get
 *
 * @param handle Represents a pointer to the timer device handle.
 * @param useconds Represents the timer interval.
 * @param isPeriod Represents whether the timer call once
 *
 * @since 1.0
 */
int32_t TimerGet(DevHandle handle, uint32_t *useconds, bool *isPeriod);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* TIMER_IF_H */
/** @} */
