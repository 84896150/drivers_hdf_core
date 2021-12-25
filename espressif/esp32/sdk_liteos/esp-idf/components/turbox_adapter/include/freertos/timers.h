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


#ifndef TIMERS_H
#define TIMERS_H

#ifndef INC_FREERTOS_H
    #error "include FreeRTOS.h must appear in source files before include timers.h"
#endif

/*lint -save -e537 This headers are only multiply included if the application code
 * happens to also be including task.h. */
#include "task.h"
/*lint -restore */

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

/*-----------------------------------------------------------
 * MACROS AND DEFINITIONS
 *----------------------------------------------------------*/

/* IDs for commands that can be sent/received on the timer queue.  These are to
 * be used solely through the macros that make up the public software timer API,
 * as defined below.  The commands that are sent from interrupts must use the
 * highest numbers as tmrFIRST_FROM_ISR_COMMAND is used to determine if the task
 * or interrupt version of the queue send function should be used. */
#define tmrCOMMAND_EXECUTE_CALLBACK_FROM_ISR    ( ( BaseType_t ) -2 )
#define tmrCOMMAND_EXECUTE_CALLBACK             ( ( BaseType_t ) -1 )
#define tmrCOMMAND_START_DONT_TRACE             ( ( BaseType_t ) 0 )
#define tmrCOMMAND_START                        ( ( BaseType_t ) 1 )
#define tmrCOMMAND_RESET                        ( ( BaseType_t ) 2 )
#define tmrCOMMAND_STOP                         ( ( BaseType_t ) 3 )
#define tmrCOMMAND_CHANGE_PERIOD                ( ( BaseType_t ) 4 )
#define tmrCOMMAND_DELETE                       ( ( BaseType_t ) 5 )

#define tmrFIRST_FROM_ISR_COMMAND               ( ( BaseType_t ) 6 )
#define tmrCOMMAND_START_FROM_ISR               ( ( BaseType_t ) 6 )
#define tmrCOMMAND_RESET_FROM_ISR               ( ( BaseType_t ) 7 )
#define tmrCOMMAND_STOP_FROM_ISR                ( ( BaseType_t ) 8 )
#define tmrCOMMAND_CHANGE_PERIOD_FROM_ISR       ( ( BaseType_t ) 9 )


/**
 * Type by which software timers are referenced.  For example, a call to
 * xTimerCreate() returns an TimerHandle_t variable that can then be used to
 * reference the subject timer in calls to other software timer API functions
 * (for example, xTimerStart(), xTimerReset(), etc.).
 */
struct tmrTimerControl; /* The old naming convention is used to prevent breaking kernel aware debuggers. */
// #ifdef ESP_PLATFORM // IDF-3768
// typedef void* TimerHandle_t;
// #else
typedef struct tmrTimerControl * TimerHandle_t;
// #endif // ESP_PLATFORM
/*
 * Defines the prototype to which timer callback functions must conform.
 */
typedef void (* TimerCallbackFunction_t)( TimerHandle_t xTimer );

typedef void (*PendedFunction_t)( void *, 
								uint32_t );
/*********************************************************************/
/* function                       */
/*********************************************************************/
extern void *turboxGetTimerID( const TimerHandle_t );
extern BaseType_t turboxTimerStop( TimerHandle_t, TickType_t );
extern BaseType_t turboxTimerStart( TimerHandle_t, TickType_t );
extern BaseType_t turboxTimerReset( TimerHandle_t, TickType_t );
extern BaseType_t turboxTimerDelete( TimerHandle_t, TickType_t );
extern BaseType_t turboxTimerStartFromISR( TimerHandle_t, BaseType_t * const );
extern BaseType_t turboxTimerChgPeriod( TimerHandle_t, TickType_t, TickType_t );
extern TimerHandle_t turboxCreateDynamicTimer( const char * const, const TickType_t, 
                                                const UBaseType_t, void * const, 
                                                TimerCallbackFunction_t );
extern TimerHandle_t turboxCreateStaticTimer( const char * const, const TickType_t, 
                                                const UBaseType_t, void * const, 
                                                TimerCallbackFunction_t, StaticTimer_t * );

/*********************************************************************/
/* macro                       */
/*********************************************************************/
#define xTimerStart                     turboxTimerStart
#define xTimerReset                     turboxTimerReset
#define xTimerStartFromISR              turboxTimerStartFromISR

#define xTimerStop                      turboxTimerStop
#define xTimerDelete                    turboxTimerDelete

#define pvTimerGetTimerID               turboxGetTimerID
#define xTimerChangePeriod              turboxTimerChgPeriod

#define xTimerCreate                    turboxCreateDynamicTimer
#define xTimerCreateStatic              turboxCreateStaticTimer

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */
#endif /* TIMERS_H */
