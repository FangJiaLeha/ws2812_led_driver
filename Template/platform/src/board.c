#include "board.h"

void init_board(void)
{
    uint8_t setI2CBuffLen = 0;
    /* IAP 相关设置 */
    __enable_irq();
    SCB->VTOR = FLASH_APP_ADDR;

    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    systick_config();

    // 调用初始化灯驱寄存器接口
    init_register();

    // 根据寄存器个数申请i2c的发送和接收buff
    control_register(GET_REG_NUM_INFO, 0, (void *)&setI2CBuffLen, 0);
    init_i2c(SET_I2C0_ADDR, setI2CBuffLen);

    init_led_bars(LED_BAR_INDEX);
}
