/**
 * @file register_info.h
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _REGISTER_INFO_H
#define _REGISTER_INFO_H

#include "common.h"
#include "drv_pwm.h"

typedef enum
{
    /**
     * @brief 写灯驱寄存器指令
     *
     */
    WR_RGE_INFO = 0x01,
    /**
     * @brief 读灯驱寄存器指令
     *
     */
    RD_REG_INFO,
    /**
     * @brief 复位灯驱寄存器指令
     *
     */
    RESET_REG_INFO,
    /**
     * @brief 获取TLC59108寄存器个数指令
     *
     */
    GET_TLC59108REG_NUM_INFO,
    /**
     * @brief 获取WS2812寄存器个数指令
     *
     */
    GET_WS2812REG_NUM_INFO,
    /**
     * @brief 获取寄存器总数量指令
     *
     */
    GET_REG_NUM_INFO,
    /**
     * @brief 获取驱动寄存器驱动类型指令
     *
     */
    GET_REG_DRIVER_TYPE,
    /**
     * @brief 获取TLC59108寄存器偏移位置指令
     *
     */
    GET_TLC59108REG_POS,
    /**
     * @brief 获取WS2812寄存器偏移位置指令
     *
     */
    GET_WS2812REG_POS,
    /**
     * @brief 根据读取寄存器位置获取其后寄存器数量指令
     *
     */
    RESET_PARAM_SEG
} ctrl_reg_cmd_type;

/**
 * @brief WS2812/TLC59108寄存器参数区地址偏移
 *
 */
#define WS2812_PARA_BASE_ADDR (0x16u)
#define TLC59108_PARA_BASE_ADDR (0x02u)

/**
 * @brief WS2812/TLC59108灯条工作模式寄存器地址
 *
 */
#define WS2812_WORK_MODE_REG_ADDR (0x13u)
#define TLC59108_WORK_MODE_REG_ADDR (0x01u)

#define CTRL_REG_CMD_CHECK(cmd)                \
    do                                         \
    {                                          \
        if (cmd != WR_RGE_INFO &&              \
            cmd != RD_REG_INFO &&              \
            cmd != RESET_REG_INFO &&           \
            cmd != GET_TLC59108REG_NUM_INFO && \
            cmd != GET_WS2812REG_NUM_INFO &&   \
            cmd != GET_REG_NUM_INFO &&         \
            cmd != GET_REG_DRIVER_TYPE &&      \
            cmd != GET_TLC59108REG_POS &&      \
            cmd != GET_WS2812REG_POS &&        \
            cmd != RESET_PARAM_SEG)            \
        {                                      \
            goto set_error;                    \
        }                                      \
    } while (0)

#define REGADDR_CHECK(cmd, regAddr, regMaxNum)            \
    do                                                    \
    {                                                     \
        if ((cmd == WR_RGE_INFO || cmd == RD_REG_INFO) && \
            (regAddr > regMaxNum - 1))                    \
        {                                                 \
            goto set_error;                               \
        }                                                 \
    } while (0)

#define CTRL_REG_SIZE_CHECK(cmd, size)                    \
    do                                                    \
    {                                                     \
        if ((cmd == WR_RGE_INFO || cmd == RD_REG_INFO) && \
            size == 0)                                    \
        {                                                 \
            goto set_error;                               \
        }                                                 \
    } while (0)

#define CTRL_REG_ARG_CHECK(cmd, arg)                      \
    do                                                    \
    {                                                     \
        if ((cmd == WR_RGE_INFO || cmd == RD_REG_INFO) && \
            arg == NULL)                                  \
        {                                                 \
            goto set_error;                               \
        }                                                 \
    } while (0)

/******************************************************************************/
/**
 * @brief 初始化灯驱寄存器
 *
 * @return ErrStatus        @SUCCESS:初始化灯驱寄存器成功 @ERROR:初始化灯驱寄存器失败
 */
ErrStatus init_register(void);

/**
 * @brief 操作灯驱寄存器
 *
 * @param cmd           操作命令
 * @param regAddr       寄存器地址
 * @param arg           指针参数 用于传入读写buff
 * @param size          读写字节数
 * @return ErrStatus
 *
 * @note  输入对应操作灯驱寄存器的指令即可操作该寄存器
 *        控制命令说明:
 *        @cmd RD_REG_INFO(读寄存器指令) regAddr(寄存器的地址) arg(接收数据地址) size(读取字节数)
 *             WR_RGE_INFO(写寄存器指令) regAddr(寄存器的地址) arg(写入数据地址) size(写入字节数)
 *             GET_REG_NUM_INFO(读取灯驱寄存器个数指令) regAddr/size(无效参数) arg(接收数据地址)
 *             GET_REG_DRIVER_TYPE(获取当前驱动设备类型指令) 同上
 *             GET_TLC59108REG_NUM_INFO(获取TLC59108灯条寄存器数量指令) 同上
 *             GET_WS2812REG_NUM_INFO(获取WS2812灯条寄存器数量指令) 同上
 *             GET_TLC59108REG_POS(获取TLC59108寄存器偏移地址指令) 同上
 *             GET_WS2812REG_POS(获取WS2812寄存器偏移地址指令) 同上
 *             RESET_REG_INFO(复位灯驱寄存器所有数据指令) 同上
 *             RESET_PARAM_SEG(复位不同驱动类型下的灯驱参数指令指令) regAddr(复位寄存器地址) arg/size(无效值)
 */
ErrStatus control_register(const ctrl_reg_cmd_type cmd,
                           const uint8_t regAddr,
                           void *arg,
                           uint8_t size);

#endif
