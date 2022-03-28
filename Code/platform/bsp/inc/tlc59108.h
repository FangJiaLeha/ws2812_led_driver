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
    void *pwm_dev_addr;
} TLC59108DevAttrType;

typedef struct tlc59108_dev
{
    TLC59108DevAttrType dev_attr;
    struct dev_ops tlc59108_dev_ops;
} tlc59108_dev_type;
typedef tlc59108_dev_type *tlc59108_dev_t;

typedef enum tlc59108_ctrl_cmd
{
    TLC59108_CTRL_INIT = 0x01,
    TLC59108_CTRL_GET_PWMx,
    TLC59108_CTRL_UPDATE_DEVDATA
} TLC59108CtrlCmdType;

#define TLC59108_CHANNNEL_MAX_NUM (0x08u)

/******************************************************************************/
/**
 * @brief 提供对外获取tlc59108设备地址接口
 *
 * @return tlc59108_dev_t 返回tlc59108设备地址
 *
 * @note   通过该接口可使用内置的init()/control()方法
 *         方法说明:
 *         init(void *dev): 可传入TLC59108灯条设备地址 对该灯条进行初始化
 *         control(void *dev, const uint8_t cmd, void *arg)：输入相关TLC59108灯条设备控制命令即可控制该设备
 *         @cmd TLC59108_CTRL_INIT(初始化灯条通道命令) arg(对应的通道值)
 *              TLC59108_CTRL_UPDATE_DEVDATA(灯条数据更新命令) arg(无效值)
 *              TLC59108_CTRL_GET_PWMx(获取灯条PWM通道buff) arg(操作指针地址)
 */
tlc59108_dev_t find_tlc59108_dev(void);

#endif
