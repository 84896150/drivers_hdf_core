#ifndef TOUCH_FT5x06_H
#define TOUCH_FT5x06_H

#define WORK_REGISTER_REPORT_RATE	0x08

#define TOUCH_EVENT_DOWN		0x00
#define TOUCH_EVENT_UP			0x01
#define TOUCH_EVENT_ON			0x02
#define TOUCH_EVENT_RESERVED		0x03

#define EDT_NAME_LEN			23

#define EDT_M06        0
#define EDT_M09        1
#define EDT_M12        2
#define EV_FT          3
#define GENERIC_FT     4

#define POINT_BUFFER_LEN_M06     26
#define POINT_BUFFER_LEN_M09     33

#define MAX_POINT 5

#define GT_FINGER_NUM_MASK    0x0F

#endif