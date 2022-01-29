#include "board.h"
#include "systick.h"
#include "drv_i2c.h"
#include "led_bar.h"

void init_board(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    systick_config();

    init_i2c(SET_I2C0_ADDR, SET_I2C0_BUFF_SIZE);

    init_led_bars(LED_BAR_INDEX);
}
