/**
 * Copyright (c) 2020-2021 ThunderSoft
 * All Rights Reserved by Thunder Software Technology Co., Ltd and its affiliates.
 * You may not use, copy, distribute, modify, transmit in any form this file
 * except in compliance with ThunderSoft in writing by applicable law.
 *
 */
/**
 * @file    turbox_core_data.h
 * @brief   turbox_core_data funtion header file for freertos adapter
 * @details turbox_core_data funtion for freertos adapter
 * @version 1.0
 * @author  Hu lei
 * @date    2021-03-05
 *
 * Edit History
 * ----------------------------------------------------------------------------
 * DATE                     NAME               DESCRIPTION
 * 2021-03-05               Hu lei              Create it.
 *
 */
#ifndef TS_CORE_DATA_H
#define TS_CORE_DATA_H

#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <portmacro.h>
#include <event_groups.h>
#include <los_swtmr.h>
#include <los_interrupt.h>
#include <cmsis_os2.h>

#define container_of(ptr, type, member) ({ \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) );})


typedef struct QueuePointers {
    int8_t *pcTail;
    int8_t *pcReadFrom;
} QueuePointers_t;

typedef struct SemaphoreData { 
    TaskHandle_t xMutexHolder;
    UBaseType_t uxRecursiveCallCount;
} SemaphoreData_t;

typedef struct QueueDefinition {
    int8_t *pcHead;
    int8_t *pcWriteTo;

    union {
        QueuePointers_t xQueue;
        SemaphoreData_t xSemaphore;
    } u;

    List_t xTasksWaitingToSend;
    List_t xTasksWaitingToReceive;

    volatile UBaseType_t uxMessagesWaiting;
    UBaseType_t uxLength;
    UBaseType_t uxItemSize;

    volatile int8_t cRxLock;
    volatile int8_t cTxLock;

    #if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
        uint8_t ucStaticallyAllocated;
    #endif

    #if ( configUSE_QUEUE_SETS == 1 )
        struct QueueDefinition *pxQueueSetContainer;
    #endif

    #if ( configUSE_TRACE_FACILITY == 1 )
        UBaseType_t uxQueueNumber;
        uint8_t ucQueueType;
    #endif
#ifdef ESP_PLATFORM
    portMUX_TYPE mux;       //Mutex required due to SMP
#endif // ESP_PLATFORM
    osSemaphoreId_t pvLiteOSSem;  // add for liteos
    osMutexId_t pvLiteOSMutexSem;  // add for liteos
    osMessageQueueId_t pvLiteOSQueue;  // add for liteos
} xQUEUE;

/* The old xQUEUE name is maintained above then typedefed to the new Queue_t
 * name below to enable the use of older kernel aware debuggers. */
typedef xQUEUE Queue_t;

typedef struct EventGroupDef_t {
    EventBits_t uxEventBits;
    List_t xTasksWaitingForBits;
    #if( configUSE_TRACE_FACILITY == 1 )
        UBaseType_t uxEventGroupNumber;
    #endif

    #if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
        uint8_t ucStaticallyAllocated;
    #endif
#ifdef ESP_PLATFORM
        portMUX_TYPE eventGroupMux;     //Mutex required due to SMP
#endif // ESP_PLATFORM

    osEventFlagsId_t pvLiteOSEvtFlag;       // add for liteOS
} EventGroup_t;

typedef struct tmrTimerControl {
    const char              *pcTimerName;
    ListItem_t              xTimerListItem;
    TickType_t              xTimerPeriodInTicks;
    void                    *pvTimerID;
    TimerCallbackFunction_t pxCallbackFunction;
    #if( configUSE_TRACE_FACILITY == 1 )
        UBaseType_t         uxTimerNumber;
    #endif
    uint8_t                 ucStatus;

    osTimerId_t            pvLiteOSSWTmr;  // add for liteOS
} xTIMER;

typedef struct StreamBufferDef_t
{
    volatile size_t xTail;                       /* Index to the next item to read within the buffer. */
    volatile size_t xHead;                       /* Index to the next item to write within the buffer. */
    size_t xLength;                              /* The length of the buffer pointed to by pucBuffer. */
    size_t xTriggerLevelBytes;                   /* The number of bytes that must be in the stream buffer before a task that is waiting for data is unblocked. */
    volatile TaskHandle_t xTaskWaitingToReceive; /* Holds the handle of a task waiting for data, or NULL if no tasks are waiting. */
    volatile TaskHandle_t xTaskWaitingToSend;    /* Holds the handle of a task waiting to send data to a message buffer that is full. */
    uint8_t * pucBuffer;                         /* Points to the buffer itself - that is - the RAM that stores the data passed through the buffer. */
    uint8_t ucFlags;

    #if ( configUSE_TRACE_FACILITY == 1 )
        UBaseType_t uxStreamBufferNumber; /* Used for tracing purposes. */
    #endif
#ifdef ESP_PLATFORM
    portMUX_TYPE xStreamBufferMux;  //Mutex required due to SMP
#endif // ESP_PLATFORM
    osMessageQueueId_t pvMsgQueue;        // add for liteOS
} StreamBuffer_t;

static inline uint32_t priority_liteos_to_freertos(uint32_t priority) {
    return configMAX_PRIORITIES - priority;
}

static inline uint32_t priority_freertos_to_liteos(uint32_t priority) {
    return configMAX_PRIORITIES - priority;
}

#endif  // TS_CORE_DATA_H
