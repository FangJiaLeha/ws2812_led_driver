#include <string.h>
#include "soft_tlc59108.h"
#include "i2c.h"
#include "PWM.h"


#define Tlc59108_READ 0x01
#define	Tlc59108_WRITE 0x00

uint8_t ReadLend=0;
uint8_t WriteLend=0;
uint8_t RW_Address=0;
unsigned char Tlc59108_Register[18]={0};
unsigned char Led_Ldrx[8];

unsigned char Led_blinkTime[8]={10,55,10,0,0,25,35,25};
unsigned char Led_blink_duty[8]={0x80,0x80,0x80,0,0,0x30,0x45,0x80};

//unsigned char Led_blinkTime[8]={0};
//unsigned char Led_blink_duty[8]={0};

volatile uint32_t *PWM_CHx_Reg[8]={
		&TIMER_CH0CV(TIMER1),
		&TIMER_CH1CV(TIMER1),
		&TIMER_CH2CV(TIMER1),
		&TIMER_CH3CV(TIMER1),
		
		&TIMER_CH0CV(TIMER13),
	
		&TIMER_CH0CV(TIMER2),
		&TIMER_CH1CV(TIMER2),
		&TIMER_CH3CV(TIMER2)
};
	
uint8_t check_data_dir(uint8_t *RW_add,uint8_t *Tlc_59108Add,const uint8_t *R_buff)
{
		uint8_t data_status=0;
		uint32_t R_time_Cnt=1000;
		uint8_t Rddress_off=0;
		//i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
		/* wait until ADDSEND bit is set */
   // while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    //i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
	
		/* wait until the RBNE bit is set */
//		while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE))
//		{
//				R_time_Cnt--;
//				if(!R_time_Cnt)
//				{
//						return 3;
//				}
//		
//		}
		/* read a data byte from I2C_DATA */
		if(ReadLend)
		{
				*RW_add = R_buff[0];
				Rddress_off=*RW_add - TLC_59108BaseAddress;
				memcpy((Tlc_59108Add+Rddress_off), &R_buff[1], ReadLend);
				ReadLend=0;
		}
		else{
				return 3;
		}
		
		if(i2c_flag_get(I2C0, I2C_FLAG_TR))
		{
			data_status=Tlc59108_READ;
		}
		else 
		{
			data_status=Tlc59108_WRITE;
		}
		
		return data_status;
 
}

void Tlc59108_write(uint8_t *RegAddress,uint8_t offset,uint8_t *Len)
{
		
		
		i2c_Write_Byte_Len(RegAddress+offset,Len);
	



}

void Tlc59108_read(uint8_t *RegAddress,uint8_t offset,uint8_t *Len)
{
		

		i2c_Read_Byte_Len(RegAddress + offset,Len);


}


void Tlc59108_PwmControl(uint8_t *Tlc_59108Add,uint8_t off)
{
		uint16_t	Ledout=0;
	
		Ledout=(Tlc_59108Add[LEDOUT1]<<8) | Tlc_59108Add[LEDOUT0];
	
		if(Tlc_59108Add[MODE2] & BIT_GROUP_CONT)	//blinking else dimming
		{
				__disable_irq();
			
				for(uint8_t i=0;i<8;i++)
				{
						Led_Ldrx[i] = (Ledout >> (i*2)) & 0x03;
					
						switch(Led_Ldrx[i])
						{
							case 1:
								
								break;
							case 2:
								*PWM_CHx_Reg[i]=Tlc_59108Add[PWM0 + i];
								Led_blinkTime[i]=0;
								Led_blink_duty[i]=0;
								break;
							case 3:
								Led_blinkTime[i]=Tlc_59108Add[GRPFREQ];
								Led_blink_duty[i]=Tlc_59108Add[GRPPWM];
								break;
							default:
								break;
						}
				}
				
			__enable_irq();
		}
		else																			
		{
				__disable_irq();
				for(uint8_t i=0;i<8;i++)
				{
						*PWM_CHx_Reg[i]=Tlc_59108Add[PWM0 + i];
						Led_blinkTime[i]=0;
				}
				__enable_irq();
		}
}


void LEDx_Blink_Control(uint8_t *Tlc_59108Add,uint8_t *Duty,uint8_t *Blink_Per)
{
		static uint32_t Blink_Cnt=0;
		static uint32_t Level_Hight_Time[8]={0};
		static uint32_t Level_Hight_Cnt[8]={0};
		static uint32_t Level_Low_Time[8]={0};
		static uint32_t Level_Low_Cnt[8]={0};
		
		//Level_Hight_Cnt=
	
		//if(!F_50Ms) return;
		
		for(uint8_t i=0;i<8;i++)
		{
//				Level_Hight_Cnt[i]=Blink_Time[i]*Duty[i]*50/256;
//				Level_Low_Cnt[i]=
			
				if(Blink_Per[i])
				{		
//						if(++Blink_Cnt>=50)
//						{
//								Blink_Cnt=0;
//								Blink_Time[i]-=1;
//						}
					
						Level_Hight_Time[i]=Blink_Per[i]*Duty[i]*50/256;
						//Level_Low_Time[i]=50-Level_Hight_Time[i];
					
						if(Level_Hight_Cnt[i] <= Level_Hight_Time[i])
						{
								//Level_Hight_Cnt[i]=0;
								*PWM_CHx_Reg[i]=Tlc_59108Add[PWM0 + i];
						}
						else
						{
								*PWM_CHx_Reg[i]=0;
						}
						
						if(++Level_Hight_Cnt[i] >= (Blink_Per[i]*50))
						{
								Level_Hight_Cnt[i]=0;
								//Blink_Time[i]-=1;
						}
						
				}
		
		}
		

}



void Tlc59108_logic(uint8_t *Tlc_59108Add,const uint8_t *R_buff)
{
		uint8_t data_status=2;
		uint8_t Rddress_off=0;
	
		data_status=check_data_dir(&RW_Address,Tlc_59108Add,R_buff);	//获取数据方向和读写地址,data_status=1(写)，data_status=0（读）
		
		if(data_status==3)		//超时退出；
		{
				return ;
		}
		Rddress_off=RW_Address - TLC_59108BaseAddress;
		
		if(data_status==1)
		{
				//Tlc59108_write(Tlc_59108Add,Rddress_off,&WriteLend);
		
		}
		else if(data_status==0)
		{
				//Tlc59108_read(Tlc_59108Add,Rddress_off,&ReadLend);
		
		}
		
		Tlc59108_PwmControl(Tlc_59108Add,Rddress_off);

}








