/**
 * @file tlc59108_led_bar.h
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-16
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _TLC59108_LED_BAR_H
#define _TLC59108_LED_BAR_H

#include "common.h"
#include "tlc59108.h"
#include "led_frame.h"

//==============================================================================
/**
 * @brief TLC59108灯条的渲染参数结构体声明
 *
 */
typedef struct
{
    uint8_t *pwmx_blink_period;  // 闪烁周期
    uint8_t *pwmx_blink_duty;    // 闪烁占空比
    uint8_t group_duty_ctrl_reg; // 组占空比寄存器
    uint8_t group_freq_ctrl_reg; // 组频率寄存器
    uint8_t *led_ldrx;           // 通道引脚输出状态
} TLC59108RenderParamType;

/**
 * @brief TLC59108灯条对象结构体声明
 *
 */
typedef struct tlc59108_bar
{
    struct led_bar parent;
    uint8_t render_switch;                // tlc59108渲染引擎开关状态：0 停止私有渲染引擎; 1 开启私有渲染引擎;
    uint8_t *pwmx_databuff;               // 8个通道的pwmx寄存器值
    uint8_t channel_num;                  // 通道数
    TLC59108RenderParamType render_param; // tlc59108渲染引擎渲染参数
    // tlc59108灯条的blink方法
    RtvStatus (*blink)(void *bar,
                        const uint16_t led_out_state,
                        const uint8_t *pwmx_value);
    // tlc59108灯条的dimming方法
    RtvStatus (*dimming)(void *bar,
                          const uint8_t *pwmx_value);
} tlc59108_bar_type;
typedef tlc59108_bar_type *tlc59108_bar_type_t;

//==============================================================================
/**
 * @brief 对外提供一个初始化TLC59108灯条对象的接口
 *
 * @param tbar          TLC59108灯条对象地址
 * @param id            TLC59108灯条ID
 * @param set_color     TLC59108灯条设置颜色回调函数
 * @param priv_data     TLC59108灯条对象的私有数据 用来保存TLC59108设备地址
 * @param channel_num   TLC59108灯条初始化通道数
 * @return RtvStatus   @SUCCESS:初始化TLC59108灯条成功 @其他值:初始化TLC59108灯条失败
 */
RtvStatus init_tlc59108_bar(tlc59108_bar_type_t tbar,
                             uint8_t id,
                             RtvStatus (*set_color)(led_bar_t bar, float *color),
                             void *priv_data,
                             const uint8_t channel_num);

/**
 * @brief 对外提供一个TLC59108灯条渲染接口
 *
 * @note  该接口主要用来处理TLC59108灯条的blinking模式
 *
 */
void tlc59108_render(void);

#endif
