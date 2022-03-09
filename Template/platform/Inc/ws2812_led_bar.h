#ifndef _WS2812_LED_BAR_H
#define _WS2812_LED_BAR_H

#include <stdint.h>
#include "common.h"
#include "led_frame.h"

struct ws2812_render_param{
    uint32_t show_pos;              // 显示灯条起始位置；
    uint8_t render_animation;       // 需要渲染的动画
    uint8_t render_color1[3];       // 渲染起始颜色颜色
    uint8_t render_color2[3];       // 用于渐变流水灯/呼吸模式时 记录终点颜色值
    uint8_t color[3];               // 用于在呼吸模式下 操作颜色分量
    uint8_t render_light_leds;      // 渲染显示的灯珠数量
    uint8_t breath_singal_period;   // 单次呼吸周期
    uint16_t breath_timers;         // 记录呼吸次数 呼吸总周期/单次呼吸周期
    uint16_t breath_cnt;            // 呼吸次数计数变量
    float rgb_step[3];            // 记录rgb三色值步进量
    uint8_t blink_flag:1;           // 用于标记闪烁模式时 前后两次状态
    uint8_t breath_state:1;         // 记录呼吸模式下 呼吸状态 0:呼 1:吸
};

struct ws2812_bar
{
    struct led_bar parent;
    uint32_t led_num;               // led灯总数
    uint32_t ctrl_led_num;          // 控制的led数量
    uint32_t start;
    uint8_t render_switch;          // ws2812渲染引擎开关状态：0 停止ws2812私有渲染引擎; 1 开启ws2812私有渲染引擎;
    struct ws2812_render_param render_param; // ws2812 渲染引擎渲染参数

    uint8_t (*dis_buff)[3];
    Rtv_Status (*_parent_off)(led_bar_t bar);
};
typedef struct ws2812_bar* ws2812_bar_t;

enum ws2812_led_ctrl_mode
{
    BASE_WATER_LEFT = 0x01,
    BASE_WATER_RIGHT,
    CHANGE_WATER_LEFT,
    CHANGE_WATER_RIGHT,
    BLINK_LEFT,
    BLINK_RIGHT,
    BREATH
};

#define BREATH_SINGAL_PERIOD    20      // 设置呼吸模式下 灯条单次呼吸周期 ms

#define BASE_WATER_MODE_CHECK(_water_mode)                                      \
do {                                                                            \
    if (_water_mode != BASE_WATER_LEFT && _water_mode != BASE_WATER_RIGHT) {    \
        goto set_error;                                                         \
    }                                                                           \
} while(0)

#define CHANGE_WATER_MODE_CHECK(_water_mode)                                        \
do {                                                                                \
    if (_water_mode != CHANGE_WATER_LEFT && _water_mode != CHANGE_WATER_RIGHT) {    \
        goto set_error;                                                             \
    }                                                                               \
} while(0)

#define BLINK_MODE_CHECK(_blink_mode)                                          \
do {                                                                           \
    if (_blink_mode != BLINK_LEFT && _blink_mode != BLINK_RIGHT) {             \
        goto set_error;                                                        \
    }                                                                          \
} while(0)

Rtv_Status init_ws2812_bar(ws2812_bar_t wbar, uint8_t id,
                           Rtv_Status (*set_color)(led_bar_t bar, float *color ),
                    void * priv_data, uint32_t led_num, uint32_t start);
void ws2812_render(void);

#endif
