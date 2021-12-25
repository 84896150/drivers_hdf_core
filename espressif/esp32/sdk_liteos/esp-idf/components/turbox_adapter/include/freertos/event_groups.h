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

#ifndef EVENT_GROUPS_H
#define EVENT_GROUPS_H

#ifndef INC_FREERTOS_H
    #error "include FreeRTOS.h" must appear in source files before "include event_groups.h"
#endif

/* FreeRTOS includes. */
#include "timers.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

/**
 * An event group is a collection of bits to which an application can assign a
 * meaning.  For example, an application may create an event group to convey
 * the status of various CAN bus related events in which bit 0 might mean "A CAN
 * message has been received and is ready for processing", bit 1 might mean "The
 * application has queued a message that is ready for sending onto the CAN
 * network", and bit 2 might mean "It is time to send a SYNC message onto the
 * CAN network" etc.  A task can then test the bit values to see which events
 * are active, and optionally enter the Blocked state to wait for a specified
 * bit or a group of specified bits to be active.  To continue the CAN bus
 * example, a CAN controlling task can enter the Blocked state (and therefore
 * not consume any processing time) until either bit 0, bit 1 or bit 2 are
 * active, at which time the bit that was actually active would inform the task
 * which action it had to take (process a received message, send a message, or
 * send a SYNC).
 *
 * The event groups implementation contains intelligence to avoid race
 * conditions that would otherwise occur were an application to use a simple
 * variable for the same purpose.  This is particularly important with respect
 * to when a bit within an event group is to be cleared, and when bits have to
 * be set and then tested atomically - as is the case where event groups are
 * used to create a synchronisation point between multiple tasks (a
 * 'rendezvous').
 *
 * @cond
 * \defgroup EventGroup EventGroup
 * @endcond
 */



/**
 * event_groups.h
 *
 * Type by which event groups are referenced.  For example, a call to
 * xEventGroupCreate() returns an EventGroupHandle_t variable that can then
 * be used as a parameter to other event group functions.
 *
 * @cond
 * \defgroup EventGroupHandle_t EventGroupHandle_t
 * @endcond
 * \ingroup EventGroup
 */
struct EventGroupDef_t;
// #ifdef ESP_PLATFORM // IDF-3770
// typedef void * EventGroupHandle_t;
// #else
typedef struct EventGroupDef_t * EventGroupHandle_t;
// #endif // ESP_PLATFORM
/*
 * The type that holds event bits always matches TickType_t - therefore the
 * number of bits it holds is set by configUSE_16_BIT_TICKS (16 bits if set to 1,
 * 32 bits if set to 0.
 *
 * @cond
 * \defgroup EventBits_t EventBits_t
 * @endcond
 * \ingroup EventGroup
 */
typedef TickType_t               EventBits_t;


extern void turboxDeleteEvent(EventGroupHandle_t);
extern EventGroupHandle_t turboxCreateDynamicEvent(void);
extern EventGroupHandle_t turboxCreateStaticEvent(StaticEventGroup_t *);
extern EventBits_t turboxSetEvent(EventGroupHandle_t, const EventBits_t );
extern EventBits_t turboxClearEvent(EventGroupHandle_t,  const EventBits_t);
extern BaseType_t turboxSetEventFromISR(EventGroupHandle_t, const EventBits_t, BaseType_t * );
extern EventBits_t turboxWaitEvent(EventGroupHandle_t, const EventBits_t, const BaseType_t, const BaseType_t, TickType_t );

#define xEventGroupCreate           turboxCreateDynamicEvent		
#define xEventGroupCreateStatic     turboxCreateStaticEvent		
#define xEventGroupWaitBits         turboxWaitEvent			
#define xEventGroupClearBits        turboxClearEvent			
#define xEventGroupSetBits          turboxSetEvent				
#define xEventGroupSetBitsFromISR   turboxSetEventFromISR			
#define vEventGroupDelete           turboxDeleteEvent
#define xEventGroupGetBits( xEventGroup )    xEventGroupClearBits( xEventGroup, 0 )
/** @endcond */	
/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* EVENT_GROUPS_H */
