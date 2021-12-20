
#include "i2c.h"
#include "soft_tlc59108.h"

#define Add_Tlc_59108  0x98 //0x19


void gpio_config(void)
{
	
		 /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* connect PB6 to I2C0_SCL */
    //gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);
    /* connect PB7 to I2C0_SDA */
    //gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);
    /* configure GPIO pins of I2C0 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	
		gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_9|GPIO_PIN_10);
}
    
void i2c_config(void)
{
		
		gpio_config();
		/* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);
    /* I2C clock configure */
    i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
    /* I2C address configure */
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, Add_Tlc_59108);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
	
		 /* enable the I2C1 interrupt */
    i2c_interrupt_enable(I2C0, I2C_INT_ERR);
    i2c_interrupt_enable(I2C0, I2C_INT_BUF);
    i2c_interrupt_enable(I2C0, I2C_INT_EV);
		//nvic_irq_enable(I2C0_EV_IRQn, 6, 0);
    nvic_irq_enable(I2C0_EV_IRQn, 5, 0);
    //nvic_irq_enable(I2C0_ER_IRQn, 4, 0);
    nvic_irq_enable(I2C0_ER_IRQn, 3, 0);
}


//void i2c_Read_Byte_Len(unsigned char *data,unsigned char Len)
//{
//		uint8_t i=0;
//		/* wait until ADDSEND bit is set */
//    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
//    /* clear ADDSEND bit */
//    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

//		for(i=0;i<Len;i++)
//		{
//			/* wait until the RBNE bit is set */
//			 while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
//			 /* read a data byte from I2C_DATA */
//			 data[i] = i2c_data_receive(I2C0);
//		}
//		/* wait until the STPDET bit is set */
//    while(!i2c_flag_get(I2C0, I2C_FLAG_STPDET));
//    /* clear the STPDET bit */
//    //i2c_enable(I2C0);
//		i2c_flag_clear(I2C0, I2C_FLAG_STPDET);
//}

//void i2c_Write_Byte_Len(unsigned char *data,unsigned char Len)
//{
//		/* wait until ADDSEND bit is set */
//    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
//    /* clear ADDSEND bit */
//    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
//		/* wait until the transmission data register is empty */
//    //while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
//	
//		for(uint8_t i=0;i<Len;i++)
//		{
//			/* wait until the transmission data register is empty */
//			while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
//			/* send a data byte */
//			i2c_data_transmit(I2C0, data[i]);
//			
//			/* the master doesn't acknowledge for the last byte */
//			while(i2c_flag_get(I2C0, I2C_FLAG_AERR))
//			{
//				/* clear the bit of AERR */
//				i2c_flag_clear(I2C0, I2C_FLAG_AERR);
//			
//			}
//		}
//		 /* the master doesn't acknowledge for the last byte */
//    while(!i2c_flag_get(I2C0, I2C_FLAG_AERR));
//    /* clear the bit of AERR */
//    i2c_flag_clear(I2C0, I2C_FLAG_AERR);
//}


void i2c_Read_Byte_Len(unsigned char *data,unsigned char *Len)
{
		uint8_t ReadLen=0;
		uint32_t read_cnt=0;
		
		do{
				/* wait until the RBNE bit is set */
				while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE))
				{
						read_cnt++;
						if(i2c_flag_get(I2C0, I2C_FLAG_STPDET))
						{
								*Len=ReadLen;	
								i2c_flag_clear(I2C0, I2C_FLAG_STPDET);
								return;
						}
				}
				
				/* read a data byte from I2C_DATA */
				*data = i2c_data_receive(I2C0);
				ReadLen++;
				data++;
				
			 }while(!i2c_flag_get(I2C0, I2C_FLAG_STPDET));		/* wait until the STPDET bit is set */
		
		//*Len=ReadLen;	
		//i2c_flag_clear(I2C0, I2C_FLAG_STPDET);
}


void i2c_Write_Byte_Len(unsigned char *data,unsigned char *Len)
{
		
	
		do{
		
				/* wait until the transmission data register is empty */
			while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
			/* send a data byte */
			i2c_data_transmit(I2C0, *data++);	
			
			//while(!i2c_flag_get(I2C0, I2C_FLAG_AERR);
			//i2c_flag_clear(I2C0, I2C_FLAG_AERR);
			
		}while(!i2c_flag_get(I2C0, I2C_FLAG_AERR));		/* the master doesn't acknowledge for the last byte */
		
    i2c_flag_clear(I2C0, I2C_FLAG_AERR);
}


uint8_t i2c_rxbuffer[39]={0};
uint16_t stop_cnt=0;
uint16_t addsend_cnt=0;
uint16_t res_cnt=0;
uint16_t Tx_cnt=0;
//uint8_t *i2c_rxbuffer[20];

void I2C0_EventIRQ_Handler(void)
{	
		uint8_t *Tx_Buff=0;
	
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_ADDSEND)){
        /* clear the ADDSEND bit */
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_ADDSEND);
				addsend_cnt++;
				//res_cnt=0;
				//i2c_rxbuffer=Tlc59108_Register;
			
    }else if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_RBNE)){
        /* if reception data register is not empty ,I2C0 will read a data from I2C_DATA */
        i2c_rxbuffer[res_cnt++] = i2c_data_receive(I2C0);
			
    }else if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_STPDET)){
       // status = SUCCESS;
        /* clear the STPDET bit */
        i2c_enable(I2C0);
				ReadLend=res_cnt;
				res_cnt=0;
//				stop_cnt++;
//				if(stop_cnt>=2)
//				{
//					stop_cnt+=0;
//				
//				}
//				
//				if(stop_cnt>=1)
//				{
//					stop_cnt+=0;
//				
//				}
        /* disable I2C0 interrupt */
        //i2c_interrupt_disable(I2C0, I2C_INT_ERR);
        //i2c_interrupt_disable(I2C0, I2C_INT_BUF);
        //i2c_interrupt_disable(I2C0, I2C_INT_EV);
    }
		if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_TBE) && (!i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_AERR)))
		{
				//i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_TBE);
				if(res_cnt==1)
				{
						res_cnt=0;
						Tx_Buff=(Get_Tlc59108_Addr() + (i2c_rxbuffer[0]));
				}
						i2c_data_transmit(I2C0, *Tx_Buff++);	
				
		}
	  if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_BTC))
		{
				
					i2c_data_transmit(I2C0, *Tx_Buff++);
				//i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_TBE);
		}
}

/*!
    \brief      handle I2C0 error interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C0_ErrorIRQ_Handler(void)
{
    /* no acknowledge received */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_AERR)){
			
				
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_AERR);
    }

    /* SMBus alert */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SMBALT)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBALT);
    }

    /* bus timeout in SMBus mode */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SMBTO)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBTO);
    }

    /* over-run or under-run when SCL stretch is disabled */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_OUERR)){
       i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_OUERR);
    }

    /* arbitration lost */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_LOSTARB)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_LOSTARB);
    }

    /* bus error */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_BERR)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_BERR);
				
    }

    /* CRC value doesn't match */
    if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_PECERR)){
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_PECERR);
    }

    /* disable the error interrupt */
//    i2c_interrupt_disable(I2C0, I2C_INT_ERR);
//    i2c_interrupt_disable(I2C0, I2C_INT_BUF);
//    i2c_interrupt_disable(I2C0, I2C_INT_EV);
}


void I2C0_EV_IRQHandler(void)
{
    I2C0_EventIRQ_Handler();
}

/*!
    \brief      this function handles I2C1 error interrupt request exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C0_ER_IRQHandler(void)
{
    I2C0_ErrorIRQ_Handler();
}


