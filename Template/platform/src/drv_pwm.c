/**
 * @file drv_pwm.c
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "drv_pwm.h"

/******************************************************************************/
/**
 * @brief PWM通道宏静态编译快速配置
 *
 */
#if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x01) )
    #define USING_PWM_CH0   // PA0 -> timer1_ch0
    #if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x02) )
        #define USING_PWM_CH1   // PA1 -> timer1_ch1
    #endif
    #if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x03) )
        #define USING_PWM_CH2   // PA2 -> timer1_ch2
    #endif
    #if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x04) )
        #define USING_PWM_CH3   // PA3 -> timer1_ch3
    #endif
    #if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x05) )
        #define USING_PWM_CH4   // PA4 -> timer13_ch0(不支持DMA)
    #endif
    #if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x06) )
        #define USING_PWM_CH5   // PA6 -> timer2_ch0
    #endif
    #if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x07) )
        #define USING_PWM_CH6   // PA7 -> timer2_ch1
    #endif
    #if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x08) )
        #define USING_PWM_CH7   // PB1 -> tiemr2_ch3
    #endif
#endif

/**
 * @brief 提取PWM通用配置属性
 *
 */
static GeneralPwmCfgType pwm_cfg_stc = {
    .gpio_cfg = {
        .mode = GPIO_MODE_AF,
        .pull_up_down = GPIO_PUPD_NONE,
        .otype = GPIO_OTYPE_PP,
        .speed = GPIO_OSPEED_10MHZ
    },
    .timer_cfg = {
        .oc_mode = TIMER_OC_MODE_PWM0,
        .oc_shadow_set = TIMER_OC_SHADOW_ENABLE,
        .dma_trig_src_set = TIMER_DMA_UPD
    },
    .dma_cfg = {
        .clk = RCU_DMA,
        .dma_int_src = DMA_INT_FTF
    }
};

/**
 * @brief PWM输出通道
 *
 */
static PwmDriverChannelType pwm_channels[] = {
    #ifdef USING_PWM_CH0
    {
        ._gpio_attr = 
        {
            .clk = RCU_GPIOA,
            .port = GPIOA,
            .pin = GPIO_PIN_0,
            .alt_func_num = GPIO_AF_2   // alt timer1_ch0
        },
        ._timer_attr = 
        {
            .clk = RCU_TIMER1,
            .periph = TIMER1,
            .channel = TIMER_CH_0
        },
        ._dma_attr = 
        {
            .channel = DMA_CH1,
            .per_addr = (uint32_t)&TIMER_CH0CV(TIMER1),
            .dma_int_irq = DMA_Channel1_2_IRQn
        }
    },
    #endif
    #ifdef USING_PWM_CH1
    {
        ._gpio_attr =
        {
            .clk = RCU_GPIOA,
            .port = GPIOA,
            .pin = GPIO_PIN_1,
            .alt_func_num = GPIO_AF_2   // alt timer1_ch1
        },
        ._timer_attr =
        {
            .clk = RCU_TIMER1,
            .periph = TIMER1,
            .channel = TIMER_CH_1
        },
        ._dma_attr =
        {
            .channel = DMA_CH1,
            .per_addr = (uint32_t)&TIMER_CH1CV(TIMER1),
            .dma_int_irq = DMA_Channel1_2_IRQn
        }
    },
    #endif
    #ifdef USING_PWM_CH2
    {
        ._gpio_attr =
        {
            .clk = RCU_GPIOA,
            .port = GPIOA,
            .pin = GPIO_PIN_2,
            .alt_func_num = GPIO_AF_2   // alt timer1_ch2
        },
        ._timer_attr =
        {
            .clk = RCU_TIMER1,
            .periph = TIMER1,
            .channel = TIMER_CH_2
        },
        ._dma_attr =
        {
            .channel = DMA_CH1,
            .per_addr = (uint32_t)&TIMER_CH2CV(TIMER1),
            .dma_int_irq = DMA_Channel1_2_IRQn
        }
    },
    #endif
    #ifdef USING_PWM_CH3
    {
        ._gpio_attr =
        {
            .clk = RCU_GPIOA,
            .port = GPIOA,
            .pin = GPIO_PIN_3,
            .alt_func_num = GPIO_AF_2   // alt timer1_ch3
        },
        ._timer_attr =
        {
            .clk = RCU_TIMER1,
            .periph = TIMER1,
            .channel = TIMER_CH_3
        },
        ._dma_attr =
        {
            .channel = DMA_CH1,
            .per_addr = (uint32_t)&TIMER_CH3CV(TIMER1),
            .dma_int_irq = DMA_Channel1_2_IRQn
        }
    },
    #endif
    #ifdef USING_PWM_CH4 // 该通道不支持DMA
    {
        ._gpio_attr =
        {
            .clk = RCU_GPIOA,
            .port = GPIOA,
            .pin = GPIO_PIN_4,
            .alt_func_num = GPIO_AF_4   // alt timer13_ch0
        },
        ._timer_attr =
        {
            .clk = RCU_TIMER13,
            .periph = TIMER13,
            .channel = TIMER_CH_0
        },
        ._dma_attr = 0
    },
    #endif
    #ifdef USING_PWM_CH5
    {
        ._gpio_attr =
        {
            .clk = RCU_GPIOA,
            .port = GPIOA,
            .pin = GPIO_PIN_6,
            .alt_func_num = GPIO_AF_1   // alt timer2_ch0
        },
        ._timer_attr =
        {
            .clk = RCU_TIMER2,
            .periph = TIMER2,
            .channel = TIMER_CH_0
        },
        ._dma_attr =
        {
            .channel = DMA_CH2,
            .per_addr = (uint32_t)&TIMER_CH0CV(TIMER2),
            .dma_int_irq = DMA_Channel1_2_IRQn
        }
    },
    #endif
    #ifdef USING_PWM_CH6
    {
        ._gpio_attr =
        {
            .clk = RCU_GPIOA,
            .port = GPIOA,
            .pin = GPIO_PIN_7,
            .alt_func_num = GPIO_AF_1   // alt timer2_ch1
        },
        ._timer_attr =
        {
            .clk = RCU_TIMER2,
            .periph = TIMER2,
            .channel = TIMER_CH_1
        },
        ._dma_attr =
        {
            .channel = DMA_CH2,
            .per_addr = (uint32_t)&TIMER_CH1CV(TIMER2),
            .dma_int_irq = DMA_Channel1_2_IRQn
        }
    },
    #endif
    #ifdef USING_PWM_CH7
    {
        ._gpio_attr =
        {
            .clk = RCU_GPIOA,
            .port = GPIOB,
            .pin = GPIO_PIN_1,
            .alt_func_num = GPIO_AF_1   // alt timer2_ch3
        },
        ._timer_attr =
        {
            .clk = RCU_TIMER2,
            .periph = TIMER2,
            .channel = TIMER_CH_3
        },
        ._dma_attr =
        {
            .channel = DMA_CH2,
            .per_addr = (uint32_t)&TIMER_CH3CV(TIMER2),
            .dma_int_irq = DMA_Channel1_2_IRQn
        }
    }
    #endif
};

static Rtv_Status _init_hard_pwm(void *pwm_dev);
static Size_Type _write_pwm(void *pwm_dev,
                            const uint8_t pwm_channel_index,
                            const void *buffer,
                            const uint32_t size);
static Rtv_Status _control_pwm(void *pwm_dev,
                               const uint8_t pwm_channel_index,
                               const PWMCtrlCmdType cmd,
                               void *arg);

/******************************************************************************/
static struct pwm_dev pwm_devs =
{
    .driver_channel = pwm_channels,
    .driver_channel_cur = PWM_CHX_INDEX_INIT_SET,    // 默认设置为第一个PWM通道
    .driver_channel_num = PWM_CHX_NUM_SET,           // PWM输出通道总数
    .driver_type = WS2812DEV,
    .trans_completion = 0,
    .init = _init_hard_pwm,
    .write = _write_pwm,
    .control = _control_pwm
};

/******************************************************************************/
void DMA_Channel1_2_IRQHandler(void)
{
    // 获取当前选择通道的定时器外设/通道和dma通道
    uint32_t timer_periph = pwm_devs.driver_channel[pwm_devs.driver_channel_cur - 1]._timer_attr.periph;
    uint16_t timer_channel = pwm_devs.driver_channel[pwm_devs.driver_channel_cur - 1]._timer_attr.channel;
    dma_channel_enum dma_channel = pwm_devs.driver_channel[pwm_devs.driver_channel_cur - 1]._dma_attr.channel;

    if (dma_interrupt_flag_get(dma_channel, DMA_INT_FLAG_ERR)) {
        dma_interrupt_flag_clear(dma_channel, DMA_INT_FLAG_ERR);
    }
    if (dma_interrupt_flag_get(dma_channel, DMA_INT_FLAG_HTF)) {
        dma_interrupt_flag_clear(dma_channel, DMA_INT_FLAG_HTF);
    }
    if (dma_interrupt_flag_get(dma_channel, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(dma_channel, DMA_INT_FLAG_FTF);
        dma_interrupt_flag_clear(dma_channel, DMA_INT_FLAG_HTF);
        dma_interrupt_flag_clear(dma_channel, DMA_INT_FLAG_G);
        timer_channel_output_pulse_value_config(timer_periph,
                                                timer_channel,
                                                GD32F3X0_PWM_TIMER_SET(WS2812DEV));
        /* disable DMA channelx */
        dma_channel_disable(dma_channel);
        /* timer channel output disable config */
        timer_channel_output_state_config(timer_periph, timer_channel, TIMER_CCX_DISABLE);
        /* disable the timer*/
        timer_disable(timer_periph);
        timer_counter_value_config(timer_periph, 0);
        pwm_devs.trans_completion = 1;
    }
    if (dma_interrupt_flag_get(dma_channel, DMA_INT_FLAG_G)) {
        dma_interrupt_flag_clear(dma_channel, DMA_INT_FLAG_G);
    }
}

/******************************************************************************/
/**
 * @brief PWM 通道GPIO初始化
 *
 * @param pwm_dev_para
 * @return Rtv_Status
 */
static Rtv_Status _pwm_io_init(PwmDevType_t pwm_dev_para)
{
    uint32_t gpio_periph, gpio_pin;
    if (pwm_dev_para == NULL) {
        return EINVAL;
    }
    for (uint8_t pwm_dev_chnl_cnt = 0; pwm_dev_chnl_cnt < pwm_dev_para->driver_channel_num; pwm_dev_chnl_cnt++) {
        gpio_periph = pwm_dev_para->driver_channel[pwm_dev_chnl_cnt]._gpio_attr.port;
        gpio_pin = pwm_dev_para->driver_channel[pwm_dev_chnl_cnt]._gpio_attr.pin;
        /* gpio clk set */
        rcu_periph_clock_enable(pwm_dev_para->driver_channel[pwm_dev_chnl_cnt]._gpio_attr.clk);
        /* gpio mode set */
        gpio_mode_set(gpio_periph,
                      pwm_cfg_stc.gpio_cfg.mode,
                      pwm_cfg_stc.gpio_cfg.pull_up_down,
                      gpio_pin);
        /* gpio output set */
        gpio_output_options_set(gpio_periph,
                                pwm_cfg_stc.gpio_cfg.otype,
                                pwm_cfg_stc.gpio_cfg.speed,
                                gpio_pin);
        /* gpio alt func set */
        gpio_af_set(gpio_periph,
                    pwm_dev_para->driver_channel[pwm_dev_chnl_cnt]._gpio_attr.alt_func_num,
                    gpio_pin);
    }
    return SUCCESS;
}
/**
 * @brief PWM 通道Timer初始化
 *
 * @param pwm_dev_para
 * @return Rtv_Status
 */
static Rtv_Status _pwm_timer_init(PwmDevType_t pwm_dev_para)
{
    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocintpara;
    uint32_t timer_periph;
    uint16_t timer_channel, period_set, psc_set;

    if (pwm_dev_para == NULL) {
        return EINVAL;
    }
    period_set = (pwm_dev_para->driver_type == WS2812DEV ? GD32F3X0_PWM_TIMER_SET(WS2812_PWM_PERIOD):
                                                           TLC59108_PWM_PERIOD);
    psc_set = (pwm_dev_para->driver_type == WS2812DEV ? 0 : TLC59108_TIMER_PSC);

    for (uint8_t pwm_dev_chnl_cnt = 0; pwm_dev_chnl_cnt < pwm_dev_para->driver_channel_num; pwm_dev_chnl_cnt++) {
        timer_periph = pwm_dev_para->driver_channel[pwm_dev_chnl_cnt]._timer_attr.periph;
        timer_channel = pwm_dev_para->driver_channel[pwm_dev_chnl_cnt]._timer_attr.channel;

        rcu_periph_clock_enable(pwm_dev_para->driver_channel[pwm_dev_chnl_cnt]._timer_attr.clk);
        timer_deinit(timer_periph);

        memset((void *)&timer_initpara, 0, sizeof(timer_initpara));
        memset((void *)&timer_ocintpara, 0, sizeof(timer_ocintpara));

        /* timer base struct config */
        timer_initpara.prescaler = psc_set;
        timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
        timer_initpara.period = period_set;
        timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
        timer_initpara.counterdirection  = TIMER_COUNTER_UP;
        timer_initpara.repetitioncounter = 0;
        timer_init(timer_periph, &timer_initpara);

        /* clear the timer interrupt flag */
        timer_interrupt_flag_clear(timer_periph, TIMER_INT_FLAG_CH3);
        timer_interrupt_flag_clear(timer_periph, TIMER_INT_FLAG_CH2);
        timer_interrupt_flag_clear(timer_periph, TIMER_INT_FLAG_CH1);
        timer_interrupt_flag_clear(timer_periph, TIMER_INT_FLAG_CH0);
        timer_interrupt_flag_clear(timer_periph, TIMER_INT_FLAG_UP);
        /* timer oc para init */
        timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
        timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
        timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
        timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
        timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
        timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
        timer_channel_output_config(timer_periph, timer_channel, &timer_ocintpara);

        /* timer channel pulse value set */
        timer_channel_output_pulse_value_config(timer_periph, timer_channel, period_set * (1 - TIMER_PWM_REVERSE));
        /* timer channel pwm output mode set */
        timer_channel_output_mode_config(timer_periph, timer_channel, pwm_cfg_stc.timer_cfg.oc_mode);
        /* timer channel output shadow set */
        timer_channel_output_shadow_config(timer_periph, timer_channel, pwm_cfg_stc.timer_cfg.oc_shadow_set);

        /* timer update event set as the DMA request enable */
        timer_dma_enable(timer_periph, pwm_cfg_stc.timer_cfg.dma_trig_src_set);
        /* timer update event set as the DMA request source */
        timer_channel_dma_request_source_select(timer_periph, TIMER_DMAREQUEST_UPDATEEVENT);
        timer_update_source_config(timer_periph, TIMER_UPDATE_SRC_REGULAR);

        /* auto-reload preload enable */
        timer_auto_reload_shadow_enable(timer_periph);
        if (pwm_dev_para->driver_type == WS2812DEV) {
            /* disable the timer */
            timer_disable(timer_periph);
        } else if (pwm_dev_para->driver_type == TLC59108DEV){
            /* disable the timer */
            timer_enable(timer_periph);
        }
    }
    return SUCCESS;
}
/**
 * @brief PWM 通道DMA初始化
 *
 * @param pwm_dev_para
 * @return Rtv_Status
 */
static Rtv_Status _pwm_dma_init(PwmDevType_t pwm_dev_para)
{
    dma_channel_enum dma_channel;
    dma_parameter_struct dma_init_struct;

    if (pwm_dev_para == NULL) {
        return EINVAL;
    } else if (pwm_dev_para->driver_type == TLC59108DEV) {
        // TLC59108驱动 PWM设备不需要配置通道dma
        return SUCCESS;
    }
    
    for (uint8_t pwm_dev_chnl_cnt = 0; pwm_dev_chnl_cnt < pwm_dev_para->driver_channel_num; pwm_dev_chnl_cnt++) {
        // PWM设备中第5个通道不支持DMA
        if (pwm_dev_chnl_cnt == 4) {
            continue;
        }
        dma_channel = pwm_dev_para->driver_channel[pwm_dev_chnl_cnt]._dma_attr.channel;
        /* enable DMA clock */
        rcu_periph_clock_enable(pwm_cfg_stc.dma_cfg.clk);

        /* initialize DMA channelx */
        dma_deinit(dma_channel);

        memset((void *)&dma_init_struct, 0, sizeof(dma_init_struct));
        /* DMA channelx initialize */
        dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
        dma_init_struct.number       = 0;
        dma_init_struct.periph_addr  = pwm_dev_para->driver_channel[pwm_dev_chnl_cnt]._dma_attr.per_addr;
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
        dma_init_struct.priority     = DMA_PRIORITY_HIGH;
        dma_init(dma_channel, &dma_init_struct);

        /* configure DMA mode */
        dma_circulation_disable(dma_channel);
        dma_memory_to_memory_disable(dma_channel);

        /* clear the DMA transfer end flag and disable the half transfer interrupt */
        dma_interrupt_flag_clear(dma_channel, DMA_INT_FLAG_FTF);
        dma_interrupt_disable(dma_channel, DMA_INT_HTF);

        /* enable DMA transfer complete interrupt */
        dma_interrupt_enable(dma_channel, pwm_cfg_stc.dma_cfg.dma_int_src);

        nvic_irq_enable(pwm_dev_para->driver_channel[pwm_dev_chnl_cnt]._dma_attr.dma_int_irq, 0, 0);

        /* disable DMA channelx */
        dma_channel_disable(dma_channel);
    }
    return SUCCESS;
}

/******************************************************************************/
/**
 * @brief PWM设备内置初始化方法
 *
 * @param pwm_dev               PWM设备地址
 * @return Rtv_Status           @SUCCESS:初始化PWM成功 @ERROR:初始化PWM失败
 *
 * @note  初始化PWM相关的IO引脚、定时器以及DMA配置
 *        同时初始化完成量和当前PWM的信号输出通道
 */
static Rtv_Status _init_hard_pwm(void *pwm_dev)
{
    PwmDevType_t pwm_dev_para = (PwmDevType_t)pwm_dev;
    if (pwm_dev_para == NULL) {
        goto set_error;
    }

    if (_pwm_io_init(pwm_dev_para) != SUCCESS) {
        goto set_error;
    }
    if (_pwm_timer_init(pwm_dev_para) != SUCCESS) {
        goto set_error;
    }
    if (_pwm_dma_init(pwm_dev_para) != SUCCESS) {
        goto set_error;
    }
    pwm_dev_para->trans_completion = 0;
    pwm_dev_para->driver_channel_cur = PWM_CHX_INDEX_INIT_SET;

    return SUCCESS;
set_error:
    return ERROR;
}
/**
 * @brief PWM设备内置写PWM方法
 *
 * @param pwm_dev               PWM设备地址
 * @param pwm_channel_index     传入写PWM通道的序号(1~4/6~8)
 * @param buffer                写入的数据
 * @param size                  写入数据的大小
 * @return Size_Type            返回成功写入的字节数
 */
static Size_Type _write_pwm(void *pwm_dev,
                            const uint8_t pwm_channel_index,
                            const void *buffer,
                            const uint32_t size)
{
    PwmDevType_t pwm_dev_para = (PwmDevType_t)pwm_dev;
    dma_channel_enum channel;
    uint16_t *data = (uint16_t *)buffer;

    if (pwm_dev_para == NULL ||
        pwm_channel_index > pwm_dev_para->driver_channel_num ||
        pwm_channel_index == 0 ||
        data == NULL ||
        size == 0) {
        return EINVAL;
    }

    pwm_dev_para->driver_channel_cur = pwm_channel_index;
    channel = pwm_dev_para->driver_channel[pwm_channel_index - 1]._dma_attr.channel;
    for (uint32_t cnt = 0; cnt < size; cnt++) {
        if (data[cnt] == 0) {
            #if ( defined(TIMER_PWM_REVERSE) && (TIMER_PWM_REVERSE == 0x01) )
            data[cnt] = GD32F3X0_PWM_TIMER_SET(WS2812_PWM_PERIOD);
            #else
            data[cnt] = 0;
            #endif
        } else {
            /* 根据高低电平时间转化为对应的比较器值 */
            #if ( defined(TIMER_PWM_REVERSE) && (TIMER_PWM_REVERSE == 0x01) )
            data[cnt] = GD32F3X0_PWM_TIMER_SET(WS2812_PWM_PERIOD) - GD32F3X0_PWM_TIMER_SET(data[cnt]);
            #else
            data[cnt] = GD32F3X0_PWM_TIMER_SET(data[cnt]);
            #endif
        }
    }

    pwm_dev_para->trans_completion = 0;

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
    timer_channel_output_state_config(pwm_dev_para->driver_channel[pwm_channel_index - 1]._timer_attr.periph,
                                      pwm_dev_para->driver_channel[pwm_channel_index - 1]._timer_attr.channel,
                                      TIMER_CCX_ENABLE);

    /* enable the timer*/
    timer_enable(pwm_dev_para->driver_channel[pwm_channel_index - 1]._timer_attr.periph);

    while(pwm_dev_para->trans_completion == 0);

    /* return the calculate tansfered bytes */
    return (size - dma_transfer_number_get(channel));
}
/**
 * @brief PWM设备内置控制PWM方法
 *
 * @param pwm_dev               PWM设备地址
 * @param pwm_channel_index     控制的PWM通道序号(1~8)
 * @param cmd                   控制命令 @PWMCtrlCmdType
 * @param arg                   控制参数
 * @return Rtv_Status           返回控制的结果 @SUCCESS:成功 @其他:失败
 */
static Rtv_Status _control_pwm(void *pwm_dev,
                               const uint8_t pwm_channel_index,
                               const PWMCtrlCmdType cmd,
                               void *arg)
{
    PwmDevType_t pwm_dev_para = (PwmDevType_t)pwm_dev;
    PWMChannelBaseAttrType_t _base_attr = NULL;
    uint32_t timer_periph, pulse, period;
    uint16_t timer_channel;

    if ((pwm_dev_para == NULL ||
        pwm_channel_index > pwm_dev_para->driver_channel_num ||
        pwm_channel_index == 0) &&
        cmd != PWM_CTRL_SET_DRV_TYPE &&
        cmd != PWM_CTRL_DEV_DEINIT) {
        return EINVAL;
    }

    if (cmd != PWM_CTRL_SET_DRV_TYPE && cmd != PWM_CTRL_DEV_DEINIT) {
        timer_periph = pwm_dev_para->driver_channel[pwm_channel_index - 1]._timer_attr.periph;
        timer_channel = pwm_dev_para->driver_channel[pwm_channel_index - 1]._timer_attr.channel;
    }

    switch(cmd)
    {
        case PWM_CTRL_ENABLE:
            /* timer channel output enable config */
            timer_channel_output_state_config(timer_periph, timer_channel, TIMER_CCX_ENABLE);
        break;
        case PWM_CTRL_DISABLE:
            /* timer channel output disable config */
            timer_channel_output_state_config(timer_periph, timer_channel, TIMER_CCX_DISABLE);
        break;
        case PWM_CTRL_SET_BASEATTR:
            if (arg == NULL) {
                return EINVAL;
            }
            _base_attr = (PWMChannelBaseAttrType_t)arg;

            /* period vaild check */
            if (_base_attr->set_period <= 0 || _base_attr->set_period < (1000/FTIMER)) {
                period = 0;
            } else {
                if (pwm_dev_para->driver_type == WS2812DEV) {
                    period = GD32F3X0_PWM_TIMER_SET(_base_attr->set_period);
                } else if (pwm_dev_para->driver_type == TLC59108DEV) {
                    period = _base_attr->set_period;
                }
            }
            timer_autoreload_value_config(timer_periph, period);

            /* pulse vaild check */
            if (_base_attr->set_pulse <= 0 || _base_attr->set_pulse < (1000/FTIMER)) {
                pulse = 0;
            } else if (_base_attr->set_pulse >= _base_attr->set_period) {
                if (pwm_dev_para->driver_type == WS2812DEV) {
                    pulse = GD32F3X0_PWM_TIMER_SET(_base_attr->set_period);
                } else if (pwm_dev_para->driver_type == TLC59108DEV) {
                    pulse = _base_attr->set_period;
                }
            } else {
                if (pwm_dev_para->driver_type == WS2812DEV) {
                    pulse = GD32F3X0_PWM_TIMER_SET(_base_attr->set_pulse);
                } else if (pwm_dev_para->driver_type == TLC59108DEV) {
                    pulse = _base_attr->set_pulse;
                }
            }

            timer_channel_output_pulse_value_config(timer_periph, timer_channel, pulse);
        break;
        case PWM_CTRL_SET_DRV_TYPE:
            pwm_dev_para->driver_type = (DriverDevType)(*(uint8_t *)arg);
        break;
        case PWM_CTRL_DEV_DEINIT:
            _pwm_timer_init(pwm_dev_para);
            _pwm_dma_init(pwm_dev_para);
        break;
        default:
        break;
    }
    return SUCCESS;
}
/******************************************************************************/
Rtv_Status init_pwm_dev(void)
{
    return pwm_devs.init((void *)&pwm_devs);
}

PwmDevType_t find_pwm_dev(void)
{
    return &pwm_devs;
}
