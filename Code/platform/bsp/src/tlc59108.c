/**
 * @file tlc59108.c
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-16
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "tlc59108.h"

/******************************************************************************/
/**
 * @brief 内置初始化TLC59108硬件接口
 *
 * @return RtvStatus       返回值 @SUCCESS:初始化成功 @其他:初始化失败
 */
static RtvStatus init_tlc59108_hardware(tlc59108_dev_t ws_dev_para)
{
    pwm_dev_t temp_pwm_dev = NULL;
    driver_dev_type set_driver_type = TLC59108DEV;
    RtvStatus rtv_status = SUCCESS;
    if (ws_dev_para == NULL)
    {
        return EINVAL;
    }

    temp_pwm_dev = (pwm_dev_t)ws_dev_para->dev_attr.pwm_dev_addr;
    if (temp_pwm_dev == NULL)
    {
        return EINVAL;
    }
    // 设置pwm设备的驱动设备类型为tlc59108
    rtv_status = temp_pwm_dev->control(temp_pwm_dev,
                                       0,
                                       PWM_CTRL_SET_DRV_TYPE,
                                       (void *)&set_driver_type);
    // 调用pwm设备的驱动设备重新初始化方法
    rtv_status = temp_pwm_dev->deinit((void *)temp_pwm_dev);
    return rtv_status;
}

/**
 * @brief 内置TLC59108数据更新方法
 *
 * @param tlc_dev       TLC59108设备地址
 * @return RtvStatus   返回值 @SUCCESS:更新成功 @其他值:更新失败
 */
static RtvStatus _update_led_data(tlc59108_dev_t tlc_dev)
{
    pwm_dev_t temp_pwm_dev = NULL;
    pwm_channel_base_attr_type pwm_channel_base_attr = {0};
    if (tlc_dev == NULL)
    {
        return EINVAL;
    }

    // 获取PWM设备
    temp_pwm_dev = (pwm_dev_t)tlc_dev->dev_attr.pwm_dev_addr;
    if (temp_pwm_dev == NULL)
    {
        return EINVAL;
    }
    // 设置周期
    pwm_channel_base_attr.set_period = TLC59108_PWM_PERIOD;
    for (uint8_t pwmx_channel_cnt = 0; pwmx_channel_cnt < tlc_dev->dev_attr.pwmx_max_num; pwmx_channel_cnt++)
    {
        pwm_channel_base_attr.set_pulse = *(tlc_dev->dev_attr.pwmx)++;
        temp_pwm_dev->control(temp_pwm_dev,
                              tlc_dev->dev_attr.pwmx_start_index + pwmx_channel_cnt,
                              PWM_CTRL_SET_BASEATTR,
                              (void *)&pwm_channel_base_attr);
    }
    return SUCCESS;
}

/******************************************************************************/
/**
 * @brief 内置初始化tlc59108设备方法
 *
 * @param dev             TLC59108设备地址
 * @return RtvStatus     返回值  @SUCCESS:初始化设备成功   @其他值:初始化设备失败
 */
static RtvStatus _init(void *dev)
{
    tlc59108_dev_t tlcdev = (tlc59108_dev_t)dev;
    RtvStatus rtv_status = SUCCESS;
    if (NULL == tlcdev)
    {
        return EINVAL;
    }

    rtv_status = init_tlc59108_hardware(tlcdev);
    return rtv_status;
}

/**
 * @brief 内置控制tlc59108设备方法
 *
 * @param dev           TLC59108设备地址
 * @param cmd           控制TLC59108设备的命令 @ TLC59108CtrlCmdType
 * @param arg           控制参数
 * @return RtvStatus   返回值  @SUCCESS:控制成功  @其他值:控制失败
 */
static RtvStatus _control(void *dev,
                           const uint8_t cmd,
                           void *arg)
{
    tlc59108_dev_t tlc_dev = (tlc59108_dev_t)dev;
    RtvStatus update_status = SUCCESS;

    if (tlc_dev == NULL)
    {
        return EINVAL;
    }
    if (cmd == TLC59108_CTRL_INIT)
    {
        if (tlc_dev->dev_attr.pwmx != NULL)
        {
            free(tlc_dev->dev_attr.pwmx);
            tlc_dev->dev_attr.pwmx = NULL;
        }
        if (arg != NULL && *(uint8_t *)arg != 0)
        {
            tlc_dev->dev_attr.pwmx_max_num = *(uint8_t *)arg;
        }
        tlc_dev->dev_attr.pwmx = (uint8_t *)malloc(sizeof(uint8_t) * tlc_dev->dev_attr.pwmx_max_num);
        if (tlc_dev->dev_attr.pwmx == NULL)
        {
            return ENOMEM;
        }
        memset(tlc_dev->dev_attr.pwmx, 0, sizeof(uint8_t) * tlc_dev->dev_attr.pwmx_max_num);
    }
    else if (cmd == TLC59108_CTRL_UPDATE_DEVDATA)
    {
        update_status = _update_led_data(tlc_dev);
    }
    else if (cmd == TLC59108_CTRL_GET_PWMx)
    {
        *(uint8_t **)arg = tlc_dev->dev_attr.pwmx;
    }

    return update_status;
}
/******************************************************************************/
/* definition the tlc59108_dev */
tlc59108_dev_type TLC59108_DEV =
{
    .dev_attr =
    {
        .pwmx_start_index = 0x01u,                 // 默认起始通道为第一通道
        .pwmx_max_num = TLC59108_CHANNNEL_MAX_NUM, // 最大通道数
        .pwmx = NULL,                              // 用于保存pwm0~pwm7 8个通道的占空比
        .pwm_dev_addr = NULL                            // 用于存储pwm设备地址
    },
    .tlc59108_dev_ops =
    {
        .init = _init,
        .control = _control,
    }
};

/******************************************************************************/
tlc59108_dev_t find_tlc59108_dev(void)
{
    return &TLC59108_DEV;
}
