/**
 * Copyright (c) 2020-2021 ThunderSoft
 * All Rights Reserved by Thunder Software Technology Co., Ltd and its affiliates.
 * You may not use, copy, distribute, modify, transmit in any form this file
 * except in compliance with ThunderSoft in writing by applicable law.
 *
 */
/**
 * @file    turbox_queue.c
 * @brief   harmony bl platform queue adaptor
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
#include <queue.h>
#include <string.h>
#include <cmsis_os2.h>
#include <los_memory.h>
#include "turbox_core_data.h"
#include "semphr.h"

/**
 * @brief create dynamic queue
 * 
 * @param a [in] const UBaseType_t uxQueueLength: queue length
 * @param b [in] const UBaseType_t uxItemSize: item size
 * @param c [in] const uint8_t ucQueueType: queue type
 * @return QueueHandle_t
 * @retval
 * 1 : result1； queue handle
 * @see
 */
QueueHandle_t turboxCreateDynamicQueue(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType)
{
    QueueHandle_t xReturn = NULL;
    if (queueQUEUE_TYPE_BASE == ucQueueType) {
        /* alloc memory for creating queue */
        xReturn = (QueueHandle_t)LOS_MemAlloc(m_aucSysMem0, sizeof(xQUEUE));
        if (NULL == xReturn) {
            TURBOX_PRINTF("malloc data failed!\r\n");
            return NULL;
        }

        /* new message queue */
        (xReturn->pvLiteOSQueue) = (osMessageQueueId_t)osMessageQueueNew(uxQueueLength, uxItemSize, NULL);
        if (NULL == (xReturn->pvLiteOSQueue)) {
            TURBOX_PRINTF("osMessageQueueNew failed!\r\n");
            return NULL;
        }

        /* dyanmic queue; need to free */
        xReturn->ucStaticallyAllocated  = pdFALSE;
        xReturn->ucQueueType            = ucQueueType;
        xReturn->uxLength               = uxQueueLength;
        xReturn->uxItemSize             = uxItemSize;
    } else {
        TURBOX_PRINTF("xQueueGenericCreate is error type %d\r\n", ucQueueType);
        while(INITIALIZATION_VALUE_1);
    }

    return xReturn;
}

/**
 * @brief create static queue
 * 
 * @param a [in] const UBaseType_t uxQueueLength: queue length
 * @param b [in] const UBaseType_t uxItemSize: item size
 * @param c [in] uint8_t *pucQueueStorage: queue memory
 * @param d [in] StaticQueue_t *pxStaticQueue: static queue
 * @return QueueHandle_t 
 * @retval
 * 1 : result1； queue handle
 * @see
 */
QueueHandle_t turboxCreateStaticQueue(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, uint8_t *pucQueueStorage, StaticQueue_t *pxStaticQueue) {
    osMessageQueueAttr_t attr;
    QueueHandle_t xReturn = NULL;

    if (NULL == pucQueueStorage || NULL == pxStaticQueue) {
        TURBOX_PRINTF("pucQueueStorage = %p---pxStaticQueue = %p\r\n", pucQueueStorage, pxStaticQueue);
        return NULL;
    }

    xReturn = (QueueHandle_t)pxStaticQueue;
    memset(&attr, INITIALIZATION_VALUE_0, sizeof(osMessageQueueAttr_t));

    attr.cb_mem     = pxStaticQueue;
    attr.mq_mem     = pucQueueStorage;
    attr.cb_size    = sizeof(StaticQueue_t);
    attr.mq_size    = (uxQueueLength * uxItemSize);

    /* new message queue */
    (xReturn->pvLiteOSQueue) = osMessageQueueNew(uxQueueLength, uxItemSize, &attr);
    if (NULL == (xReturn->pvLiteOSQueue)) {
        TURBOX_PRINTF("osMessageQueueNew Failed!\r\n");
        return NULL;
    }

    /* static memory queue, no need to free */
    xReturn->ucStaticallyAllocated  = pdTRUE;
    xReturn->ucQueueType            = queueQUEUE_TYPE_BASE;
    xReturn->uxLength               = uxQueueLength;
    xReturn->uxItemSize             = uxItemSize;

    return xReturn;
}

/**
 * @brief send data to queue
 * 
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] const void * const pvItemToQueue : item
 * @param c [in] TickType_t xTicksToWait: wait ticks
 * @return BaseType_t 
 * @retval
 * 1 : result1； the result of put data to queue 
 * @see
 */
BaseType_t turboxSendToQueueBack( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == xQueue) {
        TURBOX_PRINTF("Queue Handle is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == (xQueue->pvLiteOSQueue)) {
        TURBOX_PRINTF("xQueue->pvLiteOSQueue is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == pvItemToQueue) {
        TURBOX_PRINTF("pvItemToQueue Handle is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    /* put the message to queue */
    if (osOK == osMessageQueuePut((osMessageQueueId_t)xQueue->pvLiteOSQueue, 
                                    pvItemToQueue, INITIALIZATION_VALUE_0, xTicksToWait)) {
        xReturn = pdTRUE;
    } else {
        TURBOX_PRINTF("Cannot put message to queue!\r\n");
        xReturn = pdFALSE;
    }

    return xReturn;
}

/**
 * @brief send message to queue
 * 
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] const void * const pvItemToQueue : item
 * @param c [in] TickType_t xTicksToWait: wait ticks
 * @return BaseType_t 
 * @retval
 * 1 : result1； the result of put data to queue 
 * @see
 */
BaseType_t turboxSendToQueue(QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == xQueue) {
        TURBOX_PRINTF("Queue Handle is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == (xQueue->pvLiteOSQueue)) {
        TURBOX_PRINTF("xQueue->pvLiteOSQueue is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == pvItemToQueue) {
        TURBOX_PRINTF("pvItemToQueue Handle is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (osOK == osMessageQueuePut((osMessageQueueId_t)(xQueue->pvLiteOSQueue), 
                                    pvItemToQueue, INITIALIZATION_VALUE_0, xTicksToWait)) {
        xReturn = pdTRUE;
    } else {
        TURBOX_PRINTF("Cannot put message to queue!\r\n");
        xReturn = pdFALSE;
    }

    return xReturn;
}

/**
 * @brief receive data from queue
 * 
 * @param a [in] QueueHandle_t xQueue: data src buffer
 * @param b [out] void * const pvBuffer: data dst buffer
 * @param c [in] TickType_t xTicksToWait: wait ticks
 * @return BaseType_t 
 * @retval
 * 1 : result1； the result of receive data from queue 
 * @see
 */
BaseType_t turboxRecvFromQueue(QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == xQueue) {
        TURBOX_PRINTF("Queue Handle is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == (xQueue->pvLiteOSQueue)) {
        TURBOX_PRINTF("xQueue->pvLiteOSQueue is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == pvBuffer) {
        TURBOX_PRINTF("pvItemToQueue Handle is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (osOK == osMessageQueueGet((osMessageQueueId_t)xQueue->pvLiteOSQueue, pvBuffer, NULL, xTicksToWait)) {
        xReturn = pdTRUE;
    } else {
        TURBOX_PRINTF("Cannot receive message to queue!\r\n");
        xReturn = pdFALSE;
    }
    return xReturn;
}

/**
 * @brief wait for queue message
 * 
 * @param a [in] const QueueHandle_t xQueue: queue handle
 * @return UBaseType_t
 * @retval
 * 1 : result1； count
 * @see
 */
UBaseType_t turboxWaitQueueMsg(const QueueHandle_t xQueue) {
    UBaseType_t xReturn = INITIALIZATION_VALUE_0;

    if (NULL == xQueue) {
        TURBOX_PRINTF("Queue Handle is NULL!\r\n");
        xReturn = INITIALIZATION_VALUE_0;
        return xReturn;
    }

    if (NULL == (xQueue->pvLiteOSQueue)) {
        TURBOX_PRINTF("xQueue->pvLiteOSQueue is NULL!\r\n");
        xReturn = INITIALIZATION_VALUE_0;
        return xReturn;
    }

    xReturn = (BaseType_t)osMessageQueueGetCount((osMessageQueueId_t)xQueue->pvLiteOSQueue);
    
    return xReturn;
}

/**
 * @brief delete queue
 * 
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @return void
 * @retval
 * @see
 */
void turboxDeleteQueue(QueueHandle_t xQueue) {
    if (NULL == xQueue) {
        TURBOX_PRINTF("Queue Handle is NULL!\r\n");
        return;
    }

    if (xQueue->ucQueueType == queueQUEUE_TYPE_BASE) {
        if (osOK != osMessageQueueDelete((osMessageQueueId_t)xQueue->pvLiteOSQueue)) {
            TURBOX_PRINTF("xQueue->pvLiteOSQueue delete Failed!\r\n");
            return;
        }

        if((uint8_t)pdFALSE == xQueue->ucStaticallyAllocated) {
            if (LOS_NOK == LOS_MemFree(m_aucSysMem0, xQueue)) {
                TURBOX_PRINTF("free memory failed!\r\n");
                return;
            }
        }
    } else if (xQueue->ucQueueType == queueQUEUE_TYPE_MUTEX || xQueue->ucQueueType == queueQUEUE_TYPE_RECURSIVE_MUTEX ||
               xQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE || xQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE) {
        turboxDelSemMux(xQueue);
    }  else {
        TURBOX_PRINTF("xQueueGenericCreate is error type %d\r\n", xQueue->ucQueueType);
        while(1);
    }
    return;
}

/**
 * @brief Send to queue in ISR
 * 
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] const void * const pvItemToQueue: item
 * @param c [out] BaseType_t * const pxHigherPriorityTaskWoken
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdTRUE/pdFALSE
 * @see
 */           
BaseType_t turboxSendToQueueFromISR( QueueHandle_t xQueue, 
                                     const void * const pvItemToQueue, 
                                     BaseType_t * const pxHigherPriorityTaskWoken) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == xQueue) {
        TURBOX_PRINTF("Queue Handle is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == xQueue->pvLiteOSQueue) {
        TURBOX_PRINTF("xQueue->pvLiteOSQueue is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn; 
    }

    if (NULL == pvItemToQueue) {
        TURBOX_PRINTF("pvItemToQueue is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }
    
    if (osOK == osMessageQueuePut((osMessageQueueId_t)xQueue->pvLiteOSQueue,
                                  (void*)pvItemToQueue, INITIALIZATION_VALUE_0, INITIALIZATION_VALUE_0)) {
        xReturn = pdTRUE;
    } else {
        TURBOX_PRINTF("osMessageQueuePut Failed!\r\n");
        xReturn = pdFALSE;
    }

    return xReturn;
}

/**
 * @brief func是一个普通成员函数
 * 
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] const void * const pvItemToQueue: item
 * @param c [in] TickType_t xTicksToWait: wait ticks
 * @param d [in] const BaseType_t xCopyPosition: copy position
 * @return BaseType_t 
 * @retval
 * 1 : result1；  pdTRUE/pdFALSE
 * @see
 */
BaseType_t turboxSendQueue(QueueHandle_t xQueue,
                            const void * const pvItemToQueue,
                            TickType_t xTicksToWait,
                            const BaseType_t xCopyPosition) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == xQueue) {
        TURBOX_PRINTF("Queue Handle is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == xQueue->pvLiteOSQueue) {
        TURBOX_PRINTF("xQueue->pvLiteOSQueue is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn; 
    }    

    if (NULL == pvItemToQueue) {
        TURBOX_PRINTF("pvItemToQueue is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (osOK == osMessageQueuePut((osMessageQueueId_t)xQueue->pvLiteOSQueue, 
                                    pvItemToQueue, INITIALIZATION_VALUE_0, xTicksToWait)) {
        xReturn = pdTRUE;
    } else {
        TURBOX_PRINTF("osMessageQueuePut Failed!\r\n");
        xReturn = pdFALSE;
    }

    return xReturn;
}

/**
 * @brief send queue from ISR
 * 
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] const void * const pvItemToQueue: item
 * @param c [in] BaseType_t * const pxHigherPriorityTaskWoken
 * @param d [in] const BaseType_t xCopyPosition: copy position
 * @return BaseType_t 
 * @retval
 * 1 : result1；  pdTRUE/pdFALSE
 * @see
 */
BaseType_t turboxSendQueueFromISR(QueueHandle_t xQueue,
                                const void * const pvItemToQueue,
                                BaseType_t * const pxHigherPriorityTaskWoken,
                                const BaseType_t xCopyPosition) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == xQueue) {
        TURBOX_PRINTF("Queue Handle is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == xQueue->pvLiteOSQueue) {
        TURBOX_PRINTF("xQueue->pvLiteOSQueue is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn; 
    }    

    if (NULL == pvItemToQueue) {
        TURBOX_PRINTF("pvItemToQueue is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (osOK == osMessageQueuePut((osMessageQueueId_t)xQueue->pvLiteOSQueue, 
                                    pvItemToQueue, INITIALIZATION_VALUE_0, INITIALIZATION_VALUE_0)) {
        xReturn = pdTRUE;
    } else {
        TURBOX_PRINTF("osMessageQueuePut Failed!\r\n");
        xReturn = pdFALSE;
    }

    return xReturn;
}

//****************************************************************************************************//
//                                    add for esp32
//****************************************************************************************************//

/**
 * @brief Adds a queue or semaphore to a queue set
 *
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] const void * const pvItemToQueue: item
 * @param c [in] BaseType_t * const pxHigherPriorityTaskWoken
 * @param d [in] const BaseType_t xCopyPosition: copy position
 * @return BaseType_t
 * @retval
 * 1 : result1；  pdTRUE/pdFALSE
 * @see
 */
BaseType_t turboxAddToSetQueue( QueueSetMemberHandle_t xQueueOrSemaphore,
                                  QueueSetHandle_t xQueueSet ) {
    BaseType_t xReturn;

    taskENTER_CRITICAL();
    {
    if( ( ( Queue_t * ) xQueueOrSemaphore )->pxQueueSetContainer != NULL ) {
        /* Cannot add a queue/semaphore to more than one queue set. */
        xReturn = pdFAIL;
    }
    else if( ( ( Queue_t * ) xQueueOrSemaphore )->uxMessagesWaiting != ( UBaseType_t ) 0 ) {
        /* Cannot add a queue/semaphore to a queue set if there are already
         * items in the queue/semaphore. */
        xReturn = pdFAIL;
    }
    else {
        ( ( Queue_t * ) xQueueOrSemaphore )->pxQueueSetContainer = xQueueSet;
        xReturn = pdPASS;
    }
    }
    taskEXIT_CRITICAL();

    return xReturn;
}

/**
 * @brief Removes a queue or semaphore from a queue set
 *
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] const void * const pvItemToQueue: item
 * @param c [in] BaseType_t * const pxHigherPriorityTaskWoken
 * @param d [in] const BaseType_t xCopyPosition: copy position
 * @return BaseType_t
 * @retval
 * 1 : result1；  pdTRUE/pdFALSE
 * @see
 */
BaseType_t turboxRemoveFromSetQueue( QueueSetMemberHandle_t xQueueOrSemaphore,
                                    QueueSetHandle_t xQueueSet ) {
    BaseType_t xReturn;
    Queue_t * const pxQueueOrSemaphore = ( Queue_t * ) xQueueOrSemaphore;

    if( pxQueueOrSemaphore->pxQueueSetContainer != xQueueSet ) {
        /* The queue was not a member of the set. */
        xReturn = pdFAIL;
    }
    else if( pxQueueOrSemaphore->uxMessagesWaiting != ( UBaseType_t ) 0 ) {
        /* It is dangerous to remove a queue from a set when the queue is
         * not empty because the queue set will still hold pending events for
         * the queue. */
        xReturn = pdFAIL;
    }
    else {
        taskENTER_CRITICAL();
        {
            /* The queue is no longer contained in the set. */
            pxQueueOrSemaphore->pxQueueSetContainer = NULL;
        }
        taskEXIT_CRITICAL();
        xReturn = pdPASS;
        }

    return xReturn;
} /*lint !e818 xQueueSet could not be declared as pointing to const as it is a typedef. */

/**
 * @brief Reset a queue back to its original empty state
 *
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] const void * const pvItemToQueue: item
 * @param c [in] BaseType_t * const pxHigherPriorityTaskWoken
 * @param d [in] const BaseType_t xCopyPosition: copy position
 * @return BaseType_t
 * @retval
 * 1 : result1；  pdTRUE/pdFALSE
 * @see
 */
BaseType_t turboxResetQueue( QueueHandle_t xQueue)
{
    BaseType_t xReturn = pdFALSE;
    // if( osMessageQueueReset (xQueue->pvLiteOSQueue) == 0 )
    // {
    //     xReturn = pdTRUE;
    // }
    // else
    // {
    //     xReturn = pdFALSE;
    // }

    return xReturn;
}

/**
 * @brief Check if a queue is full of messages
 *
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] const void * const pvItemToQueue: item
 * @param c [in] BaseType_t * const pxHigherPriorityTaskWoken
 * @param d [in] const BaseType_t xCopyPosition: copy position
 * @return BaseType_t
 * @retval
 * 1 : result1；  pdTRUE/pdFALSE
 * @see
 */
BaseType_t turboxIsQueueFullFromISR( const QueueHandle_t xQueue )
{
    BaseType_t xReturn;
    Queue_t * const pxQueue = xQueue;

    if( pxQueue->uxMessagesWaiting == pxQueue->uxLength ) {
        xReturn = pdTRUE;
    }
    else {
        xReturn = pdFALSE;
    }

    return xReturn;
} /*lint !e818 xQueue could not be pointer to const because it is a typedef. */

/**
 * @brief Receive an item from the queue
 *
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] const void * const pvItemToQueue: item
 * @param c [in] BaseType_t * const pxHigherPriorityTaskWoken
 * @param d [in] const BaseType_t xCopyPosition: copy position
 * @return BaseType_t
 * @retval
 * 1 : result1；  pdTRUE/pdFALSE
 * @see
 */
BaseType_t turboxReceiveFromISRQueue(QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait) {
    BaseType_t xReturn = pdFALSE;
    if (NULL == xQueue) {
        TURBOX_PRINTF("Queue Handle is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == (xQueue->pvLiteOSQueue)) {
        TURBOX_PRINTF("xQueue->pvLiteOSQueue is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (NULL == pvBuffer) {
        TURBOX_PRINTF("pvItemToQueue Handle is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    if (osOK == osMessageQueueGet((osMessageQueueId_t)xQueue->pvLiteOSQueue, pvBuffer, NULL, xTicksToWait)) {
        xReturn = pdTRUE;
    } else {
        TURBOX_PRINTF("Cannot receive message to queue!\r\n");
        xReturn = pdFALSE;
    }
    return xReturn;
}

/**
 * @brief Return the number of free spaces available in a queue.
 *
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] const void * const pvItemToQueue: item
 * @param c [in] BaseType_t * const pxHigherPriorityTaskWoken
 * @param d [in] const BaseType_t xCopyPosition: copy position
 * @return BaseType_t
 * @retval
 * 1 : result1；  pdTRUE/pdFALSE
 * @see
 */
UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue )
{
    UBaseType_t uxReturn;
    Queue_t * const pxQueue = xQueue;

    taskENTER_CRITICAL();
    {
        uxReturn = pxQueue->uxLength - pxQueue->uxMessagesWaiting;
    }
    taskEXIT_CRITICAL();

    return uxReturn;
} /*lint !e818 Pointer cannot be declared const as xQueue is a typedef not pointer. */
