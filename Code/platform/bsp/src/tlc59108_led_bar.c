/**
 * @file tlc59108_led_bar.c
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-16
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "tlc59108_led_bar.h"

//==============================================================================
/**
 * @brief 全局TLC59108灯条对象 主要用于渲染任务中
 *
 */
static tlc59108_bar_type_t TLC59108_BAR = NULL;

//==============================================================================
/**
 * @brief TLC59108灯条的blink方法
 *
 * @param bar               TLC59108灯条对象地址
 * @param led_out_state     TLC59108灯条通道输出状态寄存器0/1值
 * @param pwmx_value        TLC59108灯条8个通道的pwmx值
 * @return RtvStatus       @SUCCESS:当前blink模式参数设置有效 @其他值:当前blink模式参数设置无效
 */
static RtvStatus _tlc59108_blink(void *bar,
                                  const uint16_t led_out_state,
                                  const uint8_t *pwmx_value)
{
    tlc59108_bar_type_t tbar = (tlc59108_bar_type_t)bar;
    tlc59108_dev_t tlc_dev = NULL;
    if (tbar == NULL || pwmx_value == NULL)
    {
        return EINVAL;
    }

    tlc_dev = (tlc59108_dev_t)tbar->parent.led_bar_dev_addr;
    if (tlc_dev == NULL)
    {
        return EINVAL;
    }
    tbar->render_switch = 0;
    for (uint8_t channel_cnt = 0; channel_cnt < tbar->channel_num; channel_cnt++)
    {
        tbar->render_param.led_ldrx[channel_cnt] = (led_out_state >> (channel_cnt * 2)) & 0x03u;
        switch (tbar->render_param.led_ldrx[channel_cnt])
        {
        /**
         * @brief led driver turn down
         */
        case 0:
            tbar->pwmx_databuff[channel_cnt] = 0;
            break;
        /**
         * @brief led driver is fully on
         */
        case 1:
            tbar->pwmx_databuff[channel_cnt] = TLC59108_PWM_PERIOD;
            break;
        /**
         * @brief led driver dividual brightness can be controlled through its PWMx register
         */
        case 2:
            tbar->pwmx_databuff[channel_cnt] = *pwmx_value++;
            tbar->render_param.pwmx_blink_period[channel_cnt] = 0;
            tbar->render_param.pwmx_blink_duty[channel_cnt] = 0;
            break;
        /**
         * @brief led driver individual brightness and group dimming/blinking can be controlled
         *        through its PWMx register and the GRPPWM registers
         */
        case 3:
            tbar->pwmx_databuff[channel_cnt] = *pwmx_value++;
            tbar->render_param.pwmx_blink_period[channel_cnt] =
                tbar->render_param.group_freq_ctrl_reg;
            tbar->render_param.pwmx_blink_duty[channel_cnt] =
                tbar->render_param.group_duty_ctrl_reg;
            tbar->render_switch = 1;
            break;
        default:
            break;
        }
    }

    if (tbar->render_switch == 0)
    {
        tlc_dev->tlc59108_dev_ops.control(tlc_dev,
                                          TLC59108_CTRL_UPDATE_DEVDATA,
                                          NULL);
    }

    return SUCCESS;
}

/**
 * @brief TLC59108灯条的dimming方法
 *
 * @param bar               TLC59108灯条对象地址
 * @param pwmx_value        TLC59108灯条8个通道的pwmx值
 * @return RtvStatus       @SUCCESS:当前breath模式参数设置有效 @其他值:当前breath模式参数设置无效
 */
static RtvStatus _tlc59108_dimming(void *bar, const uint8_t *pwmx_value)
{
    tlc59108_bar_type_t tbar = (tlc59108_bar_type_t)bar;
    tlc59108_dev_t tlc_dev = NULL;
    if (tbar == NULL || pwmx_value == NULL)
    {
        return EINVAL;
    }

    tlc_dev = (tlc59108_dev_t)tbar->parent.led_bar_dev_addr;
    if (tlc_dev == NULL)
    {
        return EINVAL;
    }

    tbar->render_switch = 0;
    for (uint8_t channel_cnt = 0; channel_cnt < tbar->channel_num; channel_cnt++)
    {
        tbar->pwmx_databuff[channel_cnt] = *pwmx_value++;
    }

    tlc_dev->tlc59108_dev_ops.control(tlc_dev,
                                      TLC59108_CTRL_UPDATE_DEVDATA,
                                      NULL);
    return SUCCESS;
}

//==============================================================================
void tlc59108_render(void)
{
    static uint16_t blink_high_level_time[8] = {0};
    static uint16_t blink_high_level_cnt[8] = {0};
    uint8_t blink_period = 0;
    uint8_t blink_duty = 0;
    if (TLC59108_BAR->render_switch == 0)
    {
    }
    else
    {
        for (uint8_t channel_num = 0; channel_num < TLC59108_BAR->channel_num; channel_num++)
        {
            blink_period = TLC59108_BAR->render_param.pwmx_blink_period[channel_num];
            if (blink_period)
            {
                blink_duty = TLC59108_BAR->render_param.pwmx_blink_duty[channel_num];
                blink_high_level_time[channel_num] = blink_period * blink_duty * 50 / TLC59108_PWM_PERIOD;
                if (blink_high_level_cnt[channel_num] <= blink_high_level_time[channel_num])
                {
                }
                else
                {
                    TLC59108_BAR->pwmx_databuff[channel_num] = 0;
                }

                if (++blink_high_level_cnt[channel_num] >= blink_period * 50)
                {
                    blink_high_level_cnt[channel_num] = 0;
                }
            }
        }
        tlc59108_dev_t tlc_dev = (tlc59108_dev_t)TLC59108_BAR->parent.led_bar_dev_addr;
        if (tlc_dev == NULL)
        {
            return;
        }
        tlc_dev->tlc59108_dev_ops.control(tlc_dev,
                                          TLC59108_CTRL_UPDATE_DEVDATA,
                                          NULL);
    }
}

RtvStatus init_tlc59108_bar(tlc59108_bar_type_t tbar,
                             uint8_t id,
                             RtvStatus (*set_color)(led_bar_t bar, float *color),
                             void *priv_data,
                             const uint8_t channel_num)
{
    tlc59108_dev_t tlcdev = NULL;
    led_bar_t bar = (led_bar_t)tbar;

    if (bar == NULL ||
        priv_data == NULL ||
        channel_num > TLC59108_CHANNNEL_MAX_NUM)
    {
        return EINVAL;
    }

    // 调用tlc59108_bar父类led_bar的初始化灯条方法
    init_led_bar(bar, id, set_color, priv_data);

    tbar->pwmx_databuff = NULL;
    tlcdev = (tlc59108_dev_t)priv_data;
    // 使用二级指针 使该tlc59108_bar对象的dis_buff显存 可以 直接操作 tlc59108.c中的pwmx数据
    tlcdev->tlc59108_dev_ops.control(tlcdev,
                                     TLC59108_CTRL_GET_PWMx,
                                     (void *)&tbar->pwmx_databuff);
    if (tbar->pwmx_databuff == NULL)
    {
        return ERROR;
    }

    tbar->render_switch = 0;
    tbar->channel_num = channel_num;
    // TLC59108灯条对象中闪烁占空比属性初始化
    if (tbar->render_param.pwmx_blink_duty != NULL)
    {
        free(tbar->render_param.pwmx_blink_duty);
        tbar->render_param.pwmx_blink_duty = NULL;
    }
    tbar->render_param.pwmx_blink_duty = (uint8_t *)malloc(sizeof(uint8_t) * channel_num);
    if (tbar->render_param.pwmx_blink_duty == NULL)
    {
        return ENOMEM;
    }
    memset(tbar->render_param.pwmx_blink_duty, 0, sizeof(uint8_t) * channel_num);
    // TLC59108灯条对象中闪烁周期属性初始化
    if (tbar->render_param.pwmx_blink_period != NULL)
    {
        free(tbar->render_param.pwmx_blink_period);
        tbar->render_param.pwmx_blink_period = NULL;
    }
    tbar->render_param.pwmx_blink_period = (uint8_t *)malloc(sizeof(uint8_t) * channel_num);
    if (tbar->render_param.pwmx_blink_period == NULL)
    {
        return ENOMEM;
    }
    memset(tbar->render_param.pwmx_blink_period, 0, sizeof(uint8_t) * channel_num);
    // TLC59108灯条对象中通道输出引脚状态属性初始化
    if (tbar->render_param.led_ldrx != NULL)
    {
        free(tbar->render_param.led_ldrx);
        tbar->render_param.led_ldrx = NULL;
    }
    tbar->render_param.led_ldrx = (uint8_t *)malloc(sizeof(uint8_t) * channel_num);
    if (tbar->render_param.led_ldrx == NULL)
    {
        return ENOMEM;
    }
    memset(tbar->render_param.led_ldrx, 0, sizeof(uint8_t) * channel_num);

    /* 初始化tlc59108_bar的blink/dimming方法 */
    tbar->blink = _tlc59108_blink;
    tbar->dimming = _tlc59108_dimming;

    TLC59108_BAR = tbar;

    return SUCCESS;
}
