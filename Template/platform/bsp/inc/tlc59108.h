/**
 * @file tlc59108.h
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-16
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _TLC59108_H
#define _TLC59108_H

#include "common.h"
#include "drv_pwm.h"

/******************************************************************************/
typedef struct tlc59108_dev_attr
{
    uint8_t pwmx_start_index;
    uint8_t pwmx_max_num;
    uint8_t *pwmx;
    void *private;
}TLC59108DevAttrType;

typedef struct tlc59108_dev
{
    TLC59108DevAttrType dev_attr;
    struct dev_ops tlc59108_dev_ops;
}TLC59108DevType;
typedef TLC59108DevType* TLC59108DevType_t;

typedef enum tlc59108_ctrl_cmd
{
    TLC59108_CTRL_INIT = 0x01,
    TLC59108_CTRL_GET_PWMx,
    TLC59108_CTRL_UPDATE_DEVDATA
}TLC59108CtrlCmdType;

#define TLC59108_CHANNNEL_MAX_NUM               (0x08u)

/******************************************************************************/
/**
 * @brief 提供对外获取tlc59108设备地址接口
 *
 * @return TLC59108DevType_t 返回tlc59108设备地址
 */
TLC59108DevType_t find_tlc59108_dev(void);

#endif
