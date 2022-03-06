#ifndef _BOARD_H
#define _BOARD_H

#include "common.h"
#include "systick.h"
#include "drv_i2c.h"
#include "led_bar.h"
#include "iap_config.h"
#include "register_info.h"

/* i2c0 addr and buff size config */
#define SET_I2C0_ADDR               (0x80)

void init_board(void);

#endif
