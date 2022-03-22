/**
 * @file led_bar.c
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "led_bar.h"

/******************************************************************************/
static WS2812BarType led_bar1;
static TLC59108BarType led_bar2;
static LedBarType_t Lbar[] = {(LedBarType_t)&led_bar1, (LedBarType_t)&led_bar2};

static uint16_t recv_data_len = 0;
static uint8_t *recv_data_buff = NULL;
static uint8_t *set_send_buff = NULL;

// 当前驱动灯条类型
static DriverDevType cur_driver_type;

/******************************************************************************/
// 应用逻辑接口声明
/**
 * @brief WS2812灯条控制接口
 */
static void ctrl_ws2812_off(LedBarType_t led_bar, uint8_t *ctrl_para);
static void ctrl_ws2812_on(LedBarType_t led_bar, uint8_t *ctrl_para);
static void ctrl_ws2812_blink(LedBarType_t led_bar, uint8_t *ctrl_para);
static void ctrl_ws2812_base_water(LedBarType_t led_bar, uint8_t *ctrl_para);
static void ctrl_ws2812_change_water(LedBarType_t led_bar, uint8_t *ctrl_para);
static void ctrl_ws2812_breath(LedBarType_t led_bar, uint8_t *ctrl_para);
/**
 * @brief TLC59108灯条控制接口
 */
static void ctrl_tlc59108_dimming(LedBarType_t led_bar, uint8_t *ctrl_para);
static void ctrl_tlc59108_blinking(LedBarType_t led_bar, uint8_t *ctrl_para);

struct cmd_list
{
    const uint8_t cmd_type;
    void (*callback)(LedBarType_t led_bar, uint8_t *para);
};

/******************************************************************************/
/**
 * @brief WS2812灯条控制命令及其控制逻辑索引列表
 *
 */
const struct cmd_list ws2812_ctrl_cmds[] = {
    /**
     * @brief 控制WS2812灯条熄灭
     */
    {WS2812_LED_OFF, ctrl_ws2812_off},

    /**
     * @brief 控制WS2812灯条常亮
     */
    {WS2812_LED_ON, ctrl_ws2812_on},

    /**
     * @brief 控制WS2812灯条进入闪烁模式
     */
    {WS2812_LED_BLINK, ctrl_ws2812_blink},

    /**
     * @brief 控制WS2812灯条进入基础流水灯模式
     */
    {WS2812_LED_BASE_WATER, ctrl_ws2812_base_water},

    /**
     * @brief 控制WS2812灯条进入渐变流水灯模式
     */
    {WS2812_LED_CHANGE_WATER, ctrl_ws2812_change_water},

    /**
     * @brief 控制WS2812灯条进入呼吸模式
     */
    {WS2812_LED_BREATH, ctrl_ws2812_breath}
};

/**
 * @brief TLC59108灯条控制命令及其控制逻辑索引列表
 *
 */
const struct cmd_list tlc59108_ctrl_cmds[] = {
    /**
     * @brief 控制TLC59108灯条进入dimming模式
     */
    {TLC59108_LED_DIMMING, ctrl_tlc59108_dimming},

    /**
     * @brief 控制TLC59108灯条进入blinking模式
     */
    {TLC59108_LED_BLINK, ctrl_tlc59108_blinking}
};

/******************************************************************************/
static const struct cmd_list *find_proccessor(const DriverDevType driver_type,
                                              uint8_t cmd_type)
{
    uint8_t cmds_num = 0;
    if (driver_type == WS2812DEV) {
        cmds_num = ITEM_NUM(ws2812_ctrl_cmds);
    } else {
        cmds_num = ITEM_NUM(tlc59108_ctrl_cmds);
    }

    for (uint8_t cmds_cnt = 0; cmds_cnt < cmds_num; cmds_cnt++) {
        if (driver_type == WS2812DEV) {
            if (cmd_type == ws2812_ctrl_cmds[cmds_cnt].cmd_type) {
                return &ws2812_ctrl_cmds[cmds_cnt];
            }
        } else {
            if (cmd_type == tlc59108_ctrl_cmds[cmds_cnt].cmd_type) {
                return &tlc59108_ctrl_cmds[cmds_cnt];
            }
        }
    }
    return NULL;
}
/******************************************************************************/
static Rtv_Status ws2812_bar_set_color(LedBarType_t bar, float *color)
{
    WS2812BarType_t wbar = (WS2812BarType_t)bar;
    WS2812DevType_t wsdev = (WS2812DevType_t)bar->private;
    WS2812BarCtrlPackType pack;

    pack.color[0] = (uint8_t)color[0];
    pack.color[1] = (uint8_t)color[1];
    pack.color[2] = (uint8_t)color[2];

    pack.start = wbar->start;
    pack.count = wbar->ctrl_led_num;

    wsdev->ws2812_dev_ops.control(wsdev, WS2812_CTRL_BAR_COLOR, &pack);
    return SUCCESS;
}


static uint8_t CheckXOR(const uint8_t *data_buff,uint8_t buff_len)
{
    uint8_t checkXor = 0;
    if (NULL == data_buff || buff_len == 0) {
        return 0;
    }

    for(uint8_t data_len_cnt = 0; data_len_cnt < buff_len - 1; data_len_cnt++)
    {
        checkXor ^= *data_buff++;
    }
    return checkXor;
}

/******************************************************************************/
Rtv_Status init_led_bars(const uint8_t ws2812_led_num,
                         const uint8_t tlc59108_channel_num)
{
    Rtv_Status rtv_status = SUCCESS;
    PwmDevType_t pwm_dev = find_pwm_dev();
    WS2812DevType_t wsdev = find_ws2812_dev();
    TLC59108DevType_t tlcdev = find_tlc59108_dev();

    if (NULL == pwm_dev ||
        NULL == wsdev ||
        ws2812_led_num == 0 ||
        tlc59108_channel_num == 0 ||
        tlc59108_channel_num > TLC59108_CHANNNEL_MAX_NUM) {
        return EINVAL;
    }

    /**
     * @brief WS2812灯条参数默认初始化
     *
     */
    wsdev->dev_attr.private = pwm_dev;
    // 初始化WS2812灯条 BSP层
    rtv_status = wsdev->ws2812_dev_ops.control(wsdev,
                                               WS2812_CTRL_INIT,
                                               (void *)&ws2812_led_num);
    if (rtv_status != SUCCESS) {
        return rtv_status;
    }
    init_ws2812_bar(&led_bar1, 1, ws2812_bar_set_color, wsdev, ws2812_led_num, 0);

    /**
     * @brief TLC59108灯条参数默认初始化
     *
     */
    tlcdev->dev_attr.private = pwm_dev;
    // 初始化TLC59108灯条 BSP层
    rtv_status = tlcdev->tlc59108_dev_ops.control(tlcdev,
                                                  TLC59108_CTRL_INIT,
                                                  (void *)&tlc59108_channel_num);
    if (rtv_status != SUCCESS) {
        return rtv_status;
    }
    init_tlc59108_bar(&led_bar2, 2, NULL, tlcdev, tlc59108_channel_num);

    // 默认初始化设置驱动灯条类型为WS2812
    cur_driver_type = WS2812DEV;

    // 获取I2C设备的发送和接收buff
    control_i2c(I2C0_DEV, I2C_GET_RECV_BUFF, (void *)&recv_data_buff);
    control_i2c(I2C0_DEV, I2C_GET_SEND_BUFF, (void *)&set_send_buff);

    return rtv_status;
}

/**
 * @brief led灯带控制协议接口
 *
 */
#if (defined(_TEST_) && _TEST_ == 0x01)
void led_bar_control(uint8_t *req, uint8_t req_len)
#else
static void led_bar_control(uint8_t *req, uint8_t req_len)
#endif
{
    const struct cmd_list *cmd = NULL;
    LedBarType_t bar = NULL;
    WS2812DevType_t ws2812_dev = NULL;
    TLC59108DevType_t tlc59108_dev = NULL;
    WS2812BarType_t wbar = NULL;
    uint8_t *bar_ctrl_para = NULL;
    uint8_t bar_driver_type, bar_work_mode;
    uint8_t ws2812_bar_index, ws2812_bar_ctrl_led_all;
    LED_BAR_CTRL_PARA_CHECK(req, req_len);
    BAR_REQ_LEN_CHECK(req[0], req_len);

    bar_driver_type = req[0];

    // 根据灯驱设备类型获取对应的灯条设备句柄
    bar = Lbar[bar_driver_type - 1];
    if (bar == NULL) {
        goto set_error;
    }
    if (bar_driver_type == WS2812DEV) {
        ws2812_dev = (WS2812DevType_t)bar->private;
        if (ws2812_dev == NULL) {
            goto set_error;
        }

        wbar = (WS2812BarType_t)bar;
        // 获取工作模式
        bar_work_mode = req[1];
        // 选择控制ws2812灯条序号
        ws2812_bar_index = req[2];
        // 控制ws2812灯珠总数
        if (bar_work_mode != WS2812_LED_OFF) {
            ws2812_bar_ctrl_led_all = req[10];
        } else {
            // 其他模式切换至熄灭模式时 由于模式不一致 前面会清除参数区 因此本处需赋值最大灯珠数
            ws2812_bar_ctrl_led_all = ws2812_dev->dev_attr.led_num;
        }

        // WS2812灯条序号及控制灯珠总数有效性判断
        WS2812_DRIVER_CHANNEL_CHECK(ws2812_bar_index);
        WS2812_CTRL_LED_ALL_CHECK(ws2812_bar_ctrl_led_all);

        // 发生驱动设备类型改变时 需重新初始化ws2812设备
        if (cur_driver_type != WS2812DEV) {
            cur_driver_type = WS2812DEV;
            ws2812_dev->dev_attr.index = 1; // 默认设置第一个通道输出PWM波
            ws2812_dev->ws2812_dev_ops.init(ws2812_dev);
        }

        // 上一次控制的灯条编号与当前需控制编号不一致
        if (ws2812_bar_index != ws2812_dev->dev_attr.index) {
            ws2812_dev->dev_attr.index = ws2812_bar_index;
            ws2812_dev->ws2812_dev_ops.init(ws2812_dev);
        }
        // 上一次控制的灯珠总数与当前需控制的灯珠总数不一致
        if (ws2812_bar_ctrl_led_all != ws2812_dev->dev_attr.led_num) {
            ws2812_dev->ws2812_dev_ops.control(ws2812_dev, WS2812_LED_NUM_RESET, (void *)&ws2812_bar_ctrl_led_all);
            wbar->led_num = ws2812_bar_ctrl_led_all; // 更新灯珠总数
        }
        // 通道输出使能状态
        ws2812_dev->dev_attr.index_enable = req[3];
        bar_ctrl_para = &req[4];
    } else {
        tlc59108_dev = (TLC59108DevType_t)bar->private;
        if (tlc59108_dev == NULL) {
            goto set_error;
        }
        // 获取工作模式寄存器2中的数据
        bar_work_mode = req[2];

        // 发生驱动设备类型改变时 需重新初始化tlc59108设备
        if (cur_driver_type != TLC59108DEV) {
            cur_driver_type = TLC59108DEV;
            tlc59108_dev->tlc59108_dev_ops.init(tlc59108_dev);
        }
        bar_ctrl_para = &req[3];
    }

    cmd = find_proccessor((DriverDevType)bar_driver_type, (uint8_t)bar_work_mode);
    if (NULL != cmd) {
        cmd->callback(bar, bar_ctrl_para);
    }
set_error:
    return;
}

/**
 * @brief 数据解析任务
 *
 */
void data_analysis_task(void)
{
    uint8_t read_reg_buff[20] = {0}, read_reg_pos = 0, reg_need_rdwr_num = 0, reg_num;
    control_i2c(I2C0_DEV, I2C_GET_RECV_DATA_LEN, (void *)&recv_data_len);
    if (recv_data_len != 0) {
        if (recv_data_len > 0x01) {
            control_i2c(I2C0_DEV, I2C_RESET_RECV_DATA_LEN, NULL);
            // 接收数据后 先进行XOR校验
            XOR_CHECK(recv_data_buff[recv_data_len - 1], CheckXOR(recv_data_buff, recv_data_len));

            // 获取当前WS2812/TLC59108灯条 工作模式寄存器中设置的工作模式
            control_register(RD_REG_INFO, WS2812_WORK_MODE_REG_ADDR, &read_reg_buff[1], 0x01);
            control_register(RD_REG_INFO, TLC59108_WORK_MODE_REG_ADDR, &read_reg_buff[3], 0x01);

            // 获取灯驱寄存器总数
            control_register(GET_REG_NUM_INFO, 0, (void *)&reg_num, 0);

            // 写寄存器
            control_register(WR_RGE_INFO,
                             recv_data_buff[0],
                             &recv_data_buff[1],
                             recv_data_len - 2);
            // 实时获取寄存器的数据 到发送buff中
            control_register(RD_REG_INFO,
                             0x00,
                             set_send_buff,
                             reg_num);

            // 尝试获取驱动寄存器中 设置的驱动设备类型
            control_register(GET_REG_DRIVER_TYPE, 0, &read_reg_buff[0], 0x01);
            // 根据驱动设备类型寄存器值获取 该设备的寄存器偏移地址 和 该设备的寄存器数量
            if (read_reg_buff[0] == TLC59108DEV) {
                control_register(GET_TLC59108REG_POS, 0, &read_reg_pos, 0);
                control_register(GET_TLC59108REG_NUM_INFO, 0, &reg_need_rdwr_num, 0);
                // 获取写入数据后 TLC59108工作模式寄存器中的工作模式
                control_register(RD_REG_INFO, TLC59108_WORK_MODE_REG_ADDR, &read_reg_buff[4], 0x01);
                // 若获取的工作模式 和 写入之前的工作模式不一样 则清除之前的工作参数
                if (read_reg_buff[3] != read_reg_buff[4]) {
                    control_register(RESET_PARAM_SEG, TLC59108_PARA_BASE_ADDR, NULL, 0);
                }
            } else if (read_reg_buff[0] == WS2812DEV) {
                control_register(GET_WS2812REG_POS, 0, &read_reg_pos, 0);
                control_register(GET_WS2812REG_NUM_INFO, 0, &reg_need_rdwr_num, 0);
                // 获取写入数据后 WS2812工作模式寄存器中的工作模式
                control_register(RD_REG_INFO, WS2812_WORK_MODE_REG_ADDR, &read_reg_buff[2], 0x01);
                // 若获取的工作模式 和 写入之前的工作模式不一样 则清除之前的工作参数
                if (read_reg_buff[1] != read_reg_buff[2]) {
                    control_register(RESET_PARAM_SEG, WS2812_PARA_BASE_ADDR, NULL, 0);
                }
            } else {
                goto set_error;
            }
            // 根据寄存器偏移位置 及 需读取寄存器个数 读寄存器参数到read_reg_buff 用于控制灯条灯效
            control_register(RD_REG_INFO,
                             read_reg_pos,
                             &read_reg_buff[1],
                             reg_need_rdwr_num);
            led_bar_control(read_reg_buff, reg_need_rdwr_num + 1);
        set_error:
            control_i2c(I2C0_DEV, I2C_RESET_RECV_BUFF, (void *)&recv_data_buff);
        }
    }
}

/******************************************************************************/
/**
 * @brief 控制WS2812灯条进入熄灭模式
 *
 * @param led_bar       WS2812灯条对象地址
 * @param ctrl_para     WS2812灯条控制参数
 */
static void ctrl_ws2812_off(LedBarType_t led_bar, uint8_t *ctrl_para)
{
    WS2812BarType_t wbar = (WS2812BarType_t)led_bar;
    if (ctrl_para == NULL || wbar == NULL) {
        return;
    }

    // 熄灭模式下 熄灭最大灯珠数
    wbar->ctrl_led_num = wbar->led_num;
    wbar->start = 0;

    led_bar->off(led_bar);
}

/**
 * @brief 控制WS2812灯条进入常亮模式
 *
 * @param led_bar       WS2812灯条对象地址
 * @param ctrl_para     WS2812灯条控制参数
 */
static void ctrl_ws2812_on(LedBarType_t led_bar, uint8_t *ctrl_para)
{
    WS2812BarType_t wbar = (WS2812BarType_t)led_bar;
    uint8_t ctrl_led_pos, ctrl_led_num;
    float color[3];
    if (NULL == ctrl_para || NULL == wbar) {
        return;
    }

    // 关闭渲染
    if (wbar->render_switch != 0) {
        wbar->render_switch = 0;
    }
    // 关闭渲染动画
    if (wbar->render_param.render_animation != 0) {
        wbar->render_param.render_animation = 0;
    }

    color[0] = (float)ctrl_para[0];
    color[1] = (float)ctrl_para[1];
    color[2] = (float)ctrl_para[2];
    ctrl_led_num = ctrl_para[7];
    ctrl_led_pos = ctrl_para[8];

    // 默认设置
    wbar->ctrl_led_num = wbar->led_num;
    wbar->start = 0;

    if (ctrl_led_num != 0) {
        wbar->ctrl_led_num = ctrl_led_num;
    }
    if (ctrl_led_pos != 0) {
        wbar->start = ctrl_led_pos - 1;
    }

    led_bar->on(led_bar, color);
}

/**
 * @brief 控制WS2812灯条进入闪烁模式
 *
 * @param led_bar       WS2812灯条对象地址
 * @param ctrl_para     WS2812灯条控制参数
 */
static void ctrl_ws2812_blink(LedBarType_t led_bar, uint8_t *ctrl_para)
{
    WS2812BarType_t wbar = (WS2812BarType_t)led_bar;
    uint8_t blink_led_num, blink_start_pos, blink_mode, blink_period;
    if (NULL == ctrl_para || NULL == wbar) {
        return;
    }

    wbar->render_param.render_color1[0] = (float)ctrl_para[0];
    wbar->render_param.render_color1[1] = (float)ctrl_para[1];
    wbar->render_param.render_color1[2] = (float)ctrl_para[2];
    blink_led_num = ctrl_para[7];
    blink_start_pos = ctrl_para[8];
    blink_mode = ctrl_para[9];
    blink_period = ctrl_para[10] << 8 | ctrl_para[11];

    if (blink_period == 0 ||
        blink_start_pos == 0 ||
        blink_start_pos > wbar->led_num) {
        return;
    }
    BLINK_MODE_CHECK(blink_mode);

    // 设置默认闪烁周期50ms
    task_ms_reset(WS2812_RENDER_TASK, TASK_AUTO_SET_MS_LEVEL, blink_period * 50);

    led_bar->blink(led_bar, blink_mode, blink_led_num, blink_start_pos - 1);

set_error:
    return;
}

/**
 * @brief 控制WS2812灯条进入基本流水灯模式
 *
 * @param led_bar       WS2812灯条对象地址
 * @param ctrl_para     WS2812灯条控制参数
 */
static void ctrl_ws2812_base_water(LedBarType_t led_bar, uint8_t *ctrl_para)
{
    WS2812BarType_t wbar = (WS2812BarType_t)led_bar;
    uint8_t singal_led_num, water_mode, water_start_pos, water_period;
    if (NULL == ctrl_para || NULL == wbar) {
        return;
    }

    wbar->render_param.render_color1[0] = (float)ctrl_para[0];
    wbar->render_param.render_color1[1] = (float)ctrl_para[1];
    wbar->render_param.render_color1[2] = (float)ctrl_para[2];
    singal_led_num = ctrl_para[7];
    water_start_pos = ctrl_para[8];
    water_mode = ctrl_para[9];
    water_period = ctrl_para[10] << 8 | ctrl_para[11];

    if (water_start_pos == 0 ||
        water_period == 0||
        water_start_pos > wbar->led_num) {
        return;
    }
    BASE_WATER_MODE_CHECK(water_mode);

    // 设置默认流水周期10ms
    task_ms_reset(WS2812_RENDER_TASK, TASK_AUTO_SET_MS_LEVEL, water_period * 10);

    led_bar->water(led_bar, water_mode, singal_led_num, water_start_pos - 1);

set_error:
    return;
}

/**
 * @brief 控制WS2812灯条进入渐变流水灯模式
 *
 * @param led_bar       WS2812灯条对象地址
 * @param ctrl_para     WS2812灯条控制参数
 */
static void ctrl_ws2812_change_water(LedBarType_t led_bar, uint8_t *ctrl_para)
{
    WS2812BarType_t wbar = (WS2812BarType_t)led_bar;
    uint8_t singal_led_num, water_mode, water_start_pos, water_period;
    if (NULL == ctrl_para || NULL == wbar) {
        return;
    }

    // 渐变模式起始颜色
    wbar->render_param.render_color1[0] = (float)ctrl_para[0];
    wbar->render_param.render_color1[1] = (float)ctrl_para[1];
    wbar->render_param.render_color1[2] = (float)ctrl_para[2];
    // 渐变模式终止颜色
    wbar->render_param.render_color2[0] = (float)ctrl_para[3];
    wbar->render_param.render_color2[1] = (float)ctrl_para[4];
    wbar->render_param.render_color2[2] = (float)ctrl_para[5];
    singal_led_num = ctrl_para[7];
    water_start_pos = ctrl_para[8];
    water_mode = ctrl_para[9];
    water_period = ctrl_para[10] << 8 | ctrl_para[11];

    if (water_start_pos == 0 ||
        water_period == 0||
        water_start_pos > wbar->led_num) {
        return;
    }
    CHANGE_WATER_MODE_CHECK(water_mode);

    // 设置默认流水周期10ms
    task_ms_reset(WS2812_RENDER_TASK, TASK_AUTO_SET_MS_LEVEL, water_period * 10);

    led_bar->water(led_bar, water_mode, singal_led_num, water_start_pos - 1);

set_error:
    return;
}

/**
 * @brief 控制WS2812灯条进入呼吸模式
 *
 * @param led_bar
 * @param ctrl_para
 */
static void ctrl_ws2812_breath(LedBarType_t led_bar, uint8_t *ctrl_para)
{
    WS2812BarType_t wbar = (WS2812BarType_t)led_bar;
    uint8_t breath_period;
    if (NULL == ctrl_para || NULL == wbar) {
        return;
    }

    // 记录呼吸模式下 起始灯带颜色值
    wbar->render_param.render_color1[0] = (float)ctrl_para[0];
    wbar->render_param.render_color1[1] = (float)ctrl_para[1];
    wbar->render_param.render_color1[2] = (float)ctrl_para[2];
    // 记录呼吸模式下 终点灯带颜色值
    wbar->render_param.render_color2[0] = (float)ctrl_para[3];
    wbar->render_param.render_color2[1] = (float)ctrl_para[4];
    wbar->render_param.render_color2[2] = (float)ctrl_para[5];

    breath_period = ctrl_para[10] << 8 | ctrl_para[11];
    if (breath_period == 0) {
        return;
    }

    // 设置呼吸总周期100ms
    task_ms_reset(WS2812_RENDER_TASK, TASK_AUTO_SET_MS_LEVEL, wbar->render_param.breath_singal_period);

    led_bar->breath(led_bar, breath_period * 100);
}

static void ctrl_tlc59108_dimming(LedBarType_t led_bar, uint8_t *ctrl_para)
{
    TLC59108BarType_t tbar = (TLC59108BarType_t)led_bar;
    uint8_t *pwmx_databuff = NULL;
    if (tbar == NULL || ctrl_para == NULL) {
        return;
    }
    pwmx_databuff = &ctrl_para[0];
    tbar->dimming((void *)tbar, pwmx_databuff);
}

static void ctrl_tlc59108_blinking(LedBarType_t led_bar, uint8_t *ctrl_para)
{
    TLC59108BarType_t tbar = (TLC59108BarType_t)led_bar;
    uint8_t *pwmx_databuff = NULL;
    if (tbar == NULL || ctrl_para == NULL) {
        return;
    }
    pwmx_databuff = &ctrl_para[0];
    tbar->render_param.group_duty_ctrl_reg = ctrl_para[8];
    tbar->render_param.group_freq_ctrl_reg = ctrl_para[9];
    tbar->blink((void *)tbar, ctrl_para[11] << 8 | ctrl_para[10], pwmx_databuff);
}
