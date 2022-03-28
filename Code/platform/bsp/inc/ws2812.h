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
    uint8_t index;           // 灯条编号
    uint8_t index_enable;    // 灯条输出使能
    uint8_t render_loop : 1; // 用于渲染模式下 控制使能下一次通道输出
    void *pwm_dev_addr;
} WS2812DevAttrType;

typedef struct ws2812_dev
{
    WS2812DevAttrType dev_attr;
    struct dev_ops ws2812_dev_ops;
} ws2812_dev_type;
typedef ws2812_dev_type *ws2812_dev_t;

typedef struct
{
    uint8_t color[3];
    uint8_t start;
    uint8_t count;
} ws2812_bar_ctrlpack_type;
typedef ws2812_bar_ctrlpack_type *ws2812_bar_ctrlpack_t;

typedef enum ws2812_ctrl_cmd
{
    WS2812_CTRL_INIT = 0x01,
    WS2812_CTRL_GET_DISBUFF,
    WS2812_CTRL_UPDATE_DEVDATA,
    WS2812_CTRL_BAR_COLOR,
    WS2812_LED_NUM_RESET,
} WS2812CtrlCmdType;

#define WS2812_RETAIN_LED_NUM (46)
#define WS2812_LED_DEFAULT_NUM (14)

/******************************************************************************/
/**
 * @brief 提供对外获取ws2812设备地址接口
 *
 * @return ws2812_dev_t 返回ws2812设备地址
 *
 * @note   通过该接口可使用内置的init()/control()方法
 *         方法说明:
 *         init(void *dev): 可传入WS2812灯条设备地址 对该灯条进行初始化
 *         control(void *dev, const uint8_t cmd, void *arg)：输入相关WS2812灯条设备控制命令即可控制该设备
 *         @cmd WS2812_CTRL_INIT(初始化灯条最大灯珠数命令) arg(对应的灯珠数)
 *              WS2812_CTRL_BAR_COLOR(灯条常规颜色包设置命令) arg(对应设置的颜色包地址)
 *              WS2812_CTRL_UPDATE_DEVDATA(灯条数据更细命令) arg(无效值)
 *              WS2812_CTRL_GET_DISBUFF(获取灯条设备显存) arg(操作指针地址)
 *              WS2812_LED_NUM_RESET(重置灯条最大灯珠数命令) arg(对应的灯珠数)
 */
ws2812_dev_t find_ws2812_dev(void);

#endif
