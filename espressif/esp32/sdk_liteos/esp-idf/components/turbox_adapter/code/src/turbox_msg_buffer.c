/**
 * Copyright (c) 2020-2021 ThunderSoft
 * All Rights Reserved by Thunder Software Technology Co., Ltd and its affiliates.
 * You may not use, copy, distribute, modify, transmit in any form this file
 * except in compliance with ThunderSoft in writing by applicable law.
 *
 */
/**
 * @file    turbox_msg_buf.c
 * @brief   harmony bl platform message buffer adaptor
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
#include <task.h>
#include <stream_buffer.h>
#include <string.h>
#include <cmsis_os2.h>
#include <los_context.h>
#include <los_interrupt.h>

#include "turbox_buffer.h"
#include "turbox_core_data.h"

/**
 * @brief create static message buffer
 * 
 * @param a [in] size_t xBufferSizeBytes: buffer size bytes
 * @param a [in] size_t xTriggerLevelBytes: trigger level bytes
 * @param a [in] BaseType_t xIsMessageBuffer: is ornot message buffer
 * @param a [in] uint8_t * const pucStreamBufferStorageArea: buffer entity,storage area
 * @param a [in] StaticStreamBuffer_t * const pxStaticStreamBuffer: the buffer entity
 * @return StreamBufferHandle_t 
 * @retval
 * 1 : result1；stream buffer handle
 * @see f()
 */           
StreamBufferHandle_t turboxCreateStaticMsgBuffer(size_t xBufferSizeBytes,
                                                      size_t xTriggerLevelBytes,
                                                      BaseType_t xIsMessageBuffer,
                                                      uint8_t * const pucStreamBufferStorageArea,
                                                      StaticStreamBuffer_t * const pxStaticStreamBuffer) {
        StreamBufferHandle_t xReturn = NULL;
        uint8_t ucFlags = INITIALIZATION_VALUE_0;
        StreamBuffer_t * const pxStreamBuffer = (StreamBuffer_t*) pxStaticStreamBuffer;
        
        /* the min value of xTriggerLevelBytes is 1 */
        if ((size_t)INITIALIZATION_VALUE_0 == xTriggerLevelBytes) {
            xTriggerLevelBytes = (size_t)INITIALIZATION_VALUE_1;
        }

        /* static malloc memory for message buffer; no need to free */
        ucFlags = sbFLAGS_IS_MESSAGE_BUFFER | sbFLAGS_IS_STATICALLY_ALLOCATED;
        if ((pucStreamBufferStorageArea != NULL) && (pxStaticStreamBuffer != NULL)) {
            /* initialize the message buffer */
            prvInitialiseNewStreamBuffer(pxStreamBuffer,
                                          pucStreamBufferStorageArea,
                                          xBufferSizeBytes,
                                          xTriggerLevelBytes,
                                          ucFlags);

            /* static malloc memory for message buffer; no need to free */
            pxStreamBuffer->ucFlags |= sbFLAGS_IS_STATICALLY_ALLOCATED;
            xReturn = (StreamBufferHandle_t)pxStaticStreamBuffer;
        } else {
            TURBOX_PRINTF("pucStreamBufferStorageArea = %p----pxStaticStreamBuffer = %p\r\n",
                        pucStreamBufferStorageArea, pxStaticStreamBuffer);
            xReturn = NULL;
        }

        return xReturn;
}

/**
 * @brief Send Message to buffer
 * 
 * @param a [in] StreamBufferHandle_t xStreamBuffer: buffer entity
 * @param b [in] const void *pvTxData: Message
 * @param c [in] xDataLengthBytes: data length
 * @param c [in] xTicksToWait: wait ticks
 * @return size_t 
 * @retval
 * 1 : result1； write count
 * @see 
 */
size_t turboxSendToMsgBuffer(StreamBufferHandle_t xStreamBuffer,
                         const void *pvTxData,
                         size_t xDataLengthBytes,
                         TickType_t xTicksToWait) {
    size_t xSpace = INITIALIZATION_VALUE_0;
    size_t xReturn = INITIALIZATION_VALUE_0;
    size_t xRequiredSpace = xDataLengthBytes;
    StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;

    /* check the message type is message buffer */
    if ((uint8_t)INITIALIZATION_VALUE_0 != (pxStreamBuffer->ucFlags & sbFLAGS_IS_MESSAGE_BUFFER)) {
        xRequiredSpace += sbBYTES_TO_STORE_MESSAGE_LENGTH;
    }

    /* need to wait for xTicksToWait */
    if ((TickType_t)INITIALIZATION_VALUE_0 != xTicksToWait) {
        /* Wait until the required number of bytes are free in the message buffer. */
        do {
            turboxEnterCritical();
            xSpace = xStreamBufferSpacesAvailable(pxStreamBuffer);

            /* the rest space is not enough */
            if(xSpace < xRequiredSpace) {
                xTaskNotifyStateClear(NULL);
                /* clear the notify state ,get the current task handle prepare to task wait */
                pxStreamBuffer->xTaskWaitingToSend = xTaskGetCurrentTaskHandle();
            } else {
                LOS_IntUnLock();
                break;
            }
            turboxExitCritical();

            TURBOX_PRINTF("wait for enough space\r\n");
            /* wait for the enough space in buffer */
            xTaskNotifyWait((uint32_t)INITIALIZATION_VALUE_0, (uint32_t)INITIALIZATION_VALUE_0, NULL, xTicksToWait);
            pxStreamBuffer->xTaskWaitingToSend = NULL;
        } while(INITIALIZATION_VALUE_0);
    }

    if ((size_t)INITIALIZATION_VALUE_0 == xSpace) {
        /* get the buffer available space Perhaps execute after wait */
        xSpace = xStreamBufferSpacesAvailable(pxStreamBuffer);
    }

    /* write the message to buffer pvTxData ----> pxStreamBuffer*/
    xReturn = prvWriteMessageToBuffer(pxStreamBuffer, pvTxData, xDataLengthBytes, xSpace, xRequiredSpace);
    if ((size_t)INITIALIZATION_VALUE_0 < xReturn) {
        if (prvBytesInBuffer(pxStreamBuffer) >= pxStreamBuffer->xTriggerLevelBytes) {
            osKernelLock();
            /* notify receive task to receive */
            if (pxStreamBuffer->xTaskWaitingToReceive != NULL) {
                xTaskNotify(pxStreamBuffer->xTaskWaitingToReceive, (uint32_t)INITIALIZATION_VALUE_0, INITIALIZATION_VALUE_0);
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
 * @brief receive message from buffer
 * 
 * @param a [in] StreamBufferHandle_t xStreamBuffer src buffer
 * @param b [in]
 * @param c [out] output
 * @return int 
 * @retval
 * 1 : result1； 
 * 2 : result2；
 * @see f()
 */
size_t turboxRecvFromMsgBuffer(StreamBufferHandle_t xStreamBuffer,
                            void *pvRxData,
                            size_t xBufferLengthBytes,
                            TickType_t xTicksToWait) {
    StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
    size_t xReceivedLength = INITIALIZATION_VALUE_0; 
    size_t xBytesAvailable = INITIALIZATION_VALUE_0;
    size_t xBytesToStoreMessageLength = INITIALIZATION_VALUE_0;

    /* check the message type is message buffer */
    if ((uint8_t)INITIALIZATION_VALUE_0 != (pxStreamBuffer->ucFlags & sbFLAGS_IS_MESSAGE_BUFFER)) {
        xBytesToStoreMessageLength = sbBYTES_TO_STORE_MESSAGE_LENGTH;
    }

    if ((TickType_t)INITIALIZATION_VALUE_0 != xTicksToWait) {
        turboxEnterCritical();
        xBytesAvailable = prvBytesInBuffer(pxStreamBuffer);

        /* have not the enough data */
        if (xBytesAvailable <= xBytesToStoreMessageLength) {
            xTaskNotifyStateClear(NULL);

            /* get the receive tcb */
            pxStreamBuffer->xTaskWaitingToReceive = xTaskGetCurrentTaskHandle();
        }
        turboxExitCritical();

        if (xBytesAvailable <= xBytesToStoreMessageLength) {
            /* wait for enough data */
            TURBOX_PRINTF("wait for enough data\r\n");
            xTaskNotifyWait((uint32_t)INITIALIZATION_VALUE_0, (uint32_t)INITIALIZATION_VALUE_0, NULL, xTicksToWait);
            pxStreamBuffer->xTaskWaitingToReceive = NULL;
            xBytesAvailable = prvBytesInBuffer(pxStreamBuffer);
        }
    } else {
        /* get the rest space size of buffer */
        xBytesAvailable = prvBytesInBuffer(pxStreamBuffer);
    }

    if (xBytesAvailable > xBytesToStoreMessageLength) {
        /* read message from buffer */
        xReceivedLength = prvReadMessageFromBuffer(pxStreamBuffer, pvRxData, xBufferLengthBytes, xBytesAvailable, xBytesToStoreMessageLength);
        if((size_t)INITIALIZATION_VALUE_0 != xReceivedLength) {
            osKernelLock();
            if (pxStreamBuffer->xTaskWaitingToSend != NULL) {
                /* notify send task to send */
                xTaskNotify(pxStreamBuffer->xTaskWaitingToSend, (uint32_t)INITIALIZATION_VALUE_0, INITIALIZATION_VALUE_0);
                pxStreamBuffer->xTaskWaitingToSend = NULL;
            }
            osKernelUnlock();
        } else {
            TURBOX_PRINTF("Cannot receive the message from buffer, return count is 0!\r\n");
        }
    }

    return xReceivedLength;
}