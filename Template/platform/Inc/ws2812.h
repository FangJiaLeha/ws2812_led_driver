#ifndef _WS2812_H
#define _WS2812_H

#include "common.h"
#include "drv_pwm.h"

struct ws2812_dev_attr
{
    uint32_t led_num;
    uint16_t *dma_buff;
    uint8_t (*render_buff)[3];
    uint8_t need_update;
    uint8_t index;
};

struct ws2812_dev
{
    struct ws2812_dev_attr dev_attr;
    struct dev_ops ws2812_dev_ops;
};
typedef struct ws2812_dev* ws2812_dev_t;

struct ws2812_bar_ctrlpack
{
    uint8_t color[3];
    uint8_t start;
    uint8_t count;
};

enum ws2812_cmd_list
{
    WS2812_CTRL_INIT = 0x01,
    WS2812_CTRL_GET_DISBUFF,
    WS2812_CTRL_UPDATE_DEVDATA,
    WS2812_CTRL_BAR_COLOR
};

#define WS2812_LED_NUM          (14)

ws2812_dev_t find_ws2812_dev(void);

#endif
