/**
 * @file test.h
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _TEST_H
#define _TEST_H

#include "common.h"
#include "led_bar.h"
#include "task_sch.h"
#include "ws2812_led_bar.h"

#define _PWM_TEST 0x00

#define _WS2812_DRV_TSET 0x00

#define _LED_BAR_TEST 0x00
#if defined(_LED_BAR_TEST) && (_LED_BAR_TEST == 0x01)
#define _LED_OFF_TEST 0x00
#define _LED_ON_TEST 0x00
#define _LED_RGB_TEST 0x00
#define _LED_BLINK_TEST 0x01
#define _LEFT_BLINK_TEST 0x00
#define _LED_WATER_TEST 0x00
#define _LEFT_WATER_TEST 0x00
#define _LED_BREATH_TEST 0x00
#endif

#define _TASH_SCH_TEST 0x01

void test_func_enter(void);

#endif
