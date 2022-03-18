/**
 * @file ws2812.h
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _WS2812_H
#define _WS2812_H

#include "common.h"
#include "drv_pwm.h"

/******************************************************************************/
typedef struct ws2812_dev_attr
{
    uint32_t led_num;
    uint32_t ctrl_led_num;
    uint16_t *dma_buff;
    uint8_t (*render_buff)[3];
    uint8_t index;          // 灯条编号
    uint8_t index_enable;   // 灯条输出使能
    uint8_t render_loop:1;  // 用于渲染模式下 控制使能下一次通道输出
    void *private;
}WS2812DevAttrType;

typedef struct ws2812_dev
{
    WS2812DevAttrType dev_attr;
    struct dev_ops ws2812_dev_ops;
}WS2812DevType;
typedef WS2812DevType* WS2812DevType_t;

typedef struct ws2812_bar_ctrlpack
{
    uint8_t color[3];
    uint8_t start;
    uint8_t count;
}WS2812BarCtrlPackType;
typedef WS2812BarCtrlPackType* WS2812BarCtrlPackType_t;

typedef enum ws2812_ctrl_cmd
{
    WS2812_CTRL_INIT = 0x01,
    WS2812_CTRL_GET_DISBUFF,
    WS2812_CTRL_UPDATE_DEVDATA,
    WS2812_CTRL_BAR_COLOR,
    WS2812_LED_NUM_RESET,
}WS2812CtrlCmdType;

#define WS2812_RETAIN_LED_NUM           (46)
#define WS2812_LED_DEFAULT_NUM          (14)

/******************************************************************************/
/**
 * @brief 提供对外获取ws2812设备地址接口
 *
 * @return ws2812_dev_t 返回ws2812设备地址
 */
WS2812DevType_t find_ws2812_dev(void);

#endif
