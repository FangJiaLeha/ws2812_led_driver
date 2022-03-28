/**
 * @file led_frame.h
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _LED_FRAME_H
#define _LED_FRAME_H

#include "common.h"

//==============================================================================
/**
 * @brief 灯条对象结构体声明
 *
 * @note  通用灯条灯效只提供 设置颜色 常亮 常灭方法
 */
typedef struct led_bar
{
    uint8_t id;

    RtvStatus (*set_color)(struct led_bar *bar, float *color);
    RtvStatus (*on)(struct led_bar *bar, float *color);
    RtvStatus (*off)(struct led_bar *bar);
    void *led_bar_dev_addr;
} led_bar_type;
typedef led_bar_type *led_bar_t;

//==============================================================================
/**
 * @brief 对外提供一个通用的灯条对象初始化方法
 *
 * @param bar           灯条bar对象地址
 * @param id            灯条bar的id
 * @param set_color     灯条bar的设置颜色方法
 * @param priv_data     灯条bar的私有数据
 * @return RtvStatus   @SUCCESS:初始化灯条成功 @其他值:初始化灯条失败
 */
RtvStatus init_led_bar(led_bar_t bar,
                        uint8_t id,
                        RtvStatus (*set_color)(led_bar_t bar, float *color),
                        void *priv_data);
#endif
