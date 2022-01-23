#include "drv_i2c.h"

#define WS2812_BASE_ADDR    0x41

static uint16_t recv_cnt = 0, ReadLend;
static uint8_t i2c_rxbuffer[16]={0};

void gpio_config(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOA);

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
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, WS2812_BASE_ADDR);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);

     /* enable the I2C1 interrupt */
    i2c_interrupt_enable(I2C0, I2C_INT_ERR);
    i2c_interrupt_enable(I2C0, I2C_INT_BUF);
    i2c_interrupt_enable(I2C0, I2C_INT_EV);
    nvic_irq_enable(I2C0_EV_IRQn, 5, 0);
    nvic_irq_enable(I2C0_ER_IRQn, 3, 0);
}

uint16_t get_data_len(void)
{
    return ReadLend;
}

void clear_data_len(void)
{
    ReadLend = 0;
}

uint8_t *get_data_buff(void)
{
    return &i2c_rxbuffer[0];
}

void I2C0_EventIRQ_Handler(void)
{
    if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_ADDSEND)) {
        /* clear the ADDSEND bit */
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_ADDSEND);
    } else if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_RBNE)) {
        /* if reception data register is not empty ,I2C0 will read a data from I2C_DATA */
        i2c_rxbuffer[recv_cnt++] = i2c_data_receive(I2C0);
    } else if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_STPDET)) {
        /* clear the STPDET bit */
        i2c_enable(I2C0);
        ReadLend = recv_cnt;
        recv_cnt = 0;
    } else if(i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_AERR)) {
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_AERR);
    } else if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_BERR)) {
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_BERR);
    } else {
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


