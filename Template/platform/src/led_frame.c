#include "led_frame.h"
/******************************************************************************/
static Rtv_Status on( led_bar_t bar, float *color )
{
    Rtv_Status ret;
    bar->param.ctrl_mode = LEDBAR_CTRL_MODE_CONST_ON;
    bar->param.color1[0] = color[0];
    bar->param.color1[1] = color[1];
    bar->param.color1[2] = color[2];
    ret = bar->set_color( bar, color );
    return ret;
}

static Rtv_Status off( led_bar_t bar )
{
    Rtv_Status ret;
    bar->param.ctrl_mode = LEDBAR_CTRL_MODE_OFF;
    bar->param.color1[0] = 0;
    bar->param.color1[1] = 0;
    bar->param.color1[2] = 0;
    ret =  bar->set_color( bar, bar->param.color1);
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
static Rtv_Status breath(struct led_bar *bar, uint8_t s_color_index, 
                         uint8_t e_color_index, uint8_t breath_period)
{
    return SUCCESS;
}

/******************************************************************************/
Rtv_Status init_led_bar(led_bar_t bar,
                        uint8_t id,
                        Rtv_Status (*set_color)(led_bar_t bar, float *color), void *priv_data)
{
    bar->id = id;
    bar->set_color = set_color; // callback the tlc59108_set_color() func in led_bar.c
    bar->on = on;
    bar->off = off;
    bar->blink = blink;
    bar->water = water;
    bar->breath = breath;
    bar->private = priv_data;

    memset(&bar->param, 0, sizeof( bar->param) + sizeof( bar->state ) );

    return SUCCESS;
}
