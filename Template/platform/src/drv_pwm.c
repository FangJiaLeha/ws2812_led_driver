#include "drv_pwm.h"

/******************************************************************************/
/* pwm gpio manage */
struct pwm_gpio_config
{
    uint32_t mode;
    uint32_t pull_up_down;
    uint32_t speed;
    uint8_t otype;
};

struct pwm_gpio_attr
{
    rcu_periph_enum clk;
    uint32_t port;
    uint32_t pin;
    uint32_t alt_func_num;
    struct pwm_gpio_config config;
};

/* pwm timer manage */
struct pwm_timer_attr
{
    rcu_periph_enum clk;
    uint32_t periph;
    uint16_t channel;
    uint16_t oc_mode;          // PWM输出模式设置
    uint16_t oc_shadow_set;    // 输出通道影子寄存器使能设置
    uint16_t dma_trig_src_set; // 定时器触发DMA请求源设置
};

/* pwm dma manage */
struct pwm_dma_attr
{
    rcu_periph_enum clk;
    dma_channel_enum channel;
    uint32_t per_addr;
    uint32_t dma_int_src;
    IRQn_Type dma_int_irq;
};

/******************************************************************************/
struct pwm_dev
{
    struct pwm_gpio_attr  _gpio_attr;
    struct pwm_timer_attr _timer_attr;
    struct pwm_dma_attr   _dma_attr;
    uint8_t trans_completion;
};
typedef struct pwm_dev pwm_dev_t;

/******************************************************************************/
static struct pwm_dev pwm_devs[] =
{
    #ifdef USING_PWM_CH1
    {
        ._gpio_attr = 
        {
            .clk = RCU_GPIOA,
            .port = GPIOA,
            #if defined(TIMER_PWM_PIN_SET) && (TIMER_PWM_PIN_SET == 0)
            .pin = GPIO_PIN_0,
            #else
            .pin = GPIO_PIN_2,
            #endif
            .alt_func_num = GPIO_AF_2,
            .config = {
                .mode = GPIO_MODE_AF,
                .pull_up_down = GPIO_PUPD_NONE,
                .otype = GPIO_OTYPE_PP,
                .speed = GPIO_OSPEED_50MHZ
            },
        },
        ._timer_attr = 
        {
            .clk = RCU_TIMER1,
            .periph = TIMER1,
            #if defined(TIMER_PWM_PIN_SET) && (TIMER_PWM_PIN_SET == 0)
            .channel = TIMER_CH_0,
            #else
            .channel = TIMER_CH_2,
            #endif
            .oc_mode = TIMER_OC_MODE_PWM0,
            .oc_shadow_set = TIMER_OC_SHADOW_ENABLE,
            .dma_trig_src_set = TIMER_DMA_UPD,
        },
        ._dma_attr = 
        {
            .clk = RCU_DMA,
            .channel = DMA_CH1,
            #if defined(TIMER_PWM_PIN_SET) && (TIMER_PWM_PIN_SET == 0)
            .per_addr = (uint32_t)&TIMER_CH0CV(TIMER1),
            #else
            .per_addr = (uint32_t)&TIMER_CH2CV(TIMER1),
            #endif
            .dma_int_src = DMA_INT_FTF,                 // DMA传输完成中断源设置
            .dma_int_irq = DMA_Channel1_2_IRQn,
        },
        .trans_completion = 0,
    },
    #endif
};

/******************************************************************************/
void DMA_Channel1_2_IRQHandler(void)
{
    if (dma_interrupt_flag_get(pwm_devs[0]._dma_attr.channel, DMA_INT_FLAG_ERR)) {
        dma_interrupt_flag_clear(pwm_devs[0]._dma_attr.channel, DMA_INT_FLAG_ERR);
    }
    if (dma_interrupt_flag_get(pwm_devs[0]._dma_attr.channel, DMA_INT_FLAG_HTF)) {
        dma_interrupt_flag_clear(pwm_devs[0]._dma_attr.channel, DMA_INT_FLAG_HTF);
    }
    if (dma_interrupt_flag_get(pwm_devs[0]._dma_attr.channel, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(pwm_devs[0]._dma_attr.channel, DMA_INT_FLAG_FTF);
        dma_interrupt_flag_clear(pwm_devs[0]._dma_attr.channel, DMA_INT_FLAG_HTF);
        dma_interrupt_flag_clear(pwm_devs[0]._dma_attr.channel, DMA_INT_FLAG_G);
        timer_channel_output_pulse_value_config(pwm_devs[0]._timer_attr.periph,
                                                pwm_devs[0]._timer_attr.channel,
                                                GD32F3X0_PWM_TIMER_SET(TIMER_PERIOD));
        /* disable DMA channelx */
        dma_channel_disable(pwm_devs[0]._dma_attr.channel);
        /* timer channel output disable config */
        timer_channel_output_state_config(pwm_devs[0]._timer_attr.periph,
                                          pwm_devs[0]._timer_attr.channel,
                                          TIMER_CCX_DISABLE);
        /* disable the timer*/
        timer_disable(pwm_devs[0]._timer_attr.periph);
        timer_counter_value_config(pwm_devs[0]._timer_attr.periph, 0);
        pwm_devs[0].trans_completion = 1;
    }
    if (dma_interrupt_flag_get(pwm_devs[0]._dma_attr.channel, DMA_INT_FLAG_G)) {
        dma_interrupt_flag_clear(pwm_devs[0]._dma_attr.channel, DMA_INT_FLAG_G);
    }
}

/******************************************************************************/
static void _pwm_io_init(void)
{
    uint8_t pwm_dev_num = sizeof(pwm_devs) / sizeof(pwm_devs[0]);
    
    for (uint8_t pwm_dev_count = 0; pwm_dev_count < pwm_dev_num; pwm_dev_count++) {
        /* gpio clk set */
        rcu_periph_clock_enable(pwm_devs[pwm_dev_count]._gpio_attr.clk);
        /* gpio mode set */
        gpio_mode_set(pwm_devs[pwm_dev_count]._gpio_attr.port,
                      pwm_devs[pwm_dev_count]._gpio_attr.config.mode,
                      pwm_devs[pwm_dev_count]._gpio_attr.config.pull_up_down,
                      pwm_devs[pwm_dev_count]._gpio_attr.pin);
        /* gpio output set */
        gpio_output_options_set(pwm_devs[pwm_dev_count]._gpio_attr.port,
                      pwm_devs[pwm_dev_count]._gpio_attr.config.otype,
                      pwm_devs[pwm_dev_count]._gpio_attr.config.speed,
                      pwm_devs[pwm_dev_count]._gpio_attr.pin);
        /* gpio alt func set */
        gpio_af_set(pwm_devs[pwm_dev_count]._gpio_attr.port,
                    pwm_devs[pwm_dev_count]._gpio_attr.alt_func_num,
                    pwm_devs[pwm_dev_count]._gpio_attr.pin);
    }
}

static void _pwm_timer_init(void)
{
    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocintpara;
    uint8_t pwm_dev_num = sizeof(pwm_devs) / sizeof(pwm_devs[0]);

    for (uint8_t pwm_dev_count = 0; pwm_dev_count < pwm_dev_num; pwm_dev_count++) {
        rcu_periph_clock_enable(pwm_devs[pwm_dev_count]._timer_attr.clk);
        timer_deinit(pwm_devs[pwm_dev_count]._timer_attr.periph);

        memset((void *)&timer_initpara, 0, sizeof(timer_initpara));
        memset((void *)&timer_ocintpara, 0, sizeof(timer_ocintpara));

        /* timer base struct config */
        timer_initpara.prescaler = 0;
        timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
        timer_initpara.period = GD32F3X0_PWM_TIMER_SET(TIMER_PERIOD);
        timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
        timer_initpara.counterdirection  = TIMER_COUNTER_UP;
        timer_initpara.repetitioncounter = 0;
        timer_init(pwm_devs[pwm_dev_count]._timer_attr.periph, &timer_initpara);

        /* clear the timer interrupt flag */
        timer_interrupt_flag_clear(pwm_devs[pwm_dev_count]._timer_attr.periph,
                                   TIMER_INT_FLAG_CH3);
        timer_interrupt_flag_clear(pwm_devs[pwm_dev_count]._timer_attr.periph,
                                   TIMER_INT_FLAG_CH2);
        timer_interrupt_flag_clear(pwm_devs[pwm_dev_count]._timer_attr.periph,
                                   TIMER_INT_FLAG_CH1);
        timer_interrupt_flag_clear(pwm_devs[pwm_dev_count]._timer_attr.periph,
                                   TIMER_INT_FLAG_CH0);
        timer_interrupt_flag_clear(pwm_devs[pwm_dev_count]._timer_attr.periph,
                                   TIMER_INT_FLAG_UP);
        /* timer oc para init */
        timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
        timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
        timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
        timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
        timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
        timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
        timer_channel_output_config(pwm_devs[pwm_dev_count]._timer_attr.periph,
                                    pwm_devs[pwm_dev_count]._timer_attr.channel,
                                    &timer_ocintpara);

        /* timer channel pulse value set */
        timer_channel_output_pulse_value_config(pwm_devs[pwm_dev_count]._timer_attr.periph,
                                                pwm_devs[pwm_dev_count]._timer_attr.channel,
                                                GD32F3X0_PWM_TIMER_SET(TIMER_PERIOD) * (1 - TIMER_PWM_REVERSE));
        /* timer channel pwm output mode set */
        timer_channel_output_mode_config(pwm_devs[pwm_dev_count]._timer_attr.periph,
                                    pwm_devs[pwm_dev_count]._timer_attr.channel,
                                    pwm_devs[pwm_dev_count]._timer_attr.oc_mode);
        /* timer channel output shadow set */
        timer_channel_output_shadow_config(pwm_devs[pwm_dev_count]._timer_attr.periph,
                                    pwm_devs[pwm_dev_count]._timer_attr.channel,
                                    pwm_devs[pwm_dev_count]._timer_attr.oc_shadow_set);

        /* timer update event set as the DMA request enable */
        timer_dma_enable(pwm_devs[pwm_dev_count]._timer_attr.periph, 
                         pwm_devs[pwm_dev_count]._timer_attr.dma_trig_src_set);
        /* timer update event set as the DMA request source */
        timer_channel_dma_request_source_select(pwm_devs[pwm_dev_count]._timer_attr.periph,
                                                TIMER_DMAREQUEST_UPDATEEVENT);
        timer_update_source_config(pwm_devs[pwm_dev_count]._timer_attr.periph,
                                  TIMER_UPDATE_SRC_REGULAR);

        /* auto-reload preload enable */
        timer_auto_reload_shadow_enable(pwm_devs[pwm_dev_count]._timer_attr.periph);
        /* disable the timer */
        timer_disable(pwm_devs[pwm_dev_count]._timer_attr.periph);

        pwm_devs[pwm_dev_count].trans_completion = 0;
    }
}

static void _pwm_dma_init(void)
{
    dma_parameter_struct dma_init_struct;
    uint8_t pwm_dev_num = sizeof(pwm_devs) / sizeof(pwm_devs[0]);
    
    for (uint8_t pwm_dev_count = 0; pwm_dev_count < pwm_dev_num; pwm_dev_count++) {
        /* enable DMA clock */
        rcu_periph_clock_enable(pwm_devs[pwm_dev_count]._dma_attr.clk);

        /* initialize DMA channelx */
        dma_deinit(pwm_devs[pwm_dev_count]._dma_attr.channel);

        memset((void *)&dma_init_struct, 0, sizeof(dma_init_struct));
        /* DMA channelx initialize */
        dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
        dma_init_struct.number       = 0;
        dma_init_struct.periph_addr  = pwm_devs[pwm_dev_count]._dma_attr.per_addr;
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
        dma_init_struct.priority     = DMA_PRIORITY_HIGH;
        dma_init(pwm_devs[pwm_dev_count]._dma_attr.channel, &dma_init_struct);

        /* configure DMA mode */
        dma_circulation_disable(pwm_devs[pwm_dev_count]._dma_attr.channel);
        dma_memory_to_memory_disable(pwm_devs[pwm_dev_count]._dma_attr.channel);

        /* clear the DMA transfer end flag and disable the half transfer interrupt */
        dma_interrupt_flag_clear(pwm_devs[pwm_dev_count]._dma_attr.channel, DMA_INT_FLAG_FTF);
        dma_interrupt_disable(pwm_devs[pwm_dev_count]._dma_attr.channel, DMA_INT_HTF);

        /* enable DMA transfer complete interrupt */
        dma_interrupt_enable(pwm_devs[pwm_dev_count]._dma_attr.channel,
                             pwm_devs[pwm_dev_count]._dma_attr.dma_int_src);

        nvic_irq_enable(pwm_devs[pwm_dev_count]._dma_attr.dma_int_irq, 0, 0);

        /* disable DMA channelx */
        dma_channel_disable(pwm_devs[pwm_dev_count]._dma_attr.channel);
    }
}

/******************************************************************************/
void init_pwm(void)
{
    _pwm_io_init();
    _pwm_timer_init();
    _pwm_dma_init();
}

Size_Type write_pwm(uint8_t pwm_channel_index, const void *buffer, uint32_t size)
{
    dma_channel_enum channel;
    uint16_t *data = (uint16_t *)buffer;

    if (pwm_channel_index >= (sizeof(pwm_devs) / sizeof(pwm_devs[0]) )) {
        return EINVAL;
    }

    channel = pwm_devs[pwm_channel_index]._dma_attr.channel;
    for (uint32_t cnt = 0; cnt < size; cnt++) {
        if (data[cnt] == 0) {
            #if ( defined(TIMER_PWM_REVERSE) && (TIMER_PWM_REVERSE == 0x01) )
            data[cnt] = GD32F3X0_PWM_TIMER_SET(TIMER_PERIOD);
            #else
            data[cnt] = 0;
            #endif
        } else {
            /* 根据高低电平时间转化为对应的比较器值 */
            #if ( defined(TIMER_PWM_REVERSE) && (TIMER_PWM_REVERSE == 0x01) )
            data[cnt] = GD32F3X0_PWM_TIMER_SET(TIMER_PERIOD) - GD32F3X0_PWM_TIMER_SET(data[cnt]);
            #else
            data[cnt] = GD32F3X0_PWM_TIMER_SET(data[cnt]);
            #endif
        }
    }

    pwm_devs[pwm_channel_index].trans_completion = 0;

    /* disable DMA channelx */
    dma_channel_disable(channel);

    /* clear the DMA interrupt flag */
    dma_interrupt_flag_clear(channel, DMA_INT_FLAG_G);
    dma_interrupt_flag_clear(channel, DMA_INT_FLAG_FTF);
    dma_interrupt_flag_clear(channel, DMA_INT_FLAG_HTF);
    dma_interrupt_flag_clear(channel, DMA_INT_FLAG_ERR);

    /* set the DMA mem addr and transfor numbers of data */
    dma_memory_address_config(channel, (uint32_t)buffer);
    dma_transfer_number_config(channel, size);

    /* enable DMA channelx */
    dma_channel_enable(channel);

    /* timer channel output enable config */
    timer_channel_output_state_config(pwm_devs[pwm_channel_index]._timer_attr.periph,
                                      pwm_devs[pwm_channel_index]._timer_attr.channel,
                                      TIMER_CCX_ENABLE);

    /* enable the timer*/
    timer_enable(pwm_devs[pwm_channel_index]._timer_attr.periph);

    while(pwm_devs[pwm_channel_index].trans_completion == 0);

    return size;
}

Rtv_Status control_pwm(uint8_t pwm_channel_index, int cmd, void *arg)
{
    pwm_base_attr_t _base_attr = NULL;
    uint32_t timer_periph, pulse, period;
    uint16_t timer_channel;

    if (pwm_channel_index >= ITEM_NUM(pwm_devs) ) {
        return EINVAL;
    }
    timer_periph = pwm_devs[pwm_channel_index]._timer_attr.periph;
    timer_channel = pwm_devs[pwm_channel_index]._timer_attr.channel;

    switch(cmd)
    {
        case PWM_CMD_ENABLE:
            /* timer channel output enable config */
            timer_channel_output_state_config(timer_periph, timer_channel, TIMER_CCX_ENABLE);
        break;
        case PWM_CMD_DISABLE:
            /* timer channel output disable config */
            timer_channel_output_state_config(timer_periph, timer_channel, TIMER_CCX_DISABLE);
        break;
        case PWM_CMD_SET:
            if (arg == NULL) {
                return EINVAL;
            }
            _base_attr = (pwm_base_attr_t)arg;

            /* period vaild check */
            if (_base_attr->set_period <= 0 || _base_attr->set_period < (1000/FTIMER)) {
                period = 0;
            } else {
                period = GD32F3X0_PWM_TIMER_SET(_base_attr->set_period);
            }
            timer_autoreload_value_config(timer_periph, period);

            /* pulse vaild check */
            if (_base_attr->set_pulse <= 0 || _base_attr->set_pulse < (1000/FTIMER)) {
                pulse = 0;
            } else if (_base_attr->set_pulse > _base_attr->set_period) {
                pulse = GD32F3X0_PWM_TIMER_SET(_base_attr->set_period);
            } else {
                pulse = GD32F3X0_PWM_TIMER_SET(_base_attr->set_pulse);
            }
            
            timer_channel_output_pulse_value_config(timer_periph, timer_channel, pulse);
        break;
        default:
        break;
    }
    return SUCCESS;
}
