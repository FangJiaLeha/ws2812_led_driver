#ifndef __SOFT_TLC59108_H
#define __SOFT_TLC59108_H

#include "gd32f3x0.h"

#define I2C2_LedDriverWrite					0x98
#define I2C2_LedDriverRead					0x99

/***********define TLC59108 internal register address****************/
#define	MODE1						0x00	//Mode register 1
#define	MODE2				    0x01	//Mode register 2
#define	PWM0				    0x02    //Brightness control LED0
#define	PWM1    				0x03    //Brightness control LED1
#define	PWM2				    0x04    //Brightness control LED2
#define	PWM3    				0x05    //Brightness control LED3
#define	PWM4				    0x06    //Brightness control LED4
#define	PWM5    				0x07    //Brightness control LED5
#define	PWM6				    0x08    //Brightness control LED6
#define	PWM7    				0x09    //Brightness control LED7
#define	GRPPWM					0x0A	//Group duty cycle control
#define	GRPFREQ				  0x0B	//Group frequency
#define	LEDOUT0					0x0C	//LED output state 0
#define	LEDOUT1					0x0D	//LED output state 1
#define	SUBADR1					0x0E	//I2C bus sub-address 1
#define	SUBADR2					0x0F	//I2C bus sub-address 2
#define	SUBADR3					0x10	//I2C bus sub-address 3
#define	ALLCALLADR			0x11	//LED all call I2C bus address

#define TLC_59108BaseAddress 0x80

#define BIT_GROUP_CONT (1<<5)

extern unsigned char Tlc59108_Register[18];
extern unsigned char Led_Ldrx[8];
extern unsigned char Led_blinkTime[8];
extern unsigned char Led_blink_duty[8];

extern uint8_t ReadLend;
extern uint8_t WriteLend;

//void Tlc59108_logic(uint8_t *Tlc_59108Add);
void Tlc59108_logic(uint8_t *Tlc_59108Add,const uint8_t *R_buff);
void LEDx_Blink_Control(uint8_t *Tlc_59108Add,uint8_t *Duty,uint8_t *Blink_Time);

inline uint8_t* Get_Tlc59108_Addr(void)
{
		return &Tlc59108_Register[0];

}


#endif


