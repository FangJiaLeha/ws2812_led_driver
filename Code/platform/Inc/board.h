/**
 * @file board.h
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-22
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _BOARD_H
#define _BOARD_H

#include "common.h"
#include "systick.h"
#include "drv_i2c.h"
#include "led_bar.h"
#include "register_info.h"
#include "ws2812_led_bar.h"
#include "drv_wdgt.h"
#include "test.h"

//==============================================================================
/* i2c0 addr and buff size config */
#define SET_I2C0_ADDR (0x98)

//==============================================================================
/**
 * @brief 初始化板极底层和应用层相关资源
 *
 * @note  此接口会初始化看门狗/PWM I2C设备/灯条参数等
 */
void init_board(void);

#endif
