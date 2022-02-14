#include "board.h"
#include "drv_i2c.h"

static void cpu_soft_wait(void)
{
    uint16_t i, j;
    for (i = 0; i < 100; i++) {
        for (j = 0; j < 100; j++) {
            ;
        }
    }
}

void init_board(void)
{
    // iap 相关设置
    __enable_irq();
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    // cpu soft reset delay
    cpu_soft_wait();

    // init i2c
    init_i2c(SET_I2C0_ADDR, SET_I2C0_BUFF_SIZE);
}
