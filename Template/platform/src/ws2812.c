#include "ws2812.h"

#define RESET_TIME_SET          (300u)          // ws2812 reset time 300us
#define RESET_BITS_CNT          (RESET_TIME_SET * 1000 / TIMER_PERIOD)

uint16_t pulse_buff[RESET_BITS_CNT*2 + WS2812_LED_NUM * 3 * 8] = {0};

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
static Rtv_Status _update_led_data(ws2812_dev_t ws_dev, uint32_t pos, uint32_t count)
{
    Rtv_Status result = SUCCESS;
    Size_Type write_bytes;
    uint32_t pulse_size;
    uint16_t idx = 0;
    uint8_t i, j, tmp;

    if( pos >= ws_dev->dev_attr.led_num )
    {
        return EINVAL;
    }

    if( ( pos + count ) > ws_dev->dev_attr.led_num )
    {
        count = ws_dev->dev_attr.led_num - pos;
    }

    for (idx = 0; idx < ws_dev->dev_attr.led_num; idx++)
    {
        //GRB <==> RGB
        for (j = 0; j < 8; j++)
        {
            if (ws_dev->dev_attr.render_buff[idx][1] & (1U << (7 - j)))
            {
                pulse_buff[RESET_BITS_CNT + (3*idx + 0)*8 + j] = PWM_ONE;
            }
            else {
                pulse_buff[RESET_BITS_CNT + (3*idx + 0)*8 + j] = PWM_ZERO;
            }
        }
        for (j = 0; j < 8; j++)
        {
            if (ws_dev->dev_attr.render_buff[idx][0] & (1U << (7 - j)))
            {
                pulse_buff[RESET_BITS_CNT + (3*idx + 1)*8 + j] = PWM_ONE;
            }
            else {
                pulse_buff[RESET_BITS_CNT + (3*idx + 1)*8 + j] = PWM_ZERO;
            }
        }
        for (j = 0; j < 8; j++)
        {
            if (ws_dev->dev_attr.render_buff[idx][2] & (1U << (7 - j)))
            {
                pulse_buff[RESET_BITS_CNT + (3*idx + 2)*8 + j] = PWM_ONE;
            }
            else {
                pulse_buff[RESET_BITS_CNT + (3*idx + 2)*8 + j] = PWM_ZERO;
            }
        }
    }
    write_bytes = write_pwm(ws_dev->dev_attr.index - 1, pulse_buff, ITEM_NUM(pulse_buff));

    if (write_bytes != sizeof(pulse_buff)/sizeof(pulse_buff[0])) {
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
    int i;

    if( pos >= wsdev->dev_attr.led_num || size  == 0 )
    {
        return 0;
    }

    if( ( pos + size ) > wsdev->dev_attr.led_num )
    {
        size = wsdev->dev_attr.led_num - pos;
    }

    for( i = pos; i < ( pos + size ); i++ )
    {
        wsdev->dev_attr.render_buff[i][0] = dis_buff[i-pos][0];
        wsdev->dev_attr.render_buff[i][1] = dis_buff[i-pos][1];
        wsdev->dev_attr.render_buff[i][2] = dis_buff[i-pos][2];
    }
    _update_led_data(wsdev, pos, size);
    return size;
}

Rtv_Status _control(void *dev, int cmd, void *arg)
{
    ws2812_dev_t ws_dev = (ws2812_dev_t)dev;
    Rtv_Status update_status;
    uint8_t bar_buff_idx;
    uint8_t led_count   = 0;
    uint8_t led_bar_idx = 0;
    int i = 0;
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
        memset( ws_dev->dev_attr.render_buff, 0, ( *led_num ) * 3 );
      } else if( cmd == WS2812_CTRL_BAR_COLOR ) {
        struct ws2812_bar_ctrlpack *pack = arg;
        uint32_t count;
        uint32_t i;
        
        if( pack->count == 0 || pack->start >= ws_dev->dev_attr.led_num )
        {
            return EINVAL;
        }
        count = pack->count;
        if( pack->start + pack->count > ws_dev->dev_attr.led_num )
        {
            count = ws_dev->dev_attr.led_num - pack->start;
        }
        for( i = pack->start; i < ( count + pack->start); i++ )
        {
            ws_dev->dev_attr.render_buff[i][0] = pack->color[0];
            ws_dev->dev_attr.render_buff[i][1] = pack->color[1];
            ws_dev->dev_attr.render_buff[i][2] = pack->color[2];
        }
        update_status = _update_led_data(ws_dev, pack->start, count);
        if (update_status == SUCCESS) {
            memset(ws_dev->dev_attr.render_buff, 0, ws_dev->dev_attr.led_num * 3);
        }
    } else if( cmd == WS2812_CTRL_UPDATE_DEVDATA ) {
        update_status = _update_led_data(ws_dev, 0, ws_dev->dev_attr.led_num );
        if (update_status == SUCCESS) {
            memset(ws_dev->dev_attr.render_buff, 0, ws_dev->dev_attr.led_num * 3);
        }
    } else if ( cmd == WS2812_CTRL_GET_DISBUFF ) {
        uint8_t (**dis_buff)[3] = arg;
        *dis_buff = ws_dev->dev_attr.render_buff;
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

