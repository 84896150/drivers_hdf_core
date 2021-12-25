/**
 * Copyright (c) 2020-2021 ThunderSoft
 * All Rights Reserved by Thunder Software Technology Co., Ltd and its affiliates.
 * You may not use, copy, distribute, modify, transmit in any form this file
 * except in compliance with ThunderSoft in writing by applicable law.
 *
 */
/**
 * @file    turbox_memory.c
 * @brief   harmony bl platform memory adaptor
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
#include <los_memory.h>
#undef CLINT_CTRL_ADDR
#include <FreeRTOS.h>
#include <cmsis_os2.h>

/**
 * @brief define heap regions
 * 
 * @param a [in] const HeapRegion_t * const pxHeapRegions: heap regions
 * @return void
 * @retval
 * @see
 */
void turboxDefineHeapRegions(const HeapRegion_t * const pxHeapRegions) {
    /* nothing need to do */
	UNUSED(pxHeapRegions);
    return;
}

/**
 * @brief malloc memory
 * 
 * @param a [in] param a
 * @param b [in, out] param b
 * @param c [out] output
 * @return int 
 * @retval
 * 1 : result1； 
 * 2 : result2；
 * @see f()
 */
void *turboxMallocMemory(size_t xWantedSize) {
    void *xReturn = NULL;

    if (INITIALIZATION_VALUE_0 == xWantedSize) {
        TURBOX_PRINTF("WantSize is 0, no excute malloc action!\r\n");
        return NULL;
    }

    /* malloc the memory */
    // xReturn = LOS_MemAlloc(m_aucSysMem0, xWantedSize);
    xReturn = LOS_MemAllocAlign(m_aucSysMem0, xWantedSize, 4);
    if (NULL == xReturn) {
        TURBOX_PRINTF("Malloc Memory Failed!\r\n");
        return NULL;
    }

    return xReturn;
}

/**
 * @brief Free Memory
 * 
 * @param a [in] void *pv: pointer which need to free
 * @return void
 * @retval
 * @see
 */
void turboxFreeMemory(void *pv) {
    if (NULL == pv) {
        TURBOX_PRINTF("Free Memory Input Parameter Error!\r\n");
        return;
    }

    /* free the memory */
    if (LOS_NOK == LOS_MemFree(m_aucSysMem0, pv)) {
        TURBOX_PRINTF("Free Memory Error!\r\n");
    }
    return;
}

/**
 * @brief get free heap size
 * 
 * @param a [-] 
 * @return size_t 
 * @retval
 * 1 : result1: Free Heap Size 
 * @see
 */
size_t turboxGetFreeHeapSize(void) {
	size_t xReturn = INITIALIZATION_VALUE_0;
    LOS_MEM_POOL_STATUS status = {INITIALIZATION_VALUE_0};

    /* get all the memory information freeSize,UsedSize etc. */
    if (LOS_OK == LOS_MemInfoGet(m_aucSysMem0, &status)) {
        xReturn = status.totalFreeSize;
    } else {
        TURBOX_PRINTF("Get Memory Information Error!\r\n");
    }

    return xReturn;
}