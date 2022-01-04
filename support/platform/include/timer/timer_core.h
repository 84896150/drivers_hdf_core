/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef TIMER_CORE_H
#define TIMER_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_mutex.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CHECK_NULL_PTR_RETURN_VALUE(ptr, ret) do { \
    if ((ptr) == NULL) { \
        HDF_LOGE("%s:line %d pointer is null and return ret", __func__, __LINE__); \
        return (ret); \
    } \
} while (0)

#define CHECK_NULL_PTR_RETURN(ptr) do { \
    if ((ptr) == NULL) { \
        HDF_LOGE("%s:line %d pointer is null and return", __func__, __LINE__); \
        return; \
    } \
} while (0)

#define CHECK_PARSER_RESULT_RETURN_VALUE(ret, str) do { \
    if (ret != HDF_SUCCESS) { \
        HDF_LOGE("%s:line %d %s fail, ret = %d!", __func__, __LINE__, str, ret); \
        return HDF_FAILURE; \
    } \
} while (0)

/**
 * @brief Defines a callback that will be invoked when a timer's interrupt involved.
 *
 */
typedef int32_t (*TimerHandleCb)(void);

struct TimerInfo {
    uint32_t id;
    TimerHandleCb cb;
    uint32_t useconds;
    bool isPeriod;
};

struct TimerCntrl {
    struct TimerInfo info;
    struct DListHead node;
    struct TimerCntrlMethod *ops;
    void *priv;
    struct OsalMutex lock;
};

struct TimerCntrlMethod {
    int32_t (*remove)(struct TimerCntrl *cntrl);
    int32_t (*open)(struct TimerCntrl *cntrl);
    int32_t (*close)(struct TimerCntrl *cntrl);
    int32_t (*start)(struct TimerCntrl *cntrl);
    int32_t (*stop)(struct TimerCntrl *cntrl);
    int32_t (*set)(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb);
    int32_t (*setOnce)(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb);
    int32_t (*get)(struct TimerCntrl *cntrl, uint32_t *useconds, bool *isPeriod);
};

/**
 * @brief add a timer controller to manager list
 * @param cntrl Indicates a timer controller.
 * @constraints:
 * @return success or fail
 */
int32_t TimerCntrlAdd(struct TimerCntrl *cntrl);

/**
 * @brief remove a timer controller from manager list
 * @param id Indicates a timer id.
 * @constraints:
 * @return success or fail
 */
int32_t TimerCntrlRemove(const uint32_t id);

/**
 * @brief Find and return a timer controller by id
 * @param id Indicates a timer id.
 * @return a timer controller
 */
struct TimerCntrl *TimerCntrlOpen(const uint32_t id);

/**
 * @brief close a timer controller
 * @param cntrl Indicates a timer controller.
 * @return success or fail
 */
int32_t TimerCntrlClose(struct TimerCntrl *cntrl);

/**
 * @brief set a timer controller
 * @param cntrl Indicates a period timer controller.
 * @param useconds Indicates a timer timerout time us.
 * @param cb Indicates a timer callback.
 * @return success or fail
 */
int32_t TimerCntrlSet(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb);

/**
 * @brief set a timer controller
 * @param cntrl Indicates a timer controller.
 * @param useconds Indicates a timer timerout time us.
 * @param cb Indicates a timer callback.
 * @return success or fail
 */
int32_t TimerCntrlSetOnce(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb);

/**
 * @param cntrl Indicates a timer controller.
 * @param useconds Represents the timer interval.
 * @param isPeriod Represents whether the timer call once
 */
int32_t TimerCntrlGet(struct TimerCntrl *cntrl, uint32_t *useconds, bool *isPeriod);

/**
 * @brief start a timer controller
 * @param cntrl Indicates a timer controller.
 * @return success or fail
 */
int32_t TimerCntrlStart(struct TimerCntrl *cntrl);

/**
 * @brief stop a timer controller
 * @param cntrl Indicates a timer controller.
 * @return success or fail
 */
int32_t TimerCntrlStop(struct TimerCntrl *cntrl);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* TIMER_CORE_H */
