/*
 * Copyright (c) 2020-2022 Chipsea Technologies (Shenzhen) Corp., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PPG_CS1262_H
#define PPG_CS1262_H

#include "ppg_cs1262_spi.h"
#include "hdf_device_desc.h"
/****************************************** DEFINE ******************************************/
#ifdef __LITEOS__
#define HEART_REG_TAB    "/etc/cs1262_default.bin"
#else
#define HEART_REG_TAB    "/system/etc/ppgconfig/cs1262_default.bin"
#endif
#define CS1262_MAX_FIFO_READ_NUM 800
#define CS1262_DEVICE_ID   0x1262
/******************************************* ENUM ********************************************/
typedef enum {
    CS1262_REG_LOCK = 0,
    CS1262_REG_UNLOCK = 1,
} Cs1262LockStatus;
/***************************************** TYPEDEF ******************************************/
// tab regs num
#define SYS_REGS_NUM      6
#define TL_REGS_NUM       40
#define TX_REGS_NUM       15
#define RX_REGS_NUM       77
#define TE_REGS_NUM       38
#define WEAR_REGS_NUM     25
#define FIFO_REGS_NUM     10
#define FIFO_WRITE_OFFSET 1

typedef uint16_t TxConfig[TX_REGS_NUM];
typedef uint16_t RxConfig[RX_REGS_NUM];
typedef uint16_t TlConfig[TL_REGS_NUM];
typedef uint16_t TeConfig[TE_REGS_NUM];
typedef uint16_t WearConfig[WEAR_REGS_NUM];
typedef uint16_t FifoConfig[FIFO_REGS_NUM];

typedef struct {
    uint16_t startMagic;
    uint16_t clock;
    TlConfig tlTab;
    TxConfig txTab;
    RxConfig rxTab;
    TeConfig teTab;
    WearConfig wearTab;
    FifoConfig fifoTab;
    uint16_t checksum;
} Cs1262RegConfigTab;

enum PpgMode{
    NONE_MODE = 0,
    DEFAULT_MODE = 1,
    HEART_RATE_MODE = 1,
    REG_MODE_MAX,
};

struct Cs1262DrvData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct PpgCfgData *ppgCfg;
    enum PpgMode regMode;
};

struct PpgModeTab {
    enum PpgMode mode;
    Cs1262RegConfigTab *regTab;
};

int32_t Cs1262Loadfw(enum PpgMode mode, Cs1262RegConfigTab **configTab);

#endif /* CS1262_H */
