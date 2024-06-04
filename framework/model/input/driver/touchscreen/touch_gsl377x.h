/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef TOUCH_GSL377X_H
#define TOUCH_GSL377X_H

#define GSL_FW_VER_MASK 0x00000080
/* cmd list */
#define GSL_GET_TOUCH_CMD 0x0100 //get touch data cmd
#define GSL_SRAM_READ_CMD 0x0714 //sram mode read
#define GSL_READ_REG_CMD  0x0801 //read one reg cmd
#define GSL_WRITE_REG_CMD 0x0802 //write one reg cmd
#define GSL_READ_MASS_CMD 0x0803 //read mass data
#define GSL_WRITE_MASS_CMD 0x0804 //write mass data
#define GSL_CHK_STATUS_CMD 0x0807 //chip status cmd
#define GSL_FW_VER_CMD    0x0809 //firmware version cmd
#define GSL_CFG_VER_CMD   0x080A //cfg version cmd
#define GSL_HW_VER_CMD    0x080B //hardware version cmd
#define GSL_SENSOR_ID_CMD 0x080C //senosr id cmd
#define GSL_SUSPEND_CMD    0x0810 //suspend
#define GSL_RESUME_CMD     0X0811 //resume
#define GSL_ENTER_GESTURE  0x0812 //gesture mode
#define GSL_EXIT_GESTURE   0x0813 //exit gesture
#define GSL_VERIFY_DATA_CMD 0x0832 //verify data cmd
#define GSL_RUN_BOOTLOADER_CMD 0x083a //jump to bootloader cmd
#define GSL_RUN_FIRMWARE_CMD   0x083B //jump to firmware cmd

#define CMD_READ_RESP_LEN 8
#define GSL_BYTE_WIDTH 8
#define GSL_NIBBLE_WIDTH 4
#endif
