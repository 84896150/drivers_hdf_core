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


#ifndef INC_TASK_H
#define INC_TASK_H

#ifndef INC_FREERTOS_H
    #error "include FreeRTOS.h must appear in source files before include task.h"
#endif

#include "list.h"
#ifdef ESP_PLATFORM // IDF-3793
#include "freertos/portmacro.h"
#endif // ESP_PLATFORM

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */
// #include <cmsis_os2.h>

/*-----------------------------------------------------------
 * MACROS AND DEFINITIONS
 *----------------------------------------------------------*/

#define tskKERNEL_VERSION_NUMBER       "V10.4.3"
#define tskKERNEL_VERSION_MAJOR        10
#define tskKERNEL_VERSION_MINOR        4
#define tskKERNEL_VERSION_BUILD        3

/* MPU region parameters passed in ulParameters
 * of MemoryRegion_t struct. */
#define tskMPU_REGION_READ_ONLY        ( 1UL << 0UL )
#define tskMPU_REGION_READ_WRITE       ( 1UL << 1UL )
#define tskMPU_REGION_EXECUTE_NEVER    ( 1UL << 2UL )
#define tskMPU_REGION_NORMAL_MEMORY    ( 1UL << 3UL )
#define tskMPU_REGION_DEVICE_MEMORY    ( 1UL << 4UL )

/* The direct to task notification feature used to have only a single notification
 * per task.  Now there is an array of notifications per task that is dimensioned by
 * configTASK_NOTIFICATION_ARRAY_ENTRIES.  For backward compatibility, any use of the
 * original direct to task notification defaults to using the first index in the
 * array. */
#define tskDEFAULT_INDEX_TO_NOTIFY     ( 0 )

#define tskNO_AFFINITY  ( 0x7FFFFFFF )

typedef void (*TlsDeleteCallbackFunction_t)( int, void * );
/**
 * task. h
 *
 * Type by which tasks are referenced.  For example, a call to xTaskCreate
 * returns (via a pointer parameter) an TaskHandle_t variable that can then
 * be used as a parameter to vTaskDelete to delete the task.
 *
 * @cond
 * \defgroup TaskHandle_t TaskHandle_t
 * @endcond
 * \ingroup Tasks
 */
struct tskTaskControlBlock;     /* The old naming convention is used to prevent breaking kernel aware debuggers. */
// #ifdef ESP_PLATFORM // IDF-3769
// typedef void* TaskHandle_t;
// #else
typedef struct tskTaskControlBlock* TaskHandle_t;
// #endif // ESP_PLATFORM
/**
 * Defines the prototype to which the application task hook function must
 * conform.
 */
typedef BaseType_t (* TaskHookFunction_t)( void * );

/** Task states returned by eTaskGetState. */
typedef enum
{
    eRunning = 0,     /* A task is querying the state of itself, so must be running. */
    eReady,           /* The task being queried is in a read or pending ready list. */
    eBlocked,         /* The task being queried is in the Blocked state. */
    eSuspended,       /* The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out. */
    eDeleted,         /* The task being queried has been deleted, but its TCB has not yet been freed. */
    eInvalid          /* Used as an 'invalid state' value. */
} eTaskState;

/* Actions that can be performed when vTaskNotify() is called. */
typedef enum
{
    eNoAction = 0,                /* Notify the task without updating its notify value. */
    eSetBits,                     /* Set bits in the task's notification value. */
    eIncrement,                   /* Increment the task's notification value. */
    eSetValueWithOverwrite,       /* Set the task's notification value to a specific value even if the previous value has not yet been read by the task. */
    eSetValueWithoutOverwrite     /* Set the task's notification value if the previous value has been read by the task. */
} eNotifyAction;

/** @cond */
/**
 * Used internally only.
 */
typedef struct xTIME_OUT
{
    BaseType_t xOverflowCount;
    TickType_t xTimeOnEntering;
} TimeOut_t;

/**
 * Defines the memory ranges allocated to the task when an MPU is used.
 */
typedef struct xMEMORY_REGION
{
    void * pvBaseAddress;
    uint32_t ulLengthInBytes;
    uint32_t ulParameters;
} MemoryRegion_t;

/*
 * Parameters required to create an MPU protected task.
 */
typedef struct xTASK_PARAMETERS
{
    TaskFunction_t pvTaskCode;
    const char * const pcName; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
    configSTACK_DEPTH_TYPE usStackDepth;
    void * pvParameters;
    UBaseType_t uxPriority;
    StackType_t * puxStackBuffer;
    MemoryRegion_t xRegions[ portNUM_CONFIGURABLE_REGIONS ];
    #if ( ( portUSING_MPU_WRAPPERS == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )
        StaticTask_t * const pxTaskBuffer;
    #endif
} TaskParameters_t;

/* Used with the uxTaskGetSystemState() function to return the state of each task
 * in the system. */
typedef struct xTASK_STATUS
{
    TaskHandle_t xHandle;                            /* The handle of the task to which the rest of the information in the structure relates. */
    const char * pcTaskName;                         /* A pointer to the task's name.  This value will be invalid if the task was deleted since the structure was populated! */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
    UBaseType_t xTaskNumber;                         /* A number unique to the task. */
    eTaskState eCurrentState;                        /* The state in which the task existed when the structure was populated. */
    UBaseType_t uxCurrentPriority;                   /* The priority at which the task was running (may be inherited) when the structure was populated. */
    UBaseType_t uxBasePriority;                      /* The priority to which the task will return if the task's current priority has been inherited to avoid unbounded priority inversion when obtaining a mutex.  Only valid if configUSE_MUTEXES is defined as 1 in FreeRTOSConfig.h. */
    uint32_t ulRunTimeCounter;                       /* The total run time allocated to the task so far, as defined by the run time stats clock.  See https://www.FreeRTOS.org/rtos-run-time-stats.html.  Only valid when configGENERATE_RUN_TIME_STATS is defined as 1 in FreeRTOSConfig.h. */
    StackType_t * pxStackBase;                       /* Points to the lowest address of the task's stack area. */
    configSTACK_DEPTH_TYPE usStackHighWaterMark;     /* The minimum amount of stack space that has remained for the task since the task was created.  The closer this value is to zero the closer the task has come to overflowing its stack. */
#if configTASKLIST_INCLUDE_COREID
    BaseType_t xCoreID;                              /*!< Core this task is pinned to (0, 1, or -1 for tskNO_AFFINITY). This field is present if CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID is set. */
#endif
} TaskStatus_t;

/** @endcond */

/**
 * Possible return values for eTaskConfirmSleepModeStatus().
 */
typedef enum
{
    eAbortSleep = 0,           /* A task has been made ready or a context switch pended since portSUPPORESS_TICKS_AND_SLEEP() was called - abort entering a sleep mode. */
    eStandardSleep,            /* Enter a sleep mode that will not last any longer than the expected idle time. */
    eNoTasksWaitingTimeout     /* No tasks are waiting for a timeout so it is safe to enter a sleep mode that can only be exited by an external interrupt. */
} eSleepModeStatus;



#define tskIDLE_PRIORITY            ( ( UBaseType_t ) 0U )
#define taskENABLE_INTERRUPTS()     __asm volatile( "csrs mstatus, 8" )
/* Definitions returned by xTaskGetSchedulerState().  taskSCHEDULER_SUSPENDED is
 * 0 to generate more optimal code when configASSERT() is defined as the constant
 * is used in assert() statements. */
#define taskSCHEDULER_SUSPENDED      ( ( BaseType_t ) 0 )
#define taskSCHEDULER_NOT_STARTED    ( ( BaseType_t ) 1 )
#define taskSCHEDULER_RUNNING        ( ( BaseType_t ) 2 )

#ifdef ESP_PLATFORM
/* TODO: IDF-3683 */
#include "freertos/task_snapshot.h"
#endif // ESP_PLATFORM
/*-----------------------------------------------------------
 * FUNCTION API
 *----------------------------------------------------------*/
extern void turboxYieldTask(void);
extern void turboxEnterCritical( void );
extern void turboxExitCritical( void );
extern void turboxDisableISR(void);
extern BaseType_t turboxCreateDynamicTask( TaskFunction_t, const char * const ,
                            const configSTACK_DEPTH_TYPE, void * const,
                            UBaseType_t, TaskHandle_t * const );
extern TaskHandle_t turboxCreateStaticTask( TaskFunction_t, const char * const, const unsigned int, void * const,
                                    UBaseType_t, StackType_t * const, StaticTask_t * const);
extern BaseType_t turboxAddTick( void );
extern void turboxStartScheduler( void );
extern void turboxSwitchTaskContext( void );
extern TickType_t turboxGetTickCount( void );
extern void turboxDeleteTask( TaskHandle_t  );
extern void turboxDelayTask( const TickType_t );
extern TaskHandle_t turboxGetCurrentTCB( void );
extern BaseType_t xPortIsInsideInterrupt( void );
extern void turboxTaskStepTick( const TickType_t );
extern TickType_t turboxGetTickCountFromISR( void );
extern void turboxTaskStepTickSafe( const TickType_t );
extern BaseType_t turboxClearNotifyState( TaskHandle_t );
extern uint32_t turboxTakeTaskNotify(BaseType_t, TickType_t);
extern UBaseType_t turboxGetStackHighWaterMark( TaskHandle_t );
extern BaseType_t turboxGetTickCount2( TickType_t *, BaseType_t * );
extern BaseType_t turboxWaitTaskNotify(uint32_t, uint32_t, uint32_t *, TickType_t);
extern void turboxGiveTaskNotifyFromISR( TaskHandle_t xTaskToNotify, BaseType_t * );
extern BaseType_t turboxGenericNotify( TaskHandle_t, uint32_t, eNotifyAction, uint32_t * );
extern BaseType_t turboxGenericNotifyFromISR( TaskHandle_t, uint32_t, eNotifyAction, uint32_t *, BaseType_t * );

extern void turboxSuspendAllvTask( void );
extern BaseType_t turboxResumeAllTask( void );
extern char * turboxpcTaskGetName( TaskHandle_t); 
extern void turboxvTaskSuspend( TaskHandle_t);
extern void * turboxpvTaskGetThreadLocalStoragePointer( TaskHandle_t, BaseType_t);
extern void turboxvTaskSetThreadLocalStoragePointerAndDelCallback( TaskHandle_t,
                                                                    BaseType_t,
                                                                    void *,
                                                                    TlsDeleteCallbackFunction_t);
extern BaseType_t turboxTaskGetAffinity( TaskHandle_t xTask );
extern TaskHandle_t turboxTaskGetCurrentTaskHandleForCPU( BaseType_t cpuid );
extern TaskHandle_t turboxTaskGetIdleTaskHandleForCPU( UBaseType_t cpuid );
extern void turboxTaskDelayUntil( TickType_t * const, const TickType_t);
extern BaseType_t turboxTaskGetSchedulerState( void );
extern void turboxvTaskResume( TaskHandle_t xTaskToResume );

#define taskYIELD                   turboxYieldTask
#define taskDISABLE_INTERRUPTS      turboxDisableISR
#define vTaskExitCritical           turboxExitCritical
#define taskEXIT_CRITICAL           turboxExitCritical
#define vTaskEnterCritical          turboxEnterCritical     
#define taskENTER_CRITICAL          turboxEnterCritical
#define xTaskCreate                 turboxCreateDynamicTask
#define xTaskCreateStatic           turboxCreateStaticTask
#define xTaskGetTickCount           turboxGetTickCount
#define xTaskGetTickCount2          turboxGetTickCount2
#define xTaskGetTickCountFromISR    turboxGetTickCountFromISR	
#define vTaskSwitchContext          turboxSwitchTaskContext	
#define vTaskDelete                 turboxDeleteTask			
#define vTaskDelay                  turboxDelayTask			
#define vTaskStartScheduler         turboxStartScheduler		    
#define uxTaskGetStackHighWaterMark turboxGetStackHighWaterMark 	
#define ulTaskNotifyTake            turboxTakeTaskNotify			
#define xTaskNotifyWait             turboxWaitTaskNotify			
#define xTaskGenericNotifyFromISR   turboxGenericNotifyFromISR		
#define vTaskNotifyGiveFromISR      turboxGiveTaskNotifyFromISR		
#define xTaskNotifyStateClear       turboxClearNotifyState			
#define xTaskIncrementTick          turboxAddTick				    
#define xTaskGetCurrentTaskHandle   turboxGetCurrentTCB			    
#define vTaskStepTick               turboxTaskStepTick			    
#define vTaskStepTickSafe           turboxTaskStepTickSafe
#define xTaskNotify(A, B, C)        turboxGenericNotify(A, B, C, NULL)
#define xTaskNotifyGive( A )        turboxGenericNotify( ( A ), ( 0 ), eIncrement, NULL )
#define xTaskNotifyFromISR(A, B, C, D) turboxGenericNotifyFromISR(A, B, C, NULL, D)

//add for esp32 start
#define xTaskCreatePinnedToCore(A, B, C, D, E, F, G)  turboxCreateDynamicTask(A, B, C, D, E, F);
#define xTaskCreateStaticPinnedToCore(A, B, C, D, E, F, G) turboxCreateDynamicTask(A, B, C, D, E, F);
#define vTaskSuspendAll                                   turboxSuspendAllvTask
#define xTaskResumeAll                                    turboxResumeAllTask
#define pcTaskGetName                                     turboxpcTaskGetName
#define vTaskSuspend                                      turboxvTaskSuspend
#define pvTaskGetThreadLocalStoragePointer                turboxpvTaskGetThreadLocalStoragePointer
#define vTaskSetThreadLocalStoragePointerAndDelCallback   turboxvTaskSetThreadLocalStoragePointerAndDelCallback
#define xTaskGetAffinity                                  turboxTaskGetAffinity
#define xTaskGetCurrentTaskHandleForCPU                   turboxTaskGetCurrentTaskHandleForCPU
#define xTaskGetIdleTaskHandleForCPU                      turboxTaskGetIdleTaskHandleForCPU
#define vTaskDelayUntil                                   turboxTaskDelayUntil
#define xTaskGetSchedulerState                            turboxTaskGetSchedulerState
#define vTaskResume                                       turboxvTaskResume

#ifdef __cplusplus
    }
#endif

/* *INDENT-ON* */
#endif /* INC_TASK_H */
