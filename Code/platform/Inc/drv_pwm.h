/**
 * @file drv_pwm.h
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _DRV_PWM_H
#define _DRV_PWM_H

#include "common.h"

/******************************************************************************/
/**
 * @brief pwm gpio config attr
 *
 */
typedef struct pwm_gpio_config
{
    uint32_t mode;
    uint32_t pull_up_down;
    uint32_t speed;
    uint8_t otype;
} PwmGpioCfgType;
/**
 * @brief pwm timer config attr
 *
 */
typedef struct pwm_timer_config
{
    uint16_t oc_mode;          // PWM输出模式设置
    uint16_t oc_shadow_set;    // 输出通道影子寄存器使能设置
    uint16_t dma_trig_src_set; // 定时器触发DMA请求源设置
} PwmTimerCfgType;
/**
 * @brief pwm dma config attr
 *
 */
typedef struct pwm_dma_config
{
    rcu_periph_enum clk;
    uint32_t dma_int_src;
} PwmDmaCfgType;

/**
 * @brief PWM通用配置
 *
 */
typedef struct general_pwm_config
{
    PwmGpioCfgType gpio_cfg;
    PwmTimerCfgType timer_cfg;
    PwmDmaCfgType dma_cfg;
} GeneralPwmCfgType;
/* pwm gpio manage */
struct pwm_gpio_attr
{
    rcu_periph_enum clk;
    uint32_t port;
    uint32_t pin;
    uint32_t alt_func_num;
};
/* pwm timer manage */
struct pwm_timer_attr
{
    rcu_periph_enum clk;
    uint32_t periph;
    uint16_t channel;
};
/* pwm dma manage */
struct pwm_dma_attr
{
    dma_channel_enum channel;
    uint32_t per_addr;
    IRQn_Type dma_int_irq;
};

/******************************************************************************/
typedef struct pwm_driver_channel
{
    struct pwm_gpio_attr _gpio_attr;
    struct pwm_timer_attr _timer_attr;
    struct pwm_dma_attr _dma_attr;
} PwmDriverChannelType;
typedef PwmDriverChannelType *PwmDriverChannelType_t;

/**
 * @brief PWM设备驱动类型
 *
 */
typedef enum
{
    WS2812DEV = 0x01u,
    TLC59108DEV,
} driver_dev_type;

/**
 * @brief PWM设备控制命令
 *
 */
typedef enum pwm_ctrl_cmd
{
    PWM_CTRL_ENABLE = 0x01,
    PWM_CTRL_DISABLE,
    PWM_CTRL_SET_BASEATTR,
    PWM_CTRL_SET_DRV_TYPE,
    PWM_CTRL_DEV_DEINIT,
} PWMCtrlCmdType;

typedef struct
{
    /**
     * @brief PWM设备驱动通道
     *
     */
    PwmDriverChannelType_t driver_channel;
    /**
     * @brief 驱动设备类型
     *
     * @note  默认设置为WS2812
     */
    driver_dev_type driver_type;
    /**
     * @brief 用于保存WS2812驱动设备下 当前选择的PWM波输出通道
     *
     * @note  该通道主要用于DMA搬运占空比之后 在中断函数中清除该通道标志位等状态
     */
    uint8_t driver_channel_cur;
    /**
     * @brief PWM设备驱动通道总数
     *
     * @note  默认设置为8通道 WS2812驱动设备仅支持7路PWM波输出
     */
    uint8_t driver_channel_num : 7;
    /**
     * @brief 完成量
     *
     * @note  仅用于WS2812驱动设备模式下 用于保证单次控制PWM波输出正常
     */
    uint8_t trans_completion : 1;
    RtvStatus (*init)(void *pwm_dev);
    RtvStatus (*deinit)(void *pwm_dev);
    SizeType (*write)(void *pwm_dev,
                       const uint8_t pwm_channel_index,
                       const void *buffer,
                       const uint32_t size);
    RtvStatus (*control)(void *pwm_dev,
                          const uint8_t pwm_channel_index,
                          const PWMCtrlCmdType cmd,
                          void *arg);
} pwm_dev_type;
typedef pwm_dev_type *pwm_dev_t;

/**
 * @brief PWM设备通道基本属性 PWM的占空比和周期
 *
 */
typedef struct
{
    uint32_t set_pulse;
    uint32_t set_period;
} pwm_channel_base_attr_type;
typedef pwm_channel_base_attr_type *pwm_channel_base_attr_t;
/******************************************************************************/
/**
 * @brief PWM通道数量及初始化PWM信号输出通道设置
 *
 */
#define PWM_CHX_NUM_SET (0x01u)
#define PWM_CHX_INDEX_INIT_SET (0x01u)

#define SET_DRIVER_TYPE_CHECK(type) \
    do                              \
    {                               \
        if (type != TLC59108DEV &&  \
            type != WS2812DEV)      \
        {                           \
            goto set_error;         \
        }                           \
    } while (0)

/**
 * @brief 定时器时钟频率 及 通过定时时间ns计算给定定时器的周期值
 *
 */
#define FTIMER (84u)
#define GD32F3X0_PWM_TIMER_SET(ns) (uint32_t)(((ns)*FTIMER / 1000.0) - 1.0)

/**
 * @brief B09和PJ0019开发环境兼容处理
 *
 */
//#define TEST_BOARD_B09

/**
 * @brief WS2812 PWM波驱动频率
 *
 * @note  Fq >= 800KHz  -> Period <= 1/Fq(1.25us ~ 1250ns) Unit：ns
 */
#define WS2812_PWM_PERIOD (1250u)
#if defined(TEST_BOARD_B09)
#define WS2812_PWM_ONE (780)
#else
#define WS2812_PWM_ONE (780)
#endif
#define WS2812_PWM_ZERO (WS2812_PWM_PERIOD - WS2812_PWM_ONE)

/**
 * @brief TLC59108 PWM波驱动频率
 *
 * @note  Fq >= 100KHz  -> F / (Period * psc) >= 100KHz (F:84MHz)
 *                      -> Period * psc <= 840
 */
#define TLC59108_PWM_FRE_NEED (100) // pwm时速要求大于100KHz
#define TLC59108_PWM_PERIOD (255)
#define TLC59108_TIMER_PSC (FTIMER * 1000 / (TLC59108_PWM_PERIOD * TLC59108_PWM_FRE_NEED))

/* TIMER output pwm polarity configuration */
#if defined(TEST_BOARD_B09)
#define TIMER_PWM_REVERSE (0x01)
#else
#define TIMER_PWM_REVERSE (0x00)
#endif

/******************************************************************************/
/**
 * @brief 提供对外初始化PWM设备接口
 *
 */
RtvStatus init_pwm_dev(void);

/**
 * @brief 提供对外重新初始化PWM设备接口
 *
 * @return RtvStatus 
 */
RtvStatus deinit_pwm_dev(void);

/**
 * @brief 提供对外获取PWM设备接口
 *
 * @note   通过该接口可使用内置的init()/write()/control()方法
 *         方法说明:
 *         init(void *pwm_dev): 可传入PWM设备地址 对该设备进行初始化
 *         通过以下接口 可对PWM进行写操作
 *         write(void *pwm_dev, const uint8_t pwm_channel_index, const void *buffer, const uint32_t size)
 *         @pwm_dev             PWM设备地址
 *         @pwm_channel_index   PWM设备的通道 默认为1
 *         @buffer              待写入的数据缓存区
 *         @size                待写入的PWM波个数(字节数)
 *         control(void *pwm_dev, const uint8_t pwm_channel_index, const PWMCtrlCmdType cmd, void *arg)
 *         @pwm_dev             PWM设备地址
 *         @pwm_channel_index   PWM设备的通道 默认为1
 *         @cmd PWM_CTRL_ENABLE(使能PWM设备输出PWM波指令) arg(无效参数)
 *              PWM_CTRL_DISABLE(失能PWM设备输出PWM波指令) arg(无效参数)
 *              PWM_CTRL_SET_BASEATTR(PWM设备PWM波周期/占空比基本属性设置指令) arg(基本属性参数地址)
 *              PWM_CTRL_SET_DRV_TYPE(PWM设备驱动类型设置指令) arg(驱动类型参数)
 *              PWM_CTRL_DEV_DEINIT(PWM设置重新初始化) arg(无效参数)
 */
pwm_dev_t find_pwm_dev(void);

#endif /**************************** _DRV_PWM_H *******************************/
