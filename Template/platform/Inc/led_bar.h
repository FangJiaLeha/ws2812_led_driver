#ifndef _LED_BAR_H
#define _LED_BAR_H

#include "common.h"
#include "drv_i2c.h"
#include "ws2812.h"
#include "ws2812_led_bar.h"
#include "led_frame.h"
#include "task_sch.h"
#include "iap_config.h"

#define LED_BAR_INDEX               0x01u

enum led_bar_cmd_mode
{
    IAP_CMD = 0x16,
    BASE_CMD = 0x90
};

enum led_bar_base_ctrl_mode
{
    LED_OFF = 0x00,     // 常灭模式
    LED_ON,             // 常亮模式
    LED_RGB,            // RGB模式
    LED_WATER,          // 流水灯模式
    LED_BLINK,          // 闪烁模式
    LED_BREATH          // 呼吸灯模式
};

enum led_bar_iap_ctrl_mode
{
    SOFT_RESET = 0x01,  // 软复位
    CHECK_WORK_MODE,    // 查询工作在BOOT/APP模式(1/0)
    GET_SOFT_VERSION,   // 获取软件版本
};
/******************************************************************************/
#define BAR_REQ_LEN_CHECK(_len)                         \
do {                                                    \
    if (_len < 9) {                                     \
        goto set_error;                                 \
    }                                                   \
} while(0)

#define BAR_CMD_MODE_CHECK(_mode)                       \
do {                                                    \
    if (_mode != BASE_CMD &&                            \
        _mode != IAP_CMD ) {                          \
        goto set_error;                                 \
    }                                                   \
} while(0)

#define BAR_CMD_ID_CHECK(_id, _id_num)                  \
do {                                                    \
    if (_id == 0 || _id > _id_num) {                    \
        goto set_error;                                 \
    }                                                   \
} while(0)

#define BAR_CMD_CTL_MODE_CHECK(_ctrl_mode)              \
do {                                                    \
    if (_ctrl_mode != LED_OFF &&                        \
        _ctrl_mode != LED_ON &&                         \
        _ctrl_mode != LED_RGB &&                        \
        _ctrl_mode != LED_BLINK &&                      \
        _ctrl_mode != LED_WATER &&                      \
        _ctrl_mode != LED_BREATH) {                     \
        goto set_error;                                 \
    }                                                   \
} while(0)

#define BAR_CMD_XOR_CHECK(a, b)                         \
do {                                                    \
    if (a != b) {                                       \
        goto set_error;                                 \
    }                                                   \
} while(0)


/**
 * @brief      生成程序版本号
 *
 * @param      major   主版本号
 * @param      minor   次版本号
 * @param      suffix  修订版本号
 *
 * @return     返回生成的版本号数据类型为uint32
 */
#define MK_PROGRAM_VERSION( major, minor, suffix )			\
( ( ( (major) & 0xff ) << 16 ) |  ( ( (minor) & 0xff ) << 8 ) | ( (suffix) & 0xff) )

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
 */
#define PROGRAM_VERSION			MK_PROGRAM_VERSION(1, 0, 0)

#define MCU_WORK_IN_APP_MODE    0x00

/******************************************************************************/
Rtv_Status init_led_bars(uint8_t led_bar_index);
void data_analysis_task(void);
#endif
