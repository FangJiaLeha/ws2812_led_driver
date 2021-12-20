#ifndef __I2C_H
#define __I2C_H


#include "gd32f3x0.h"

extern uint8_t i2c_rxbuffer[39];

void i2c_config(void);
//void i2c_Read_Byte_Len(unsigned char *data,unsigned char Len);
void i2c_Read_Byte_Len(unsigned char *data,unsigned char *Len);
void i2c_Write_Byte_Len(unsigned char *data,unsigned char *Len);
//void i2c_Write_Byte_Len(unsigned char *data,unsigned char Len);

#endif

