/*
 * FreeRTOS Kernel V10.4.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


#ifndef QUEUE_H
#define QUEUE_H

#ifndef INC_FREERTOS_H
    #error "include FreeRTOS.h" must appear in source files before "include queue.h"
#endif

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

#include "task.h"

/**
 * Type by which queues are referenced.  For example, a call to xQueueCreate()
 * returns an QueueHandle_t variable that can then be used as a parameter to
 * xQueueSend(), xQueueReceive(), etc.
 */
struct QueueDefinition; /* Using old naming convention so as not to break kernel aware debuggers. */
typedef struct QueueDefinition   * QueueHandle_t;

/**
 * Type by which queue sets are referenced.  For example, a call to
 * xQueueCreateSet() returns an xQueueSet variable that can then be used as a
 * parameter to xQueueSelectFromSet(), xQueueAddToSet(), etc.
 */
typedef struct QueueDefinition   * QueueSetHandle_t;

/**
 * Queue sets can contain both queues and semaphores, so the
 * QueueSetMemberHandle_t is defined as a type to be used where a parameter or
 * return value can be either an QueueHandle_t or an SemaphoreHandle_t.
 */
typedef struct QueueDefinition   * QueueSetMemberHandle_t;

/** @cond */

/* For internal use only. */
#define queueSEND_TO_BACK                     ( ( BaseType_t ) 0 )
#define queueSEND_TO_FRONT                    ( ( BaseType_t ) 1 )
#define queueOVERWRITE                        ( ( BaseType_t ) 2 )

/* For internal use only.  These definitions *must* match those in queue.c. */
#define queueQUEUE_TYPE_BASE                  ( ( uint8_t ) 0U )
#define queueQUEUE_TYPE_SET                   ( ( uint8_t ) 0U )
#define queueQUEUE_TYPE_MUTEX                 ( ( uint8_t ) 1U )
#define queueQUEUE_TYPE_COUNTING_SEMAPHORE    ( ( uint8_t ) 2U )
#define queueQUEUE_TYPE_BINARY_SEMAPHORE      ( ( uint8_t ) 3U )
#define queueQUEUE_TYPE_RECURSIVE_MUTEX       ( ( uint8_t ) 4U )

extern void turboxDeleteQueue(QueueHandle_t );
extern UBaseType_t turboxWaitQueueMsg(const QueueHandle_t );
extern BaseType_t turboxRecvFromQueue(QueueHandle_t, void * const, TickType_t );
extern BaseType_t turboxSendToQueue(QueueHandle_t, const void * const, TickType_t);
extern BaseType_t turboxSendToQueueBack(QueueHandle_t, const void * const, TickType_t);
extern QueueHandle_t turboxCreateQueue(const UBaseType_t, const UBaseType_t, const uint8_t);
extern BaseType_t turboxSendToQueueFromISR(QueueHandle_t, const void * const, BaseType_t * const);
extern QueueHandle_t turboxCreateDynamicQueue(const UBaseType_t, const UBaseType_t, const uint8_t);
extern BaseType_t turboxSendQueue(QueueHandle_t, const void * const, TickType_t, const BaseType_t );
extern QueueHandle_t turboxCreateStaticQueue(const UBaseType_t, const UBaseType_t, uint8_t *, StaticQueue_t *);
extern BaseType_t turboxSendQueueFromISR(QueueHandle_t, const void * const, BaseType_t * const, const BaseType_t);

extern BaseType_t turboxAddToSetQueue( QueueSetMemberHandle_t,QueueSetHandle_t);
extern BaseType_t turboxRemoveFromSetQueue( QueueSetMemberHandle_t, QueueSetHandle_t);
extern BaseType_t turboxResetQueue( QueueHandle_t);
extern BaseType_t turboxIsQueueFullFromISR( const QueueHandle_t);
extern BaseType_t turboxReceiveFromISRQueue( QueueHandle_t, void * const, TickType_t);
extern UBaseType_t turboxuxQueueSpacesAvailable( const QueueHandle_t);
// extern QueueHandle_t turboxQueueCreateMutex( const uint8_t );
// extern QueueHandle_t turboxQueueCreateMutexStatic( const uint8_t, StaticQueue_t * );
QueueHandle_t turboxCreateDynamicMux(void);
QueueHandle_t turboxCreateStaticMux(StaticQueue_t *pxStaticQueue );

#define xQueueSend turboxSendToQueue
#define xQueueOverwrite turboxSendQueue
#define xQueueSendToFront turboxSendQueue
#define xQueueSendToBack turboxSendToQueueBack
#define xQueueCreateStatic turboxCreateStaticQueue
#define xQueueSendFromISR turboxSendToQueueFromISR
#define xQueueCreate(A, B) turboxCreateDynamicQueue(A, B, queueQUEUE_TYPE_BASE)
#define xQueueOverwriteFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken) turboxSendQueueFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueOVERWRITE )
#define xQueueSendToBackFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken) turboxSendQueueFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueSEND_TO_BACK )
#define xQueueSendToFrontFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken) turboxSendQueueFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueSEND_TO_FRONT )		

#define vQueueDelete                           turboxDeleteQueue
#define xQueueReceive                          turboxRecvFromQueue
#define uxQueueMessagesWaiting                 turboxWaitQueueMsg

#define xQueueAddToSet                         turboxAddToSetQueue
#define xQueueRemoveFromSet                    turboxRemoveFromSetQueue
#define xQueueReset( xQueue )                  turboxResetQueue( xQueue )
#define xQueueIsQueueFullFromISR( xQueue )     turboxIsQueueFullFromISR( xQueue )
#define xQueueReceiveFromISR                   turboxReceiveFromISRQueue
#define uxQueueSpacesAvailable                 turboxuxQueueSpacesAvailable
#define xQueueCreateMutex(A)                   turboxCreateDynamicMux()
#define xQueueCreateMutexStatic                turboxCreateStaticMux

/** @endcond */

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* QUEUE_H */
