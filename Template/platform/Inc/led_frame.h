#ifndef _LED_FRAME_H
#define _LED_FRAME_H

#include <stdint.h>
#include <string.h>
#include "common.h"

enum ledbar_ctrl_mode
{
    LEDBAR_CTRL_MODE_OFF = 0x00,
    LEDBAR_CTRL_MODE_CONST_ON,
    LEDBAR_CTRL_MODE_BLINK_OFF,
    LEDBAR_CTRL_MODE_BLINK_ON,
    LEDBAR_CTRL_MODE_BREATHING,
    LEDBAR_CTRL_MODE_SECTRL
};

struct render_param
{
    uint8_t ctrl_mode;
    float color1[3];
    float color2[3];
    float breath_step[3];
    uint16_t intval;
    uint16_t times;
    uint16_t loop_delay;  //!< 渲染引擎渲染间隔时间，单位10ms
};

struct render_state
{
    uint8_t action_done;
    uint16_t looped;      //!< 已循环的次数或运行的次数
};

struct led_bar{
    uint8_t id;

    struct render_param param;
    struct render_state state;

    Rtv_Status (*set_color)(struct led_bar *bar, float *color);
    Rtv_Status (*on)(struct led_bar *bar, float *color);
    Rtv_Status (*off)(struct led_bar *bar );
    Rtv_Status (*blink)(struct led_bar *bar, uint8_t mode, uint8_t blink_led_num);
    Rtv_Status (*water)(struct led_bar *bar, uint8_t mode, uint8_t single_led_num, uint8_t move_period);
    Rtv_Status (*breath)(struct led_bar *bar, uint8_t breath_period);
    void *private;
};
typedef struct led_bar* led_bar_t;

Rtv_Status init_led_bar(led_bar_t bar,
                        uint8_t id,
                        Rtv_Status (*set_color)(led_bar_t bar, float *color), void *priv_data);
#endif
