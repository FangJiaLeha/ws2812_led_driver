#include "ws2812.h"

#define RESET_TIME_SET          (300u)          // ws2812 reset time 300us
#define RESET_BITS_CNT          (RESET_TIME_SET * 1000 / TIMER_PERIOD)

uint16_t pulse_buff[RESET_BITS_CNT*2 + (WS2812_LED_NUM + WS2812_RETAIN_LED_NUM) * 3 * 8] = {0};

/******************************************************************************/
static Rtv_Status init_ws2812_hardware(uint8_t ws2812_index,
                                       uint32_t pwm_period,
                                       uint32_t pwm_pulse)
{
    Rtv_Status rtv_status = SUCCESS;
    struct pwm_base_attr pwm_base_attr;
    
    init_pwm();

    memset((void *)&pwm_base_attr, 0, sizeof(pwm_base_attr));
    pwm_base_attr.set_period = pwm_period;
    pwm_base_attr.set_pulse = pwm_pulse;
    rtv_status = control_pwm(ws2812_index - 1, PWM_CMD_SET, (void *)&pwm_base_attr);
    return rtv_status;
}

/******************************************************************************/
static Rtv_Status _update_led_data(ws2812_dev_t ws_dev, uint32_t count)
{
    Rtv_Status result = SUCCESS;
    Size_Type write_bytes;
    uint32_t need_write_pwm_num = 0;

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
                pulse_buff[RESET_BITS_CNT + (3*idx + 0)*8 + i] = PWM_ONE;
            }
            else {
                pulse_buff[RESET_BITS_CNT + (3*idx + 0)*8 + i] = PWM_ZERO;
            }
        }
        for (uint8_t i = 0; i < 8; i++)
        {
            if (ws_dev->dev_attr.render_buff[idx][0] & (1U << (7 - i)))
            {
                pulse_buff[RESET_BITS_CNT + (3*idx + 1)*8 + i] = PWM_ONE;
            }
            else {
                pulse_buff[RESET_BITS_CNT + (3*idx + 1)*8 + i] = PWM_ZERO;
            }
        }
        for (uint8_t i = 0; i < 8; i++)
        {
            if (ws_dev->dev_attr.render_buff[idx][2] & (1U << (7 - i)))
            {
                pulse_buff[RESET_BITS_CNT + (3*idx + 2)*8 + i] = PWM_ONE;
            }
            else {
                pulse_buff[RESET_BITS_CNT + (3*idx + 2)*8 + i] = PWM_ZERO;
            }
        }
    }

    // 计算需要写的pwm个数
    need_write_pwm_num = RESET_BITS_CNT * 2 + count * 3 * 8;

    // 此处必须发送整个pulse_buff 因为需要前后包含两端复位时间 否则灯带显示异常
    write_bytes = write_pwm(ws_dev->dev_attr.index - 1, pulse_buff, need_write_pwm_num);

    if (write_bytes != need_write_pwm_num) {
        return ERROR;
    }

    return result;
}

/******************************************************************************/
Rtv_Status _init(void *dev)
{
    ws2812_dev_t wsdev = (ws2812_dev_t)dev;
    Rtv_Status rtv_status = SUCCESS;
    if (NULL == wsdev) {
        return EINVAL;
    }

    rtv_status = init_ws2812_hardware(wsdev->dev_attr.index, 1250, 0);
    return rtv_status;
}

Size_Type  _write(void *dev, Offset_Type pos, const void *buffer, Size_Type size)
{
    ws2812_dev_t wsdev = (ws2812_dev_t)dev;
    uint8_t (*dis_buff)[3] = (uint8_t (*)[3])buffer;

    if( pos >= wsdev->dev_attr.led_num || size  == 0 )
    {
        return 0;
    }

    if( ( pos + size ) > wsdev->dev_attr.led_num )
    {
        size = wsdev->dev_attr.led_num - pos;
    }

    for( int i = pos; i < ( pos + size ); i++ )
    {
        wsdev->dev_attr.render_buff[i][0] = dis_buff[i-pos][0];
        wsdev->dev_attr.render_buff[i][1] = dis_buff[i-pos][1];
        wsdev->dev_attr.render_buff[i][2] = dis_buff[i-pos][2];
    }
    _update_led_data(wsdev, size);
    return size;
}

Rtv_Status _control(void *dev, int cmd, void *arg)
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
        update_status = _update_led_data(ws_dev, ws_dev->dev_attr.led_num);
        if (update_status == SUCCESS) {
            memset(ws_dev->dev_attr.render_buff, 0, ws_dev->dev_attr.led_num * 3);
        }
    } else if( cmd == WS2812_CTRL_UPDATE_DEVDATA ) {
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
        _update_led_data(ws_dev, ws_dev->dev_attr.ctrl_led_num);
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
        .need_update = 0,
        .index = 1,
    },
    .ws2812_dev_ops = 
    {
        .init = _init,
        .open = NULL,
        .close = NULL,
        .read = NULL,
        .write = _write,
        .control = _control,
    }
};

/******************************************************************************/
ws2812_dev_t find_ws2812_dev(void)
{
    return &WS2812_DEV;
}

