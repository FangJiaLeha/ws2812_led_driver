/**
 * @file drv_wdgt.h
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _DRV_WDGT_H
#define _DRV_WDGT_H

#include "common.h"

//==============================================================================
#define FRE_WDGT                (40000ul)       // 40KHz -> 40000Hz
#define WDGT_PRES_DIV           (64u)           // 分频系数
#define WDGT_TIMEOUT_SET        (10u)            // 超时时间 单位s

#define WDGT_RELOAD_VALUE(timeout)  (timeout * FRE_WDGT / WDGT_PRES_DIV)

//==============================================================================
/**
 * @brief 初始化看门狗 此处使用独立窗门狗
 *
 * @param timeout           超时时间 单位s
 * @return Rtv_Status       @SUCCESS:初始化成功 @其他值:初始化失败
 */
Rtv_Status init_wdgt(const uint16_t timeout_s);

/**
 * @brief 喂狗
 *
 */
void clear_wdgt(void);

#endif
