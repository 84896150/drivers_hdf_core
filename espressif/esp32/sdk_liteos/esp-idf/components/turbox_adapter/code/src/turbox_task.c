/**
 * Copyright (c) 2020-2021 ThunderSoft
 * All Rights Reserved by Thunder Software Technology Co., Ltd and its affiliates.
 * You may not use, copy, distribute, modify, transmit in any form this file
 * except in compliance with ThunderSoft in writing by applicable law.
 *
 */
/**
 * @file    turbox_task.c
 * @brief   harmony bl platform task adaptor
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
#include <string.h>
#include <los_reg.h>
#include <los_tick.h>
#include <los_task.h>
#include <los_sem.h>
#include <cmsis_os2.h>
#include <los_arch_interrupt.h>

#include "turbox_core_data.h"

BaseType_t TrapNetCounter = INITIALIZATION_VALUE_0;
#define EVENT_MASK_TASK_NOTIFY                      ( 0x10      )
#define tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB      ( ( uint8_t ) 0 )
#define tskSTATICALLY_ALLOCATED_STACK_ONLY          ( ( uint8_t ) 1 )
#define tskSTATICALLY_ALLOCATED_STACK_AND_TCB       ( ( uint8_t ) 2 )
#define taskNOT_WAITING_NOTIFICATION                ( ( uint8_t ) 0 )
#define taskWAITING_NOTIFICATION                    ( ( uint8_t ) 1 )
#define taskNOTIFICATION_RECEIVED                   ( ( uint8_t ) 2 )

typedef struct tskTaskControlBlock { 
    volatile StackType_t    *pxTopOfStack;

    #if ( portUSING_MPU_WRAPPERS == 1 )
        xMPU_SETTINGS   xMPUSettings;
    #endif

    ListItem_t          xStateListItem;
    ListItem_t          xEventListItem;
    UBaseType_t         uxPriority;    
    StackType_t         *pxStack;      
    char                pcTaskName[ configMAX_TASK_NAME_LEN ];
    BaseType_t xCoreID;                         /*< Core this task is pinned to */
    #if ( ( portSTACK_GROWTH > 0 ) || ( configRECORD_STACK_HIGH_ADDRESS == 1 ) )
        StackType_t     *pxEndOfStack; 
    #endif

    #if ( portCRITICAL_NESTING_IN_TCB == 1 )
        UBaseType_t     uxCriticalNesting;
    #endif

    #if ( configUSE_TRACE_FACILITY == 1 )
        UBaseType_t     uxTCBNumber;      
        UBaseType_t     uxTaskNumber;     
    #endif

    #if ( configUSE_MUTEXES == 1 )
        UBaseType_t     uxBasePriority;   
        UBaseType_t     uxMutexesHeld;
    #endif

    #if ( configUSE_APPLICATION_TASK_TAG == 1 )
        TaskHookFunction_t pxTaskTag;
    #endif

    #if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
        void            *pvThreadLocalStoragePointers[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
    #if ( configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS )
        TlsDeleteCallbackFunction_t pvThreadLocalStoragePointersDelCallback[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
    #endif
    #endif

    #if( configGENERATE_RUN_TIME_STATS == 1 )
        uint32_t        ulRunTimeCounter; 
    #endif

    #if ( configUSE_NEWLIB_REENTRANT == 1 )
        struct  _reent xNewLib_reent;
    #endif

    #if ( configUSE_TASK_NOTIFICATIONS == 1 )
        volatile uint32_t ulNotifiedValue;
        volatile uint8_t ucNotifyState;
    #endif
    #if( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )
        uint8_t ucStaticallyAllocated;
    #endif

    #if( INCLUDE_xTaskAbortDelay == 1 )
        uint8_t ucDelayAborted;
    #endif

    #if( configUSE_POSIX_ERRNO == 1 )
        int iTaskErrno;
    #endif

    osThreadId_t threadId;
    osSemaphoreId_t sem_id;
} tskTCB;

extern BOOL g_taskScheduled;
UBaseType_t uxCurrentNumberOfTasks  = (UBaseType_t)INITIALIZATION_VALUE_0;
static TaskHandle_t xTasks[LOSCFG_BASE_CORE_TSK_LIMIT] = {INITIALIZATION_VALUE_0};
typedef tskTCB TCB_t;
extern VOID LOS_Schedule(VOID);

TCB_t * volatile pxCurrentTCB[portNUM_PROCESSORS] = {NULL};

/**
 * @brief yield task
 * 
 * @param a [-]
 * @return void
 * @retval
 * @see
 */
void turboxYieldTask(void) {
    osThreadYield();
    return;
}

/**
 * @brief enter critical
 * 
 * @param a [-]
 * @return void
 * @retval
 * @see
 */
void turboxEnterCritical(void) {
    portDISABLE_INTERRUPTS();
    if (pdFALSE != g_taskScheduled) {
        osThreadId_t threadId = osThreadGetId();
        TaskHandle_t currentTCB = xTasks[((LosTaskCB *)threadId)->taskID];
        if (NULL != currentTCB) {
            (currentTCB->uxCriticalNesting)++;
        }
    }
    return;
}

/**
 * @brief exit critical
 * 
 * @param a [-]
 * @return void
 * @retval
 * @see
 */
void turboxExitCritical(void) {
    if (pdFALSE != g_taskScheduled) {
        osThreadId_t threadId = osThreadGetId();
        TaskHandle_t currentTCB = xTasks[((LosTaskCB *)threadId)->taskID];
        if (NULL != currentTCB) {
            if (INITIALIZATION_VALUE_0 < currentTCB->uxCriticalNesting) {
                (currentTCB->uxCriticalNesting)--;

                if (INITIALIZATION_VALUE_0 == currentTCB->uxCriticalNesting) {
                    portENABLE_INTERRUPTS();
                }
            }
        } else {
            portENABLE_INTERRUPTS();
        }
    }
    
    return;
}

/**
 * @brief disable interrupt
 * 
 * @param a [-]
 * @return void
 * @retval
 * @see
 */
void turboxDisableISR(void) {
    __asm volatile("csrc mstatus, 8");
    return;
}

/**
 * @brief create dynamic task
 * 
 * @param a [in] TaskFunction_t pxTaskCode
 * @param b [in] const char * const pcName
 * @param c [in] const configSTACK_DEPTH_TYPE usStackDepth
 * @param d [in] void * const pvParameters
 * @param e [in] UBaseType_t uxPriority
 * @param f [out] TaskHandle_t * const pxCreatedTask
 * @return BaseType_t 
 * @retval
 * 1 : result1； 
 * 2 : result2；
 * @see f()
 */
BaseType_t turboxCreateDynamicTask(TaskFunction_t pxTaskCode,
                            const char * const pcName,
                            const configSTACK_DEPTH_TYPE usStackDepth,
                            void * const pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask) {
    osThreadAttr_t attr;
    void *argument = NULL;
    osThreadFunc_t func = NULL;
    BaseType_t xReturn = pdFALSE;
    TCB_t *pxNewTCB;
    LosTaskCB *task = NULL;

    memset(&attr, INITIALIZATION_VALUE_0, sizeof(attr));
    osKernelLock();

    /* malloc memory for tcb because of dynamic task */
    pxNewTCB = (TCB_t*) pvPortMalloc(sizeof(TCB_t));
    if (NULL == pxNewTCB) {
        printf("Cannot Malloc the TCB for create task!!!\r\n");
        return pdFALSE;
    }

    /* dynamic task; need to free the memory when used */
    pxNewTCB->ucStaticallyAllocated = tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB;
    
    if (NULL == pxCreatedTask) {
        printf("the last parameter is NULL, cannot create the task!\r\n");
        return pdFALSE;
    }

    *pxCreatedTask = (TaskHandle_t)pxNewTCB;
    if (NULL != pxTaskCode) {
        func = pxTaskCode;
        argument = pvParameters;

        attr.name = pcName;
        attr.cb_mem = NULL;
        attr.stack_mem = NULL;
        attr.priority = uxPriority;
        attr.cb_size = INITIALIZATION_VALUE_0;
        attr.stack_size = usStackDepth * (sizeof(StackType_t));
        pxNewTCB->threadId = osThreadNew((osThreadFunc_t)func, argument, &attr);
        task = (LosTaskCB *)pxNewTCB->threadId;
    }

    /* create the task own semaphore to Simulated task notification */
    if (NULL == pxNewTCB->threadId) {
        /* free the memory */
        vPortFree(pxNewTCB);
        xReturn = pdFALSE;
    } else {
        xReturn = pdTRUE;
        uxCurrentNumberOfTasks++;

        xTasks[task->taskID] = pxNewTCB;
        memcpy(pxNewTCB->pcTaskName, pcName, (strlen(pcName) + INITIALIZATION_VALUE_1));
        (*pxCreatedTask)->uxPriority = uxPriority;
        pxNewTCB->ucNotifyState = taskNOT_WAITING_NOTIFICATION;
        
        /* create the task own semaphore */
        pxNewTCB->sem_id = osSemaphoreNew(20, INITIALIZATION_VALUE_0, NULL);        
    }

    osKernelUnlock();
    return xReturn;
}

/**
 * @brief create static task
 * 
 * @param a [in] TaskFunction_t pxTaskCode
 * @param b [in] const char * const pcName
 * @param c [in] const unsigned int ulStackDepth
 * @param d [in] void * const pvParameters
 * @param e [in] UBaseType_t uxPriority
 * @param f [in] StackType_t * const puxStackBuffer
 * @param g [in] StaticTask_t * const pxTaskBuffer
 * @return TaskHandle_t 
 * @retval
 * 1 : result1； task handle
 * 2 : result2；
 * @see f()
 */
TaskHandle_t turboxCreateStaticTask(TaskFunction_t pxTaskCode,
                                    const char * const pcName, 
                                    const unsigned int ulStackDepth,
                                    void * const pvParameters,
                                    UBaseType_t uxPriority,
                                    StackType_t * const puxStackBuffer,
                                    StaticTask_t * const pxTaskBuffer) {   
    osThreadAttr_t attr;
    void *argument = NULL;
    LosTaskCB *task = NULL;
    osThreadFunc_t func = NULL;
    TaskHandle_t tskHandle = NULL;

    if (NULL == pxTaskBuffer) {
        TURBOX_PRINTF("pxTaskBuffer is NULL!\r\n");
        return NULL;
    }

    if (NULL == pcName) {
        TURBOX_PRINTF("pcName is NULL!\r\n");
        return NULL;        
    }

    UNUSED(puxStackBuffer);
    memset(&attr, INITIALIZATION_VALUE_0, sizeof(attr));
    tskHandle = (TaskHandle_t)pxTaskBuffer;

    osKernelLock();
    /* static allocate; do not need to free when used */
    tskHandle->ucStaticallyAllocated = tskSTATICALLY_ALLOCATED_STACK_AND_TCB;

    if (NULL != pxTaskCode) {
        func = pxTaskCode;
        argument = pvParameters;

        attr.stack_size = ulStackDepth * (sizeof(StackType_t));
        attr.priority = uxPriority;
        attr.name = pcName;
        attr.cb_mem= NULL;
        attr.cb_size = 0;
        attr.stack_mem = NULL;
        tskHandle->threadId = osThreadNew((osThreadFunc_t)func, argument, &attr);
        task = (LosTaskCB *)tskHandle->threadId;
    }

    /* create the task own semaphore to Simulated task notification */
    if (NULL != tskHandle->threadId) {
        uxCurrentNumberOfTasks++;

        memcpy(tskHandle->pcTaskName, pcName, (strlen(pcName) + INITIALIZATION_VALUE_1));
        tskHandle->uxPriority = uxPriority;
        tskHandle->ucNotifyState = taskNOT_WAITING_NOTIFICATION;
        xTasks[task->taskID] = tskHandle;

        /* create the task own semaphore */
        tskHandle->sem_id = osSemaphoreNew(20, INITIALIZATION_VALUE_0, NULL);
    }

    osKernelUnlock();
    return tskHandle;
}

/**
 * @brief delete task
 * 
 * @param a [in] TaskHandle_t xTaskToDelete
 * @return void
 * @retval
 * @see
 */
void turboxDeleteTask(TaskHandle_t xTaskToDelete) {
    osThreadId_t *task = NULL;
    if (NULL != xTaskToDelete) {
        task = xTaskToDelete->threadId;

        /* clean the record of task */
        xTasks[((LosTaskCB *)task)->taskID] = NULL;

        /* delete the task own semaphore which used for Simulated task notification */
        osSemaphoreDelete(xTaskToDelete->sem_id);

        /* terminate the task */
        osThreadTerminate(task);
        if (tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB == xTaskToDelete->ucStaticallyAllocated) {
            /* free the memory if dynamic task */
            vPortFree(xTaskToDelete);
        }
        uxCurrentNumberOfTasks--;
    } else {
        /* dynamic task free */
        TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
        task = currentTask->threadId;

        /* clean the record of task */
        xTasks[((LosTaskCB *)task)->taskID] = NULL;

        /* delete the task own semaphore which used for Simulated task notification */
        osSemaphoreDelete(currentTask->sem_id);

        /* terminate the task */
        osThreadTerminate(task);

        if (tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB == currentTask->ucStaticallyAllocated) {
            /* free the memory if dynamic task */
            vPortFree(xTaskToDelete);
        }

        uxCurrentNumberOfTasks--;
        return;
    }

    return;
}

/**
 * @brief delay task
 * 
 * @param a [in] const TickType_t xTicksToDelay: delay ticks
 * @return void
 * @retval
 * @see f()
 */
void turboxDelayTask(const TickType_t xTicksToDelay) {
    osDelay(xTicksToDelay);
    return;
}

/**
 * @brief start Schedule
 * 
 * @param a [-] 
 * @return void
 * @retval
 * @see
 */
void turboxStartScheduler(void) {
    LOS_Start();
    return;
}

/**
 * @brief get tick count
 * 
 * @param a [-]
 * @return TickType_t
 * @retval
 * 1 : result1； tick count
 * @see
 */
TickType_t turboxGetTickCount(void) {
    TickType_t xReturn = 0;

    xReturn = (TickType_t)osKernelGetTickCount();

    return xReturn;
}

/**
 * @brief get tick count
 * 
 * @param a [out] TickType_t *ticks
 * @param b [out] overflow
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdTRUE
 * @see
 */
BaseType_t turboxGetTickCount2(TickType_t *ticks, BaseType_t *overflow) {
    *ticks = (TickType_t)osKernelGetTickCount();
    *overflow = INITIALIZATION_VALUE_0;

    return pdTRUE;
}

/**
 * @brief  get tick count
 * 
 * @param a [-]
 * @return TickType_t
 * @retval
 * 1 : result1； tick count
 * @see 
 */
TickType_t turboxGetTickCountFromISR(void) {
    TickType_t xReturn = INITIALIZATION_VALUE_0;

    xReturn = (TickType_t)osKernelGetTickCount();

    return xReturn;
}

/**
 * @brief get stack high water mark
 * 
 * @param a [in] TaskHandle_t xTask
 * @return UBaseType_t 
 * @retval
 * 1 : result1； 
 * 2 : result2；
 * @see f()
 */
UBaseType_t turboxGetStackHighWaterMark(TaskHandle_t xTask) {
    BaseType_t xReturn = INITIALIZATION_VALUE_0;

    if (NULL == xTask) {
        TURBOX_PRINTF("xTask is NULL!\r\n");
        return INITIALIZATION_VALUE_0;
    }

    osThreadId_t hTask = xTask->threadId;
    if (NULL != hTask) {
        /* get the stack space of thread */
        xReturn = (UBaseType_t)osThreadGetStackSpace(hTask);
    } else {
        TURBOX_PRINTF("xTask->threadId is NULL!\r\n");
    }
    return xReturn;
}

/**
 * @brief take task notify
 * 
 * @param a [in] BaseType_t xClearCountOnExit
 * @param b [in] TickType_t xTicksToWait: wait ticks
 * @return uint32_t
 * @retval
 * 1 : result1； notify value 
 * @see
 */
uint32_t turboxTakeTaskNotify(BaseType_t xClearCountOnExit, TickType_t xTicksToWait) {
    TaskHandle_t tskHnd = NULL;
    uint32_t ulReturn = pdFALSE;

    tskHnd = xTaskGetCurrentTaskHandle();
    if (NULL == tskHnd) {
        TURBOX_PRINTF("[ulTaskNotifyTake]Get tskhand is NULL!\n");
        return pdFALSE;
    }

    if (INITIALIZATION_VALUE_0 == tskHnd->ulNotifiedValue) {
        tskHnd->ucNotifyState = taskWAITING_NOTIFICATION;
        if ((TickType_t)INITIALIZATION_VALUE_0 < xTicksToWait) {
            if (osOK != osSemaphoreAcquire(tskHnd->sem_id, xTicksToWait)) {
                TURBOX_PRINTF("osSemaphoreAcquire Failed!\n");
                return pdFALSE;
            }
        }
    }
    
    turboxEnterCritical();
    ulReturn = tskHnd->ulNotifiedValue;
    if (xClearCountOnExit) {
        ((LosSemCB *)tskHnd->sem_id)->semCount = INITIALIZATION_VALUE_0;
        tskHnd->ulNotifiedValue = INITIALIZATION_VALUE_0;
    } else {
        tskHnd->ulNotifiedValue = ulReturn - INITIALIZATION_VALUE_1;
    }
    tskHnd->ucNotifyState = taskNOT_WAITING_NOTIFICATION;
    turboxExitCritical();

    return ulReturn;
}

/**
 * @brief wait task notify
 * 
 * @param a [in] uint32_t ulBitsToClearOnEntry
 * @param b [in] uint32_t ulBitsToClearOnExit
 * @param c [out] uint32_t *pulNotificationValue
 * @param b [in] TickType_t xTicksToWait
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdTRUE/pdFALSE
 * 2 : result2；
 * @see f()
 */
BaseType_t turboxWaitTaskNotify( uint32_t ulBitsToClearOnEntry, 
                            uint32_t ulBitsToClearOnExit, 
                            uint32_t *pulNotificationValue, 
                            TickType_t xTicksToWait ) {
    TaskHandle_t tskHnd = NULL;
    BaseType_t xReturn = pdTRUE;

    tskHnd = xTaskGetCurrentTaskHandle();
    if (NULL == tskHnd) {
        TURBOX_PRINTF("[xTaskNotifyWait]Get tskhand is NULL!\n");
        return pdFALSE;
    }
    if(tskHnd->ucNotifyState != taskNOTIFICATION_RECEIVED) {
        tskHnd->ucNotifyState = taskWAITING_NOTIFICATION;
        if(xTicksToWait > (TickType_t)0) {
            if (osOK != osSemaphoreAcquire(tskHnd->sem_id, xTicksToWait)) {
                TURBOX_PRINTF("osSemaphoreAcquire Failed!\n");
                return pdFALSE;                
            }
        }
    }
    turboxEnterCritical();
    if(pulNotificationValue != NULL) {
        *pulNotificationValue = tskHnd->ulNotifiedValue;
    }
    tskHnd->ucNotifyState = taskNOT_WAITING_NOTIFICATION;
    turboxExitCritical();

    return xReturn;
}


/**
 * @brief notify task
 * 
 * @param a [in] TaskHandle_t xTaskToNotify
 * @param b [in] uint32_t ulValue
 * @param c [in] eNotifyAction eAction
 * @param d [out] uint32_t *pulPreviousNotificationValue
 * @return int 
 * @retval
 * 1 : result1； 
 * 2 : result2；
 * @see f()
 */
BaseType_t turboxGenericNotify(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue) {
    BaseType_t xReturn = pdPASS;
    uint8_t ucOriginalNotifyState = INITIALIZATION_VALUE_0;

    if (NULL == xTaskToNotify) {
        TURBOX_PRINTF("[xTaskGenericNotify]xTaskToNotify is NULL!\n");
        return pdFALSE;
    }
    turboxEnterCritical();
    if(NULL != pulPreviousNotificationValue) {
        *pulPreviousNotificationValue = xTaskToNotify->ulNotifiedValue;
    }
    ucOriginalNotifyState = xTaskToNotify->ucNotifyState;
    xTaskToNotify->ucNotifyState = taskNOTIFICATION_RECEIVED;

    switch( eAction )
    {
        case eSetBits   :
            xTaskToNotify->ulNotifiedValue |= ulValue;
            break;

        case eIncrement :
            ( xTaskToNotify->ulNotifiedValue )++;
            break;

        case eSetValueWithOverwrite :
            break;

        case eSetValueWithoutOverwrite :
            break;

        case eNoAction:
            break;

        default:
            break;
    }

    if (taskWAITING_NOTIFICATION == ucOriginalNotifyState) {
        osSemaphoreRelease(xTaskToNotify->sem_id);
    }
    turboxExitCritical();

    return xReturn;
}

/**
 * @brief notify task from ISR
 * 
 * @param a [in] TaskHandle_t xTaskToNotify
 * @param b [in] uint32_t ulValue
 * @param c [in] eNotifyAction eAction
 * @param d [out] uint32_t *pulPreviousNotificationValue
 * @param e [out] BaseType_t *pxHigherPriorityTaskWoken
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdTRUE/pdFALSE
 * 2 : result2；
 * @see f()
 */
BaseType_t turboxGenericNotifyFromISR(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue, BaseType_t *pxHigherPriorityTaskWoken) {
    BaseType_t xReturn = pdTRUE;
    uint8_t ucOriginalNotifyState = INITIALIZATION_VALUE_0;

    if (NULL == xTaskToNotify) {
        TURBOX_PRINTF("[xTaskGenericNotifyFromISR]xTaskToNotify is NULL!\r\n");
        return pdFALSE;
    }
    ucOriginalNotifyState = xTaskToNotify->ucNotifyState;
    xTaskToNotify->ucNotifyState = taskNOTIFICATION_RECEIVED;
    if (NULL != pulPreviousNotificationValue) {
        *pulPreviousNotificationValue = xTaskToNotify->ulNotifiedValue;
    }

    switch(eAction) {
        case eSetBits   :
            xTaskToNotify->ulNotifiedValue |= ulValue;
            break;

        case eIncrement :
            ( xTaskToNotify->ulNotifiedValue )++;
            break;

        case eSetValueWithOverwrite :
            break;

        case eSetValueWithoutOverwrite :
            break;

        case eNoAction:
            break;

        default:
            break;
    }

    if (taskWAITING_NOTIFICATION == ucOriginalNotifyState) {
        osSemaphoreRelease(xTaskToNotify->sem_id);
    }
    
    return xReturn;
}

/**
 * @brief give task notify From ISR
 * 
 * @param a [in] TaskHandle_t xTaskToNotify
 * @param b [out] BaseType_t *pxHigherPriorityTaskWoken
 * @return void 
 * @retval
 * @see
 */
void turboxGiveTaskNotifyFromISR(TaskHandle_t xTaskToNotify, 
                             BaseType_t *pxHigherPriorityTaskWoken) {
    uint8_t ucOriginalNotifyState = INITIALIZATION_VALUE_0;
    ucOriginalNotifyState = xTaskToNotify->ucNotifyState;
    xTaskToNotify->ucNotifyState = taskNOTIFICATION_RECEIVED;
    if (NULL == xTaskToNotify) {
        TURBOX_PRINTF("[vTaskNotifyGiveFromISR]xTaskToNotify is NULL!\r\n");
        return;
    }

    (xTaskToNotify->ulNotifiedValue)++;
    if (taskWAITING_NOTIFICATION == ucOriginalNotifyState) {
        osSemaphoreRelease(xTaskToNotify->sem_id);
    }
    return;
}


/**
 * @brief clear notify state
 * 
 * @param a [in] TaskHandle_t xTask
 * @return BaseType_t 
 * @retval
 * 1 : result1； pdTRUE/pdFALSE
 * @see
 */
BaseType_t turboxClearNotifyState(TaskHandle_t xTask) {
    TaskHandle_t task = xTask;
    BaseType_t xReturn = pdFALSE;

    if (NULL == xTask) {
        task = xTaskGetCurrentTaskHandle();
    }

    if (NULL == task) {
        TURBOX_PRINTF("[xTaskNotifyStateClear]task is NULL!\n");
        return pdFALSE;
    }

    turboxEnterCritical();
    if (taskNOTIFICATION_RECEIVED == task->ucNotifyState) {
        task->ucNotifyState = taskNOT_WAITING_NOTIFICATION;
        xReturn = pdTRUE;
    } else {
        xReturn = pdFAIL;
    }
    ((LosSemCB *)task->sem_id)->semCount = INITIALIZATION_VALUE_0;
    task->ulNotifiedValue = INITIALIZATION_VALUE_0;
    turboxExitCritical();

    return xReturn;
}

/**
 * @brief add tick
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1； pdTRUE
 * @see
 */
BaseType_t turboxAddTick(void) {
    BaseType_t xReturn = pdTRUE;

    OsTickHandler();

    return xReturn;
}

/**
 * @brief get current thread control block
 * 
 * @param a [-]
 * @return TaskHandle_t 
 * @retval
 * 1 : result1； task handle
 * @see
 */
TaskHandle_t turboxGetCurrentTCB(void) {
    UINTPTR intSave = INITIALIZATION_VALUE_0;
    intSave = LOS_IntLock();

    /* get the current thread control block according to the threadId */
    osThreadId_t threadId = osThreadGetId();
    TaskHandle_t xReturn = xTasks[((LosTaskCB *)threadId)->taskID];
    LOS_IntRestore(intSave);
    return xReturn;
}

/**
 * @brief task step tick
 * 
 * @param a [in] const TickType_t xTicksToJump
 * @return void
 * @retval
 * @see
 */
void turboxTaskStepTick(const TickType_t xTicksToJump) {
    // g_ullTickCount = g_ullTickCount + xTicksToJump;
    return;
}

/**
 * @brief task step tick
 * 
 * @param a [in] const TickType_t xTicksToJump
 * @return void
 * @retval
 * @see
 */
void turboxTaskStepTickSafe(const TickType_t xTicksToJump) {
    // g_ullTickCount = g_ullTickCount + xTicksToJump;
    return;
}

/**
 * @brief switch task context
 * 
 * @param a [-]
 * @return void
 * @retval
 * @see
 */
void turboxSwitchTaskContext(void) {
    LOS_Schedule();
    return;
}

/**
 * @brief check is or not in interrupt handling
 * 
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1； 
 * 2 : result2；
 * @see f()
 */
BaseType_t xPortIsInsideInterrupt(void) {
    BaseType_t xReturn = pdTRUE;

    if (INITIALIZATION_VALUE_0 < g_intCount) {
        /* Inside Interrupt Handling */
        xReturn = pdTRUE;
        return xReturn;
    } 

    /* Outside Interrupt Handling */
    xReturn = pdFALSE;
    return xReturn;
}

//****************************************************************************************************//
//                                    add for esp32
//****************************************************************************************************//

/**
 * @brief Suspends the scheduler without disabling interrupts
 *
 * @param a [-]
 * @return void
 * @retval
 * @see
 */
void turboxSuspendAllvTask( void ) {
    LOS_TaskLock();
    return;
}

/**
 * @brief Resumes scheduler activity after it was suspended by a call to
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1；
 * 2 : result2；
 * @see f()
 */
BaseType_t turboxResumeAllTask( void ) {

    BaseType_t xReturn = pdTRUE;

    LOS_TaskUnlock();

    return xReturn;
}

/**
 * @brief Get task name
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1；
 * 2 : result2；
 * @see f()
 */
char * turboxpcTaskGetName( TaskHandle_t xTaskToQuery ) {
    return LOS_TaskNameGet(xTaskToQuery->uxTaskNumber);
}
/**
 * @brief suspend task
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1；
 * 2 : result2；
 * @see f()
 */
void turboxvTaskSuspend( TaskHandle_t xTaskToSuspend ) {
    LOS_TaskSuspend(xTaskToSuspend->uxTaskNumber);
    return;
}

/**
 * @brief Get local storage pointer specific to the given task
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1；
 * 2 : result2；
 * @see f()
 */
void * turboxpvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery,
                                                   BaseType_t xIndex ) {
    void * pvReturn = NULL;
    TCB_t * pxTCB;

    if( xIndex < configNUM_THREAD_LOCAL_STORAGE_POINTERS ) {
        pxTCB = OS_TCB_FROM_TID(xTaskToQuery->uxTaskNumber);
        pvReturn = pxTCB->pvThreadLocalStoragePointers[ xIndex ];
    }
    else {
        pvReturn = NULL;
    }

    return pvReturn;
}

/**
 * @brief Set local storage pointer and deletion callback
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1；
 * 2 : result2；
 * @see f()
 */
void turboxvTaskSetThreadLocalStoragePointerAndDelCallback( TaskHandle_t xTaskToSet,
                                                            BaseType_t xIndex, void *pvValue ,
                                                            TlsDeleteCallbackFunction_t xDelCallback) {
    TCB_t *pxTCB;

    if( xIndex < configNUM_THREAD_LOCAL_STORAGE_POINTERS ) {
        taskENTER_CRITICAL();
        pxTCB = OS_TCB_FROM_TID( xTaskToSet->uxTaskNumber );
        pxTCB->pvThreadLocalStoragePointers[ xIndex ] = pvValue;
        pxTCB->pvThreadLocalStoragePointersDelCallback[ xIndex ] = xDelCallback;
        taskEXIT_CRITICAL();
    }

    return;
}

/**
 * @brief Get the current core affinity of a task
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1；
 * 2 : result2；
 * @see f()
 */
BaseType_t turboxTaskGetAffinity( TaskHandle_t xTask ) {
    TCB_t *pxTCB;

    pxTCB = OS_TCB_FROM_TID( xTask->uxTaskNumber );

    return pxTCB->xCoreID;
}

/**
 * @brief  Return the handle of the task running on a certain CPU
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1；
 * 2 : result2；
 * @see f()
 */
TaskHandle_t turboxTaskGetCurrentTaskHandleForCPU( BaseType_t cpuid ) {
    (void)cpuid;
    UINTPTR intSave = INITIALIZATION_VALUE_0;
    intSave = LOS_IntLock();

    /* get the current thread control block according to the threadId */
    osThreadId_t threadId = osThreadGetId();

    TaskHandle_t xReturn = xTasks[((LosTaskCB *)threadId)->taskID];
    LOS_IntRestore(intSave);

    return xReturn;
}

/**
 * @brief Get the handle of idle task for the given CPU
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1；
 * 2 : result2；
 * @see f()
 */
TaskHandle_t turboxTaskGetIdleTaskHandleForCPU( UBaseType_t cpuid ) {
    TaskHandle_t xReturn=NULL;

    if (cpuid == g_idleTaskID) {
        turboxTaskGetCurrentTaskHandleForCPU(cpuid);
    }

    return xReturn;
}

/**
 * @brief Delay a task until a specified time
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1；
 * 2 : result2；
 * @see f()
 */
void turboxTaskDelayUntil(TickType_t * const pxPreviousWakeTime,
                                 const TickType_t xTimeIncrement) {
    (void)pxPreviousWakeTime;

    osDelayUntil(xTimeIncrement);

    return;
}

/**
 * @brief Returns the scheduler state
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1；
 * 2 : result2；
 * @see f()
 */
BaseType_t turboxTaskGetSchedulerState( void ) {
    BaseType_t xReturn;

    if( LOS_TaskIsRunning() == FALSE ) {
        xReturn = taskSCHEDULER_SUSPENDED;
    }
    else {
        xReturn = taskSCHEDULER_RUNNING;
    }

    return xReturn;
}

/**
 * @brief Resume task
 *
 * @param a [-]
 * @return BaseType_t
 * @retval
 * 1 : result1；
 * 2 : result2；
 * @see f()
 */
void turboxvTaskResume( TaskHandle_t xTaskToResume ) {
    LOS_TaskResume(xTaskToResume->uxTaskNumber);
    return;
}

#if ( configUSE_NEWLIB_REENTRANT == 1 )
//Return global reent struct if FreeRTOS isn't running,
struct _reent* __getreent(void) {
    return _GLOBAL_REENT;
}
#endif