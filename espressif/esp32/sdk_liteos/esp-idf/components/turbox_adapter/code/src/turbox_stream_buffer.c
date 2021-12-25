/**
 * Copyright (c) 2020-2021 ThunderSoft
 * All Rights Reserved by Thunder Software Technology Co., Ltd and its affiliates.
 * You may not use, copy, distribute, modify, transmit in any form this file
 * except in compliance with ThunderSoft in writing by applicable law.
 *
 */
/**
 * @file    turbox_stream_buf.c
 * @brief   harmony bl platform stream buffer adaptor
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
#include <string.h>
#include <stdio.h>
#include <task.h>
#include <cmsis_os2.h>
#include <los_memory.h>
#undef OS_RISCV_CUSTOM_IRQ_VECTOR_CNT
#include <los_interrupt.h>
#undef OS_RISCV_LOCAL_IRQ_VECTOR_CNT
#include "turbox_buffer.h"
#include "turbox_core_data.h"

/************************************************************************/
/* external public interface                                            */
/************************************************************************/
/**
 * @brief create stream buffer
 * 
 * @param a [in] size_t xBufferSizeBytes: buffer size
 * @param b [in] size_t xTriggerLevelBytes: trigger level
 * @return StreamBufferHandle_t 
 * @retval
 * 1 : result1； stream buffer handle
 * 2 : result2；
 * @see f()
 */
StreamBufferHandle_t turboxCreateSBuffer(size_t xBufferSizeBytes,
                                                   size_t xTriggerLevelBytes) {
    uint8_t *pucAllocatedMemory =  NULL;
    uint8_t ucFlags = INITIALIZATION_VALUE_0;

    ucFlags = sbFLAGS_IS_STREAM_BUFFER;
    if ((size_t)INITIALIZATION_VALUE_0 == xTriggerLevelBytes) {
        xTriggerLevelBytes = (size_t)INITIALIZATION_VALUE_1;
    }

    xBufferSizeBytes++;
    pucAllocatedMemory = (uint8_t*)LOS_MemAlloc(m_aucSysMem0, (xBufferSizeBytes + sizeof(StreamBuffer_t)));
    if (NULL != pucAllocatedMemory) {
        /* initialize new stream buffer */
        prvInitialiseNewStreamBuffer((StreamBuffer_t*)pucAllocatedMemory,
                                        pucAllocatedMemory + sizeof(StreamBuffer_t),
                                        xBufferSizeBytes,
                                        xTriggerLevelBytes,
                                        ucFlags);
    } else {
        TURBOX_PRINTF("LOS_MemAlloc memory Failed!\r\n");
        return NULL;
    }

    return ( StreamBufferHandle_t ) pucAllocatedMemory;
}

/**
 * @brief send to stream buffer
 * 
 * @param a [in] StreamBufferHandle_t xStreamBuffer: buffer entity
 * @param b [in] const void *pvTxData: Send data
 * @param c [in] size_t xDataLengthBytes: data length
 * @param c [in] TickType_t xTicksToWait: wait ticks
 * @return size_t 
 * @retval
 * 1 : result1； send count
 * @see
 */
size_t turboxSendToSBuffer( StreamBufferHandle_t xStreamBuffer,
                          const void *pvTxData,
                          size_t xDataLengthBytes,
                          TickType_t xTicksToWait ) {
    size_t xSpace = INITIALIZATION_VALUE_0;
    size_t xReturn = INITIALIZATION_VALUE_0;
    size_t xRequiredSpace = xDataLengthBytes;
    StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;

    /* need to wait for xTicksToWait */
    if ((TickType_t)INITIALIZATION_VALUE_0 != xTicksToWait) {
        /* Wait until the required number of bytes are free in the message buffer. */
        do {
            turboxEnterCritical();
            xSpace = xStreamBufferSpacesAvailable(pxStreamBuffer);
            /* the rest space is not enough */
            if (xSpace < xRequiredSpace) {
                xTaskNotifyStateClear(NULL);
                /* clear the notify state ,get the current task handle prepare to task wait */
                pxStreamBuffer->xTaskWaitingToSend = xTaskGetCurrentTaskHandle();
            } else {
                turboxExitCritical();
                break;
            }
            turboxExitCritical();
            TURBOX_PRINTF("wait for enough space\r\n");
            /* wait for the enough space in buffer */
            xTaskNotifyWait((uint32_t)INITIALIZATION_VALUE_0, (uint32_t)INITIALIZATION_VALUE_0, NULL, xTicksToWait);
            pxStreamBuffer->xTaskWaitingToSend = NULL;
        } while (INITIALIZATION_VALUE_0);
    }

    if ((size_t)INITIALIZATION_VALUE_0 == xSpace) {
        /* get the buffer available space Perhaps execute after wait */
        xSpace = xStreamBufferSpacesAvailable( pxStreamBuffer );
    }

    /* write the message to buffer pvTxData ----> pxStreamBuffer*/
    xReturn = prvWriteMessageToBuffer(pxStreamBuffer, pvTxData, xDataLengthBytes, xSpace, xRequiredSpace);
    if ((size_t)INITIALIZATION_VALUE_0 < xReturn) {
        if (prvBytesInBuffer(pxStreamBuffer) >= pxStreamBuffer->xTriggerLevelBytes) {
            osKernelLock();
            if ((pxStreamBuffer)->xTaskWaitingToReceive != NULL) {
                xTaskNotify(pxStreamBuffer->xTaskWaitingToReceive, (uint32_t)INITIALIZATION_VALUE_0, INITIALIZATION_VALUE_0);
                TURBOX_PRINTF("Notify receive Task to receive data ,aim to get memmory space\r\n");
                pxStreamBuffer->xTaskWaitingToReceive = NULL;
            }
            osKernelUnlock();
        }
    } else {
        TURBOX_PRINTF("Cannot write the message to buffer, return count is 0!\r\n");
    }

    return xReturn;
}

/**
 * @brief Send data to stream buffer from ISR
 * 
 * @param a [in] StreamBufferHandle_t xStreamBuffer: stream buffer
 * @param b [in] const void *pvTxData: src buffer
 * @param c [in] size_t xDataLengthBytes: data length
 * @param d [in] BaseType_t * const pxHigherPriorityTaskWoken
 * @return int 
 * @retval
 * 1 : result1； 
 * 2 : result2；
 * @see f()
 */
size_t turboxSendToSBufferFromISR( StreamBufferHandle_t xStreamBuffer,
                                 const void *pvTxData,
                                 size_t xDataLengthBytes,
                                 BaseType_t * const pxHigherPriorityTaskWoken ) {
    StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
    size_t xSpace = INITIALIZATION_VALUE_0;
    size_t xReturn = INITIALIZATION_VALUE_0;
    size_t xRequiredSpace = xDataLengthBytes;

    if (NULL == xStreamBuffer) {
        TURBOX_PRINTF("xStreamBuffer is NULL!\r\n");
        return 0;
    }

    if (NULL == pvTxData) {
        TURBOX_PRINTF("pvTxData is NULL!\r\n");
        return 0;
    }

    /* get the rest space of stream buffer */
    xSpace = xStreamBufferSpacesAvailable(pxStreamBuffer);

    /* write message to stream buffer */
    xReturn = prvWriteMessageToBuffer(pxStreamBuffer, pvTxData, xDataLengthBytes, xSpace, xRequiredSpace);
    if ((size_t)INITIALIZATION_VALUE_0 < xReturn) {
        if (prvBytesInBuffer(pxStreamBuffer) >= pxStreamBuffer->xTriggerLevelBytes) {
            /* no enough space in buffer */
            osKernelLock();
            if ((pxStreamBuffer)->xTaskWaitingToReceive != NULL) {
                xTaskNotifyFromISR(pxStreamBuffer->xTaskWaitingToReceive, (uint32_t)INITIALIZATION_VALUE_0, eNoAction, pxHigherPriorityTaskWoken);
                pxStreamBuffer->xTaskWaitingToReceive = NULL;
            }
            osKernelUnlock();
        }
    } else {
        TURBOX_PRINTF("Cannot write message to buffer!\r\n");
    }

    return xReturn;
}

/**
 * @brief receive data from buffer
 * 
 * @param a [in]: StreamBufferHandle_t xStreamBuffer: stream buffer
 * @param b [out]: void *pvRxData: dst buffer
 * @param c [in]: size_t xBufferLengthBytes: buffer length
 * @param d [in]: TickType_t xTicksToWait: wait ticks
 * @return size_t 
 * @retval
 * 1 : result1； receive count
 * @see
 */
size_t turboxRecvFromSBuffer(StreamBufferHandle_t xStreamBuffer,
                             void *pvRxData,
                             size_t xBufferLengthBytes,
                             TickType_t xTicksToWait) {
    StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
    size_t xReceivedLength = INITIALIZATION_VALUE_0;
    size_t xBytesAvailable = INITIALIZATION_VALUE_0;
    size_t xBytesToStoreMessageLength = INITIALIZATION_VALUE_0;

    if ((TickType_t)INITIALIZATION_VALUE_0 != xTicksToWait) {
        turboxEnterCritical();
        xBytesAvailable = prvBytesInBuffer(pxStreamBuffer);
        if (xBytesAvailable <= xBytesToStoreMessageLength) {
            xTaskNotifyStateClear(NULL);
            pxStreamBuffer->xTaskWaitingToReceive = xTaskGetCurrentTaskHandle();
        }
        turboxExitCritical();

        if (xBytesAvailable <= xBytesToStoreMessageLength) {
            TURBOX_PRINTF("Wait Send Task send data to stream buffer!\r\n");
            xTaskNotifyWait((uint32_t)INITIALIZATION_VALUE_0, (uint32_t)INITIALIZATION_VALUE_0, NULL, xTicksToWait);
            pxStreamBuffer->xTaskWaitingToReceive = NULL;
            xBytesAvailable = prvBytesInBuffer(pxStreamBuffer);
        }
    } else {
        xBytesAvailable = prvBytesInBuffer(pxStreamBuffer);
    }

    if (xBytesAvailable > xBytesToStoreMessageLength) {
        xReceivedLength = prvReadMessageFromBuffer(pxStreamBuffer, pvRxData, xBufferLengthBytes, xBytesAvailable, xBytesToStoreMessageLength);
        if((size_t)INITIALIZATION_VALUE_0 != xReceivedLength) {
            osKernelLock();
            if (NULL != (pxStreamBuffer)->xTaskWaitingToSend) {
                xTaskNotify(pxStreamBuffer->xTaskWaitingToSend, (uint32_t)INITIALIZATION_VALUE_0, INITIALIZATION_VALUE_0);
                pxStreamBuffer->xTaskWaitingToSend = NULL;
            }
            osKernelUnlock();
        } else {
            TURBOX_PRINTF("Cannot read message from buffer!\r\n");
        }
    }

    return xReceivedLength;
}

/**
 * @brief receive data from stream buffer
 * 
 * @param a [in]: StreamBufferHandle_t xStreamBuffer: stream buffer
 * @param b [out]: void *pvRxData: dst buffer
 * @param c [in]: size_t xBufferLengthBytes: buffer length
 * @param d [in]: BaseType_t * const pxHigherPriorityTaskWoken
 * @return size_t 
 * @retval
 * 1 : result1； receive count
 * @see
 */
size_t turboxRecvFromSBufferFromISR(StreamBufferHandle_t xStreamBuffer,
                                    void *pvRxData,
                                    size_t xBufferLengthBytes,
                                    BaseType_t * const pxHigherPriorityTaskWoken) {
    StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
    size_t xReceivedLength = INITIALIZATION_VALUE_0; 
    size_t xBytesAvailable = INITIALIZATION_VALUE_0;
    size_t xBytesToStoreMessageLength = INITIALIZATION_VALUE_0;

    if (NULL == xStreamBuffer) {
        TURBOX_PRINTF("xStreamBuffer is NULL!\r\n");
        return INITIALIZATION_VALUE_0;
    }

    if (NULL == pvRxData) {
        TURBOX_PRINTF("pvRxData is NULL!\r\n");
        return INITIALIZATION_VALUE_0;        
    }

    xBytesToStoreMessageLength = INITIALIZATION_VALUE_0;
    
    /* check byte available */
    xBytesAvailable = prvBytesInBuffer(pxStreamBuffer);
    if (xBytesAvailable > xBytesToStoreMessageLength) {
        /* read data from stream buffer */
        xReceivedLength = prvReadMessageFromBuffer(pxStreamBuffer, pvRxData, xBufferLengthBytes, xBytesAvailable, xBytesToStoreMessageLength);
        if ((size_t)INITIALIZATION_VALUE_0 != xReceivedLength) {
            osKernelLock();
            if (NULL != (pxStreamBuffer)->xTaskWaitingToSend) {
                xTaskNotifyFromISR(pxStreamBuffer->xTaskWaitingToSend, (uint32_t)INITIALIZATION_VALUE_0, eNoAction, pxHigherPriorityTaskWoken);
                pxStreamBuffer->xTaskWaitingToSend = NULL;
            }
            osKernelUnlock();
        }
    }

    return xReceivedLength;
}

/**
 * @brief delete stream buffer
 * 
 * @param a [in] StreamBufferHandle_t xStreamBuffer: stream buffer
 * @return void 
 * @retval
 * 1 : result1； 
 * @see f()
 */
void turboxDeleteSBuffer(StreamBufferHandle_t xStreamBuffer) {
    StreamBuffer_t * pxStreamBuffer = NULL;
    
    if (NULL == xStreamBuffer) {
        TURBOX_PRINTF("xStreamBuffer is NULL!\r\n");
        return;
    } else {
       pxStreamBuffer = xStreamBuffer;
    }

    if ((uint8_t)pdFALSE == (pxStreamBuffer->ucFlags & sbFLAGS_IS_STATICALLY_ALLOCATED)) {
        if (LOS_NOK == LOS_MemFree(m_aucSysMem0, (void*)pxStreamBuffer)) {
            TURBOX_PRINTF("LOS_MemFree FAILED!\r\n");
            return;
        }
    } else {
        memset(pxStreamBuffer, INITIALIZATION_VALUE_0, sizeof(StreamBuffer_t));
    }

    return;
}

/**
 * @brief check stream buffer is empty
 * 
 * @param a [in] StreamBufferHandle_t xStreamBuffer: stream buffer
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdTRUE/pdFALSE
 * @see
 */
BaseType_t turboxCheckSBufferEmpty( StreamBufferHandle_t xStreamBuffer ) {
    BaseType_t xReturn = pdFALSE;
    size_t xTail = INITIALIZATION_VALUE_0;
    const StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;

    if (NULL == xStreamBuffer) {
        TURBOX_PRINTF("xStreamBuffer is NULL!\r\n");
        xReturn = pdFALSE;
        return xReturn;
    }

    xTail = pxStreamBuffer->xTail;
    if (pxStreamBuffer->xHead == xTail) {
        /* stream buffer is empty */
        xReturn = pdTRUE;
    } else {
        /* stream buffer is not empty */
        xReturn = pdFALSE;
    }

    return xReturn;
}