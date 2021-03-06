/**
 * @file led_bar.h
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _LED_BAR_H
#define _LED_BAR_H

#include "common.h"
#include "drv_i2c.h"
#include "ws2812.h"
#include "ws2812_led_bar.h"
#include "led_frame.h"
#include "task_sch.h"
#include "register_info.h"
#include "tlc59108.h"
#include "tlc59108_led_bar.h"

/**
 * @brief WS2812灯条默认序号(第一条灯带)
 *
 */
#define WS2812_BAR_DEFAULT_INDEX PWM_CHX_INDEX_INIT_SET
/**
 * @brief WS2812灯条最大数
 *
 */
#define WS2812_BAR_MAX_NUM (PWM_CHX_NUM_SET)
/**
 * @brief TLC59108灯条默认数量(一条灯带)
 *
 */
#define TLC59108_BAR_CHANNEL_NUM (TLC59108_CHANNNEL_MAX_NUM)

enum ws2812_bar_ctrl_mode
{
    WS2812_LED_OFF = 0x01,     // 熄灭模式
    WS2812_LED_ON,             // 常亮模式
    WS2812_LED_BLINK,          // 分段闪烁模式
    WS2812_LED_BASE_WATER,     // 基础流水灯模式
    WS2812_LED_CHANGE_WATER,   // 渐变流水灯模式
    WS2812_LED_INCREASE_WATER, // 递增式流水灯模式
    WS2812_LED_SECTOR_WATER,   // 分段式流水灯模式
    WS2812_LED_BREATH,         // 呼吸灯模式
};

enum tlc59108_bar_ctrl_mode
{
    TLC59108_LED_DIMMING, // dimmming模式
    TLC59108_LED_BLINK    // blink模式
};

enum driver_recv_len
{
    /**
     * @brief 控制TCL59108灯条需写入的字节数
     *
     */
    TLC59108_RECV_LEN = 18 + 1,

    /**
     * @brief 控制WS2812灯条需写入的字节数
     *
     */
    WS2812_RECV_LEN = 15 + 1
};
/******************************************************************************/
/**
 * @brief 校验
 *
 */
#define XOR_CHECK(value1, value2) \
    do                            \
    {                             \
        if (value1 != value2)     \
        {                         \
            goto set_error;       \
        }                         \
    } while (0)
/**
 * @brief 灯效控制参数有效性判断
 *
 */
#define LED_BAR_CTRL_PARA_CHECK(req, req_len) \
    do                                        \
    {                                         \
        if (req == NULL || req_len == 0)      \
        {                                     \
            goto set_error;                   \
        }                                     \
    } while (0)
#define BAR_REQ_LEN_CHECK(driver_type, recv_len)                           \
    do                                                                     \
    {                                                                      \
        if ((driver_type == WS2812DEV && recv_len != WS2812_RECV_LEN) ||   \
            (driver_type == TLC59108DEV && recv_len != TLC59108_RECV_LEN)) \
        {                                                                  \
            goto set_error;                                                \
        }                                                                  \
    } while (0)

/**
 * @brief WS2812灯驱控制参数有效性判断
 *
 * @note  第五个通道不支持WS2812灯条控制 需要滤掉
 */
#define WS2812_DRIVER_CHANNEL_CHECK(channel_index) \
    do                                             \
    {                                              \
        if (channel_index == 0 ||                  \
            channel_index > WS2812_BAR_MAX_NUM ||  \
            channel_index == 0x05)                 \
        {                                          \
            goto set_error;                        \
        }                                          \
    } while (0)
/**
 * @brief WS2812灯条可控制总数参数有效性判断
 *
 */
#define WS2812_CTRL_LED_ALL_CHECK(ctrl_led_all_num)                            \
    do                                                                         \
    {                                                                          \
        if (ctrl_led_all_num == 0 ||                                           \
            ctrl_led_all_num > WS2812_LED_DEFAULT_NUM + WS2812_RETAIN_LED_NUM) \
        {                                                                      \
            goto set_error;                                                    \
        }                                                                      \
    } while (0)

/**
 * @brief TLC59108灯驱控制参数有效性判断
 *
 */
/******************************************************************************/
/**
 * @brief      生成程序版本号
 *
 * @param      major   主版本号
 * @param      minor   次版本号
 * @param      suffix  修订版本号
 *
 * @return     返回生成的版本号数据类型为uint32
 */
#define MK_PROGRAM_VERSION(major, minor, suffix) \
    ((((major)&0xff) << 16) | (((minor)&0xff) << 8) | ((suffix)&0xff))

/**
 * 当前程序版本
 * 版本编码规则参考@ref
 * @brief
 *  V1.0.0  the first version
 *  V2.0.0  Add the iap analysis
 *  V2.1.0  Modify the flash and sram partition and test ok
 *  V2.1.1  Fix the led can't blink bug
 *  V2.2.0  Adapt the head board and modify the pwm output pin to PA0
 *  实时语义化版本:    // 2022.02.18
 *  V0.0.1  the first version and add the iap analysis
 *  V0.0.2  Modify the flash and sram partition and test ok
 *  V0.0.3  Fix the led can't blink bug
 *  V0.1.0  Adapt the head board and modify the pwm output pin to PA0
 *  V1.0.0  the first release version
 *  V1.0.1  Fix the i2c overshoot bug that reduce the gpio speed
 *  V1.0.3  Modify the ws2812 high level time to 780ns base on B09 and new head led board
 *  V1.1.0  Add the new led effect
 *  V1.2.0  Modify firmware to universal module driver
 *  V1.2.1  Add the interface comment
 *  V1.2.2  Add the wdgt but no test
 *  V1.2.3  Add the tlc59108 driver but no test
 *  V1.2.4  Optimization logic and add the RELEASE macro for test
 *  V1.2.5  Add the new UI needs and modify ws2812_led_bar driver
 *  V1.2.6  Adapt the sector water mode、fix the increase water mode bug and test OK
 *  V1.2.7  After firmware review version
 *  V1.2.8  Function selftest, then fix the sector water bug
 *  V1.2.9  Modify the test.c/test.h and support regist test task
 *  V1.3.0  Forget comment B09 test macro
 *  V1.3.1  Modify the i2c addr to fix screen bug(i2c addr confilct)
 *  V1.3.2  Adapt the tlc59108 addr of i2c
 */
#define PROGRAM_VERSION MK_PROGRAM_VERSION(1, 3, 2)

/******************************************************************************/
/**
 * @brief 灯条默认参数初始化接口
 *
 * @param ws2812_led_num            WS2812灯条默认灯珠数
 * @param tlc59108_channel_num      TLC59108默认初始化灯条通道数(3通道/1条灯带)
 * @return RtvStatus               返回值 @SUCCESS:初始化成功  @其他值:初始化失败
 */
RtvStatus init_led_bars(const uint8_t ws2812_led_num,
                         const uint8_t tlc59108_channel_num);

/**
 * @brief 数据解析任务接口
 *
 */
void data_analysis_task(void);
#endif
