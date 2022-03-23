/**
 * @file ws2812_led_bar.c
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "ws2812_led_bar.h"

//==============================================================================
/**
 * @brief 全局变量 保存wbar对象地址 主要用于WS2812的渲染任务中
 *
 */
static WS2812BarType_t WS2812_BAR_DEV = NULL;

//==============================================================================
/**
 * @brief WS2812灯条的blink方法
 *
 * @param bar               WS2812灯条对象地址
 * @param mode              WS2812灯条blink模式 @ws2812_led_ctrl_mode:BLINK_RIGHT/BLINK_LEFT
 * @param blink_led_num     WS2812灯条blink灯珠数设置参数
 * @param blink_start_pos   WS2812灯条blink起始位置设置参数
 * @return Rtv_Status       @SUCCESS:当前blink模式参数设置有效 @其他值:当前blink模式参数设置无效
 */
static Rtv_Status _ws2812_blink(LedBarType_t bar,
                                const uint8_t mode,
                                const uint8_t blink_led_num,
                                const uint8_t blink_start_pos)
{
    WS2812BarType_t wbar = (WS2812BarType_t)bar;
    WS2812DevType_t ws_dev = NULL;
    uint8_t save_cur_state;
    if (wbar == NULL ||
        blink_led_num > wbar->led_num ||
        blink_led_num == 0) {
        return EINVAL;
    }

    ws_dev = (WS2812DevType_t)wbar->parent.private;
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
}
/**
 * @brief WS2812灯条的water方法
 *
 * @param bar               WS2812灯条对象地址
 * @param mode              WS2812灯条water模式
 *                          @ws2812_led_ctrl_mode: BASE_WATER_LEFT / BASE_WATER_RIGHT
 *                                                 CHANGE_WATER_LEFT / CHANGE_WATER_RIGHT
 * @param single_led_num    WS2812灯条单次流水灯数设置参数
 * @param water_start_pos   WS2812灯条单次流水起始位置
 * @return Rtv_Status       @SUCCESS:当前water模式参数设置有效 @其他值:当前water模式参数设置无效
 */
static Rtv_Status _ws2812_water(LedBarType_t bar,
                                const uint8_t mode,
                                const uint8_t single_led_num,
                                const uint8_t water_start_pos)
{
    WS2812BarType_t wbar = (WS2812BarType_t)bar;
    WS2812DevType_t ws_dev = NULL;
    uint8_t save_cur_state;
    if (wbar == NULL ||
        single_led_num == 0 ||
        single_led_num > wbar->led_num ||
        water_start_pos > wbar->led_num) {
        return EINVAL;
    }

    ws_dev = (WS2812DevType_t)wbar->parent.private;
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
    // 兼容渐变式流水灯模式
    if (mode == CHANGE_WATER_LEFT || mode == CHANGE_WATER_RIGHT) {
        wbar->render_param.rgb_step[0] =
        (float)(wbar->render_param.render_color2[0] - wbar->render_param.render_color1[0]) / single_led_num;
        wbar->render_param.rgb_step[1] =
        (float)(wbar->render_param.render_color2[1] - wbar->render_param.render_color1[1]) / single_led_num;
        wbar->render_param.rgb_step[2] =
        (float)(wbar->render_param.render_color2[2] - wbar->render_param.render_color1[2]) / single_led_num;
    }

    wbar->render_param.render_light_leds = single_led_num;
    wbar->render_param.show_pos = single_led_num + water_start_pos - 1;
    // 递增式流水灯模式 显示起始位置为0
    if (mode == INCREASE_WATER_LEFT || mode == INCREASE_WATER_RIGHT) {
        wbar->render_param.show_pos = water_start_pos;
    } else if (mode == SECTOR_WATER_LEFT || mode == SECTOR_WATER_RIGHT) { // 分段式流水灯模式
        wbar->render_param.start_pos = wbar->render_param.show_pos;
    }
    wbar->render_switch = 1;

    return SUCCESS;
}
/**
 * @brief WS2812灯条的breath方法
 *
 * @param bar               WS2812灯条对象地址
 * @param breath_period     WS2812灯条breath周期参数
 * @return Rtv_Status       @SUCCESS:当前breath模式参数设置有效 @其他值:当前breath模式参数设置无效
 */
static Rtv_Status _ws2812_breath(LedBarType_t bar, const uint16_t breath_period)
{
    WS2812BarType_t wbar = (WS2812BarType_t)bar;
    WS2812DevType_t ws_dev = NULL;
    uint8_t save_cur_state;
    if (wbar == NULL) {
        return EINVAL;
    }

    ws_dev = (WS2812DevType_t)wbar->parent.private;
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
    wbar->render_param.breath_state = 0; // 记录呼状态
    wbar->render_switch = 1;

    return SUCCESS;
}
/**
  * @brief 重写LED_BAR off 方法
  */
static Rtv_Status _ws2812_off(LedBarType_t bar)
{
    WS2812BarType_t wbar = (WS2812BarType_t)bar;
    if (wbar == NULL) {
        return EINVAL;
    }
    wbar->_parent_off(bar); // 回调用父类led_bar的off方法
    wbar->render_param.render_animation = 0;
    wbar->render_switch = 0;
    return SUCCESS;
}
//==============================================================================
void ws2812_render(void)
{
    WS2812DevType_t ws_dev;
    uint32_t pos;
    uint8_t color[3];

    if (WS2812_BAR_DEV == NULL) {
        return;
    }
    ws_dev = (WS2812DevType_t)WS2812_BAR_DEV->parent.private;
    if (ws_dev == NULL) {
        return;
    } else if (WS2812_BAR_DEV->render_switch == 0) {
        // 未开启动画渲染
        return;
    }

    if( WS2812_BAR_DEV->render_param.render_animation == BASE_WATER_LEFT ||
        WS2812_BAR_DEV->render_param.render_animation == CHANGE_WATER_LEFT ||
        WS2812_BAR_DEV->render_param.render_animation == BASE_WATER_RIGHT ||
        WS2812_BAR_DEV->render_param.render_animation == CHANGE_WATER_RIGHT) { // 基础流水灯/渐变流水灯 显示
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
            } else if (WS2812_BAR_DEV->render_param.render_animation == BASE_WATER_LEFT){
                WS2812_BAR_DEV->dis_buff[pos][0] = WS2812_BAR_DEV->render_param.render_color1[0];
                WS2812_BAR_DEV->dis_buff[pos][1] = WS2812_BAR_DEV->render_param.render_color1[1];
                WS2812_BAR_DEV->dis_buff[pos][2] = WS2812_BAR_DEV->render_param.render_color1[2];
            } else if (WS2812_BAR_DEV->render_param.render_animation == CHANGE_WATER_RIGHT) {
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
        ws_dev->ws2812_dev_ops.control(ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL);
    } else if(WS2812_BAR_DEV->render_param.render_animation == BLINK_LEFT ||
              WS2812_BAR_DEV->render_param.render_animation == BLINK_RIGHT) { // 灯条段闪显示
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

        if (WS2812_BAR_DEV->render_param.breath_state == 0) {
            WS2812_BAR_DEV->render_param.color[0] += WS2812_BAR_DEV->render_param.rgb_step[0];
            WS2812_BAR_DEV->render_param.color[1] += WS2812_BAR_DEV->render_param.rgb_step[1];
            WS2812_BAR_DEV->render_param.color[2] += WS2812_BAR_DEV->render_param.rgb_step[2];
        } else {
            WS2812_BAR_DEV->render_param.color[0] -= WS2812_BAR_DEV->render_param.rgb_step[0];
            WS2812_BAR_DEV->render_param.color[1] -= WS2812_BAR_DEV->render_param.rgb_step[1];
            WS2812_BAR_DEV->render_param.color[2] -= WS2812_BAR_DEV->render_param.rgb_step[2];
        }

        WS2812_BAR_DEV->render_param.breath_cnt--;
        // 关闭渲染
        if (WS2812_BAR_DEV->render_param.breath_cnt == 0) {
            WS2812_BAR_DEV->render_param.breath_cnt = WS2812_BAR_DEV->render_param.breath_timers;
            WS2812_BAR_DEV->render_param.breath_state = !WS2812_BAR_DEV->render_param.breath_state;
            if (WS2812_BAR_DEV->render_param.breath_state == 1) {
                WS2812_BAR_DEV->render_param.color[0] = WS2812_BAR_DEV->render_param.render_color2[0];
                WS2812_BAR_DEV->render_param.color[1] = WS2812_BAR_DEV->render_param.render_color2[1];
                WS2812_BAR_DEV->render_param.color[2] = WS2812_BAR_DEV->render_param.render_color2[2];
            } else {
                WS2812_BAR_DEV->render_param.color[0] = WS2812_BAR_DEV->render_param.render_color1[0];
                WS2812_BAR_DEV->render_param.color[1] = WS2812_BAR_DEV->render_param.render_color1[1];
                WS2812_BAR_DEV->render_param.color[2] = WS2812_BAR_DEV->render_param.render_color1[2];
            }
        }
    } else if(WS2812_BAR_DEV->render_param.render_animation == INCREASE_WATER_LEFT ||
              WS2812_BAR_DEV->render_param.render_animation == INCREASE_WATER_RIGHT) { // 递增式流水
        color[0] = WS2812_BAR_DEV->render_param.render_color1[0];
        color[1] = WS2812_BAR_DEV->render_param.render_color1[1];
        color[2] = WS2812_BAR_DEV->render_param.render_color1[2];
        pos = (WS2812_BAR_DEV->render_param.show_pos++) % WS2812_BAR_DEV->led_num;
        for (uint8_t i = 0; i <= pos; i++) {
            if (WS2812_BAR_DEV->render_param.render_animation == INCREASE_WATER_LEFT) {
                WS2812_BAR_DEV->dis_buff[i][0] = WS2812_BAR_DEV->render_param.render_color1[0];
                WS2812_BAR_DEV->dis_buff[i][1] = WS2812_BAR_DEV->render_param.render_color1[1];
                WS2812_BAR_DEV->dis_buff[i][2] = WS2812_BAR_DEV->render_param.render_color1[2];
            } else {
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-i-1][0] = WS2812_BAR_DEV->render_param.render_color1[0];
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-i-1][1] = WS2812_BAR_DEV->render_param.render_color1[1];
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-i-1][2] = WS2812_BAR_DEV->render_param.render_color1[2];
            }
        }
        ws_dev->dev_attr.ctrl_led_num = WS2812_BAR_DEV->led_num;
        ws_dev->ws2812_dev_ops.control( ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL );
    } else if(WS2812_BAR_DEV->render_param.render_animation == SECTOR_WATER_LEFT ||
              WS2812_BAR_DEV->render_param.render_animation == SECTOR_WATER_RIGHT) { // 分段式流水
        color[0] = WS2812_BAR_DEV->render_param.render_color1[0];
        color[1] = WS2812_BAR_DEV->render_param.render_color1[1];
        color[2] = WS2812_BAR_DEV->render_param.render_color1[2];
        for (uint8_t i = 0; i < WS2812_BAR_DEV->render_param.render_light_leds; i++) {
            pos = ( WS2812_BAR_DEV->render_param.show_pos - i ) % WS2812_BAR_DEV->led_num;
            if (WS2812_BAR_DEV->render_param.render_animation == SECTOR_WATER_LEFT) {
                WS2812_BAR_DEV->dis_buff[pos][0] = WS2812_BAR_DEV->render_param.render_color1[0];
                WS2812_BAR_DEV->dis_buff[pos][1] = WS2812_BAR_DEV->render_param.render_color1[1];
                WS2812_BAR_DEV->dis_buff[pos][2] = WS2812_BAR_DEV->render_param.render_color1[2];
            } else {
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-pos-1][0] = WS2812_BAR_DEV->render_param.render_color1[0];
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-pos-1][1] = WS2812_BAR_DEV->render_param.render_color1[1];
                WS2812_BAR_DEV->dis_buff[WS2812_BAR_DEV->led_num-pos-1][2] = WS2812_BAR_DEV->render_param.render_color1[2];
            }
        }
        WS2812_BAR_DEV->render_param.show_pos++;
        ws_dev->dev_attr.ctrl_led_num = WS2812_BAR_DEV->led_num;
        ws_dev->ws2812_dev_ops.control(ws_dev, WS2812_CTRL_UPDATE_DEVDATA, NULL);
        // 左分段式流水模式下 当到达尾灯位置时 重新定位显示的起始位置
        if (WS2812_BAR_DEV->render_param.render_animation == SECTOR_WATER_LEFT &&
            WS2812_BAR_DEV->render_param.show_pos == WS2812_BAR_DEV->led_num) {
            WS2812_BAR_DEV->render_param.show_pos = WS2812_BAR_DEV->render_param.start_pos;
        }
    } else {
    }
}

Rtv_Status init_ws2812_bar(WS2812BarType_t wbar,
                           uint8_t id,
                           Rtv_Status (*set_color)(LedBarType_t bar, float *color ),
                           void *priv_data,
                           const uint32_t led_num,
                           const uint32_t start)
{
    WS2812DevType_t wsdev = NULL;
    LedBarType_t bar = (LedBarType_t)wbar;

    if( bar == NULL || set_color == NULL || priv_data == NULL)
    {
        return EINVAL;
    }

    // 调用ws2812_bar父类led_bar的初始化灯条方法
    init_led_bar(bar, id, set_color, priv_data);
    wbar->start = start;
    wbar->led_num = led_num;
    // 增加一个灯珠数量显示可控制功能
    wbar->ctrl_led_num = 0;

    // 该ws2812_bar对象的off方法 保存父类bar的off方法
    wbar->_parent_off = bar->off;
    // 重写该ws2812_bar的led_bar对象的off方法 应用层调用时 先调用WS2812_off方法
    bar->off = _ws2812_off;

    wbar->dis_buff = NULL;
    wsdev = (WS2812DevType_t)priv_data;
    // 使用二级指针 使该ws2812_bar对象的dis_buff显存 可以 直接操作 WS2812.c中的显存
    wsdev->ws2812_dev_ops.control(wsdev, WS2812_CTRL_GET_DISBUFF, (void *)&wbar->dis_buff);
    if (wbar->dis_buff == NULL) {
        return ERROR;
    }

    wbar->render_param.render_light_leds = led_num;
    // 设置WS2812灯条默认颜色为白色
    wbar->render_param.render_color1[0] = 0xFF;
    wbar->render_param.render_color1[1] = 0xFF;
    wbar->render_param.render_color1[2] = 0xFF;

    // 单次呼吸周期初始化
    wbar->render_param.breath_singal_period = BREATH_SINGAL_PERIOD;

    wbar->render_param.render_animation = 0;
    wbar->render_param.show_pos = 0;
    wbar->render_switch = 1;

    // 初始化分段式流水灯模式下 起始位置
    wbar->render_param.start_pos = 0;

    /* 重写该ws2812_bar父类led_bar的blink/water/breath方法 */
    wbar->parent.blink = _ws2812_blink;
    wbar->parent.water = _ws2812_water;
    wbar->parent.breath = _ws2812_breath;

    WS2812_BAR_DEV = wbar;

    return SUCCESS;
}
