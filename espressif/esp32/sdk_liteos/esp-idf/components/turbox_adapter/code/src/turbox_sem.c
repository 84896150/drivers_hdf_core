/**
 * Copyright (c) 2020-2021 ThunderSoft
 * All Rights Reserved by Thunder Software Technology Co., Ltd and its affiliates.
 * You may not use, copy, distribute, modify, transmit in any form this file
 * except in compliance with ThunderSoft in writing by applicable law.
 *
 */
/**
 * @file    turbox_sem.c
 * @brief   harmony bl platform semaphore adaptor
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
#include <FreeRTOS.h>
#include <list.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <cmsis_os2.h>
#include <los_memory.h>
#include <turbox_core_data.h>

#define tsSEM_TYPE_BASE                    ( ( uint8_t ) 0U )
#define tsSEM_TYPE_SET                     ( ( uint8_t ) 0U )
#define tsSEM_TYPE_MUTEX                   ( ( uint8_t ) 1U )
#define tsSEM_TYPE_COUNTING_SEMAPHORE      ( ( uint8_t ) 2U )
#define tsSEM_TYPE_BINARY_SEMAPHORE        ( ( uint8_t ) 3U )
#define tsSEM_TYPE_RECURSIVE_MUTEX         ( ( uint8_t ) 4U )

#define tspdTRUE                           ( ( uint8_t ) 1U )
#define tspdFALSE                          ( ( uint8_t ) 0U )

#define TSSEM_PARA_UNUSED(x)              ((void) (x))
/**
 * @brief create dynamic binary semaphore
 * 
 * @param a [in] SemaphoreHandle_t sem
 * @return QueueHandle_t  
 * @retval
 * 1 : result1； queue Handle
 * @see
 */
QueueHandle_t turboxCreateDynamicBinarySem(SemaphoreHandle_t sem) {
    QueueHandle_t xReturn = NULL;

    TSSEM_PARA_UNUSED(sem);

    /* malloc memory for creating dynamic semaphore */
    xReturn = (QueueHandle_t)LOS_MemAlloc(m_aucSysMem0, sizeof(struct QueueDefinition));
    if (NULL != xReturn) {
        xReturn->pvLiteOSSem = osSemaphoreNew(INITIALIZATION_VALUE_1, INITIALIZATION_VALUE_1, NULL);
        if (NULL == xReturn->pvLiteOSSem) {
            TURBOX_PRINTF("osSemaphoreNew Failed\n");
            return NULL;
        }

        xReturn->uxLength = INITIALIZATION_VALUE_1;
        xReturn->uxItemSize = INITIALIZATION_VALUE_0;
        xReturn->ucQueueType = tsSEM_TYPE_BINARY_SEMAPHORE;

        /* dynamic allocate */
        xReturn->ucStaticallyAllocated = tspdFALSE;
    } else {
        TURBOX_PRINTF("tsSemaphoreCreateBinary create null semaphore!\n");
    }

    return xReturn;
}

/**
 * @brief create static binary semaphore
 * 
 * @param a [in] StaticQueue_t *pxStaticQueue static queue
 * @return QueueHandle_t
 * @retval
 * 1 : result1； queue handle
 * @see
 */
QueueHandle_t turboxCreateStaticBinarySem(StaticQueue_t *pxStaticQueue) {
    QueueHandle_t xReturn = NULL;

    if (NULL == pxStaticQueue) {
        TURBOX_PRINTF("pxStaticQueue is NULL\n");
        return NULL;
    }

    xReturn = (QueueHandle_t) pxStaticQueue;
    xReturn->pvLiteOSSem = osSemaphoreNew(INITIALIZATION_VALUE_1, INITIALIZATION_VALUE_1, NULL);
    if (NULL == xReturn->pvLiteOSSem) {
        TURBOX_PRINTF("osSemaphoreNew Failed\n");
        return NULL;
    }

    xReturn->uxLength = INITIALIZATION_VALUE_1;
    xReturn->uxItemSize = INITIALIZATION_VALUE_0;
    xReturn->ucQueueType = tsSEM_TYPE_BINARY_SEMAPHORE;

    /* create static semaphore, no need to free */
    xReturn->ucStaticallyAllocated = tspdTRUE;

    return xReturn;
}

/**
 * @brief Get semaphore
 * 
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @param b [in] xTicksToWait wait ticks
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdTRUE/ pdFALSE
 * @see
 */
BaseType_t turboxGetSem(QueueHandle_t xQueue, TickType_t xTicksToWait) {
    BaseType_t xReturn = pdTRUE;

    if (NULL == xQueue) {
        TURBOX_PRINTF("xQueue is NULL\n");
        xReturn = pdFALSE;
        return xReturn;
    }
    
    /* check semaphore is mutex or semaphore */
    if (tsSEM_TYPE_MUTEX == xQueue->ucQueueType) {
        if (NULL == xQueue->pvLiteOSMutexSem) {
            TURBOX_PRINTF("xQueue->pvLiteOSMutexSem is NULL\n");
            xReturn = pdFALSE;
            return xReturn;
        }

        if (osOK == osMutexAcquire(xQueue->pvLiteOSMutexSem, xTicksToWait)) {
            xReturn = pdTRUE;
        } else {
            xReturn = pdFALSE;
            TURBOX_PRINTF("osMutexAcquire Failed\n");
        }
    } else {
        if (NULL == xQueue->pvLiteOSSem) {
            TURBOX_PRINTF("xQueue->pvLiteOSSem is NULL\n");
            xReturn = pdFALSE;
            return xReturn;
        }

        if (osOK == osSemaphoreAcquire(xQueue->pvLiteOSSem, xTicksToWait)) {
            xReturn = pdTRUE;
        } else {
            xReturn = pdFALSE;
            TURBOX_PRINTF("osSemaphoreAcquire Failed\n");
        }
    }

    return xReturn;
}

/**
 * @brief get semaphore
 * 
 * @param a [in] QueueHandle_t xMutex
 * @param b [in] TickType_t xTicksToWait
 * @return BaseType_t
 * @retval
 * 1 : result1； pdFALSE/pdTRUE
 * @see
 */
BaseType_t turboxGetSemRecursive(QueueHandle_t xMutex, TickType_t xTicksToWait) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == xMutex) {
        TURBOX_PRINTF("xMutex is NULL\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    /* check semaphore is mutex or semaphore */
    if (tsSEM_TYPE_MUTEX == xMutex->ucQueueType) {
        if (NULL == xMutex->pvLiteOSMutexSem) {
            TURBOX_PRINTF("xMutex->pvLiteOSMutexSem is NULL\n");
            xReturn = pdFALSE;
            return xReturn;
        }
        if (osOK == osMutexAcquire(xMutex->pvLiteOSMutexSem, xTicksToWait)) {
            xReturn = pdTRUE;
        } else {
            xReturn = pdFALSE;
            TURBOX_PRINTF("osMutexAcquire Failed\n");
        }
    } else {
        if (NULL == xMutex->pvLiteOSSem) {
            TURBOX_PRINTF("xMutex->pvLiteOSSem is NULL\n");
            xReturn = pdFALSE;
            return xReturn;
        }

        if (osOK == osSemaphoreAcquire(xMutex->pvLiteOSSem, xTicksToWait)) {
            xReturn = pdTRUE;
        } else {
            xReturn = pdFALSE;
            TURBOX_PRINTF("osSemaphoreAcquire Failed\n");
        }
    }

    return xReturn;
}

/**
 * @brief put semaphore
 * 
 * @param a [in] QueueHandle_t sem
 * @return BaseType_t
 * @retval
 * 1 : result1； pdFALSE/ pdTRUE
 * @see
 */
BaseType_t turboxPutSem(QueueHandle_t sem) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == sem) {
        TURBOX_PRINTF("sem is NULL\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    /* check semaphore is mutex or semaphore */
    if (tsSEM_TYPE_MUTEX == sem->ucQueueType) {
        if (NULL == sem->pvLiteOSMutexSem) {
            TURBOX_PRINTF("sem->pvLiteOSMutexSem is NULL\n");
            xReturn = pdFALSE;
            return xReturn;
        }

        if (osOK == osMutexRelease(sem->pvLiteOSMutexSem)) {
            xReturn = pdTRUE;
        } else {
            TURBOX_PRINTF("osMutexRelease Failed\n");
            xReturn = pdFALSE;
        }
    } else {
        if (NULL == sem->pvLiteOSSem) {
            TURBOX_PRINTF("sem->pvLiteOSSem is NULL\n");
            xReturn = pdFALSE;
            return xReturn;
        }

        if (osOK == osSemaphoreRelease(sem->pvLiteOSSem)) {
            xReturn = pdTRUE;
        } else {
            TURBOX_PRINTF("osSemaphoreRelease Failed\n");
            xReturn = pdFALSE;            
        }
    }

    return xReturn;
}

/**
 * @brief put semaphore
 * 
 * @param a [in] QueueHandle_t xMutex
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdFALSE/pdTRUE
 * 2 : result2；
 * @see f()
 */
BaseType_t turboxPutSemRecursive(QueueHandle_t xMutex) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == xMutex) {
        TURBOX_PRINTF("xMutex is NULL\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    /* check semaphore is mutex or semaphore */
    if (tsSEM_TYPE_MUTEX == xMutex->ucQueueType) {
        if (NULL == xMutex->pvLiteOSMutexSem) {
            TURBOX_PRINTF("xMutex->pvLiteOSMutexSem is NULL\n");
            xReturn = pdFALSE;
            return xReturn;
        }

        if (osOK == osMutexRelease(xMutex->pvLiteOSMutexSem)) {
            xReturn = pdTRUE;
        } else {
            TURBOX_PRINTF("osMutexRelease Failed\n");
            xReturn = pdFALSE;
        }
    } else {
        if (NULL == xMutex->pvLiteOSSem) {
            TURBOX_PRINTF("xMutex->pvLiteOSSem is NULL\n");
            xReturn = pdFALSE;
            return xReturn;
        }

        if (osOK == osSemaphoreRelease(xMutex->pvLiteOSSem)) {
            xReturn = pdTRUE;
        } else {
            TURBOX_PRINTF("osSemaphoreRelease Failed\n");
            xReturn = pdFALSE;            
        }
    }

    return xReturn;
}

/**
 * @brief Put Semaphore From ISR
 * 
 * @param a [in] QueueHandle_t xQueue: queue handle
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdFALSE/pdTRUE
 * @see
 */
BaseType_t turboxPutSemFromISR(QueueHandle_t xQueue, BaseType_t * const pxHigherPriorityTaskWoken) {
    BaseType_t xReturn = pdFALSE;

    if (NULL == xQueue) {
        TURBOX_PRINTF("xQueue is NULL\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    /* check semaphore is mutex or semaphore */
    if (tsSEM_TYPE_MUTEX == xQueue->ucQueueType) {
        if (NULL == xQueue->pvLiteOSMutexSem) {
            TURBOX_PRINTF("xQueue->pvLiteOSMutexSem is NULL\n");
            xReturn = pdFALSE;
            return xReturn;
        }

        if (osOK == osMutexRelease(xQueue->pvLiteOSMutexSem)) {
            xReturn = pdTRUE;
        } else {
            TURBOX_PRINTF("osMutexRelease Failed\n");
            xReturn = pdFALSE;
        }
    } else {
        if (NULL == xQueue->pvLiteOSSem) {
            TURBOX_PRINTF("xQueue->pvLiteOSSem is NULL\n");
            xReturn = pdFALSE;
            return xReturn;
        }
        
        if (osOK == osSemaphoreRelease(xQueue->pvLiteOSSem)) {
            xReturn = pdTRUE;
        } else {
            TURBOX_PRINTF("osSemaphoreRelease Failed\n");
            xReturn = pdFALSE;            
        }
    }

    return xReturn;
}

/**
 * @brief create dynamic mutex
 * 
 * @param a [-] void
 * @return QueueHandle_t
 * @retval
 * 1 : result1； queue handle
 * @see
 */
QueueHandle_t turboxCreateDynamicMux(void) {
    QueueHandle_t xReturn = NULL;

    xReturn = (QueueHandle_t)LOS_MemAlloc(m_aucSysMem0, sizeof(struct QueueDefinition));
    if (NULL != xReturn) {
        xReturn->pvLiteOSMutexSem = osMutexNew(NULL);
        if (NULL == xReturn->pvLiteOSMutexSem) {
            TURBOX_PRINTF("osMutexNew Failed!\n");    
            return NULL;
        }

        xReturn->uxLength = INITIALIZATION_VALUE_1;
        xReturn->uxItemSize = INITIALIZATION_VALUE_0;
        xReturn->ucQueueType = tsSEM_TYPE_MUTEX;

        /* dynamic mutex, need to free memory */
        xReturn->ucStaticallyAllocated = tspdFALSE;
    } else {
        TURBOX_PRINTF("create Memory Failed for mutex!\n");
        xReturn = NULL;
    }

    return xReturn;
}

/**
 * @brief create binary semaphore
 * 
 * @param a [-] void
 * @return QueueHandle_t
 * @retval
 * 1 : result1； queue handle
 * @see
 */
QueueHandle_t turboxSemaphoreCreateBinary(void) {
    QueueHandle_t xReturn = NULL;

    xReturn = (QueueHandle_t)LOS_MemAlloc(m_aucSysMem0, sizeof(struct QueueDefinition));
    if (NULL != xReturn) {
        xReturn->pvLiteOSSem = osSemaphoreNew(INITIALIZATION_VALUE_1, INITIALIZATION_VALUE_1, NULL);
        if (NULL == xReturn->pvLiteOSSem) {
            TURBOX_PRINTF("osSemaphoreNew Failed!\n");    
            return NULL;
        }
        xReturn->uxLength = INITIALIZATION_VALUE_1;
        xReturn->uxItemSize = INITIALIZATION_VALUE_0;
        xReturn->ucQueueType = tsSEM_TYPE_BINARY_SEMAPHORE;

        /* dynamic mutex, need to free memory */
        xReturn->ucStaticallyAllocated = tspdFALSE;
    } else {
        TURBOX_PRINTF("create Memory Failed for mutex!\n");
        xReturn = NULL;
    }
    return xReturn;
}
/**
 * @brief create static mutex
 * 
 * @param a [in] StaticQueue_t *pxStaticQueue: static queue
 * @return QueueHandle_t 
 * @retval
 * 1 : result1； queue handle
 * @see
 */
QueueHandle_t turboxCreateStaticMux(StaticQueue_t *pxStaticQueue) {
    QueueHandle_t xReturn = NULL;

    if (NULL == pxStaticQueue) {
        TURBOX_PRINTF("pxStaticQueue is NULL!\n");
        return NULL;
    }

    xReturn = (QueueHandle_t)pxStaticQueue;
    xReturn->pvLiteOSMutexSem = osMutexNew(NULL);
    if (NULL == xReturn->pvLiteOSMutexSem) {
        TURBOX_PRINTF("osMutexNew Failed!\n");    
        return NULL;
    }

    xReturn->uxLength = INITIALIZATION_VALUE_1;
    xReturn->uxItemSize = INITIALIZATION_VALUE_0;
    xReturn->ucQueueType = tsSEM_TYPE_MUTEX;

    /* static mutex, no need to free memory */
    xReturn->ucStaticallyAllocated = tspdTRUE;

    return xReturn;
}

/**
 * @brief create mutex
 * 
 * @param a [-] void
 * @return  QueueHandle_t
 * @retval
 * 1 : result1； queue handle
 * @see
 */
QueueHandle_t turboxCreateRecursiveMux(void) {
    QueueHandle_t xReturn = NULL;

    xReturn = (QueueHandle_t)LOS_MemAlloc(m_aucSysMem0, sizeof(struct QueueDefinition));
    if (NULL != xReturn) {
        xReturn->pvLiteOSMutexSem = osMutexNew(NULL);
        if (NULL == xReturn->pvLiteOSMutexSem) {
            TURBOX_PRINTF("osMutexNew Failed!\n");    
            return NULL;
        }        

        xReturn->uxLength = INITIALIZATION_VALUE_1;
        xReturn->uxItemSize = INITIALIZATION_VALUE_0;
        xReturn->ucQueueType = tsSEM_TYPE_MUTEX;

        /* dynamic mutex, need to free memory */
        xReturn->ucStaticallyAllocated = tspdFALSE;
    } else {
        TURBOX_PRINTF("create Memory Failed for mutex!\n");
        xReturn = NULL;
    }
    return xReturn;
}

/**
 * @brief get semaphore count
 * 
 * @param a [in] const UBaseType_t uxMaxCount: max count
 * @param b [in] const UBaseType_t uxInitialCount: initial count
 * @return QueueHandle_t 
 * @retval
 * 1 : result1； queue handle
 * @see
 */
QueueHandle_t turboxGetSemCount(const UBaseType_t uxMaxCount, 
                                             const UBaseType_t uxInitialCount) {
    QueueHandle_t xReturn = NULL;
    
    xReturn = (QueueHandle_t)LOS_MemAlloc(m_aucSysMem0, sizeof(struct QueueDefinition));
    if (NULL != xReturn) {
        xReturn->pvLiteOSSem = osSemaphoreNew(uxMaxCount, uxInitialCount, NULL);
        if (NULL == xReturn->pvLiteOSSem) {
            TURBOX_PRINTF("osSemaphoreNew Failed!\n");    
            return NULL;
        }

        xReturn->ucQueueType = tsSEM_TYPE_COUNTING_SEMAPHORE;
        /* dynamic mutex, need to free memory */
        xReturn->ucStaticallyAllocated = tspdFALSE;
    } else {
        TURBOX_PRINTF("create Memory Failed for mutex!\n");
        xReturn = NULL;        
    }
    return xReturn;
}

/**
 * @brief delete semaphore or mutex
 * 
 * @param a [in] QueueHandle_t xQueue
 * @return int 
 * @retval
 * 1 : result1； 
 * 2 : result2；
 * @see f()
 */
void turboxDelSemMux(QueueHandle_t xQueue) {
    if (NULL == xQueue) {
        TURBOX_PRINTF("xQueue is NULL!\n");
        return;
    }

    if (tsSEM_TYPE_MUTEX == xQueue->ucQueueType) {
        if (osOK != osMutexDelete(xQueue->pvLiteOSMutexSem)) {
            TURBOX_PRINTF("osMutexDelete Failed!\n");
            return;
        }
    } else {
        if (osOK != osSemaphoreDelete(xQueue->pvLiteOSSem)) {
            TURBOX_PRINTF("osSemaphoreDelete Failed!\n");
            return;            
        }
    }

    if (tspdFALSE == xQueue->ucStaticallyAllocated) {
        if (LOS_NOK == LOS_MemFree(m_aucSysMem0, xQueue)) {
            TURBOX_PRINTF("LOS_MemFree Failed!\n");
        }
    }

    return;
}

//****************************************************************************************************//
//                                    add for esp32
//****************************************************************************************************//
/**
 * @brief determine if the calling task is the mutex
 *
 * @param a [in] QueueHandle_t xMutex
 * @param b [in] TickType_t xTicksToWait
 * @return BaseType_t
 * @retval
 * 1 : result1；pdFALSE/pdTRUE
 * @see f()
 */
TaskHandle_t turboxGetMutexHolderSemaphore( QueueHandle_t xSemaphore ) {
    TaskHandle_t pxReturn;
    Queue_t * const pxSemaphore = ( Queue_t * ) xSemaphore;

        /* This function is called by xSemaphoreGetMutexHolder(), and should not
         * be called directly.  Note:  This is a good way of determining if the
         * calling task is the mutex holder, but not a good way of determining the
         * identity of the mutex holder, as the holder may change between the
         * following critical section exiting and the function returning. */
    taskENTER_CRITICAL();
    {
        if( pxSemaphore->ucQueueType == NULL ) {
            pxReturn = pxSemaphore->u.xSemaphore.xMutexHolder;
        }
        else {
            pxReturn = NULL;
        }
    }
    taskEXIT_CRITICAL();

    return pxReturn;
} /*lint !e818 xSemaphore cannot be a pointer to const because it is a typedef. */

/**
 * @brief get semaphore from an ISR
 *
 * @param a [in] QueueHandle_t xQueue
 * @param b [in] BaseType_t * const pxHigherPriorityTaskWoken
 * @return BaseType_t
 * @retval
 * 1 : result1；pdFALSE/pdTRUE
 * @see f()
 */
BaseType_t turboxTakeFromISRSemaphore( QueueHandle_t xQueue, TickType_t pxHigherPriorityTaskWoken )
{
    BaseType_t xReturn = pdFALSE;

    if (NULL == xQueue) {
        TURBOX_PRINTF("xQueue is NULL\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    /* check semaphore is mutex or semaphore */
    if (tsSEM_TYPE_MUTEX == xQueue->ucQueueType) {
            TURBOX_PRINTF("xQueue->ucQueueType is mutex\n");
            xReturn = pdFALSE;
            return xReturn;
    } else {
        if (NULL == xQueue->pvLiteOSSem) {
            TURBOX_PRINTF("xQueue->pvLiteOSSem is NULL\n");
            xReturn = pdFALSE;
            return xReturn;
        }
        if (osOK == osMutexAcquire(xQueue->pvLiteOSMutexSem, pxHigherPriorityTaskWoken)) {
            xReturn = pdTRUE;
        } else {
            xReturn = pdFALSE;
            TURBOX_PRINTF("osMutexAcquire Failed\n");
        } 
    }

    return xReturn;
    
}