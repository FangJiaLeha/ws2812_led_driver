#include "ws2812.h"

#define RESET_TIME_SET          (300u)                                  // ws2812 reset time 300us
#define RESET_BITS_CNT          (RESET_TIME_SET * 1000 / WS2812_PWM_PERIOD)

/**
 * @brief 发送给WS2812的数据buff
 *
 */
uint16_t pulse_buff[RESET_BITS_CNT*2 + (WS2812_LED_NUM + WS2812_RETAIN_LED_NUM) * 3 * 8] = {0};

/******************************************************************************/
/**
 * @brief 内置初始化WS2812硬件接口
 *
 * @param ws2812_index      WS2812灯条序号
 * @param pwm_period        WS2812的驱动PWM波周期值
 * @param pwm_pulse         WS2812的驱动PWM波占空比
 * @return Rtv_Status       返回值 @SUCCESS:初始化成功 @其他:初始化失败
 */
static Rtv_Status init_ws2812_hardware(ws2812_dev_t ws_dev_para,
                                       uint32_t pwm_period,
                                       uint32_t pwm_pulse)
{
    PwmDevType_t temp_pwm_dev = NULL;
    Rtv_Status rtv_status = SUCCESS;
    PWMChannelBaseAttrType ws2812_pwm_channel_base_attr = {0};
    uint8_t ws2812_index = 1;

    if (ws_dev_para == NULL) {
        return EINVAL;
    }

    temp_pwm_dev = (PwmDevType_t)ws_dev_para->dev_attr.private;
    if (temp_pwm_dev == NULL) {
        return EINVAL;
    }

    ws2812_index = ws_dev_para->dev_attr.index;
    ws2812_pwm_channel_base_attr.set_period = pwm_period;
    ws2812_pwm_channel_base_attr.set_pulse = pwm_pulse;
    rtv_status = temp_pwm_dev->control(temp_pwm_dev,
                                       ws2812_index,
                                       PWM_CTRL_SET_BASEATTR,
                                       (void*)&ws2812_pwm_channel_base_attr);
    return rtv_status;
}

/**
 * @brief 内置WS2812数据更新方法
 *
 * @param ws_dev        WS2812设备地址
 * @param count         控制WS2812的灯数
 * @return Rtv_Status   返回值 @SUCCESS:更新成功 @其他值:更新失败
 */
static Rtv_Status _update_led_data(ws2812_dev_t ws_dev, uint32_t count)
{
    PwmDevType_t temp_pwm_dev = NULL;
    Size_Type write_bytes;
    uint32_t need_write_pwm_num = 0;

    if (ws_dev == NULL || ws_dev->dev_attr.index_enable == 0) {
        return EINVAL;
    }
    // 获取PWM设备
    temp_pwm_dev = (PwmDevType_t)ws_dev->dev_attr.private;
    if (temp_pwm_dev == NULL) {
        return EINVAL;
    }

    if( count > ws_dev->dev_attr.led_num )
    {
        count -= ws_dev->dev_attr.led_num;
    }

    memset(pulse_buff, 0, ITEM_NUM(pulse_buff));
    for (uint16_t idx = 0; idx < count; idx++)
    {
        //GRB <==> RGB
        for (uint8_t i = 0; i < 8; i++)
        {
            if (ws_dev->dev_attr.render_buff[idx][1] & (1U << (7 - i)))
            {
                pulse_buff[RESET_BITS_CNT + (3*idx + 0)*8 + i] = WS2812_PWM_ONE;
            }
            else {
                pulse_buff[RESET_BITS_CNT + (3*idx + 0)*8 + i] = WS2812_PWM_ZERO;
            }
        }
        for (uint8_t i = 0; i < 8; i++)
        {
            if (ws_dev->dev_attr.render_buff[idx][0] & (1U << (7 - i)))
            {
                pulse_buff[RESET_BITS_CNT + (3*idx + 1)*8 + i] = WS2812_PWM_ONE;
            }
            else {
                pulse_buff[RESET_BITS_CNT + (3*idx + 1)*8 + i] = WS2812_PWM_ZERO;
            }
        }
        for (uint8_t i = 0; i < 8; i++)
        {
            if (ws_dev->dev_attr.render_buff[idx][2] & (1U << (7 - i)))
            {
                pulse_buff[RESET_BITS_CNT + (3*idx + 2)*8 + i] = WS2812_PWM_ONE;
            }
            else {
                pulse_buff[RESET_BITS_CNT + (3*idx + 2)*8 + i] = WS2812_PWM_ZERO;
            }
        }
    }

    // 计算需要写的pwm个数
    need_write_pwm_num = RESET_BITS_CNT * 2 + count * 3 * 8;

    // 此处必须发送整个pulse_buff 因为需要前后包含两端复位时间 否则灯带显示异常
    write_bytes = temp_pwm_dev->write(temp_pwm_dev,
                                      ws_dev->dev_attr.index,
                                      pulse_buff,
                                      need_write_pwm_num);
    if (write_bytes != need_write_pwm_num) {
        return ERROR;
    }

    // 常规灯效控制下 失能下一次通道输出
    if (ws_dev->dev_attr.render_loop == 0) {
        ws_dev->dev_attr.index_enable = 0;
    }
    return SUCCESS;
}

/******************************************************************************/
/**
 * @brief 内置初始化ws2812设备方法
 *
 * @param dev             WS2812设备地址
 * @return Rtv_Status     返回值  @SUCCESS:初始化设备成功   @其他值:初始化设备失败
 */
static Rtv_Status _init(void *dev)
{
    ws2812_dev_t wsdev = (ws2812_dev_t)dev;
    Rtv_Status rtv_status = SUCCESS;
    if (NULL == wsdev) {
        return EINVAL;
    }

    rtv_status = init_ws2812_hardware(wsdev, 1250, 0);
    return rtv_status;
}

/**
 * @brief 内置控制ws2812设备方法
 *
 * @param dev           WS2812设备地址
 * @param cmd           控制WS2812设备的命令 @ws2812_cmd_list
 * @param arg           控制参数
 * @return Rtv_Status   返回值  @SUCCESS:控制成功  @其他值:控制失败
 */
static Rtv_Status _control(void *dev, int cmd, void *arg)
{
    ws2812_dev_t ws_dev = (ws2812_dev_t)dev;
    Rtv_Status update_status;
    uint32_t *led_num = (uint32_t *)arg;

    if( cmd == WS2812_CTRL_INIT )
    {
        if( led_num == NULL || *led_num == 0 )
        {
            return EINVAL;
        }
        if( ws_dev->dev_attr.render_buff != NULL )
        {
            free( ws_dev->dev_attr.render_buff );
            ws_dev->dev_attr.render_buff = NULL;
        }
        ws_dev->dev_attr.render_buff = (uint8_t (*)[3])malloc( ( *led_num ) * 3  );
        if( ws_dev->dev_attr.render_buff == NULL )
        {
            return ENOMEM;
        }
        ws_dev->dev_attr.led_num = *led_num;
        ws_dev->dev_attr.ctrl_led_num = *led_num;
        memset( ws_dev->dev_attr.render_buff, 0, ( *led_num ) * 3 );
      } else if( cmd == WS2812_CTRL_BAR_COLOR ) {
        struct ws2812_bar_ctrlpack *pack = (struct ws2812_bar_ctrlpack *)arg;

        if( pack->count == 0 || pack == NULL)
        {
            return EINVAL;
        }
        if (pack->start >= ws_dev->dev_attr.led_num) {
            pack->start -= ws_dev->dev_attr.led_num;
        }
        if( pack->start + pack->count > ws_dev->dev_attr.led_num )
        {
            pack->count = ws_dev->dev_attr.led_num - pack->start;
        }
        for( uint8_t i = pack->start; i < ( pack->count + pack->start); i++ )
        {
            ws_dev->dev_attr.render_buff[i][0] = pack->color[0];
            ws_dev->dev_attr.render_buff[i][1] = pack->color[1];
            ws_dev->dev_attr.render_buff[i][2] = pack->color[2];
        }
        // 常规控制模式下 关闭下一次通道输出
        ws_dev->dev_attr.render_loop = 0;
        update_status = _update_led_data(ws_dev, ws_dev->dev_attr.led_num);
        if (update_status == SUCCESS) {
            memset(ws_dev->dev_attr.render_buff, 0, ws_dev->dev_attr.led_num * 3);
        }
    } else if( cmd == WS2812_CTRL_UPDATE_DEVDATA ) {
        // 渲染控制模式下 使能下一次通道输出
        ws_dev->dev_attr.render_loop = 1;
        update_status = _update_led_data(ws_dev, ws_dev->dev_attr.ctrl_led_num );
        if (update_status == SUCCESS) {
            memset(ws_dev->dev_attr.render_buff, 0, ws_dev->dev_attr.ctrl_led_num * 3);
        }
    } else if ( cmd == WS2812_CTRL_GET_DISBUFF ) {
        uint8_t (**dis_buff)[3] = arg;
        *dis_buff = ws_dev->dev_attr.render_buff;
    } else if ( cmd == WS2812_LED_NUM_RESET ) { // 增加一个重新设置ws2812灯珠数量方法
        uint8_t ws2812_led_num_reset = *(uint8_t *)arg;

        // 设置led数量前 关闭灯效
        memset( ws_dev->dev_attr.render_buff, 0, ws_dev->dev_attr.led_num * 3 );
        ws_dev->dev_attr.index_enable = 0x01; // 使能通道输出
        _update_led_data(ws_dev, ws_dev->dev_attr.ctrl_led_num);
        ws_dev->dev_attr.index_enable = 0x00; // 关闭通道输出
        free(ws_dev->dev_attr.render_buff);

        ws_dev->dev_attr.render_buff = (uint8_t (*)[3])malloc(ws2812_led_num_reset * 3);
        if( ws_dev->dev_attr.render_buff == NULL )
        {
            return ENOMEM;
        }
        ws_dev->dev_attr.led_num = ws2812_led_num_reset;
        ws_dev->dev_attr.ctrl_led_num = ws2812_led_num_reset;
        memset( ws_dev->dev_attr.render_buff, 0, ws2812_led_num_reset * 3 );
    }

    return SUCCESS;
}
/******************************************************************************/
/* definition the ws2812_dev */
struct ws2812_dev WS2812_DEV = 
{
    .dev_attr = 
    {
        .led_num = 0,
        .ctrl_led_num = 0,
        .dma_buff = NULL,
        .render_buff = NULL,
        .index = 1,          // WS2812灯条序号
        .index_enable = 0,   // 默认设置该灯条输出失能
        .render_loop = 0,    // 默认设置 渲染模式下 失能下一次通道输出
        .private = NULL      // 用于存储pwm设备地址
    },
    .ws2812_dev_ops = 
    {
        .init = _init,
        .control = _control,
    },
};

/******************************************************************************/
ws2812_dev_t find_ws2812_dev(void)
{
    return &WS2812_DEV;
}

