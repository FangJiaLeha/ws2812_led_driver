#ifndef _DRV_I2C_H
#define _DRV_I2C_H

#include "gd32f3x0.h"

extern uint8_t i2c_rxbuffer[16];
extern uint16_t ReadLend;

void i2c_config(void);
uint16_t get_data_len(void);
void clear_data_len(void);
uint8_t *get_data_buff(void);

#endif

