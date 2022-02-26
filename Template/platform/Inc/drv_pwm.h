#ifndef _DRV_PWM_H
#define _DRV_PWM_H

#include "gd32f3x0.h"
#include <string.h>
#include "common.h"

#define PWM_CHX_NUM_SET     (0x01u)
#define TEST_BOARD_B09

#if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x01) )
#define USING_PWM_CH1

#if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x02) )
#define USING_PWM_CH2
#endif

#if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x03) )
#define USING_PWM_CH3
#endif

#if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x04) )
#define USING_PWM_CH4
#endif

#if ( defined(PWM_CHX_NUM_SET) && (PWM_CHX_NUM_SET >= 0x05) )
#define USING_PWM_CH5
#endif

#endif

/* TIMER clk uint MHz */
#define FTIMER                      (84u)
#define GD32F3X0_PWM_TIMER_SET(ns)  (((ns) * FTIMER / 1000.0) - 1.0)

/* TIMER period set .unit ns*/
#define TIMER_PERIOD                (1250u)
#if defined(TEST_BOARD_B09)
#define PWM_ONE                     (850)
#else
#define PWM_ONE                     (780)
#endif
#define PWM_ZERO                    (TIMER_PERIOD - PWM_ONE)

/* TIMER output pwm polarity configuration */
#if defined(TEST_BOARD_B09)
#define TIMER_PWM_REVERSE           (0x01)
#else
#define TIMER_PWM_REVERSE           (0x00)
#endif

/* TIMER output pwm pin set */
#define TIMER_PWM_PIN_SET            0x00

enum pwm_cmd_list
{
    PWM_CMD_ENABLE = 0x00,
    PWM_CMD_DISABLE,
    PWM_CMD_SET,
};

struct pwm_base_attr
{
    uint32_t set_pulse;
    uint32_t set_period;
};
typedef struct pwm_base_attr* pwm_base_attr_t;

/******************************************************************************/
void init_pwm(void);
Size_Type write_pwm(uint8_t pwm_channel_index, const void *buffer, uint32_t size);
Rtv_Status control_pwm(uint8_t pwm_channel_index, int cmd, void *arg);

#endif /**************************** _DRV_PWM_H *******************************/
