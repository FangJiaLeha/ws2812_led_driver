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
 */
typedef struct led_bar{
    uint8_t id;

    Rtv_Status (*set_color)(struct led_bar *bar, float *color);
    Rtv_Status (*on)(struct led_bar *bar, float *color);
    Rtv_Status (*off)(struct led_bar *bar );
    Rtv_Status (*blink)(struct led_bar *bar,
                        const uint8_t mode,
                        const uint8_t blink_led_num,
                        const uint8_t blink_start_pos);
    Rtv_Status (*water)(struct led_bar *bar,
                        const uint8_t mode,
                        const uint8_t single_led_num,
                        const uint8_t water_start_pos);
    Rtv_Status (*breath)(struct led_bar *bar,
                         const uint16_t breath_period);
    void *private;
}LedBarType;
typedef LedBarType* LedBarType_t;

//==============================================================================
/**
 * @brief 对外提供一个通用的灯条对象初始化方法
 *
 * @param bar           灯条bar对象地址
 * @param id            灯条bar的id
 * @param set_color     灯条bar的设置颜色方法
 * @param priv_data     灯条bar的私有数据
 * @return Rtv_Status   @SUCCESS:初始化灯条成功 @其他值:初始化灯条失败
 */
Rtv_Status init_led_bar(LedBarType_t bar,
                        uint8_t id,
                        Rtv_Status (*set_color)(LedBarType_t bar, float *color),
                        void *priv_data);
#endif
