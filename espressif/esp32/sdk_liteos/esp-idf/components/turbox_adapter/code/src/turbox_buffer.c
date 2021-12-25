/**
 * Copyright (c) 2020-2021 ThunderSoft
 * All Rights Reserved by Thunder Software Technology Co., Ltd and its affiliates.
 * You may not use, copy, distribute, modify, transmit in any form this file
 * except in compliance with ThunderSoft in writing by applicable law.
 *
 */
/**
 * @file    turbox_buffer.c
 * @brief   harmony bl platform buffer adaptor
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
#include "turbox_buffer.h"
#include <string.h>

#define AVAILABLE_MIN_SPACE   (1)

/**
 * @brief Initialize the stream buffer
 * 
 * @param a [in] pxStreamBuffer: buffer dstination entity
 * @param b [in] pucBuffer : buffer source entity
 * @param c [in] xBufferSizeBytes: buffer size bytes
 * @param d [in] xTriggerLevelBytes: trigger level bytes
 * @param e [in] ucFlags: flags
 * @return void
 * @retval 
 * @see
 */
void prvInitialiseNewStreamBuffer(StreamBuffer_t * const pxStreamBuffer,
                                          uint8_t * const pucBuffer,
                                          size_t xBufferSizeBytes,
                                          size_t xTriggerLevelBytes,
                                          uint8_t ucFlags) {
    if (NULL == pxStreamBuffer) {
        TURBOX_PRINTF("pxStreamBuffer is NULL\r\n");
        return;
    }

    if (NULL == pucBuffer) {
        TURBOX_PRINTF("pucBuffer is NULL\r\n");
        return;
    }

    memset((void*)pxStreamBuffer, INITIALIZATION_VALUE_0, sizeof(StreamBuffer_t));
    pxStreamBuffer->ucFlags             = ucFlags;
    pxStreamBuffer->pucBuffer           = pucBuffer;
    pxStreamBuffer->xLength             = xBufferSizeBytes;
    pxStreamBuffer->xTriggerLevelBytes  = xTriggerLevelBytes;
}

/**
 * @brief Get available space of stream buffer
 * 
 * @param a [in] StreamBufferHandle_t xStreamBuffer: buffer entity
 * @return size_t
 * @retval
 * 1 : result1； available space of stream buffer
 * @see
 */
size_t xStreamBufferSpacesAvailable(StreamBufferHandle_t xStreamBuffer) {
    const StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
    size_t xSpace = INITIALIZATION_VALUE_0;

    if (NULL == xStreamBuffer) {
        TURBOX_PRINTF("xStreamBuffer is NULL\r\n");
        return INITIALIZATION_VALUE_0;
    }

    /* get the space of the pxStreambuffer entity */
    xSpace = pxStreamBuffer->xLength + pxStreamBuffer->xTail;
    if (xSpace < pxStreamBuffer->xHead) {
        TURBOX_PRINTF("xSpace little to pxStreamBuffer->xHead\r\n");
    }
    xSpace = xSpace - pxStreamBuffer->xHead;

    if (AVAILABLE_MIN_SPACE > xSpace) {
        TURBOX_PRINTF("xSpace little to AVAILABLE_MIN_SPACE\r\n");
    }
    xSpace = xSpace - (size_t)AVAILABLE_MIN_SPACE;

    /* In case of ring */
    if (xSpace >= pxStreamBuffer->xLength) {
        xSpace = xSpace - pxStreamBuffer->xLength;
    }

    return xSpace;
}

/**
 * @brief calculate the count of buffer bytes
 * 
 * @param a [in] const StreamBuffer_t * const pxStreamBuffer: buffer entity
 * @return size_t
 * @retval
 * 1 : result1； the count of buffer bytes
 * @see
 */
size_t prvBytesInBuffer(const StreamBuffer_t * const pxStreamBuffer) {
    size_t xCount = INITIALIZATION_VALUE_0;

    if (NULL == pxStreamBuffer) {
        TURBOX_PRINTF("pxStreamBuffer is NULL\r\n");
        return INITIALIZATION_VALUE_0;
    }

    /* get the xCount of the pxStreambuffer entity */
    xCount = pxStreamBuffer->xLength + pxStreamBuffer->xHead;

    if (xCount < pxStreamBuffer->xTail) {
        TURBOX_PRINTF("xCount little to pxStreamBuffer->xTail\r\n");
    }
    xCount = xCount - pxStreamBuffer->xTail;

    /* In case of ring */
    if (xCount >= pxStreamBuffer->xLength) {
        xCount = xCount - pxStreamBuffer->xLength;
    }

    return xCount;
}

/**
 * @brief write the content to the buffer
 * 
 * @param a [in] StreamBuffer_t * const pxStreamBuffer buffer entity
 * @param b [in] pucData: the content which need to write into buffer
 * @param c [in] xCount: the count number that need to write into buffer
 * @return size_t
 * @retval
 * 1 : result1； the count number that have writed to the buffer 
 * @see
 */
size_t prvWriteBytesToBuffer(StreamBuffer_t * const pxStreamBuffer, const uint8_t *pucData, size_t xCount) {
    size_t xNextHead = INITIALIZATION_VALUE_0;
    size_t xFirstLength = INITIALIZATION_VALUE_0;

    if (NULL == pxStreamBuffer) {
        TURBOX_PRINTF("Parameter pxStreamBuffer is NULL\r\n");
        return INITIALIZATION_VALUE_0;
    }

    if (NULL == pucData) {
        TURBOX_PRINTF("Parameter pucData is NULL\r\n");
        return INITIALIZATION_VALUE_0;
    }

    /* get the buffer head pointer and count length */
    xNextHead = pxStreamBuffer->xHead;
    xFirstLength = configMIN((pxStreamBuffer->xLength - xNextHead), xCount);

    TURBOX_PRINTF("xNextHead = %p, xFirstLength = %ld\r\n", xNextHead, xFirstLength);

    /* copy the src buffer to puc buffer */
    memmove((void*)(&(pxStreamBuffer->pucBuffer[xNextHead])), (const void *)pucData, xFirstLength);

    /* buffer size is not enough */
    if (xCount > xFirstLength) {
        memmove((void*)pxStreamBuffer->pucBuffer, (const void*)&(pucData[xFirstLength]), (xCount - xFirstLength));
    }

    /* calculate the head ,plus the xCount */
    xNextHead = xNextHead + xCount;
    if (xNextHead >= pxStreamBuffer->xLength) {
        xNextHead = xNextHead - pxStreamBuffer->xLength;
    }

    pxStreamBuffer->xHead = xNextHead;
    TURBOX_PRINTF("the count of writed to buffer is %ld\r\n", xCount);

    return xCount;
}

/**
 * @brief write message to the buffer
 * 
 * @param a [in] StreamBuffer_t * const pxStreamBuffer: buffer entity
 * @param b [in] pvTxData: the data that need to write into buffer
 * @param c [in] xDataLengthBytes: data length
 * @param d [in] xSpace : the space
 * @param e [in] xRequiredSpace : required space
 * @return size_t
 * @retval
 * 1 : result1: the count number that have writed to the buffer 
 * @see
 */
size_t prvWriteMessageToBuffer(StreamBuffer_t * const pxStreamBuffer,
                                       const void * pvTxData,
                                       size_t xDataLengthBytes,
                                       size_t xSpace,
                                       size_t xRequiredSpace) {
    BaseType_t xShouldWrite = pdFALSE;
    size_t xReturn = INITIALIZATION_VALUE_0;

    if (NULL == pxStreamBuffer) {
        TURBOX_PRINTF("Parameter pxStreamBuffer is NULL\r\n");
        return INITIALIZATION_VALUE_0;
    }

    if (NULL == pvTxData) {
        TURBOX_PRINTF("Parameter pvTxData is NULL\r\n");
        return INITIALIZATION_VALUE_0;
    }

    if ((size_t)INITIALIZATION_VALUE_0 == xSpace) {
        /* have no enough space */
        xShouldWrite = pdFALSE;
        TURBOX_PRINTF("the rest of space is 0\r\n");
    } else if ((uint8_t)INITIALIZATION_VALUE_0 == (pxStreamBuffer->ucFlags & sbFLAGS_IS_MESSAGE_BUFFER)) {
        /* have no enough space */
        xShouldWrite = pdTRUE;
        xDataLengthBytes = configMIN( xDataLengthBytes, xSpace );
        TURBOX_PRINTF("have no enough space xDataLengthBytes = %d\r\n", xDataLengthBytes);
    } else if (xSpace >= xRequiredSpace) {
        xShouldWrite = pdTRUE;
        prvWriteBytesToBuffer(pxStreamBuffer, (const uint8_t *)&(xDataLengthBytes), sbBYTES_TO_STORE_MESSAGE_LENGTH);
    } else {
        /* other error */
        TURBOX_PRINTF("other error no write to buffer\r\n");
        xShouldWrite = pdFALSE;
    }

    if (pdFALSE != xShouldWrite) {
        /* write the src data to dst buffer */
        xReturn = prvWriteBytesToBuffer( pxStreamBuffer, ( const uint8_t * ) pvTxData, xDataLengthBytes );
        if (INITIALIZATION_VALUE_0 == xReturn) {
            TURBOX_PRINTF("Cannot write the data to buffer\r\n");
        }
    } else {
        xReturn = INITIALIZATION_VALUE_0;
    }

    return xReturn;
}

/**
 * @brief read bytes from buffer
 * 
 * @param a [in] StreamBuffer_t *pxStreamBuffer: buffer entity
 * @param b [out] pucData: the buffer what read from buffer 
 * @param c [in] xMaxCount: max count
 * @param d [in] xBytesAvailable: available bytes
 * @return size_t
 * @retval
 * 1 : result1: the bytes what read from buffer
 * @see
 */
size_t prvReadBytesFromBuffer(StreamBuffer_t *pxStreamBuffer, uint8_t *pucData, size_t xMaxCount, size_t xBytesAvailable) {
    size_t xCount = INITIALIZATION_VALUE_0;
    size_t xNextTail = INITIALIZATION_VALUE_0;
    size_t xFirstLength = INITIALIZATION_VALUE_0;

    if (NULL == pxStreamBuffer) {
        TURBOX_PRINTF("Parameter pxStreamBuffer is NULL\r\n");
        return INITIALIZATION_VALUE_0;
    }

    if (NULL == pucData) {
        TURBOX_PRINTF("Parameter pucData is NULL\r\n");
        return INITIALIZATION_VALUE_0;
    }

    xCount = configMIN(xBytesAvailable, xMaxCount);
    if ((size_t)INITIALIZATION_VALUE_0 < xCount) {
        xNextTail = pxStreamBuffer->xTail;
        xFirstLength = configMIN((pxStreamBuffer->xLength - xNextTail), xCount);

        /* copy the data from buffer to dst buffer */
        memcpy((void *)pucData, (const void *)&(pxStreamBuffer->pucBuffer[xNextTail]), xFirstLength);

        /* have no enough buffer size */
        if (xCount > xFirstLength) {
            memcpy(( void * )&(pucData[xFirstLength]), (void*)(pxStreamBuffer->pucBuffer), (xCount - xFirstLength));
        }
        xNextTail = xNextTail + xCount;

        /* In case of ring */
        if (xNextTail >= pxStreamBuffer->xLength) {
            xNextTail = xNextTail - pxStreamBuffer->xLength;
        }

        pxStreamBuffer->xTail = xNextTail;
    } else {
        TURBOX_PRINTF("xBytesAvailable = %ld---xMaxCount = %ld\r\n", xBytesAvailable, xMaxCount);
    }

    return xCount;
}

/**
 * @brief read message from buffer
 * 
 * @param a [in] StreamBuffer_t *pxStreamBuffer: the buffer entity
 * @param b [out] void *pvRxData: the buffer that read from the buffer
 * @param c [in] xBufferLengthBytes: buffer length bytes
 * @param d [in] xBytesAvailable: available bytes
 * @param e [in] xBytesToStoreMessageLength: message length
 * @return size_t 
 * @retval
 * 1 : result1: the size of readed message 
 * @see
 */
size_t prvReadMessageFromBuffer(StreamBuffer_t *pxStreamBuffer,
                                        void *pvRxData,
                                        size_t xBufferLengthBytes,
                                        size_t xBytesAvailable,
                                        size_t xBytesToStoreMessageLength) {
    size_t xOriginalTail = (size_t)INITIALIZATION_VALUE_0;
    size_t xReceivedLength = (size_t)INITIALIZATION_VALUE_0;
    size_t xNextMessageLength = (size_t)INITIALIZATION_VALUE_0;
    configMESSAGE_BUFFER_LENGTH_TYPE xTempNextMessageLength = (size_t)INITIALIZATION_VALUE_0;

    if (NULL == pxStreamBuffer) {
        TURBOX_PRINTF("Parameter pxStreamBuffer is NULL\r\n");
        return INITIALIZATION_VALUE_0;
    }

    if (NULL == pvRxData) {
        TURBOX_PRINTF("Parameter pvRxData is NULL\r\n");
        return INITIALIZATION_VALUE_0;
    }

    if ((size_t)INITIALIZATION_VALUE_0 != xBytesToStoreMessageLength) {
        xOriginalTail = pxStreamBuffer->xTail;

        /* read the bytes from buffer */
        prvReadBytesFromBuffer(pxStreamBuffer, (uint8_t*)&xTempNextMessageLength, xBytesToStoreMessageLength, xBytesAvailable);
        xNextMessageLength = (size_t)xTempNextMessageLength;

        /* get the available size after readed from buffer */
        xBytesAvailable = xBytesAvailable - xBytesToStoreMessageLength;
        if (xNextMessageLength > xBufferLengthBytes) {
            pxStreamBuffer->xTail = xOriginalTail;
            xNextMessageLength = (size_t)INITIALIZATION_VALUE_0;
        }
    } else {
        xNextMessageLength = xBufferLengthBytes;
        TURBOX_PRINTF("xBytesToStoreMessageLength = %ld\r\n", xBytesToStoreMessageLength);
        
    }

    /* read the data and return the readed length */
    xReceivedLength = prvReadBytesFromBuffer(pxStreamBuffer, (uint8_t*)pvRxData, xNextMessageLength, xBytesAvailable);
    if ((size_t)INITIALIZATION_VALUE_0 == xReceivedLength) {
        TURBOX_PRINTF("Cannot read data from buffer!\r\n");
    }

    return xReceivedLength;
}