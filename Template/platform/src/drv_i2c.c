#include "drv_i2c.h"
#include "string.h"

//==============================================================================
struct i2c_gpio_attr
{
    rcu_periph_enum scl_clk;
    rcu_periph_enum sda_clk;
    uint32_t scl_port;
    uint32_t scl_pin;
    uint32_t sda_port;
    uint32_t sda_pin;
};

struct i2c_gpio_config_attr
{
    uint32_t mode;
    uint32_t pull_up_down;
    uint32_t speed;
    uint32_t alt_func_num;
    uint8_t otype;
};

struct i2c_periph_attr
{
    rcu_periph_enum clk;
    uint32_t periph;
};

struct i2c_periph_config_attr
{
    uint32_t clk_speed;
    uint32_t dutycyc;
    uint32_t mode;
    uint32_t addformat;
    uint32_t addr;
    uint32_t ack_en;
    i2c_interrupt_enum int_err_src;
    i2c_interrupt_enum int_buf_src;
    i2c_interrupt_enum int_ev_src;
    IRQn_Type int_err_irq;
    IRQn_Type int_ev_irq;
    uint8_t  int_err_en:1;
    uint8_t  int_buf_en:1;
    uint8_t  int_ev_en:1;
    uint8_t  reserved:5;
};

struct i2c_hard_attr
{
    struct i2c_gpio_attr _gpio;
    struct i2c_gpio_config_attr _gpio_cfg;
    struct i2c_periph_attr _periph;
    struct i2c_periph_config_attr _periph_cfg;
};

struct i2c_dev
{
    struct i2c_hard_attr _hard_attr;
    uint16_t set_i2c_addr;
    uint16_t recv_cnt;
    uint16_t read_len;
    uint16_t send_cnt;
    uint8_t  *recv_buff;
    uint8_t  *send_buff;
    uint8_t  malloc_size;
};
typedef struct i2c_dev_t i2c_dev_t;

//==============================================================================
static struct i2c_dev i2c_devs[] =
{
    #if (defined(USING_I2C0) && (USING_I2C0 == 0x01))
    {
        ._hard_attr = {
            ._gpio = {
                .scl_clk = RCU_GPIOA,
                .scl_port = GPIOA,
                .scl_pin = GPIO_PIN_9,
                .sda_clk = RCU_GPIOA,
                .sda_port = GPIOA,
                .sda_pin = GPIO_PIN_10
            },
            ._gpio_cfg = {
                .mode = GPIO_MODE_AF,
                .pull_up_down = GPIO_PUPD_PULLUP,
                .speed = GPIO_OSPEED_50MHZ,
                .otype = GPIO_OTYPE_OD,
                .alt_func_num = GPIO_AF_4   // alt func i2c0
            },
            ._periph = {
                .clk = RCU_I2C0,
                .periph = I2C0
            },
            ._periph_cfg = {
                .clk_speed = I2C_CLK_FRE,
                .dutycyc = I2C_DTCY_16_9,
                .mode = I2C_I2CMODE_ENABLE,
                .addformat = I2C_ADDFORMAT_7BITS,
                .addr = I2C_DEF_ADDR,
                .ack_en = I2C_ACK_ENABLE,
                .int_err_src = I2C_INT_ERR,
                .int_ev_src = I2C_INT_EV,
                .int_buf_src = I2C_INT_BUF,
                .int_ev_irq = I2C0_EV_IRQn,
                .int_err_irq = I2C0_ER_IRQn,
                .int_err_en = 1,
                .int_buf_en = 1,
                .int_ev_en = 1
            }
        },
        .set_i2c_addr = I2C_DEF_ADDR,
        .recv_cnt = 0,
        .read_len = 0,
        .send_cnt = 0,
        .recv_buff = NULL,
        .send_buff = NULL,
        .malloc_size = 8
    },
    #endif
};

//==============================================================================
#if ( defined(USING_I2C0) && USING_I2C0 == 0x01 )
void I2C0_EV_IRQHandler(void)
{
    if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_ADDSEND)) {
        /* clear the ADDSEND bit */
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_ADDSEND);
    } else if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_RBNE)) {
        /* if reception data register is not empty ,I2C0 will read a data from I2C_DATA */
        i2c_devs[0].recv_buff[(i2c_devs[0].recv_cnt++) % (i2c_devs[0].malloc_size)] = i2c_data_receive(I2C0);
    } else if(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_STPDET)) {
        /* clear the STPDET bit */
        i2c_enable(I2C0);
        i2c_devs[0].read_len = i2c_devs[0].recv_cnt;
        i2c_devs[0].recv_cnt = 0;
    } else if( (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_TBE)) &&
              !(i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_AERR)) ) {
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_TBE);
        i2c_data_transmit(I2C0, i2c_devs[0].send_buff[(i2c_devs[0].send_cnt++) % (i2c_devs[0].malloc_size)]);
        i2c_devs[0].recv_cnt = 0;
    } else {
    }
}

void I2C0_ER_IRQHandler(void)
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
#endif

//==============================================================================
static void _i2c_io_init(void)
{
    uint8_t i2c_dev_num = ITEM_NUM(i2c_devs);
    I2C_DEV_NUM_CHECK(i2c_dev_num);

    for (uint8_t i2c_dev_cnt = 0; i2c_dev_cnt < i2c_dev_num; i2c_dev_cnt++) {
        /* enable the i2c gpio clk */
        rcu_periph_clock_enable(i2c_devs[i2c_dev_cnt]._hard_attr._gpio.scl_clk);
        rcu_periph_clock_enable(i2c_devs[i2c_dev_cnt]._hard_attr._gpio.sda_clk);

        // config the i2c gpio attr
        /* i2c scl gpio config */
        gpio_mode_set(i2c_devs[i2c_dev_cnt]._hard_attr._gpio.scl_port,
                      i2c_devs[i2c_dev_cnt]._hard_attr._gpio_cfg.mode,
                      i2c_devs[i2c_dev_cnt]._hard_attr._gpio_cfg.pull_up_down,
                      i2c_devs[i2c_dev_cnt]._hard_attr._gpio.scl_pin);
        gpio_output_options_set(i2c_devs[i2c_dev_cnt]._hard_attr._gpio.scl_port,
                                i2c_devs[i2c_dev_cnt]._hard_attr._gpio_cfg.otype,
                                i2c_devs[i2c_dev_cnt]._hard_attr._gpio_cfg.speed,
                                i2c_devs[i2c_dev_cnt]._hard_attr._gpio.scl_pin);
        /* i2c sda gpio config */
        gpio_mode_set(i2c_devs[i2c_dev_cnt]._hard_attr._gpio.sda_port,
                      i2c_devs[i2c_dev_cnt]._hard_attr._gpio_cfg.mode,
                      i2c_devs[i2c_dev_cnt]._hard_attr._gpio_cfg.pull_up_down,
                      i2c_devs[i2c_dev_cnt]._hard_attr._gpio.sda_pin);
        gpio_output_options_set(i2c_devs[i2c_dev_cnt]._hard_attr._gpio.sda_port,
                                i2c_devs[i2c_dev_cnt]._hard_attr._gpio_cfg.otype,
                                i2c_devs[i2c_dev_cnt]._hard_attr._gpio_cfg.speed,
                                i2c_devs[i2c_dev_cnt]._hard_attr._gpio.sda_pin);

        /* set the i2c gpio alt_func */
        gpio_af_set(i2c_devs[i2c_dev_cnt]._hard_attr._gpio.scl_port,
                    i2c_devs[i2c_dev_cnt]._hard_attr._gpio_cfg.alt_func_num,
                    i2c_devs[i2c_dev_cnt]._hard_attr._gpio.scl_pin);
        gpio_af_set(i2c_devs[i2c_dev_cnt]._hard_attr._gpio.sda_port,
                    i2c_devs[i2c_dev_cnt]._hard_attr._gpio_cfg.alt_func_num,
                    i2c_devs[i2c_dev_cnt]._hard_attr._gpio.sda_pin);
    }
set_error:
    return;
}

static Rtv_Status _i2c_periph_init(uint8_t _set_i2c_addr, uint8_t _recv_buff_size)
{
    uint8_t i2c_dev_num = ITEM_NUM(i2c_devs);
    uint8_t i2c_default_addr;
    I2C_DEV_NUM_CHECK(i2c_dev_num);
    I2C_ADDR_CHECK(_set_i2c_addr);

    for (uint8_t i2c_dev_cnt = 0; i2c_dev_cnt < i2c_dev_num; i2c_dev_cnt++) {
        /* enable I2Cx clock */
        rcu_periph_clock_enable(i2c_devs[i2c_dev_cnt]._hard_attr._periph.clk);
        /* I2Cx clock configure */
        i2c_clock_config(i2c_devs[i2c_dev_cnt]._hard_attr._periph.periph,
                         i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.clk_speed,
                         i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.dutycyc);

        i2c_default_addr = i2c_devs[i2c_dev_cnt].set_i2c_addr;
        /* I2Cx address configure */
        i2c_mode_addr_config(i2c_devs[i2c_dev_cnt]._hard_attr._periph.periph,
                             i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.mode,
                             i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.addformat,
                             (i2c_default_addr == _set_i2c_addr ? i2c_default_addr : _set_i2c_addr));
        if (i2c_default_addr != _set_i2c_addr) {
            i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.addr = _set_i2c_addr;
            i2c_devs[i2c_dev_cnt].set_i2c_addr = _set_i2c_addr;
        }
        /* enable I2Cx */
        i2c_enable(i2c_devs[i2c_dev_cnt]._hard_attr._periph.periph);
        /* I2Cx acknowledge config*/
        i2c_ack_config(i2c_devs[i2c_dev_cnt]._hard_attr._periph.periph,
                       i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.ack_en);

        // I2Cx interrupt config
        /* I2Cx err interrupr config */
        if (i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_err_en == 1) {
            i2c_interrupt_enable(i2c_devs[i2c_dev_cnt]._hard_attr._periph.periph,
                                 i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_err_src);
            nvic_irq_enable(i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_err_irq,
                            3, 0);
        } else {
            i2c_interrupt_disable(i2c_devs[i2c_dev_cnt]._hard_attr._periph.periph,
                                 i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_err_src);
            nvic_irq_disable(i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_err_irq);
        }
        /* I2Cx ev interrupr config */
        if (i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_ev_en == 1) {
            i2c_interrupt_enable(i2c_devs[i2c_dev_cnt]._hard_attr._periph.periph,
                                 i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_ev_src);
            nvic_irq_enable(i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_ev_irq,
                            5, 0);
        } else {
            i2c_interrupt_disable(i2c_devs[i2c_dev_cnt]._hard_attr._periph.periph,
                                 i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_ev_src);
            nvic_irq_disable(i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_ev_irq);
        }
        /* I2Cx buf interrupr config */
        if (i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_buf_en == 1) {
            i2c_interrupt_enable(i2c_devs[i2c_dev_cnt]._hard_attr._periph.periph,
                                 i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_buf_src);
        } else {
            i2c_interrupt_disable(i2c_devs[i2c_dev_cnt]._hard_attr._periph.periph,
                                 i2c_devs[i2c_dev_cnt]._hard_attr._periph_cfg.int_buf_src);
        }

        if (_recv_buff_size != i2c_devs[i2c_dev_cnt].malloc_size && _recv_buff_size == 0) {
            i2c_devs[i2c_dev_cnt].recv_buff = (uint8_t *)malloc(i2c_devs[i2c_dev_cnt].malloc_size);
            i2c_devs[i2c_dev_cnt].send_buff = (uint8_t *)malloc(i2c_devs[i2c_dev_cnt].malloc_size);
        } else {
            i2c_devs[i2c_dev_cnt].recv_buff = (uint8_t *)malloc(_recv_buff_size);
            i2c_devs[i2c_dev_cnt].send_buff = (uint8_t *)malloc(_recv_buff_size);
            i2c_devs[i2c_dev_cnt].malloc_size = _recv_buff_size;
        }
        if (i2c_devs[i2c_dev_cnt].recv_buff == NULL || i2c_devs[i2c_dev_cnt].send_buff == NULL) {
            return ENOMEM;
        }
    }
    return SUCCESS;
set_error:
    return EINVAL;
}

//==============================================================================
void init_i2c(uint8_t i2c_addr, uint8_t i2c_buff_size)
{
    _i2c_io_init();
    if (SUCCESS != _i2c_periph_init(i2c_addr, i2c_buff_size) ) {
        return;
    }
}

void control_i2c(uint8_t i2c_index, int cmd, void *arg)
{
    uint8_t **get_send_buff = NULL;
    I2C_DEV_INDEX_CHECK(i2c_index);
    I2C_DEV_CTL_CMD_CHECK(cmd);

    switch(cmd)
    {
        case I2C_GET_RECV_DATA_LEN:
            *(uint16_t *)arg = i2c_devs[i2c_index - 1].read_len;
        break;
        case I2C_RESET_RECV_DATA_LEN:
            i2c_devs[i2c_index-1].read_len = 0;
        break;
        case I2C_GET_RECV_BUFF:
            memmove((uint8_t *)arg, i2c_devs[i2c_index - 1].recv_buff, i2c_devs[i2c_index - 1].read_len);
        break;
        case I2C_RESET_RECV_BUFF:
            memset(i2c_devs[i2c_index - 1].recv_buff, 0, i2c_devs[i2c_index - 1].malloc_size);
        break;
        case I2C_GET_SEND_DATA_LEN:
            *(uint16_t *)arg = i2c_devs[i2c_index - 1].send_cnt;
        break;
        case I2C_RESET_SEND_DATA_LEN:
            i2c_devs[i2c_index-1].send_cnt = 0;
        break;
        case I2C_GET_SEND_BUFF:
            get_send_buff = arg;
            *get_send_buff = i2c_devs[i2c_index - 1].send_buff;
        break;
        case I2C_RESET_SEND_BUFF:
            memset(i2c_devs[i2c_index - 1].send_buff, 0, i2c_devs[i2c_index - 1].malloc_size);
        break;
        default:
        break;
    }
set_error:
    return;
}



