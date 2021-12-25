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

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifndef INC_FREERTOS_H
    #error "include FreeRTOS.h" must appear in source files before "include semphr.h"
#endif

#include "queue.h"

typedef QueueHandle_t SemaphoreHandle_t;

#define semBINARY_SEMAPHORE_QUEUE_LENGTH    ( ( uint8_t ) 1U )
#define semSEMAPHORE_QUEUE_ITEM_LENGTH      ( ( uint8_t ) 0U )
#define semGIVE_BLOCK_TIME                  ( ( TickType_t ) 0U )
			
						
#define xQueueGenericSend           turboxSendQueue			
#define xQueueGenericSendFromISR    turboxSendQueueFromISR			

QueueHandle_t turboxCreateDynamicMux(void);
BaseType_t turboxPutSem(QueueHandle_t sem);
void turboxDelSemMux(QueueHandle_t xQueue); 
QueueHandle_t turboxCreateRecursiveMux();
QueueHandle_t turboxSemaphoreCreateBinary(void);
BaseType_t turboxPutSemRecursive(QueueHandle_t xMutex);
QueueHandle_t turboxCreateDynamicBinarySem(SemaphoreHandle_t sem);
QueueHandle_t turboxCreateStaticMux(StaticQueue_t *pxStaticQueue );
QueueHandle_t turboxCreateStaticBinarySem(StaticQueue_t *pxStaticQueue);
BaseType_t turboxGetSem(QueueHandle_t xQueue, TickType_t xTicksToWait);
BaseType_t turboxGetSemRecursive(QueueHandle_t xMutex, TickType_t xTicksToWait);
QueueHandle_t turboxGetSemCount(const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount );
BaseType_t turboxPutSemFromISR(QueueHandle_t xQueue, BaseType_t * const pxHigherPriorityTaskWoken);
TaskHandle_t turboxGetMutexHolderSemaphore( QueueHandle_t xSemaphore );
BaseType_t turboxTakeFromISRSemaphore( QueueHandle_t xQueue,
                                 TickType_t pxHigherPriorityTaskWoken );
#define vSemaphoreCreateBinary( xSemaphore )                                                                                            \
    {                                                                                                                                   \
        ( xSemaphore ) = turboxCreateDynamicBinarySem( xSemaphore );                                                                         \
        if( ( xSemaphore ) != NULL )                                                                                                    \
        {                                                                                                                               \
            ( void ) xSemaphoreGive( ( xSemaphore ) );                                                                                  \
        }                                                                                                                               \
    }
#define xSemaphoreCreateMutex() turboxCreateDynamicMux()
#define xSemaphoreCreateBinary() turboxSemaphoreCreateBinary() 
#define vSemaphoreDelete         turboxDelSemMux
#define xSemaphoreGive( xSemaphore )   turboxPutSem( xSemaphore )
#define xSemaphoreCreateRecursiveMutex() turboxCreateRecursiveMux() 
#define xSemaphoreGiveRecursive( xMutex )   turboxPutSemRecursive( xMutex )
#define xSemaphoreTake( xSemaphore, xBlockTime )  turboxGetSem( xSemaphore, xBlockTime )
#define xSemaphoreCreateMutexStatic( pxMutexBuffer ) turboxCreateStaticMux( pxMutexBuffer )
#define uxSemaphoreGetCount( xSemaphore ) turboxWaitQueueMsg( ( QueueHandle_t ) ( xSemaphore ) )
#define xSemaphoreTakeRecursive( xMutex, xBlockTime )   turboxGetSemRecursive( xMutex, xBlockTime )
#define xSemaphoreCreateBinaryStatic( pxStaticSemaphore ) turboxCreateStaticBinarySem( pxStaticSemaphore )
#define xSemaphoreCreateCounting( uxMaxCount, uxInitialCount ) turboxGetSemCount( uxMaxCount, uxInitialCount )
#define xSemaphoreGiveFromISR( xSemaphore, pxHigherPriorityTaskWoken )  turboxPutSemFromISR( xSemaphore, pxHigherPriorityTaskWoken )

#define xSemaphoreGetMutexHolder( xSemaphore ) turboxGetMutexHolderSemaphore(xSemaphore)
#define xSemaphoreTakeFromISR                  turboxTakeFromISRSemaphore
#if ( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_RECURSIVE_MUTEXES == 1 ) )
    #define xSemaphoreCreateRecursiveMutexStatic( pxStaticSemaphore )    xQueueCreateMutexStatic( pxStaticSemaphore )
#endif /* configSUPPORT_STATIC_ALLOCATION */

#if 0
//add for esp32 start
#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
    #define xSemaphoreCreateCountingStatic( uxMaxCount, uxInitialCount, pxSemaphoreBuffer )    xQueueCreateCountingSemaphoreStatic( ( uxMaxCount ), ( uxInitialCount ), ( pxSemaphoreBuffer ) )
#endif /* configSUPPORT_STATIC_ALLOCATION */
#define uxSemaphoreGetCount( xSemaphore )                uxQueueMessagesWaiting( ( QueueHandle_t ) ( xSemaphore ) )
//add for esp32 end
#endif

#endif /* SEMAPHORE_H */
