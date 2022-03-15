/**
 * @file led_frame.c
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led_frame.h"

/******************************************************************************/
static Rtv_Status on(LedBarType_t bar, float *color)
{
    Rtv_Status ret;
    ret = bar->set_color(bar, color);
    return ret;
}

static Rtv_Status off(LedBarType_t bar)
{
    Rtv_Status ret;
    ret =  bar->set_color( bar, (float []){0, 0, 0});
    return ret;
}

static Rtv_Status blink(struct led_bar *bar, uint8_t mode, uint8_t blink_led_num)
{
    return SUCCESS;
}
static Rtv_Status water(struct led_bar *bar, uint8_t mode,
                        uint8_t single_led_num, uint8_t move_period)
{
    return SUCCESS;
}
static Rtv_Status breath(struct led_bar *bar, uint16_t breath_period)
{
    return SUCCESS;
}

/******************************************************************************/
Rtv_Status init_led_bar(LedBarType_t bar,
                        uint8_t id,
                        Rtv_Status (*set_color)(LedBarType_t bar, float *color),
                        void *priv_data)
{
    bar->id = id;
    bar->set_color = set_color; // 注册回调函数
    bar->on = on;
    bar->off = off;
    bar->blink = blink;
    bar->water = water;
    bar->breath = breath;
    bar->private = priv_data;

    return SUCCESS;
}
