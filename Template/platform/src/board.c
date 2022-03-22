/**
 * @file board.c
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "board.h"

void init_board(void)
{
    uint8_t setI2CBuffLen = 0;

    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    systick_config();

    #if defined(RELEASE) && (RELEASE == 0x01u)
    // 初始化看门狗
    if (init_wdgt(WDGT_TIMEOUT_SET) != SUCCESS) {
        return;
    }
    #endif

    // 初始化PWM设备(pwm通道配置)
    if (init_pwm_dev() != SUCCESS) {
        return;
    }

    // 调用初始化灯驱寄存器接口
    if (init_register() != SUCCESS) {
        return;
    }

    // 根据灯驱寄存器个数 申请i2c的发送和接收buff
    control_register(GET_REG_NUM_INFO, 0, (void *)&setI2CBuffLen, 0);
    if (init_i2c(SET_I2C0_ADDR, setI2CBuffLen) != SUCCESS) {
        return;
    }

    // 灯条默认参数初始化
    if (init_led_bars(WS2812_LED_DEFAULT_NUM, TLC59108_BAR_CHANNEL_NUM) != SUCCESS) {
        return;
    }
}
