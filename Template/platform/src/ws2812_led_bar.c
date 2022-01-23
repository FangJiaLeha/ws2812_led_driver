#include "ws2812_led_bar.h"
#include "ws2812.h"

static ws2812_bar_t WS2812_BAR_DEV = NULL;

void ws2812_render(void)
{
    ws2812_dev_t ws_dev;
    uint32_t i;

    if( WS2812_BAR_DEV == NULL )
    {
        return;
    }
    ws_dev = (ws2812_dev_t)WS2812_BAR_DEV->parent.private;
    if( ws_dev == NULL )
    {
        return;
    }

    if( WS2812_BAR_DEV->render_switch == 0 ) // 私有渲染引擎关闭
    {
        return;
    }
    
    if( WS2812_BAR_DEV->render_param.render_animation == WATER_LEFT) // 左转显示
    {
        uint32_t pos;
        int color;
        // 获取上一次显示的最后一颗灯珠位置，并将其清零
        i = ( WS2812_BAR_DEV->render_param.show_pos 
                        - WS2812_BAR_DEV->render_param.light_leds + 1 ) % WS2812_BAR_DEV->led_num; 
        WS2812_BAR_DEV->dis_buff[ i ][0] = 0;
        WS2812_BAR_DEV->dis_buff[ i ][1] = 0;
        WS2812_BAR_DEV->dis_buff[ i ][2] = 0;
        WS2812_BAR_DEV->render_param.show_pos++;
        for ( i = 0; i < WS2812_BAR_DEV->render_param.light_leds; i++ )
        {
            pos = ( WS2812_BAR_DEV->render_param.show_pos - i ) % WS2812_BAR_DEV->led_num;
            color = (int)WS2812_BAR_DEV->render_param.render_color[0] - (int)( i * 10 );
            color = color > 0? color:0;
            WS2812_BAR_DEV->dis_buff[pos][0] = (uint8_t)color;
            color = (int)WS2812_BAR_DEV->render_param.render_color[1] - (int)( i * 10 );
            color = color > 0? color:0;
            WS2812_BAR_DEV->dis_buff[pos][1] = (uint8_t)color;
            color = (int)WS2812_BAR_DEV->render_param.render_color[2] - (int)( i * 10 );
            color = color > 0? color:0;
            WS2812_BAR_DEV->dis_buff[pos][2] = (uint8_t)color;
        }
        ws_dev->ws2812_dev_ops.control(ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL);
    } else if( WS2812_BAR_DEV->render_param.render_animation == WATER_RIGHT ) { // 右转显示
        uint32_t pos;
        int color;
        // 获取上一次显示的最后一颗灯珠位置，并将其清零
        i = ( WS2812_BAR_DEV->render_param.show_pos 
                        + WS2812_BAR_DEV->render_param.light_leds - 1 ) % WS2812_BAR_DEV->led_num; 
        WS2812_BAR_DEV->dis_buff[ i ][0] = 0;
        WS2812_BAR_DEV->dis_buff[ i ][1] = 0;
        WS2812_BAR_DEV->dis_buff[ i ][2] = 0;
        WS2812_BAR_DEV->render_param.show_pos--;
        for ( i = 0; i < WS2812_BAR_DEV->render_param.light_leds; i++ )
        {
            pos = ( WS2812_BAR_DEV->render_param.show_pos + i )% WS2812_BAR_DEV->led_num;
            color = (int)WS2812_BAR_DEV->render_param.render_color[0] - (int)( i * 10 );
            color = color > 0? color:0;
            WS2812_BAR_DEV->dis_buff[pos][0] = (uint8_t)color;
            color = (int)WS2812_BAR_DEV->render_param.render_color[1] - (int)( i * 10 );
            color = color > 0? color:0;
            WS2812_BAR_DEV->dis_buff[pos][1] = (uint8_t)color;
            color = (int)WS2812_BAR_DEV->render_param.render_color[2] - (int)( i * 10 );
            color = color > 0? color:0;
            WS2812_BAR_DEV->dis_buff[pos][2] = (uint8_t)color;
        }
        ws_dev->ws2812_dev_ops.control( ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL );
    } else if( WS2812_BAR_DEV->render_param.render_animation == BLINK_LEFT ) {
        uint32_t pos = WS2812_BAR_DEV->led_num - WS2812_BAR_DEV->render_param.light_leds;
        uint8_t color[3];

        WS2812_BAR_DEV->render_param.blink_flag = !WS2812_BAR_DEV->render_param.blink_flag;

        if (WS2812_BAR_DEV->render_param.blink_flag) {
            color[0] = WS2812_BAR_DEV->render_param.render_color[0];
            color[1] = WS2812_BAR_DEV->render_param.render_color[1];
            color[2] = WS2812_BAR_DEV->render_param.render_color[2];
        } else {
            memset(color, 0, ITEM_NUM(color));
        }

        for (uint16_t i = pos; i < WS2812_BAR_DEV->led_num; i++)
        {
            WS2812_BAR_DEV->dis_buff[i][0] = color[0];
            WS2812_BAR_DEV->dis_buff[i][1] = color[1];
            WS2812_BAR_DEV->dis_buff[i][2] = color[2];
        }
        ws_dev->ws2812_dev_ops.control( ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL );
    } else if( WS2812_BAR_DEV->render_param.render_animation == BLINK_RIGHT ) {
        uint8_t color[3];

        WS2812_BAR_DEV->render_param.blink_flag = !WS2812_BAR_DEV->render_param.blink_flag;

        if (WS2812_BAR_DEV->render_param.blink_flag) {
            color[0] = WS2812_BAR_DEV->render_param.render_color[0];
            color[1] = WS2812_BAR_DEV->render_param.render_color[1];
            color[2] = WS2812_BAR_DEV->render_param.render_color[2];
        } else {
            memset(color, 0, ITEM_NUM(color));
        }

        for (uint16_t i = 0; i < WS2812_BAR_DEV->render_param.light_leds; i++)
        {
            WS2812_BAR_DEV->dis_buff[i][0] = color[0];
            WS2812_BAR_DEV->dis_buff[i][1] = color[1];
            WS2812_BAR_DEV->dis_buff[i][2] = color[2];
        }
        ws_dev->ws2812_dev_ops.control( ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL );
    } else if( WS2812_BAR_DEV->render_param.render_animation == 6 ) {
//        unsigned int loopCounter = WS2812_BAR_DEV->render_param.show_pos;
//        if(loopCounter < 10) {
//            for(int i = 0; i <  WS2812_BAR_DEV->led_num; i++)
//            {
//                WS2812_BAR_DEV->dis_buff[i][0] = 0;
//                WS2812_BAR_DEV->dis_buff[i][1] = 0;
//                WS2812_BAR_DEV->dis_buff[i][2] = 0;
//            }
//        } else if(loopCounter < 20) {
//            for(int i = 0; i <  WS2812_BAR_DEV->led_num; i++)
//            {
//                WS2812_BAR_DEV->dis_buff[i][0] = 0x7f;
//                WS2812_BAR_DEV->dis_buff[i][1] = 0x7f;
//                WS2812_BAR_DEV->dis_buff[i][2] = 0x7f;
//            }
//        } else if(loopCounter < 30) {
//            for(int i = 0; i <  WS2812_BAR_DEV->led_num; i++)
//            {
//                WS2812_BAR_DEV->dis_buff[i][0] = 0;
//                WS2812_BAR_DEV->dis_buff[i][1] = 0;
//                WS2812_BAR_DEV->dis_buff[i][2] = 0;
//            }
//        } else {
//            loopCounter = loopCounter - 30;
//            if(loopCounter < (( WS2812_BAR_DEV->led_num + 1) >> 1))
//            {
//                WS2812_BAR_DEV->dis_buff[loopCounter%(( WS2812_BAR_DEV->led_num + 1) >> 1)][0] = 0x7f;
//                WS2812_BAR_DEV->dis_buff[loopCounter%(( WS2812_BAR_DEV->led_num + 1) >> 1)][1] = 0x7f;
//                WS2812_BAR_DEV->dis_buff[loopCounter%(( WS2812_BAR_DEV->led_num + 1) >> 1)][2] = 0x7f;

//                WS2812_BAR_DEV->dis_buff[ WS2812_BAR_DEV->led_num - 1 - (loopCounter%(( WS2812_BAR_DEV->led_num + 1) >> 1))][0] = 0x7f;
//                WS2812_BAR_DEV->dis_buff[ WS2812_BAR_DEV->led_num - 1 - (loopCounter%(( WS2812_BAR_DEV->led_num + 1) >> 1))][1] = 0x7f;
//                WS2812_BAR_DEV->dis_buff[ WS2812_BAR_DEV->led_num - 1 - (loopCounter%(( WS2812_BAR_DEV->led_num + 1) >> 1))][2] = 0x7f;
//            } else {
//                WS2812_BAR_DEV->render_switch = 0; // 动画显示完毕
//            }
//        }
//        ws_dev->ws2812_dev_ops.control( ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL );
//        WS2812_BAR_DEV->render_param.show_pos++;
    }
}

static Rtv_Status _ws2812_blink(led_bar_t bar, uint8_t mode, uint8_t blink_led_num)
{
    ws2812_bar_t wbar = (ws2812_bar_t)bar;
    if (wbar == NULL || blink_led_num > wbar->led_num) {
        return EINVAL;
    }
    BLINK_MODE_CHECK(mode);
    wbar->parent.off(&wbar->parent);
    wbar->render_param.render_animation = mode;
    wbar->render_param.light_leds = blink_led_num;
    wbar->render_param.blink_flag = 0;
    wbar->render_param.show_pos = 0;
    wbar->render_switch = 1;

    return SUCCESS;
set_error:
    return ERROR;
}

static Rtv_Status _ws2812_water(led_bar_t bar, uint8_t mode, uint8_t single_led_num, uint8_t move_period)
{
    ws2812_bar_t wbar = (ws2812_bar_t)bar;
    if (wbar == NULL || single_led_num == 0 || single_led_num > wbar->led_num) {
        return EINVAL;
    }
    WATER_MODE_CHECK(mode);
    wbar->parent.off(&wbar->parent);
    wbar->render_param.render_animation = mode;
    wbar->render_param.light_leds = single_led_num;
    wbar->render_switch = 1;

    return SUCCESS;
set_error:
    return ERROR;
}

static Rtv_Status _ws2812_breath(led_bar_t bar, uint8_t s_color_index, uint8_t e_color_index, uint8_t breath_period)
{
    ws2812_bar_t wbar = (ws2812_bar_t)bar;
    if (wbar == NULL) {
        return EINVAL;
    }
    wbar->parent.off(&wbar->parent);
    return SUCCESS;
}

/**
  * @brief 重写LED_BAR off 方法
  */
static Rtv_Status _ws2812_off(led_bar_t bar)
{
    ws2812_bar_t wbar = (ws2812_bar_t)bar;
    if (wbar == NULL) {
        return EINVAL;
    }
    wbar->_parent_off(bar); // 调用父类off方法
    wbar->render_param.render_animation = 0;
    wbar->render_switch = 0;
    return SUCCESS;
}
//==============================================================================
Rtv_Status init_ws2812_bar(ws2812_bar_t wbar, uint8_t id,
                           Rtv_Status (*set_color)(led_bar_t bar, float *color ),
                    void * priv_data, uint32_t led_num, uint32_t start)
{
    ws2812_dev_t wsdev = NULL;
    led_bar_t bar = (led_bar_t)wbar;

    if( bar == NULL || set_color == NULL || priv_data == NULL)
    {
        return EINVAL;
    }

    init_led_bar(bar, id, set_color, priv_data);
    wbar->start = start;
    wbar->led_num = led_num;

    wbar->_parent_off = bar->off;
    bar->off = _ws2812_off;

    wbar->dis_buff = NULL;
    wsdev = (ws2812_dev_t)priv_data;
    wsdev->ws2812_dev_ops.control(wsdev, WS2812_CTRL_GET_DISBUFF, (void *)&wbar->dis_buff);

    wbar->render_param.light_leds = 10;
    wbar->render_param.render_color[0] = 0x7F;
    wbar->render_param.render_color[1] = 0x7F;
    wbar->render_param.render_color[2] = 0;

    wbar->render_param.render_animation = 0;
    wbar->render_switch = 1;
    wbar->render_param.show_pos = 0;
    WS2812_BAR_DEV = wbar;

    /* 重写父类bar的方法 */
    wbar->parent.blink = _ws2812_blink;
    wbar->parent.water = _ws2812_water;
    wbar->parent.breath = _ws2812_breath;

    if (wbar->dis_buff == NULL) {
        return ERROR;
    }
    
    // 启动硬件定时器

    return SUCCESS;
}
