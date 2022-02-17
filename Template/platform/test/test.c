#include "test.h"
#include "ws2812_led_bar.h"

uint8_t flag = 0;

void test_func_enter(void)
{
    #if ( defined(_PWM_TEST) && _PWM_TEST == 0x01u )
    uint16_t buffer[] = {1250*1/2, 0, PWM_ONE, 0, PWM_ZERO, 0};
    uint16_t buffer1[] = {PWM_ONE};
    write_pwm(0, buffer, sizeof(buffer) / sizeof(buffer[0]));
    write_pwm(0, buffer1, sizeof(buffer1) / sizeof(buffer1[0]));
    struct pwm_base_attr attr = {TIMER_PERIOD/2, TIMER_PERIOD};
    struct pwm_base_attr attr1 = {TIMER_PERIOD/2, TIMER_PERIOD*2};
    control_pwm(0, PWM_CMD_SET, (void *)&attr);
    control_pwm(0, PWM_CMD_DISABLE, (void *)&attr);
    control_pwm(0, PWM_CMD_ENABLE, (void *)&attr);
    control_pwm(0, PWM_CMD_SET, (void *)&attr1);
    control_pwm(0, PWM_CMD_DISABLE, (void *)&attr1);
    control_pwm(0, PWM_CMD_ENABLE, (void *)&attr1);
    #endif
    
    #if (defined(_TEST_) && _TEST_ == 0x01)
    extern uint8_t CheckXOR(const uint8_t *data_buff,uint8_t buff_len);
    extern void led_bar_control(uint8_t *req, uint8_t req_len);
    #endif
    #if (defined(_WS2812_DRV_TSET) && _WS2812_DRV_TSET == 0x01)
    init_led_bars(LED_BAR_INDEX);
    #endif
    
    #if (defined(_TASH_SCH_TEST) && _TASH_SCH_TEST == 0x01)
    task_register(WS2812_RENDER_TASK, TASK_10MS_LEVEL, ws2812_render);
    #endif
    
    #if (defined(_LED_BAR_TEST) && _LED_BAR_TEST == 0x01)
    uint8_t req[] = {0x90, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    #if (defined(_LED_OFF_TEST) && _LED_OFF_TEST == 0x01)
    req[2] = LED_OFF;
    req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
    #endif

    #if (defined(_LED_ON_TEST) && _LED_ON_TEST == 0x01)
    req[2] = LED_ON;
        /* 白色常亮测试 */
        req[6] = 1;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 红色常亮测试 */
        req[6] = 2;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 绿色常亮测试 */
        req[6] = 3;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 蓝色常亮测试 */
        req[6] = 4;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 黄色常亮测试 */
        req[6] = 5;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 青色常亮测试 */
        req[6] = 6;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 紫色常亮测试 */
        req[6] = 7;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 异常颜色测试 */
        req[6] = 8;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 黑色常亮测试 */
        req[6] = 0;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
    #endif

    #if (defined(_LED_RGB_TEST) && _LED_RGB_TEST == 0x01)
    req[2] = LED_RGB;
    uint32_t rgb_value;
        /* 蓝色测试 */
        rgb_value = 0x00FFFFul;
        req[4] = rgb_value >> 16;
        req[5] = rgb_value >> 8 & 0xFF;
        req[6] = rgb_value & 0xFF;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 紫色测试 */
        rgb_value = 0xFF00FFul;
        req[4] = rgb_value >> 16;
        req[5] = rgb_value >> 8 & 0xFF;
        req[6] = rgb_value & 0xFF;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 黄色测试 */
        rgb_value = 0xFFFF00ul;
        req[4] = rgb_value >> 16;
        req[5] = rgb_value >> 8 & 0xFF;
        req[6] = rgb_value & 0xFF;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
    #endif

// 流水灯测试
    #if (defined(_LED_WATER_TEST) && _LED_WATER_TEST == 0x01)
    req[2] = LED_WATER;
    #if (defined(_LEFT_WATER_TEST) && _LEFT_WATER_TEST == 0x01)
        /* 左转 蓝色 单次流水灯数量1个 流水速度：200ms*/
        req[3] = 0x01;
        req[4] = 0x04;
        req[5] = 0x05;
        req[6] = 0x03;
    req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
    #else
        /* 右转 白色 单次流水灯数量2个 流水速度：500ms */
        req[3] = 0x02;
        req[4] = 0x01;
        req[5] = 0x0A;
        req[6] = 0x03;
    req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
    #endif
    #endif

// 分段闪烁测试
    #if (defined(_LED_BLINK_TEST) && _LED_BLINK_TEST == 0x01)
    req[2] = LED_BLINK;
    #if (defined(_LEFT_BLINK_TEST) && _LEFT_BLINK_TEST == 0x01)
        /* 左边 绿色 10个led灯 闪烁周期50ms*/
        req[3] = 0x03;
        req[4] = 0x03;
        req[5] = 0x0A;
        req[6] = 0x01;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
    #else
        /* 右边 蓝色 10个led灯 闪烁周期250ms */
        req[3] = 0x04;
        req[4] = 0x04;
        req[5] = 0x0A;
        req[6] = 0x05;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
    #endif
    #endif

// 呼吸模式测试
    #if (defined(_LED_BREATH_TEST) && _LED_BREATH_TEST == 0x01)
    req[2] = LED_BREATH;
        /* 灯带白色呼吸 呼吸周期 500ms */
        req[4] = 5;
        req[5] = 0x01;
        req[6] = 0x00;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 灯带黑色->白色呼吸 呼吸周期 1000ms */
        req[4] = 10;
        req[5] = 0x00;
        req[6] = 0x01;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
        /* 灯带红色->绿色呼吸 呼吸周期 800ms */
        req[4] = 8;
        req[5] = 0x02;
        req[6] = 0x03;
        req[8] = CheckXOR(req, ITEM_NUM(req));
    led_bar_control(req, ITEM_NUM(req));
    #endif
    #endif
    while(1)
    {
        task_server();
    }
}
