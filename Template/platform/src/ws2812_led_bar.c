#include "ws2812_led_bar.h"
#include "ws2812.h"

static ws2812_bar_t WS2812_BAR_DEV = NULL;

void ws2812_render(void)
{
    ws2812_dev_t ws_dev;
    uint32_t pos;
    uint8_t color[3];

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

    // 基础流水灯/渐变流水灯 左流水显示
    if( WS2812_BAR_DEV->render_param.render_animation == BASE_WATER_LEFT ||
        WS2812_BAR_DEV->render_param.render_animation == CHANGE_WATER_LEFT) {
        color[0] = WS2812_BAR_DEV->render_param.render_color1[0];
        color[1] = WS2812_BAR_DEV->render_param.render_color1[1];
        color[2] = WS2812_BAR_DEV->render_param.render_color1[2];
        for (uint8_t i = 0; i < WS2812_BAR_DEV->render_param.render_light_leds; i++) {
            pos = ( WS2812_BAR_DEV->render_param.show_pos - i ) % WS2812_BAR_DEV->led_num;
            if (WS2812_BAR_DEV->render_param.render_animation == CHANGE_WATER_LEFT) {
                WS2812_BAR_DEV->dis_buff[pos][0] = (int)color[0] < 0 ? 0 : color[0];
                WS2812_BAR_DEV->dis_buff[pos][1] = (int)color[1] < 0 ? 0 : color[1];
                WS2812_BAR_DEV->dis_buff[pos][2] = (int)color[2] < 0 ? 0 : color[2];

                color[0] += WS2812_BAR_DEV->render_param.rgb_step[0];
                color[1] += WS2812_BAR_DEV->render_param.rgb_step[1];
                color[2] += WS2812_BAR_DEV->render_param.rgb_step[2];
            } else {
                WS2812_BAR_DEV->dis_buff[pos][0] = WS2812_BAR_DEV->render_param.render_color1[0];
                WS2812_BAR_DEV->dis_buff[pos][1] = WS2812_BAR_DEV->render_param.render_color1[1];
                WS2812_BAR_DEV->dis_buff[pos][2] = WS2812_BAR_DEV->render_param.render_color1[2];
            }
        }
        WS2812_BAR_DEV->render_param.show_pos++;
        ws_dev->dev_attr.ctrl_led_num = WS2812_BAR_DEV->led_num;
        ws_dev->ws2812_dev_ops.control(ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL);
    } else if( WS2812_BAR_DEV->render_param.render_animation == BASE_WATER_RIGHT ||
               WS2812_BAR_DEV->render_param.render_animation == CHANGE_WATER_RIGHT) {
        color[0] = WS2812_BAR_DEV->render_param.render_color1[0];
        color[1] = WS2812_BAR_DEV->render_param.render_color1[1];
        color[2] = WS2812_BAR_DEV->render_param.render_color1[2];
        for (uint8_t i = 0; i < WS2812_BAR_DEV->render_param.render_light_leds; i++ )
        {
            pos = ( WS2812_BAR_DEV->render_param.show_pos - i ) % WS2812_BAR_DEV->led_num;
            if (WS2812_BAR_DEV->render_param.render_animation == CHANGE_WATER_RIGHT) {
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-pos-1][0] = (int)color[0] < 0 ? 0 : color[0];
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-pos-1][1] = (int)color[1] < 0 ? 0 : color[1];
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-pos-1][2] = (int)color[2] < 0 ? 0 : color[2];

                color[0] += WS2812_BAR_DEV->render_param.rgb_step[0];
                color[1] += WS2812_BAR_DEV->render_param.rgb_step[1];
                color[2] += WS2812_BAR_DEV->render_param.rgb_step[2];
            } else {
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-pos-1][0] = WS2812_BAR_DEV->render_param.render_color1[0];
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-pos-1][1] = WS2812_BAR_DEV->render_param.render_color1[1];
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-pos-1][2] = WS2812_BAR_DEV->render_param.render_color1[2];
            }
        }
        WS2812_BAR_DEV->render_param.show_pos++;
        ws_dev->dev_attr.ctrl_led_num = WS2812_BAR_DEV->led_num;
        ws_dev->ws2812_dev_ops.control( ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL );
    } else if( WS2812_BAR_DEV->render_param.render_animation == BLINK_LEFT ) { // 灯条左端段闪显示
        WS2812_BAR_DEV->render_param.blink_flag = !WS2812_BAR_DEV->render_param.blink_flag;
        if (WS2812_BAR_DEV->render_param.blink_flag) {
            color[0] = WS2812_BAR_DEV->render_param.render_color1[0];
            color[1] = WS2812_BAR_DEV->render_param.render_color1[1];
            color[2] = WS2812_BAR_DEV->render_param.render_color1[2];
        } else {
            memset(color, 0, ITEM_NUM(color));
        }

        for (uint8_t i = WS2812_BAR_DEV->render_param.show_pos; i < WS2812_BAR_DEV->render_param.render_light_leds; i++)
        {
            WS2812_BAR_DEV->dis_buff[i][0] = color[0];
            WS2812_BAR_DEV->dis_buff[i][1] = color[1];
            WS2812_BAR_DEV->dis_buff[i][2] = color[2];
        }
        ws_dev->dev_attr.ctrl_led_num = WS2812_BAR_DEV->led_num;
        ws_dev->dev_attr.index_enable = 1; // 使能下一次输出
        ws_dev->ws2812_dev_ops.control( ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL );
    } else if( WS2812_BAR_DEV->render_param.render_animation == BLINK_RIGHT ) { // 灯条右端段闪显示
        WS2812_BAR_DEV->render_param.blink_flag = !WS2812_BAR_DEV->render_param.blink_flag;
        if (WS2812_BAR_DEV->render_param.blink_flag) {
            color[0] = WS2812_BAR_DEV->render_param.render_color1[0];
            color[1] = WS2812_BAR_DEV->render_param.render_color1[1];
            color[2] = WS2812_BAR_DEV->render_param.render_color1[2];
        } else {
            memset(color, 0, ITEM_NUM(color));
        }

        for (uint8_t i = WS2812_BAR_DEV->render_param.show_pos; i < WS2812_BAR_DEV->render_param.render_light_leds; i++)
        {
            WS2812_BAR_DEV->dis_buff[i][0] = color[0];
            WS2812_BAR_DEV->dis_buff[i][1] = color[1];
            WS2812_BAR_DEV->dis_buff[i][2] = color[2];
        }
        ws_dev->dev_attr.ctrl_led_num = WS2812_BAR_DEV->led_num;
        ws_dev->ws2812_dev_ops.control( ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL );
    } else if( WS2812_BAR_DEV->render_param.render_animation == BREATH ) { // 灯条呼吸模式
        color[0] = WS2812_BAR_DEV->render_param.color[0];
        color[1] = WS2812_BAR_DEV->render_param.color[1];
        color[2] = WS2812_BAR_DEV->render_param.color[2];

        for (uint8_t i = WS2812_BAR_DEV->render_param.show_pos; i < WS2812_BAR_DEV->render_param.render_light_leds; i++)
        {
            WS2812_BAR_DEV->dis_buff[i][0] = (int)color[0] < 0 ? 0 : color[0];
            WS2812_BAR_DEV->dis_buff[i][1] = (int)color[1] < 0 ? 0 : color[1];
            WS2812_BAR_DEV->dis_buff[i][2] = (int)color[2] < 0 ? 0 : color[2];
        }
        ws_dev->ws2812_dev_ops.control( ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL );

        WS2812_BAR_DEV->render_param.color[0] += WS2812_BAR_DEV->render_param.rgb_step[0];
        WS2812_BAR_DEV->render_param.color[1] += WS2812_BAR_DEV->render_param.rgb_step[1];
        WS2812_BAR_DEV->render_param.color[2] += WS2812_BAR_DEV->render_param.rgb_step[2];

        WS2812_BAR_DEV->render_param.breath_cnt--;
        // 关闭渲染
        if (WS2812_BAR_DEV->render_param.breath_cnt == 0) {
            WS2812_BAR_DEV->render_param.breath_cnt = WS2812_BAR_DEV->render_param.breath_timers;
            WS2812_BAR_DEV->render_param.color[0] = WS2812_BAR_DEV->render_param.render_color1[0];
            WS2812_BAR_DEV->render_param.color[1] = WS2812_BAR_DEV->render_param.render_color1[1];
            WS2812_BAR_DEV->render_param.color[2] = WS2812_BAR_DEV->render_param.render_color1[2];
        }
    }  else {
    }
}

static Rtv_Status _ws2812_blink(led_bar_t bar, uint8_t mode, uint8_t blink_led_num, uint8_t blink_start_pos)
{
    ws2812_bar_t wbar = (ws2812_bar_t)bar;
    ws2812_dev_t ws_dev = NULL;
    uint8_t save_cur_state;
    if (wbar == NULL || blink_led_num > wbar->led_num) {
        return EINVAL;
    }

    ws_dev = (ws2812_dev_t)wbar->parent.private;
    if (ws_dev == NULL) {
        return EINVAL;
    }

    BLINK_MODE_CHECK(mode);
    save_cur_state = ws_dev->dev_attr.index_enable;
    // 关闭上一次灯效
    wbar->ctrl_led_num = wbar->led_num; // 关闭所有灯
    wbar->parent.off(&wbar->parent);
    // 此处若当前save_cur_state为1 关闭上一次灯效后 通道输出会失能
    // 因此判断当前状态是否为1 为1 则需要重新打开通道输出
    if (save_cur_state == 0x01) {
        ws_dev->dev_attr.index_enable = 0x01;
    }

    wbar->render_param.render_animation = mode;
    wbar->render_param.render_light_leds = blink_led_num;
    wbar->render_param.blink_flag = 0;
    wbar->render_param.show_pos = (mode == BLINK_RIGHT ? blink_start_pos: wbar->led_num - blink_led_num - blink_start_pos);
    if (wbar->render_param.show_pos + wbar->render_param.render_light_leds > wbar->led_num) {
        wbar->render_param.render_light_leds = wbar->led_num;
    } else {
        // 闪烁模式下 此处记录显示的最终位置
        wbar->render_param.render_light_leds += wbar->render_param.show_pos;
    }
    wbar->render_switch = 1;

    return SUCCESS;
set_error:
    return ERROR;
}

static Rtv_Status _ws2812_water(led_bar_t bar, uint8_t mode, uint8_t single_led_num, uint8_t water_start_pos)
{
    ws2812_bar_t wbar = (ws2812_bar_t)bar;
    ws2812_dev_t ws_dev = NULL;
    uint8_t save_cur_state;
    if (wbar == NULL || single_led_num == 0 || single_led_num > wbar->led_num) {
        return EINVAL;
    }

    ws_dev = (ws2812_dev_t)wbar->parent.private;
    if (ws_dev == NULL) {
        return EINVAL;
    }

    save_cur_state = ws_dev->dev_attr.index_enable;
    // 关闭上一次灯效
    wbar->ctrl_led_num = wbar->led_num; // 关闭所有灯
    wbar->parent.off(&wbar->parent);
    // 此处若当前save_cur_state为1 关闭上一次灯效后 通道输出会失能
    // 因此判断当前状态是否为1 为1 则需要重新打开通道输出
    if (save_cur_state == 0x01) {
        ws_dev->dev_attr.index_enable = 0x01;
    }
    wbar->render_param.render_animation = mode;

    // 兼容渐变流水灯模式
    if (mode == CHANGE_WATER_LEFT || mode == CHANGE_WATER_RIGHT) {
        wbar->render_param.rgb_step[0] =
        (float)(wbar->render_param.render_color2[0] - wbar->render_param.render_color1[0]) / single_led_num;
        wbar->render_param.rgb_step[1] =
        (float)(wbar->render_param.render_color2[1] - wbar->render_param.render_color1[1]) / single_led_num;
        wbar->render_param.rgb_step[2] =
        (float)(wbar->render_param.render_color2[2] - wbar->render_param.render_color1[2]) / single_led_num;
    }

    wbar->render_param.render_light_leds = single_led_num;
    wbar->render_param.show_pos = single_led_num - 1 + water_start_pos;
    wbar->render_switch = 1;


    return SUCCESS;
set_error:
    return ERROR;
}

static Rtv_Status _ws2812_breath(led_bar_t bar, uint16_t breath_period)
{
    ws2812_bar_t wbar = (ws2812_bar_t)bar;
    ws2812_dev_t ws_dev = NULL;
    uint8_t save_cur_state;
    if (wbar == NULL) {
        return EINVAL;
    }

    ws_dev = (ws2812_dev_t)wbar->parent.private;
    if (ws_dev == NULL) {
        return EINVAL;
    }

    save_cur_state = ws_dev->dev_attr.index_enable;
    // 关闭上一次灯效
    wbar->ctrl_led_num = wbar->led_num; // 关闭所有灯
    wbar->parent.off(&wbar->parent);
    // 此处若当前save_cur_state为1 关闭上一次灯效后 通道输出会失能
    // 因此判断当前状态是否为1 为1 则需要重新打开通道输出
    if (save_cur_state == 0x01) {
        ws_dev->dev_attr.index_enable = 0x01;
    }

    wbar->render_param.render_animation = BREATH;
    wbar->render_param.render_light_leds = wbar->led_num;
    wbar->render_param.breath_timers = breath_period / wbar->render_param.breath_singal_period;  // 计算呼吸次数
    memset(wbar->render_param.rgb_step, 0, ITEM_NUM(wbar->render_param.rgb_step));
    wbar->render_param.rgb_step[0] =
    (float)(wbar->render_param.render_color2[0] - wbar->render_param.render_color1[0]) / wbar->render_param.breath_timers;
    wbar->render_param.rgb_step[1] =
    (float)(wbar->render_param.render_color2[1] - wbar->render_param.render_color1[1]) / wbar->render_param.breath_timers;
    wbar->render_param.rgb_step[2] =
    (float)(wbar->render_param.render_color2[2] - wbar->render_param.render_color1[2]) / wbar->render_param.breath_timers;
    // 记录呼吸次数
    wbar->render_param.breath_cnt = wbar->render_param.breath_timers;
    // 记录三分量起始颜色
    wbar->render_param.color[0] = wbar->render_param.render_color1[0];
    wbar->render_param.color[1] = wbar->render_param.render_color1[1];
    wbar->render_param.color[2] = wbar->render_param.render_color1[2];
    wbar->render_param.show_pos = 0;
    wbar->render_switch = 1;

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
    // 增加一个灯珠数量显示可控制功能
    wbar->ctrl_led_num = 0;

    wbar->_parent_off = bar->off;
    bar->off = _ws2812_off;

    wbar->dis_buff = NULL;
    wsdev = (ws2812_dev_t)priv_data;
    wsdev->ws2812_dev_ops.control(wsdev, WS2812_CTRL_GET_DISBUFF, (void *)&wbar->dis_buff);

    wbar->render_param.render_light_leds = led_num;
    wbar->render_param.render_color1[0] = 0xFF;
    wbar->render_param.render_color1[1] = 0xFF;
    wbar->render_param.render_color1[2] = 0;

    // 单次呼吸周期初始化
    wbar->render_param.breath_singal_period = BREATH_SINGAL_PERIOD;

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

    return SUCCESS;
}
