#ifndef _BOARD_H
#define _BOARD_H

#include "common.h"

/* i2c0 addr and buff size config */
#define SET_I2C0_ADDR               (0x41)
#define SET_I2C0_BUFF_SIZE          (16)

void init_board(void);

#endif
