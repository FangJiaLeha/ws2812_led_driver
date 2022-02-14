#include "led_bar.h"

/******************************************************************************/
const uint8_t color_table[][3] = {
    {0, 0, 0},           // 黑色 --> 0
    {0xFF, 0xFF, 0xFF},  // 白色 --> 1
    {0xFF, 0, 0},        // 红色 --> 2
    {0, 0xFF, 0},        // 绿色 --> 3
    {0, 0, 0xFF},        // 蓝色 --> 4
    {0xFF, 0xFF, 0},     // 黄色 --> 5
    {0, 0xFF, 0xFF},     // 青色 --> 6
    {0xFF, 0, 0xFF},     // 紫色 --> 7
};
#define MAX_COLOR_INDEX     ( sizeof(color_table) / sizeof(color_table[0]) - 1)

static struct ws2812_bar led_bar1;
static led_bar_t Lbar[] = {(led_bar_t)&led_bar1};

static uint16_t recv_data_len = 0;
static uint8_t *recv_data_buff = NULL;
static uint8_t *set_send_buff = NULL;

// 应用逻辑接口声明
/* 基本命令控制 */
static void ctrl_led_off(led_bar_t led_bar, uint8_t *ctrl_para);
static void ctrl_led_on(led_bar_t led_bar, uint8_t *ctrl_para);
static void ctrl_led_rgb(led_bar_t led_bar, uint8_t *ctrl_para);
static void ctrl_led_blink(led_bar_t led_bar, uint8_t *ctrl_para);
static void ctrl_led_water(led_bar_t led_bar, uint8_t *ctrl_para);
static void ctrl_led_breath(led_bar_t led_bar, uint8_t *ctrl_para);
/* IAP命令控制 */
static void ctrl_mcu_reset(led_bar_t led_bar, uint8_t *ctrl_para);
static void get_mcu_work_mode(led_bar_t led_bar, uint8_t *ctrl_para);
static void get_mcu_version(led_bar_t led_bar, uint8_t *ctrl_para);

struct cmd_list
{
    uint8_t cmd_id;
    void (*callback)(led_bar_t led_bar, uint8_t *para);
};

// 基本控制命令及其控制逻辑索引列表
const struct cmd_list base_cmds[] = {
    /**
     * @brief 控制led关闭
     */
    {LED_OFF, ctrl_led_off},

    /**
     * @brief 控制led开启
     */
    {LED_ON, ctrl_led_on},

    /**
     * @brief 控制led 进入rgb模式
     */
    {LED_RGB, ctrl_led_rgb},

    /**
     * @brief 控制led 进入流水灯模式
     */
    {LED_WATER, ctrl_led_water},

    /**
     * @brief 控制led 进入闪烁模式
     */
    {LED_BLINK, ctrl_led_blink},

    /**
     * @brief 控制led 进入呼吸模式
     */
    {LED_BREATH, ctrl_led_breath},
};

// IAP控制命令及其控制逻辑索引列表
const struct cmd_list iap_cmds[] = {
    /**
     * @brief 软复位
     */
    {SOFT_RESET, ctrl_mcu_reset},

    /**
     * @brief 查询MCU工作在boot/app模式下
     */
    {CHECK_WORK_MODE, get_mcu_work_mode},

    /**
     * @brief 获取软件版本
     */
    {GET_SOFT_VERSION, get_mcu_version},
};

static const struct cmd_list *find_proccessor(uint8_t cmd_type, uint8_t cmd_id)
{
    uint8_t cmds_num = 0;
    if (cmd_type == BASE_CMD) {
        cmds_num = ITEM_NUM(base_cmds);
    } else if (cmd_type == IAP_CMD) {
        cmds_num = ITEM_NUM(iap_cmds);
    }

    for (uint8_t cmds_cnt = 0; cmds_cnt < cmds_num; cmds_cnt++) {
        if (cmd_type == BASE_CMD) {
            if (cmd_id == base_cmds[cmds_cnt].cmd_id) {
                return &base_cmds[cmds_cnt];
            }
        } else if (cmd_type == IAP_CMD) {
            if (cmd_id == iap_cmds[cmds_cnt].cmd_id) {
                return &iap_cmds[cmds_cnt];
            }
        }
    }
    return NULL;
}
/******************************************************************************/
static Rtv_Status ws2812_bar_set_color( led_bar_t bar, float *color )
{
    ws2812_bar_t wbar = (ws2812_bar_t)bar;
    struct ws2812_bar_ctrlpack pack;
    ws2812_dev_t wsdev = (ws2812_dev_t)bar->private;

    pack.color[0] = (uint8_t)color[0];
    pack.color[1] = (uint8_t)color[1];
    pack.color[2] = (uint8_t)color[2];

    pack.start = wbar->start;
    pack.count = wbar->led_num;

    wsdev->ws2812_dev_ops.control(wsdev, WS2812_CTRL_BAR_COLOR, &pack);
    return SUCCESS;
}

#if (defined(_TEST_) && _TEST_ == 0x01)
uint8_t CheckXOR(const uint8_t *data_buff,uint8_t buff_len)
#else
static uint8_t CheckXOR(const uint8_t *data_buff,uint8_t buff_len)
#endif
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
/**
 * @brief 初始化led灯带接口
 *
 */
Rtv_Status init_led_bars(uint8_t led_bar_index)
{
    Rtv_Status rtv_status = SUCCESS;
    ws2812_dev_t wsdev = find_ws2812_dev();

    if (NULL == wsdev || led_bar_index == 0) {
        return EINVAL;
    }

    uint16_t ws_led_num = WS2812_LED_NUM;
    wsdev->dev_attr.index = led_bar_index;
    rtv_status = wsdev->ws2812_dev_ops.init(wsdev);
    if (rtv_status != SUCCESS) {
        return rtv_status;
    }
    rtv_status = wsdev->ws2812_dev_ops.control(wsdev, WS2812_CTRL_INIT, &ws_led_num);
    if (rtv_status != SUCCESS) {
        return rtv_status;
    }

    init_ws2812_bar(&led_bar1, 1, ws2812_bar_set_color, wsdev, WS2812_LED_NUM, 0);

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
    led_bar_t bar = NULL;
    const struct cmd_list *cmd = NULL;
    uint8_t *bar_ctrl_para = NULL;
    uint8_t bar_cmd_mode = req[0];
    uint8_t bar_id = req[1];
    uint8_t bar_ctrl_mode = req[2];
    uint8_t bar_cmd_xor = req[8];

    /* req vaild check */
    BAR_REQ_LEN_CHECK(req_len);
    BAR_CMD_MODE_CHECK(bar_cmd_mode);
    BAR_CMD_ID_CHECK(bar_id, ITEM_NUM(Lbar));
    BAR_CMD_CTL_MODE_CHECK(bar_ctrl_mode);
    BAR_CMD_XOR_CHECK(bar_cmd_xor, CheckXOR(req, req_len));

    bar = Lbar[bar_id - 1];
    bar_ctrl_para = &req[3];
    cmd = find_proccessor(bar_cmd_mode, bar_ctrl_mode);
    if (NULL != cmd) {
        cmd->callback(bar, bar_ctrl_para);
    }

set_error:
    return;
}

void data_analysis_task(void)
{
    control_i2c(I2C0_DEV, I2C_GET_RECV_DATA_LEN, (void *)&recv_data_len);
    if (recv_data_len != 0) {
        control_i2c(I2C0_DEV, I2C_RESET_RECV_DATA_LEN, NULL);
        led_bar_control(recv_data_buff, recv_data_len);
        control_i2c(I2C0_DEV, I2C_RESET_RECV_BUFF, (void *)&recv_data_buff);
    }
}

/******************************************************************************/
/* 灯条基本命令应用逻辑层(cmd:0x90) */
static void ctrl_led_off(led_bar_t led_bar, uint8_t *ctrl_para)
{
    led_bar->off(led_bar);
}

static void ctrl_led_on(led_bar_t led_bar, uint8_t *ctrl_para)
{
    uint8_t color_index;
    ws2812_bar_t wbar = (ws2812_bar_t)led_bar;
    float color[3];
    if (NULL == ctrl_para || NULL == wbar) {
        return;
    }

    color_index = ctrl_para[3];
    if (color_index > MAX_COLOR_INDEX) {
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

    color[0] = (float)color_table[color_index][0];
    color[1] = (float)color_table[color_index][1];
    color[2] = (float)color_table[color_index][2];
    led_bar->on(led_bar, color);
}

static void ctrl_led_rgb(led_bar_t led_bar, uint8_t *ctrl_para)
{
    ws2812_bar_t wbar = (ws2812_bar_t)led_bar;
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

    color[0] = (float)ctrl_para[1];
    color[1] = (float)ctrl_para[2];
    color[2] = (float)ctrl_para[3];

    led_bar->on(led_bar, color);
}

static void ctrl_led_blink(led_bar_t led_bar, uint8_t *ctrl_para)
{
    ws2812_bar_t wbar = (ws2812_bar_t)led_bar;
    uint8_t blink_mode, color_index, blink_led_num, blink_period;
    if (NULL == ctrl_para || NULL == wbar) {
        return;
    }
    if (color_index > MAX_COLOR_INDEX) {
        return;
    }
    
    blink_mode = ctrl_para[0];
    color_index = ctrl_para[1];
    blink_led_num = ctrl_para[2];
    blink_period = ctrl_para[3];
    wbar->render_param.render_color1[0] = (float)color_table[color_index][0];
    wbar->render_param.render_color1[1] = (float)color_table[color_index][1];
    wbar->render_param.render_color1[2] = (float)color_table[color_index][2];

    // 设置默认闪烁周期50ms
    task_ms_reset(WS2812_RENDER_TASK, TASK_AUTO_SET_MS_LEVEL, blink_period * 50);
    wbar->parent.blink(led_bar, blink_mode, blink_led_num);
}

static void ctrl_led_water(led_bar_t led_bar, uint8_t *ctrl_para)
{
    ws2812_bar_t wbar = (ws2812_bar_t)led_bar;
    uint8_t water_mode, color_index, singal_led_num, move_per;
    if (NULL == ctrl_para || NULL == wbar) {
        return;
    }
    color_index = ctrl_para[1];
    if (color_index > MAX_COLOR_INDEX) {
        return;
    }

    water_mode = ctrl_para[0];
    singal_led_num = ctrl_para[2];
    move_per = ctrl_para[3];

    wbar->render_param.render_color1[0] = (float)color_table[color_index][0];
    wbar->render_param.render_color1[1] = (float)color_table[color_index][1];
    wbar->render_param.render_color1[2] = (float)color_table[color_index][2];

    // 设置默认流水周期10ms
    task_ms_reset(WS2812_RENDER_TASK, TASK_AUTO_SET_MS_LEVEL, move_per * 10);
    wbar->parent.water(led_bar, water_mode, singal_led_num, move_per);
}

static void ctrl_led_breath(led_bar_t led_bar, uint8_t *ctrl_para)
{
    ws2812_bar_t wbar = (ws2812_bar_t)led_bar;
    uint8_t breath_period, s_color_index, e_color_index;
    if (NULL == ctrl_para || NULL == wbar) {
        return;
    }

    breath_period = ctrl_para[1];
    s_color_index = ctrl_para[2];
    e_color_index = ctrl_para[3];
    if (s_color_index > MAX_COLOR_INDEX || e_color_index > MAX_COLOR_INDEX) {
        return;
    }

    // 记录呼吸模式下 起始灯带颜色值
    wbar->render_param.render_color1[0] = (float)color_table[s_color_index][0];
    wbar->render_param.render_color1[1] = (float)color_table[s_color_index][1];
    wbar->render_param.render_color1[2] = (float)color_table[s_color_index][2];
    // 记录呼吸模式下 终点灯带颜色值
    wbar->render_param.render_color2[0] = (float)color_table[e_color_index][0];
    wbar->render_param.render_color2[1] = (float)color_table[e_color_index][1];
    wbar->render_param.render_color2[2] = (float)color_table[e_color_index][2];

    // 设置呼吸总周期100ms
    task_ms_reset(WS2812_RENDER_TASK, TASK_AUTO_SET_MS_LEVEL, wbar->render_param.breath_singal_period);
    wbar->parent.breath(led_bar, breath_period * 100);
}

/* 灯条IAP命令应用逻辑层(cmd:0x16) */
static void ctrl_mcu_reset(led_bar_t led_bar, uint8_t *ctrl_para)
{
    control_i2c(I2C0_DEV, I2C_RESET_SEND_BUFF, (void *)&set_send_buff);
    control_i2c(I2C0_DEV, I2C_RESET_SEND_DATA_LEN, NULL);

    __disable_irq();
    ResetSignature[0] = RESET_SIGNATURE0_UPDATE_FIRMWARE;
    ResetSignature[1] = RESET_SIGNATURE1_UPDATE_FIRMWARE;
    NVIC_SystemReset();
    while(1);
}

static void get_mcu_work_mode(led_bar_t led_bar, uint8_t *ctrl_para)
{
    control_i2c(I2C0_DEV, I2C_RESET_SEND_BUFF, (void *)&set_send_buff);
    set_send_buff[0] = IAP_CMD;
    set_send_buff[1] = LED_BAR_INDEX;
    set_send_buff[2] = CHECK_WORK_MODE;
    set_send_buff[3] = MCU_WORK_IN_APP_MODE;  // mcu工作在app模式下
    set_send_buff[4] = 0x00;
    set_send_buff[5] = 0x00;
    set_send_buff[6] = 0x00;
    set_send_buff[7] = 0x00;
    set_send_buff[8] = CheckXOR(set_send_buff, 0x09);
    control_i2c(I2C0_DEV, I2C_RESET_SEND_DATA_LEN, NULL);
}

static void get_mcu_version(led_bar_t led_bar, uint8_t *ctrl_para)
{
    control_i2c(I2C0_DEV, I2C_RESET_SEND_BUFF, (void *)&set_send_buff);
    set_send_buff[0] = IAP_CMD;
    set_send_buff[1] = LED_BAR_INDEX;
    set_send_buff[2] = GET_SOFT_VERSION;
    set_send_buff[3] = (PROGRAM_VERSION >> 16) & 0xFF; // Major 主版本号
    set_send_buff[4] = (PROGRAM_VERSION >> 8) & 0xFF;  // Minor 次版本号
    set_send_buff[5] = (PROGRAM_VERSION >> 0) & 0xFF;  // Revision 修订版本号
    set_send_buff[6] = 0x00;
    set_send_buff[7] = 0x00;
    set_send_buff[8] = CheckXOR(set_send_buff, 0x09);
    control_i2c(I2C0_DEV, I2C_RESET_SEND_DATA_LEN, NULL);
}
