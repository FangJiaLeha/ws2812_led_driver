#include "board.h"

void init_board(void)
{
    uint8_t setI2CBuffLen = 0;

    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    systick_config();

    // 初始化PWM设备(pwm通道配置)
    if (init_pwm_dev() != SUCCESS) {
        return;
    }

    // 调用初始化灯驱寄存器接口
    if (init_register() != SUCCESS) {
        return;
    }

    // 根据寄存器个数申请i2c的发送和接收buff
    control_register(GET_REG_NUM_INFO, 0, (void *)&setI2CBuffLen, 0);
    init_i2c(SET_I2C0_ADDR, setI2CBuffLen);

    // 灯条默认参数初始化
    init_led_bars(WS2812_BAR_DEFAULT_INDEX, TLC59108_BAR_DEFAULT_NUM);
}
