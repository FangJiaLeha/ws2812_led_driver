#ifndef __PWM_H
#define __PWM_H

#include "gd32f3x0.h"

void PWM_Init(void);
void PWM_Frequenty_Config(uint16_t psc,uint16_t per,uint32_t timer_periph);


#endif


