#include "board.h"
#include "systick.h"
#include "drv_i2c.h"
#include "led_bar.h"
#include "iap_config.h"

void init_board(void)
{
    /* IAP 相关设置 */
    __enable_irq();
    SCB->VTOR = FLASH_APP_ADDR;

    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    systick_config();

    init_i2c(SET_I2C0_ADDR, SET_I2C0_BUFF_SIZE);

    init_led_bars(LED_BAR_INDEX);
}
