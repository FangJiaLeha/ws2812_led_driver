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
static RtvStatus on(led_bar_t bar, float *color)
{
    RtvStatus ret = SUCCESS;
    if (bar->set_color == NULL)
    {
        return ERROR;
    }
    ret = bar->set_color(bar, color);
    return ret;
}

static RtvStatus off(led_bar_t bar)
{
    RtvStatus ret = SUCCESS;
    if (bar->set_color == NULL)
    {
        return ERROR;
    }
    ret = bar->set_color(bar, (float[]){0, 0, 0});
    return ret;
}

/******************************************************************************/
RtvStatus init_led_bar(led_bar_t bar,
                        uint8_t id,
                        RtvStatus (*set_color)(led_bar_t bar, float *color),
                        void *priv_data)
{
    bar->id = id;
    bar->set_color = set_color; // 注册回调函数
    bar->on = on;
    bar->off = off;
    bar->led_bar_dev_addr = priv_data;

    return SUCCESS;
}
