/**
 * Copyright (c) 2020-2021 ThunderSoft
 * All Rights Reserved by Thunder Software Technology Co., Ltd and its affiliates.
 * You may not use, copy, distribute, modify, transmit in any form this file
 * except in compliance with ThunderSoft in writing by applicable law.
 *
 */
/**
 * @file    turbox_buffer.h
 * @brief   turbox_buffer funtion header file for freertos adapter
 * @details turbox_buffer funtion for freertos adapter
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
#ifndef TS_CREATE_BUFFER_H
#define TS_CREATE_BUFFER_H

#include <FreeRTOS.h>
#include <task.h>
#include <stream_buffer.h>

#include <cmsis_os2.h>
#include <los_memory.h>
#include "turbox_core_data.h"

#define taskNOT_WAITING_NOTIFICATION        ( ( uint8_t ) 0 )
#define taskWAITING_NOTIFICATION            ( ( uint8_t ) 1 )
#define taskNOTIFICATION_RECEIVED           ( ( uint8_t ) 2 )

#define sbFLAGS_IS_STREAM_BUFFER            ( ( int8_t )  0 )
#define sbFLAGS_IS_MESSAGE_BUFFER           ( ( uint8_t ) 1 )
#define sbFLAGS_IS_STATICALLY_ALLOCATED     ( ( uint8_t ) 2 ) 
#define sbBYTES_TO_STORE_MESSAGE_LENGTH     ( sizeof( size_t ) )
#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

extern size_t prvBytesInBuffer( const StreamBuffer_t * const );
extern size_t xStreamBufferSpacesAvailable( StreamBufferHandle_t );
extern size_t prvReadBytesFromBuffer(StreamBuffer_t *, uint8_t *, size_t, size_t);
extern size_t prvWriteBytesToBuffer(StreamBuffer_t * const, const uint8_t *, size_t);
extern size_t prvReadMessageFromBuffer(StreamBuffer_t *, void *, size_t, size_t, size_t);
extern size_t prvWriteMessageToBuffer(StreamBuffer_t * const, const void *, size_t, size_t, size_t);
extern void prvInitialiseNewStreamBuffer( StreamBuffer_t * const, uint8_t * const, size_t, size_t, uint8_t );
#endif  // TS_CREATE_BUFFER_H