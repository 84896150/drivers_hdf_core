/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "gpio_if.h"
#include "hdf_bl.h"
#include "hdf_disp.h"
#include "hdf_log.h"
#include "mipi_dsi_if.h"
#include "osal.h"
#include "pwm_if.h"

#define RESET_GPIO                5
#define MIPI_DSI0                 0
#define BLK_PWM1                  1
#define PWM_MAX_PERIOD            100000
/* backlight setting */
#define MIN_LEVEL                 0
#define MAX_LEVEL                 255
#define DEFAULT_LEVEL             100

#define WIDTH                     480
#define HEIGHT                    960
#define HORIZONTAL_BACK_PORCH     20
#define HORIZONTAL_FRONT_PORCH    20
#define HORIZONTAL_SYNC_WIDTH     10
#define VERTICAL_BACK_PORCH       14
#define VERTICAL_FRONT_PORCH      16
#define VERTICAL_SYNC_WIDTH       2
#define FRAME_RATE                60

#define WIDTH1                    480
#define HEIGHT1                   960
#define HORIZONTAL_BACK_PORCH1    44
#define HORIZONTAL_FRONT_PORCH1   46
#define HORIZONTAL_SYNC_WIDTH1    4
#define VERTICAL_BACK_PORCH1      15
#define VERTICAL_FRONT_PORCH1     15
#define VERTICAL_SYNC_WIDTH1      2
#define FRAME_RATE1               60

/* panel0 on command payload */
static uint8_t g_payLoad0[] = { 0xF0, 0x5A, 0x5A };
static uint8_t g_payLoad1[] = { 0xF1, 0xA5, 0xA5 };
static uint8_t g_payLoad2[] = { 0xB3, 0x03, 0x03, 0x03, 0x07, 0x05, 0x0D, 0x0F, 0x11, 0x13, 0x09, 0x0B };
static uint8_t g_payLoad3[] = { 0xB4, 0x03, 0x03, 0x03, 0x06, 0x04, 0x0C, 0x0E, 0x10, 0x12, 0x08, 0x0A };
static uint8_t g_payLoad4[] = { 0xB0, 0x54, 0x32, 0x23, 0x45, 0x44, 0x44, 0x44, 0x44, 0x60, 0x00, 0x60, 0x1C };
static uint8_t g_payLoad5[] = { 0xB1, 0x32, 0x84, 0x02, 0x87, 0x12, 0x00, 0x50, 0x1C };
static uint8_t g_payLoad6[] = { 0xB2, 0x73, 0x09, 0x08 };
static uint8_t g_payLoad7[] = { 0xB6, 0x5C, 0x5C, 0x05 };
static uint8_t g_payLoad8[] = { 0xB8, 0x23, 0x41, 0x32, 0x30, 0x03 };
static uint8_t g_payLoad9[] = { 0xBC, 0xD2, 0x0E, 0x63, 0x63, 0x5A, 0x32, 0x22, 0x14, 0x22, 0x03 };
static uint8_t g_payLoad10[] = { 0xb7, 0x41 };
static uint8_t g_payLoad11[] = { 0xC1, 0x0c, 0x10, 0x04, 0x0c, 0x10, 0x04 };
static uint8_t g_payLoad12[] = { 0xC2, 0x10, 0xE0 };
static uint8_t g_payLoad13[] = { 0xC3, 0x22, 0x11 };
static uint8_t g_payLoad14[] = { 0xD0, 0x07, 0xFF };
static uint8_t g_payLoad15[] = { 0xD2, 0x63, 0x0B, 0x08, 0x88 };
static uint8_t g_payLoad16[] = { 0xC6, 0x08, 0x15, 0xFF, 0x10, 0x16, 0x80, 0x60 };
static uint8_t g_payLoad17[] = { 0xc7, 0x04 };
static uint8_t g_payLoad18[] = {
    0xC8, 0x7C, 0x50, 0x3B, 0x2C, 0x25, 0x16, 0x1C, 0x08, 0x27, 0x2B, 0x2F, 0x52, 0x43, 0x4C, 0x40,
    0x3D, 0x30, 0x1E, 0x06, 0x7C, 0x50, 0x3B, 0x2C, 0x25, 0x16, 0x1C, 0x08, 0x27, 0x2B, 0x2F, 0x52,
    0x43, 0x4C, 0x40, 0x3D, 0x30, 0x1E, 0x06
};
static uint8_t g_payLoad19[] = { 0x11 };
static uint8_t g_payLoad20[] = { 0x29 };

struct DsiCmdDesc g_OnCmd0[] = {
    { 0x29, 0, sizeof(g_payLoad0), g_payLoad0 },
    { 0x29, 0, sizeof(g_payLoad1), g_payLoad1 },
    { 0x29, 0, sizeof(g_payLoad2), g_payLoad2 },
    { 0x29, 0, sizeof(g_payLoad3), g_payLoad3 },
    { 0x29, 0, sizeof(g_payLoad4), g_payLoad4 },
    { 0x29, 0, sizeof(g_payLoad5), g_payLoad5 },
    { 0x29, 0, sizeof(g_payLoad6), g_payLoad6 },
    { 0x29, 0, sizeof(g_payLoad7), g_payLoad7 },
    { 0x29, 0, sizeof(g_payLoad8), g_payLoad8 },
    { 0x29, 0, sizeof(g_payLoad9), g_payLoad9 },
    { 0x23, 0, sizeof(g_payLoad10), g_payLoad10 },
    { 0x29, 0, sizeof(g_payLoad11), g_payLoad11 },
    { 0x29, 0, sizeof(g_payLoad12), g_payLoad12 },
    { 0x29, 0, sizeof(g_payLoad13), g_payLoad13 },
    { 0x29, 0, sizeof(g_payLoad14), g_payLoad14 },
    { 0x29, 0, sizeof(g_payLoad15), g_payLoad15 },
    { 0x29, 0, sizeof(g_payLoad16), g_payLoad16 },
    { 0x23, 0, sizeof(g_payLoad17), g_payLoad17 },
    { 0x29, 1, sizeof(g_payLoad18), g_payLoad18 },
    { 0x05, 120, sizeof(g_payLoad19), g_payLoad19 },
    { 0x05, 120, sizeof(g_payLoad20), g_payLoad20 },
};

/* panel1 on command payload */
static uint8_t g_payLoad1_0[] = { 0xFF,0x77,0x01,0x00,0x00,0x13};
static uint8_t g_payLoad1_1[] = { 0xEF,0x08};
static uint8_t g_payLoad1_2[] = { 0xFF,0x77,0x01,0x00,0x00,0x10};
static uint8_t g_payLoad1_3[] = { 0xC0,0xF7,0x02};
static uint8_t g_payLoad1_4[] = { 0xC1,0x0C,0x02};
static uint8_t g_payLoad1_5[] = { 0xC2,0x07,0x02};
static uint8_t g_payLoad1_6[] = { 0xC6,0x21};
static uint8_t g_payLoad1_7[] = { 0xCC,0x30};
static uint8_t g_payLoad1_8[] = { 0xB0,0x00,0x0B,0x12,0x0D,0x11,0x07,0x04,0x08,0x07,0x20,0x05,0x12,0x0F,0x28,0x30,0x1B};
static uint8_t g_payLoad1_9[] = { 0xB1,0x00,0x0B,0x12,0x0D,0x11,0x05,0x03,0x08,0x08,0x20,0x03,0x10,0x10,0x29,0x30,0x1B};
static uint8_t g_payLoad1_10[] = { 0xFF,0x77,0x01,0x00,0x00,0x11};
static uint8_t g_payLoad1_11[] = { 0xB0,0x4D};
static uint8_t g_payLoad1_12[] = { 0xB1,0x2A};
static uint8_t g_payLoad1_13[] = { 0xB2,0x87};
static uint8_t g_payLoad1_14[] = { 0xB3,0x80};
static uint8_t g_payLoad1_15[] = { 0xB5,0x49};
static uint8_t g_payLoad1_16[] = { 0xB7,0x87};
static uint8_t g_payLoad1_17[] = { 0xB8,0x21};
static uint8_t g_payLoad1_18[] = { 0xB9,0x10};
static uint8_t g_payLoad1_19[] = { 0xBB,0x00};
static uint8_t g_payLoad1_20[] = { 0xC0,0x09};
static uint8_t g_payLoad1_21[] = { 0xC1,0x78};
static uint8_t g_payLoad1_22[] = { 0xC2,0x78};
static uint8_t g_payLoad1_23[] = { 0xD0,0x88};
static uint8_t g_payLoad1_24[] = { 0xFF,0x77,0x01,0x00,0x00,0x11};
static uint8_t g_payLoad1_25[] = { 0xE0,0x00,0x19,0x00};
static uint8_t g_payLoad1_26[] = { 0xE1,0x07,0x82,0x00,0x82,0x06,0x82,0x00,0x82,0x00,0x40,0x40};
static uint8_t g_payLoad1_27[] = { 0xE2,0x30,0x30,0x40,0x40,0xCC,0x82,0x00,0x82,0xCB,0x82,0x00,0x82,0x00};
static uint8_t g_payLoad1_28[] = { 0xE3,0x00,0x00,0x33,0x33};
static uint8_t g_payLoad1_29[] = { 0xE4,0x44,0x44};
static uint8_t g_payLoad1_30[] = { 0xE5,0x09,0xC5,0xAA,0x82,0x0B,0xC7,0xAA,0x82,0x0D,0xC9,0xAA,0x82,0x0F,0xCB,0xAA,0x82};
static uint8_t g_payLoad1_31[] = { 0xE6,0x00,0x00,0x33,0x33};
static uint8_t g_payLoad1_32[] = { 0xE7,0x44,0x44};
static uint8_t g_payLoad1_33[] = { 0xE8,0x08,0xC4,0xAA,0x82,0x0A,0xC6,0xAA,0x82,0x0C,0xC8,0xAA,0x82,0x0E,0xCA,0xAA,0x82};
static uint8_t g_payLoad1_34[] = { 0xEB,0x00,0x00,0xE4,0xE4,0x88,0x00,0x10};
static uint8_t g_payLoad1_35[] = { 0xEC,0x3D,0x00};
static uint8_t g_payLoad1_36[] = { 0xED,0xAB,0x98,0x76,0x54,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x20,0x45,0x67,0x89,0xBA};
static uint8_t g_payLoad1_37[] = { 0xEF,0x08,0x08,0x08,0x45,0x3F,0x54};
static uint8_t g_payLoad1_38[] = { 0xFF,0x77,0x01,0x00,0x00,0x00};
// --Bank2 bist mode strat-- //
// static uint8_t g_payLoad1_39[] = { 0xFF,0x77,0x01,0x00,0x00,0x12};  // BK2
// static uint8_t g_payLoad1_40[] = { 0xD1,0x81}; // bist mode
// static uint8_t g_payLoad1_41[] = { 0xD2,0x08}; // test pattern
// --bist mode end-- //
static uint8_t g_payLoad1_42[] = { 0x11};
static uint8_t g_payLoad1_43[] = { 0x29};

struct DsiCmdDesc g_OnCmd1[] = {
    { 0x29, 0, sizeof(g_payLoad1_0), g_payLoad1_0 },
    { 0x23, 0, sizeof(g_payLoad1_1), g_payLoad1_1 },
    { 0x29, 0, sizeof(g_payLoad1_2), g_payLoad1_2 },
    { 0x29, 0, sizeof(g_payLoad1_3), g_payLoad1_3 },
    { 0x29, 0, sizeof(g_payLoad1_4), g_payLoad1_4 },
    { 0x29, 0, sizeof(g_payLoad1_5), g_payLoad1_5 },
    { 0x23, 0, sizeof(g_payLoad1_6), g_payLoad1_6 },
    { 0x23, 0, sizeof(g_payLoad1_7), g_payLoad1_7 },
    { 0x29, 0, sizeof(g_payLoad1_8), g_payLoad1_8 },
    { 0x29, 0, sizeof(g_payLoad1_9), g_payLoad1_9 },
    { 0x29, 0, sizeof(g_payLoad1_10), g_payLoad1_10 },
    { 0x23, 0, sizeof(g_payLoad1_11), g_payLoad1_11 },
    { 0x23, 0, sizeof(g_payLoad1_12), g_payLoad1_12 },
    { 0x23, 0, sizeof(g_payLoad1_13), g_payLoad1_13 },
    { 0x23, 0, sizeof(g_payLoad1_14), g_payLoad1_14 },
    { 0x23, 0, sizeof(g_payLoad1_15), g_payLoad1_15 },
    { 0x23, 0, sizeof(g_payLoad1_16), g_payLoad1_16 },
    { 0x23, 0, sizeof(g_payLoad1_17), g_payLoad1_17 },
    { 0x23, 0, sizeof(g_payLoad1_18), g_payLoad1_18 },
    { 0x23, 0, sizeof(g_payLoad1_19), g_payLoad1_19 },
    { 0x23, 0, sizeof(g_payLoad1_20), g_payLoad1_20 },
    { 0x23, 0, sizeof(g_payLoad1_21), g_payLoad1_21 },
    { 0x23, 0, sizeof(g_payLoad1_22), g_payLoad1_22 },
    { 0x23, 0, sizeof(g_payLoad1_23), g_payLoad1_23 },
    { 0x29, 0, sizeof(g_payLoad1_24), g_payLoad1_24 },
    { 0x29, 0, sizeof(g_payLoad1_25), g_payLoad1_25 },
    { 0x29, 0, sizeof(g_payLoad1_26), g_payLoad1_26 },
    { 0x29, 0, sizeof(g_payLoad1_27), g_payLoad1_27 },
    { 0x29, 0, sizeof(g_payLoad1_28), g_payLoad1_28 },
    { 0x29, 0, sizeof(g_payLoad1_29), g_payLoad1_29 },
    { 0x29, 0, sizeof(g_payLoad1_30), g_payLoad1_30 },
    { 0x29, 0, sizeof(g_payLoad1_31), g_payLoad1_31 },
    { 0x29, 0, sizeof(g_payLoad1_32), g_payLoad1_32 },
    { 0x29, 0, sizeof(g_payLoad1_33), g_payLoad1_33 },
    { 0x29, 0, sizeof(g_payLoad1_34), g_payLoad1_34 },
    { 0x29, 0, sizeof(g_payLoad1_35), g_payLoad1_35 },
    { 0x29, 0, sizeof(g_payLoad1_36), g_payLoad1_36 },
    { 0x29, 0, sizeof(g_payLoad1_37), g_payLoad1_37 },
    { 0x29, 0, sizeof(g_payLoad1_38), g_payLoad1_38 },
    // --Bank2 bist mode strat-- //
    // { 0x29, 0, sizeof(g_payLoad1_39), g_payLoad1_39 },
    // { 0x23, 0, sizeof(g_payLoad1_40), g_payLoad1_40 },
    // { 0x23, 0, sizeof(g_payLoad1_41), g_payLoad1_41 },
    // --bist mode end-- //
    { 0x05, 120, sizeof(g_payLoad1_42), g_payLoad1_42 },
    { 0x05, 50, sizeof(g_payLoad1_43), g_payLoad1_43 },
};

/* panel off command payload */
static uint8_t g_offPayLoad0[] = { 0x28 };
static uint8_t g_offPayLoad1[] = { 0x10 };
struct DsiCmdDesc g_offCmd[] = {
    { 0x05, 20, sizeof(g_offPayLoad0), g_offPayLoad0 },
    { 0x05, 120, sizeof(g_offPayLoad1), g_offPayLoad1 },
};

static uint8_t g_panelSolution = 0;  //0-icn9700[default], 1-ST7701SN

struct Icn9700Dev {
    struct PanelData panel;
    DevHandle mipiHandle;
    uint16_t reset_gpio;
    uint16_t reset_delay;
};

static int32_t LcdResetOn(struct Icn9700Dev *icn9700)
{
    int32_t ret;

    ret = GpioSetDir(icn9700->reset_gpio, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioSetDir failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    ret = GpioWrite(icn9700->reset_gpio, GPIO_VAL_HIGH);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioWrite failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    /* delay 20ms */
    OsalMSleep(icn9700->reset_delay);
    return HDF_SUCCESS;
}

static int32_t LcdResetOff(struct Icn9700Dev *icn9700)
{
    int32_t ret;

    ret = GpioSetDir(icn9700->reset_gpio, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioSetDir failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    ret = GpioWrite(icn9700->reset_gpio, GPIO_VAL_LOW);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("GpioWrite failed, ret:%d", ret);
        return HDF_FAILURE;
    }
    /* delay 20ms */
    OsalMSleep(icn9700->reset_delay);
    return HDF_SUCCESS;
}

static struct Icn9700Dev *PanelToIcn9700Dev(const struct PanelData *panel)
{
    struct Icn9700Dev *icn9700 = NULL;

    if (panel == NULL) {
        HDF_LOGE("%s: panel is null", __func__);
        return NULL;
    }
    if (panel->object == NULL) {
        HDF_LOGE("%s: object is null", __func__);
        return NULL;
    }
    icn9700 = (struct Icn9700Dev *)panel->object->priv;
    return icn9700;
}

static int32_t LcdCheckChipId(struct PanelData *panel)
{
    int32_t ret;
    struct Icn9700Dev *icn9700 = NULL;
    uint8_t payload = 0xA1;
    uint8_t bufRead[2] = { 0 };
    struct DsiCmdDesc cmdRead = {
        .dataType = 0x06,     /* 0x06: read data type */
        .dataLen = 1,         /* 1: data len */
        .delay = 0,
        .payload = &payload,
    };
    icn9700 = PanelToIcn9700Dev(panel);
    if ((icn9700 == NULL) || (icn9700->mipiHandle == NULL)) {
        HDF_LOGE("%s: icn9700 || mipiHandle is null", __func__);
        return HDF_FAILURE;
    }
    /* lcd reset power on */
    ret = LcdResetOn(icn9700);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LcdResetOn failed", __func__);
        return HDF_FAILURE;
    }
    ret = MipiDsiRx(icn9700->mipiHandle, &cmdRead, sizeof(bufRead), bufRead);
    if (ret == HDF_SUCCESS) {
        if(bufRead[0] == 0x99) {
            g_panelSolution = 1;
        } else {
            g_panelSolution = 0;
        }
    }
    /* lcd reset power off */
    ret = LcdResetOff(icn9700);
    return ret;
}

static int32_t Icn9700Init(struct PanelData *panel)
{
    struct Icn9700Dev *icn9700 = NULL;

    icn9700 = PanelToIcn9700Dev(panel);
    if (icn9700 == NULL) {
        HDF_LOGE("%s: icn9700 is null", __func__);
        return HDF_FAILURE;
    }
    icn9700->mipiHandle = MipiDsiOpen(MIPI_DSI0);
    if (icn9700->mipiHandle == NULL) {
        HDF_LOGE("%s: MipiDsiOpen failed", __func__);
        return HDF_FAILURE;
    }
    LcdCheckChipId(panel);
    return HDF_SUCCESS;
}

static int32_t Icn9700On(struct PanelData *panel)
{
    int32_t ret;
    struct Icn9700Dev *icn9700 = NULL;

    icn9700 = PanelToIcn9700Dev(panel);
    if (icn9700 == NULL) {
        HDF_LOGE("%s: icn9700 is null", __func__);
        return HDF_FAILURE;
    }
    /* lcd reset power on */
    ret = LcdResetOn(icn9700);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LcdResetOn failed", __func__);
        return HDF_FAILURE;
    }
    if (icn9700->mipiHandle == NULL) {
        HDF_LOGE("%s: mipiHandle is null", __func__);
        return HDF_FAILURE;
    }
    /* send mipi init code */
    int32_t count = 0;
    int32_t i;
    struct DsiCmdDesc* g_OnCmd = NULL;
    if (g_panelSolution == 1) {
        count = sizeof(g_OnCmd1) / sizeof(g_OnCmd1[0]);
        g_OnCmd = &(g_OnCmd1[0]);
    } else {
        count = sizeof(g_OnCmd0) / sizeof(g_OnCmd0[0]);
        g_OnCmd = &(g_OnCmd0[0]);
    }
    /* set mipi to lp mode */
    MipiDsiSetLpMode(icn9700->mipiHandle);
    for (i = 0; i < count; i++) {
        ret = MipiDsiTx(icn9700->mipiHandle, &(g_OnCmd[i]));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: MipiDsiTx failed", __func__);
            return HDF_FAILURE;
        }
    }
    /* set mipi to hs mode */
    MipiDsiSetHsMode(icn9700->mipiHandle);
    return HDF_SUCCESS;
}

static int32_t Icn9700Off(struct PanelData *panel)
{
    int32_t ret;
    struct Icn9700Dev *icn9700 = NULL;

    icn9700 = PanelToIcn9700Dev(panel);
    if (icn9700 == NULL) {
        HDF_LOGE("%s: icn9700 is null", __func__);
        return HDF_FAILURE;
    }
    if (icn9700->mipiHandle == NULL) {
        HDF_LOGE("%s: mipiHandle is null", __func__);
        return HDF_FAILURE;
    }
    /* send mipi init code */
    int32_t count = sizeof(g_offCmd) / sizeof(g_offCmd[0]);
    int32_t i;
    /* set mipi to lp mode */
    MipiDsiSetLpMode(icn9700->mipiHandle);
    for (i = 0; i < count; i++) {
        ret = MipiDsiTx(icn9700->mipiHandle, &(g_offCmd[i]));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: MipiDsiTx failed", __func__);
            return HDF_FAILURE;
        }
    }
    /* lcd reset power off */
    ret = LcdResetOff(icn9700);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LcdResetOff failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t Icn9700EsdCheckFunc(struct PanelData *panel)
{
    struct Icn9700Dev *icn9700 = NULL;

    icn9700 = PanelToIcn9700Dev(panel);
    if (icn9700 == NULL) {
        HDF_LOGE("%s: icn9700 is null", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: enter", __func__);
    return HDF_SUCCESS;
}

#define OUTPUT_USER 0 /* output timing type */
static struct PanelInfo g_panelInfo = {
    .width = WIDTH1,                    /* width */
    .height = HEIGHT1,                  /* height */
    .hbp = HORIZONTAL_BACK_PORCH1,      /* horizontal back porch */
    .hfp = HORIZONTAL_FRONT_PORCH1,     /* horizontal front porch */
    .hsw = HORIZONTAL_SYNC_WIDTH1,      /* horizontal sync width */
    .vbp = VERTICAL_BACK_PORCH1,        /* vertical back porch */
    .vfp = VERTICAL_FRONT_PORCH1,       /* vertical front porch */
    .vsw = VERTICAL_SYNC_WIDTH1,        /* vertical sync width */
    .frameRate = FRAME_RATE1,           /* frame rate */
    .intfType = MIPI_DSI,               /* panel interface type */
    .intfSync = OUTPUT_USER,
    /* mipi config info */
    .mipi = { DSI_2_LANES, DSI_VIDEO_MODE, VIDEO_BURST_MODE, FORMAT_RGB_24_BIT },
    /* backlight config info */
    .blk = { BLK_PWM, MIN_LEVEL, MAX_LEVEL, DEFAULT_LEVEL },
    .pwm = { BLK_PWM1, PWM_MAX_PERIOD },
};

static struct PanelEsd g_panelEsd = {
    .support = false,
    .checkFunc = Icn9700EsdCheckFunc,
};

static void Icn9700PanelInit(struct PanelData *panel)
{
    panel->info = &g_panelInfo;
    panel->esd = &g_panelEsd;
    panel->init = Icn9700Init;
    panel->on = Icn9700On;
    panel->off = Icn9700Off;
}

static int32_t Icn9700EntryInit(struct HdfDeviceObject *object)
{
    struct Icn9700Dev *icn9700 = NULL;

    if (object == NULL) {
        HDF_LOGE("%s: object is null", __func__);
        return HDF_FAILURE;
    }
    icn9700 = (struct Icn9700Dev *)OsalMemCalloc(sizeof(struct Icn9700Dev));
    if (icn9700 == NULL) {
        HDF_LOGE("%s icn9700 malloc fail", __func__);
        return HDF_FAILURE;
    }
    Icn9700PanelInit(&icn9700->panel);
    icn9700->panel.object = object;
    icn9700->reset_gpio = RESET_GPIO;
    icn9700->reset_delay = 20; // delay 20ms
    object->priv = (void *)icn9700;
    icn9700->panel.blDev = GetBacklightDev("hdf_pwm");
    if (icn9700->panel.blDev == NULL) {
        HDF_LOGE("%s GetBacklightDev fail", __func__);
        return HDF_FAILURE;
    }
    if (RegisterPanel(&icn9700->panel) != HDF_SUCCESS) {
        HDF_LOGE("%s: RegisterPanel failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: exit succ", __func__);
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_icn9700DevEntry = {
    .moduleVersion = 1,
    .moduleName = "LCD_ICN9700",
    .Init = Icn9700EntryInit,
};

HDF_INIT(g_icn9700DevEntry);
