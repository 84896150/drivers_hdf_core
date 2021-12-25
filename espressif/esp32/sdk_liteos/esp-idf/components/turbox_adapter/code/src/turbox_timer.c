/**
 * Copyright (c) 2020-2021 ThunderSoft
 * All Rights Reserved by Thunder Software Technology Co., Ltd and its affiliates.
 * You may not use, copy, distribute, modify, transmit in any form this file
 * except in compliance with ThunderSoft in writing by applicable law.
 *
 */
/**
 * @file    turbox_timer.c
 * @brief   harmony bl platform timer adaptor
 * @details
 * @version 1.0 
 * @author  ThunderSoft  
 * @date    2020/12/3
 *
 * Edit History
 * ----------------------------------------------------------------------------
 * DATE                     NAME               DESCRIPTION
 * 2020/12/3                ThunderSoft         Create it. 
 *
 */

#include <los_compiler.h>
#include <turbox_core_data.h>
#include <list.h>
#include <timers.h>
#include <cmsis_os2.h>
#include <los_memory.h>
#include <string.h>

#define tmrSTATUS_IS_ACTIVE                 ( ( uint8_t ) 0x01 )
#define tmrSTATUS_IS_STATICALLY_ALLOCATED   ( ( uint8_t ) 0x02 )
#define tmrSTATUS_IS_AUTORELOAD             ( ( uint8_t ) 0x04 )

/**
 * @brief create dynamic timer
 * 
 * @param a [in] const char * const pcTimerName
 * @param b [in] const TickType_t xTimerPeriodInTicks
 * @param c [in] const UBaseType_t uxAutoReload
 * @param d [in] void * const pvTimerID
 * @param e [in] TimerCallbackFunction_t pxCallbackFunction
 * @return TimerHandle_t 
 * @retval
 * 1 : result1； timer handle
 * @see
 */
TimerHandle_t turboxCreateDynamicTimer(const char * const pcTimerName,
                        const TickType_t xTimerPeriodInTicks,
                        const UBaseType_t uxAutoReload,
                        void * const pvTimerID,
                        TimerCallbackFunction_t pxCallbackFunction) {
    osTimerAttr_t attr;
    TimerHandle_t pxNewTimer = NULL;
    osTimerType_t type = osTimerOnce;

    pxNewTimer = (TimerHandle_t) LOS_MemAlloc(m_aucSysMem0, sizeof(xTIMER));
    if (NULL != pxNewTimer) {
        if (pdFALSE == uxAutoReload) {
            type = osTimerOnce;
        } else {
            type = osTimerPeriodic;
            pxNewTimer->ucStatus |= tmrSTATUS_IS_AUTORELOAD;
        }

        pxNewTimer->ucStatus = INITIALIZATION_VALUE_0;
        pxNewTimer->pcTimerName = pcTimerName;
        pxNewTimer->xTimerPeriodInTicks = xTimerPeriodInTicks;
        pxNewTimer->pvTimerID = pvTimerID;
        pxNewTimer->pxCallbackFunction = pxCallbackFunction;

        memset(&attr, INITIALIZATION_VALUE_0, sizeof(attr));
        attr.cb_mem = NULL;
        attr.name = pcTimerName;
        attr.cb_size = INITIALIZATION_VALUE_0;
        attr.attr_bits = INITIALIZATION_VALUE_0;

        pxNewTimer->pvLiteOSSWTmr = osTimerNew ((osTimerFunc_t)pxCallbackFunction, type, pxNewTimer, &attr);
        if (NULL == pxNewTimer->pvLiteOSSWTmr) {
            TURBOX_PRINTF("osTimerNew failed!\r\n");
            return NULL;
        }
    } else {
        TURBOX_PRINTF("Malloc memory for timer failed!\r\n");
        return NULL;
    }

    /* Return timer ID */
    return ((TimerHandle_t)pxNewTimer);
}

/**
 * @brief create static timer
 * 
 * @param a [in] const char * const pcTimerName
 * @param b [in] const TickType_t xTimerPeriodInTicks
 * @param c [in] const UBaseType_t uxAutoReload
 * @param d [in] void * const pvTimerID
 * @param e [in] TimerCallbackFunction_t pxCallbackFunction
 * @param f [in] StaticTimer_t *pxTimerBuffer
 * @return TimerHandle_t 
 * @retval
 * 1 : result1； timer handle
 * @see
 */
TimerHandle_t turboxCreateStaticTimer(const char * const pcTimerName,
                                const TickType_t xTimerPeriodInTicks,
                                const UBaseType_t uxAutoReload,
                                void * const pvTimerID,
                                TimerCallbackFunction_t pxCallbackFunction,
                                StaticTimer_t *pxTimerBuffer) {
    osTimerAttr_t attr;
    TimerHandle_t pxNewTimer = NULL;
    osTimerType_t type = osTimerOnce;

    if (NULL == pxTimerBuffer) {
        TURBOX_PRINTF("Parameter pxTimerBuffer is NULL!\r\n");
        return NULL;
    }

    pxNewTimer = (TimerHandle_t)pxTimerBuffer;
    if (NULL != pxNewTimer) {
        pxNewTimer->ucStatus = tmrSTATUS_IS_STATICALLY_ALLOCATED;
        if (pdFALSE == uxAutoReload) {
            type = osTimerOnce;
        } else {
            type = osTimerPeriodic;
            pxNewTimer->ucStatus |= tmrSTATUS_IS_AUTORELOAD;
        }

        pxNewTimer->pvTimerID = pvTimerID;
        pxNewTimer->pcTimerName = pcTimerName;
        pxNewTimer->ucStatus = INITIALIZATION_VALUE_0;
        pxNewTimer->pxCallbackFunction = pxCallbackFunction;
        pxNewTimer->xTimerPeriodInTicks = xTimerPeriodInTicks;

        memset(&attr, INITIALIZATION_VALUE_0, sizeof(attr));
        attr.cb_mem = NULL;
        attr.name = pcTimerName;
        attr.cb_size = INITIALIZATION_VALUE_0;
        attr.attr_bits = INITIALIZATION_VALUE_0;        

        pxNewTimer->pvLiteOSSWTmr = osTimerNew ((osTimerFunc_t)pxCallbackFunction, type, pxNewTimer, &attr);
        if (NULL == pxNewTimer->pvLiteOSSWTmr) {
            TURBOX_PRINTF("osTimerNew failed!\r\n");
            return NULL;
        }
    }

    /* Return timer ID */
    return ((TimerHandle_t)pxNewTimer);
}

/**
 * @brief get timer ID
 * 
 * @param a [in] const TimerHandle_t xTimer
 * @return void * 
 * @retval
 * 1 : result1； timer ID information
 * @see
 */
void *turboxGetTimerID(const TimerHandle_t xTimer) {
    void *pvReturn;

    if (NULL == xTimer) {
        TURBOX_PRINTF("parameter xTimer is NULL!\r\n");
        return NULL;
    }

    taskENTER_CRITICAL();
    pvReturn = xTimer->pvTimerID;
    taskEXIT_CRITICAL();

    return pvReturn;
}

/**
 * @brief reset the timer
 * 
 * @param a [in] TimerHandle_t xTimer
 * @param b [in] TickType_t xTicksToWait
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdFALSE/pdTRUE
 * 2 : result2；
 * @see f()
 */
BaseType_t turboxTimerReset(TimerHandle_t xTimer, TickType_t xTicksToWait) {    
    BaseType_t xReturn = pdFALSE;

    if (NULL == xTimer) {
        TURBOX_PRINTF("parameter xTimer is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == xTimer->pvLiteOSSWTmr) {
        TURBOX_PRINTF("parameter xTimer->pvLiteOSSWTmr is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    xReturn = turboxTimerStop(xTimer, xTicksToWait);
    if (pdFALSE == xReturn) {
        TURBOX_PRINTF("Stop Timer Failed!\r\n");
        return xReturn;
    }

    xReturn = turboxTimerStart(xTimer, xTicksToWait);
    if (pdFALSE == xReturn) {
        TURBOX_PRINTF("Restart Timer Failed!\r\n");
    }

    return (xReturn);
}

/**
 * @brief start timer
 * 
 * @param a [in] TimerHandle_t xTimer
 * @param b [in] TickType_t xTicksToWait
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdFALSE/pdTRUE
 * @see
 */
BaseType_t turboxTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait) {    
    osStatus_t stat = osOK;
    BaseType_t xReturn = pdFALSE;

    if (NULL == xTimer) {
        TURBOX_PRINTF("parameter xTimer is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == xTimer->pvLiteOSSWTmr) {
        TURBOX_PRINTF("parameter xTimer->pvLiteOSSWTmr is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    stat = osTimerStart(xTimer->pvLiteOSSWTmr, (uint32_t)xTimer->xTimerPeriodInTicks);
    if (stat == osOK) {
        xReturn = pdTRUE;
        xTimer->ucStatus |= tmrSTATUS_IS_ACTIVE;
    } else {
        TURBOX_PRINTF("Start Timer Failed!\r\n");
    }

    return (xReturn);
}

/**
 * @brief stop timer
 * 
 * @param a [in] TimerHandle_t xTimer
 * @param b [in] TickType_t xTicksToWait
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdTRUE/pdFALSE
 * @see
 */
BaseType_t turboxTimerStop(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    osStatus_t stat = osOK;
    BaseType_t xReturn = pdFALSE;

    if (NULL == xTimer) {
        TURBOX_PRINTF("parameter xTimer is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == xTimer->pvLiteOSSWTmr) {
        TURBOX_PRINTF("parameter xTimer->pvLiteOSSWTmr is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (OS_SWTMR_STATUS_CREATED == ((SWTMR_CTRL_S *)(xTimer->pvLiteOSSWTmr))->ucState) {
        xReturn = pdTRUE;
        TURBOX_PRINTF("timer is only created; no start!\r\n");
    } else {
        stat = osTimerStop(xTimer->pvLiteOSSWTmr);
        if (osOK == stat) {
            xReturn = pdTRUE;
            xTimer->ucStatus &= ~tmrSTATUS_IS_ACTIVE;
        } else {
            TURBOX_PRINTF("Cannot stop the timer!\r\n");
        }
    }

    return (xReturn);
}

/**
 * @brief change timer period
 * 
 * @param a [in] TimerHandle_t xTimer
 * @param b [in] TickType_t xOptionalValue
 * @param c [in] TickType_t xTicksToWait
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdFALSE/pdTRUE
 * 2 : result2；
 * @see f()
 */
BaseType_t turboxTimerChgPeriod(TimerHandle_t xTimer, TickType_t xOptionalValue, TickType_t xTicksToWait) {
    osStatus_t stat = osOK;
    BaseType_t xReturn = pdFALSE;

    if (NULL == xTimer) {
        TURBOX_PRINTF("parameter xTimer is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == xTimer->pvLiteOSSWTmr) {
        TURBOX_PRINTF("parameter xTimer->pvLiteOSSWTmr is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (osOK != osTimerStop(xTimer->pvLiteOSSWTmr)) {
        TURBOX_PRINTF("Cannot stop the timer,Perhaps the timer has not started yet!\r\n");
    }

    stat = osTimerStart(xTimer->pvLiteOSSWTmr, (uint32_t)xOptionalValue);
    if (osOK == stat) {
        xReturn = pdTRUE;
        xTimer->xTimerPeriodInTicks = xOptionalValue;
        xTimer->ucStatus |= tmrSTATUS_IS_ACTIVE;
    } else {
        TURBOX_PRINTF("Cannot start the timer!\r\n");
    }

    return (xReturn);
}

/**
 * @brief delete the timer
 * 
 * @param a [in] TimerHandle_t xTimer
 * @param b [in] TickType_t xTicksToWait
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdFALSE/pdTRUE
 * @see
 */
BaseType_t turboxTimerDelete(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    osStatus_t stat = osOK;
    BaseType_t xReturn = pdFALSE;

    if (NULL == xTimer) {
        TURBOX_PRINTF("parameter xTimer is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == xTimer->pvLiteOSSWTmr) {
        TURBOX_PRINTF("parameter xTimer->pvLiteOSSWTmr is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    stat = osTimerDelete(xTimer->pvLiteOSSWTmr);
    if (osOK == stat) {
        xReturn = pdTRUE;
    } else {
        TURBOX_PRINTF("Timer Delete Failed!\r\n");
    }

    /* dynamic timer need to free the memory */
    if (INITIALIZATION_VALUE_0 == (xTimer->ucStatus & tmrSTATUS_IS_STATICALLY_ALLOCATED)) {
        if (LOS_NOK == LOS_MemFree(m_aucSysMem0, xTimer)) {
            TURBOX_PRINTF("Timer Memory Free Failed!\r\n");
        }
    } else {
        xTimer->ucStatus &= ~tmrSTATUS_IS_ACTIVE;
    }

    return (xReturn);
}

/**
 * @brief start timer from ISR
 * 
 * @param a [in] TimerHandle_t xTimer
 * @param b [out] BaseType_t * const pxHigherPriorityTaskWoken
 * @return BaseType_t
 * @retval
 * 1 : result1； pdFALSE/pdTRUE
 * @see
 */
BaseType_t turboxTimerStartFromISR(TimerHandle_t xTimer, BaseType_t * const pxHigherPriorityTaskWoken) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == xTimer) {
        TURBOX_PRINTF("Parameter Timer is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    xReturn = turboxTimerStart(xTimer, INITIALIZATION_VALUE_0);
    if (pdFALSE == xReturn) {
        TURBOX_PRINTF("Start Timer Failed!\r\n");
    }

    return (xReturn);
}
