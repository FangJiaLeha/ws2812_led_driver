#ifndef _LED_BAR_H
#define _LED_BAR_H

#include "common.h"
#include "drv_i2c.h"
#include "ws2812.h"
#include "ws2812_led_bar.h"
#include "led_frame.h"
#include "task_sch.h"

#define LED_BAR_INDEX               0x01u

enum led_bar_cmd_mode
{
    BASE_CMD = 0x90
};

enum led_bar_ctl_mode
{
    LED_OFF = 0x00,     // 常灭模式
    LED_ON,             // 常亮模式
    LED_RGB,            // RGB模式
    LED_WATER,          // 流水灯模式
    LED_BLINK,          // 闪烁模式
    LED_BREATH          // 呼吸灯模式
};

/******************************************************************************/
#define BAR_REQ_LEN_CHECK(_len)                         \
do {                                                    \
    if (_len < 9) {                                     \
        goto set_error;                                 \
    }                                                   \
} while(0)

#define BAR_CMD_MODE_CHECK(_mode)                       \
do {                                                    \
    if (_mode != BASE_CMD) {                            \
        goto set_error;                                 \
    }                                                   \
} while(0)

#define BAR_CMD_ID_CHECK(_id, _id_num)                  \
do {                                                    \
    if (_id == 0 || _id > _id_num) {                    \
        goto set_error;                                 \
    }                                                   \
} while(0)

#define BAR_CMD_CTL_MODE_CHECK(_ctrl_mode)              \
do {                                                    \
    if (_ctrl_mode != LED_OFF &&                        \
        _ctrl_mode != LED_ON &&                         \
        _ctrl_mode != LED_RGB &&                        \
        _ctrl_mode != LED_BLINK &&                      \
        _ctrl_mode != LED_WATER &&                      \
        _ctrl_mode != LED_BREATH) {                     \
        goto set_error;                                 \
    }                                                   \
} while(0)

#define BAR_CMD_XOR_CHECK(a, b)                         \
do {                                                    \
    if (a != b) {                                       \
        goto set_error;                                 \
    }                                                   \
} while(0)

/******************************************************************************/
Rtv_Status init_led_bars(uint8_t led_bar_index);
void data_analysis_task(void);
#endif
