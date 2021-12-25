/**
 * Copyright (c) 2020-2021 ThunderSoft
 * All Rights Reserved by Thunder Software Technology Co., Ltd and its affiliates.
 * You may not use, copy, distribute, modify, transmit in any form this file
 * except in compliance with ThunderSoft in writing by applicable law.
 *
 */
/**
 * @file    turbox_event.c
 * @brief   harmony bl platform event adaptor
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
#include <FreeRTOS.h>
#include <event_groups.h>
#include <cmsis_os2.h>
#include <string.h>
#include <los_event.h>
#include <los_memory.h>
#include "turbox_core_data.h"


#define MAX_BITS_TASK_NOTIFY      (31U)
#define MAX_BITS_EVENT_GROUPS     (24U)

#define THREAD_FLAGS_INVALID_BITS (~((1UL << MAX_BITS_TASK_NOTIFY)  - 1U))
#define EVENT_FLAGS_INVALID_BITS  (~((1UL << MAX_BITS_EVENT_GROUPS) - 1U))

/**
 * @brief create the dynamic event
 * 
 * @param a [-] void
 * @return  EventGroupHandle_t
 * @retval
 * 1 : result1: event group handle
 * @see f()
 */
EventGroupHandle_t turboxCreateDynamicEvent(void) {
    EventGroupHandle_t xReturn = NULL;

    /* malloc memory from pool for event group */
    xReturn = (EventGroupHandle_t)LOS_MemAlloc(m_aucSysMem0, sizeof(EventGroup_t));
    if (NULL == xReturn) {
        TURBOX_PRINTF("Malloc memory for event failed!\r\n");
        return NULL;
    }

    /* record the event flag and event ID */
    (xReturn->pvLiteOSEvtFlag) = osEventFlagsNew(NULL);
    if (NULL == (xReturn->pvLiteOSEvtFlag)) {
        TURBOX_PRINTF("new event flag by osEventFlagsNew failed!\r\n");
        return NULL;
    }

    (xReturn->uxEventBits) = ((PEVENT_CB_S)(xReturn->pvLiteOSEvtFlag))->uwEventID;

    /* dynamic event ; need to free */
    xReturn->ucStaticallyAllocated = pdFALSE;

    return xReturn;
}

/**
 * @brief create static event
 * 
 * @param a [in] pxEventGroupBuffer: event group buffer entity
 * @return EventGroupHandle_t 
 * @retval 
 * 1 : result1: event group handle
 * @see f()
 */
EventGroupHandle_t turboxCreateStaticEvent(StaticEventGroup_t *pxEventGroupBuffer) {
    EventGroupHandle_t xReturn = NULL;

    if (NULL == pxEventGroupBuffer) {
        TURBOX_PRINTF("Parameter pxEventGroupBuffer is NULL!\r\n");
        return NULL;
    }

    xReturn = (EventGroupHandle_t)pxEventGroupBuffer;
    (xReturn->pvLiteOSEvtFlag) = osEventFlagsNew(NULL);
    if (NULL == (xReturn->pvLiteOSEvtFlag)) {
        TURBOX_PRINTF("new event flag by osEventFlagsNew failed!\r\n");
        return NULL;
    }

    (xReturn->uxEventBits) = ((PEVENT_CB_S)(xReturn->pvLiteOSEvtFlag))->uwEventID;

    /* static event; no need to free */
    xReturn->ucStaticallyAllocated = pdTRUE;

    return xReturn;
}

/**
 * @brief wait event
 * 
 * @param a [in] EventGroupHandle_t xEventGroup: event group
 * @param b [in] const EventBits_t uxBitsToWaitFor: the bit that wait for
 * @param c [in] const BaseType_t xClearOnExit: clear type
 * @param d [in] const BaseType_t xWaitForAllBits: wait all the bits?
 * @param e [in] TickType_t xTicksToWait: the ticks that wait
 * @return EventBits_t 
 * @retval
 * 1 : result1； event bits
 * @see f()
 */
EventBits_t turboxWaitEvent(EventGroupHandle_t xEventGroup, 
                                 const EventBits_t uxBitsToWaitFor, 
                                 const BaseType_t xClearOnExit, 
                                 const BaseType_t xWaitForAllBits, 
                                 TickType_t xTicksToWait) {
    uint32_t options = INITIALIZATION_VALUE_0;
    EventBits_t xReturn = INITIALIZATION_VALUE_0;

    if (NULL == xEventGroup) {
        TURBOX_PRINTF("xEventGroup is NULL!\r\n");
        return xReturn;
    }

    if (NULL == xEventGroup->pvLiteOSEvtFlag) {
        TURBOX_PRINTF("xEventGroup->pvLiteOSEvtFlag is NULL!\r\n");
        return xReturn;
    }

    if (pdFAIL == xClearOnExit) {
        /* no clear the flags when exited */
        options &= ~osFlagsNoClear; 
    } else {
        /* clear the flags when exited */
        options |= osFlagsNoClear;
    }

    if (pdTRUE == xWaitForAllBits) {
        /* wait all the bits */
        options &= ~osFlagsWaitAll; 
    } else {
        /* no wait all the bits */
        options |= osFlagsWaitAll;
    }

    /* wait the event flags and schedule */
    xReturn = (EventBits_t)osEventFlagsWait(xEventGroup->pvLiteOSEvtFlag, uxBitsToWaitFor, options, xTicksToWait);
    if (osFlagsErrorParameter <= xReturn) {
        TURBOX_PRINTF("osEventFlagsWait Error!xReturn = %ld\r\n", xReturn);
        xReturn = INITIALIZATION_VALUE_0;
    }

    return xReturn;
}

/**
 * @brief clear the event
 * 
 * @param a [in] EventGroupHandle_t xEventGroup: event group
 * @param b [in] const EventBits_t uxBitsToClear: clear the bit
 * @return EventBits_t 
 * @retval
 * 1 : result1: event bits 
 * @see
 */
EventBits_t turboxClearEvent(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear) {
    EventBits_t xReturn = INITIALIZATION_VALUE_0;

    if (NULL == xEventGroup) {
        TURBOX_PRINTF("xEventGroup is NULL!\r\n");
        return xReturn;
    }

    if (NULL == xEventGroup->pvLiteOSEvtFlag) {
        TURBOX_PRINTF("xEventGroup->pvLiteOSEvtFlag is NULL!\r\n");
        return xReturn;
    }    

    /* clear the event flags and schedule */
    xReturn = (EventBits_t)osEventFlagsClear(xEventGroup->pvLiteOSEvtFlag, uxBitsToClear);
    if (osFlagsErrorParameter <= xReturn) {
        TURBOX_PRINTF("osEventFlagsClear Error!xReturn = %ld\r\n", xReturn);
        xReturn = INITIALIZATION_VALUE_0;
    }

    return xReturn;
}

/**
 * @brief set event
 * 
 * @param a [in] EventGroupHandle_t xEventGroup: event group
 * @param b [in] const EventBits_t uxBitsToSet: set the bit
 * @return EventBits_t 
 * @retval
 * 1 : result1: event bits 
 * @see
 */
EventBits_t turboxSetEvent(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet) {
    EventBits_t xReturn = INITIALIZATION_VALUE_0;

    if (NULL == xEventGroup) {
        TURBOX_PRINTF("xEventGroup is NULL!\r\n");
        return xReturn;
    }

    if (NULL == xEventGroup->pvLiteOSEvtFlag) {
        TURBOX_PRINTF("xEventGroup->pvLiteOSEvtFlag is NULL!\r\n");
        return xReturn;
    }        

    /* set the event flags and schedule */
    xReturn = (EventBits_t)osEventFlagsSet(xEventGroup->pvLiteOSEvtFlag, uxBitsToSet);
    if (osFlagsErrorParameter <= xReturn) {
        TURBOX_PRINTF("osEventFlagsSet Error!xReturn = %ld\r\n", xReturn);
        xReturn = 0;
    }
    
    return xReturn;
}

/**
 * @brief set event in ISR
 * 
 * @param a [in] EventGroupHandle_t xEventGroup: event group
 * @param b [in] const EventBits_t uxBitsToSet: set the bit
 * @param c [out] BaseType_t *pxHigherPriorityTaskWoken
 * @return BaseType_t 
 * @retval
 * pdTRUE : result1: set OK 
 * pdFALSE : result2: set NG
 * @see
 */
BaseType_t turboxSetEventFromISR(EventGroupHandle_t xEventGroup, 
                                      const EventBits_t uxBitsToSet, 
                                      BaseType_t *pxHigherPriorityTaskWoken) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == xEventGroup) {
        TURBOX_PRINTF("xEventGroup is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == xEventGroup->pvLiteOSEvtFlag) {
        TURBOX_PRINTF("xEventGroup->pvLiteOSEvtFlag is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }      

    /* set the event flags and schedule */
    if (osFlagsErrorParameter != osEventFlagsSet(xEventGroup->pvLiteOSEvtFlag, (uxBitsToSet & (~EVENT_FLAGS_INVALID_BITS)))) {
        xReturn = pdTRUE;
        if (NULL != pxHigherPriorityTaskWoken) {
            *pxHigherPriorityTaskWoken = pdTRUE;
        } else {
            TURBOX_PRINTF("osEventFlagsSet Error!pxHigherPriorityTaskWoken = NULL\r\n");
        }
    } else {
        TURBOX_PRINTF("osEventFlagsSet Error!xReturn = %ld\r\n", xReturn);
    }

    return xReturn;
}

/**
 * @brief delete event
 * 
 * @param a [in] EventGroupHandle_t xEventGroup : event group
 * @return void 
 * @retval
 * @see
 */
void turboxDeleteEvent( EventGroupHandle_t xEventGroup ) {
    if (NULL == xEventGroup) {
        TURBOX_PRINTF("xEventGroup is NULL!\r\n");
        return;
    }

    if (NULL == xEventGroup->pvLiteOSEvtFlag) {
        TURBOX_PRINTF("xEventGroup->pvLiteOSEvtFlag is NULL!\r\n");
        return;
    }      

    /* delete the event flags */
    if (osFlagsErrorParameter == osEventFlagsDelete(xEventGroup->pvLiteOSEvtFlag)) {
        TURBOX_PRINTF("osEventFlagsDelete osFlagsErrorParameter\r\n");
    }

    /* clear the event group memory which is dynamic */
    if ((uint8_t)pdFALSE == xEventGroup->ucStaticallyAllocated) {
        if (LOS_NOK == LOS_MemFree(m_aucSysMem0,xEventGroup)) {
            TURBOX_PRINTF("dynamic LOS_MemFree memory failed!\r\n");
        }
    }
    return;
}
