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

/*
 * Stream buffers are used to send a continuous stream of data from one task or
 * interrupt to another.  Their implementation is light weight, making them
 * particularly suited for interrupt to task and core to core communication
 * scenarios.
 *
 * ***NOTE***:  Uniquely among FreeRTOS objects, the stream buffer
 * implementation (so also the message buffer implementation, as message buffers
 * are built on top of stream buffers) assumes there is only one task or
 * interrupt that will write to the buffer (the writer), and only one task or
 * interrupt that will read from the buffer (the reader).  It is safe for the
 * writer and reader to be different tasks or interrupts, but, unlike other
 * FreeRTOS objects, it is not safe to have multiple different writers or
 * multiple different readers.  If there are to be multiple different writers
 * then the application writer must place each call to a writing API function
 * (such as xStreamBufferSend()) inside a critical section and set the send
 * block time to 0.  Likewise, if there are to be multiple different readers
 * then the application writer must place each call to a reading API function
 * (such as xStreamBufferReceive()) inside a critical section section and set the
 * receive block time to 0.
 *
 */

#ifndef STREAM_BUFFER_H
#define STREAM_BUFFER_H

#ifndef INC_FREERTOS_H
    #error "include FreeRTOS.h must appear in source files before include stream_buffer.h"
#endif

/* *INDENT-OFF* */
#if defined( __cplusplus )
    extern "C" {
#endif
/* *INDENT-ON* */

/**
 * Type by which stream buffers are referenced.  For example, a call to
 * xStreamBufferCreate() returns an StreamBufferHandle_t variable that can
 * then be used as a parameter to xStreamBufferSend(), xStreamBufferReceive(),
 * etc.
 */
struct StreamBufferDef_t;
typedef struct StreamBufferDef_t * StreamBufferHandle_t;

extern void turboxDeleteSBuffer(StreamBufferHandle_t);
extern BaseType_t turboxCheckSBufferEmpty(StreamBufferHandle_t);
extern StreamBufferHandle_t turboxCreateSBuffer(size_t, size_t);
extern size_t turboxRecvFromSBuffer(StreamBufferHandle_t, void *, size_t, TickType_t);
extern size_t turboxSendToSBuffer(StreamBufferHandle_t, const void *, size_t, TickType_t);
extern size_t turboxRecvFromSBufferFromISR(StreamBufferHandle_t, void *, size_t, BaseType_t * const);
extern size_t turboxSendToSBufferFromISR(StreamBufferHandle_t, const void *, size_t, BaseType_t * const);

#define xStreamBufferSend               turboxSendToSBuffer			
#define xStreamBufferSendFromISR        turboxSendToSBufferFromISR		
#define xStreamBufferReceive            turboxRecvFromSBuffer			
#define xStreamBufferReceiveFromISR     turboxRecvFromSBufferFromISR		
#define vStreamBufferDelete             turboxDeleteSBuffer			
#define xStreamBufferIsEmpty            turboxCheckSBufferEmpty		
#define xStreamBufferCreate( xBufferSizeBytes, xTriggerLevelBytes ) \
                             turboxCreateSBuffer( xBufferSizeBytes, xTriggerLevelBytes)
/* *INDENT-OFF* */
#if defined( __cplusplus )
    }
#endif
/* *INDENT-ON* */

#endif /* !defined( STREAM_BUFFER_H ) */
