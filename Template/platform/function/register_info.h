#ifndef _REGISTER_INFO_H
#define _REGISTER_INFO_H

#include "common.h"
#include "drv_i2c.h"
#include "drv_pwm.h"

typedef enum ctrl_reg_cmd
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
    POS_GET_REMAIN_REG_NUM,
    /**
     * @brief 根据读取寄存器位置获取其后寄存器数量指令
     *
     */
    RESET_PARAM_SEG
}CtrlRegCmdType;

#define WS2812_PARA_BASE_ADDR                   (0x13u)
#define TLC59108_PARA_BASE_ADDR                 (0x03u)

#define CTRL_REG_CMD_CHECK(cmd)                 \
do {                                            \
    if (cmd != WR_RGE_INFO &&                   \
        cmd != RD_REG_INFO &&                   \
        cmd != RESET_REG_INFO &&                \
        cmd != GET_TLC59108REG_NUM_INFO &&      \
        cmd != GET_WS2812REG_NUM_INFO &&        \
        cmd != GET_REG_NUM_INFO &&              \
        cmd != GET_REG_DRIVER_TYPE &&           \
        cmd != GET_TLC59108REG_POS &&           \
        cmd != GET_WS2812REG_POS &&             \
        cmd != POS_GET_REMAIN_REG_NUM &&        \
        cmd != RESET_PARAM_SEG ) {              \
            goto set_error;                     \
        }                                       \
} while(0)

#define REGADDR_CHECK(cmd, regAddr, regMaxNum)          \
do {                                                    \
    if ((cmd == WR_RGE_INFO || cmd == RD_REG_INFO) &&   \
        (regAddr == 0 || regAddr > regMaxNum)) {        \
        goto set_error;                                 \
    }                                                   \
} while(0)

#define CTRL_REG_SIZE_CHECK(cmd, size)                  \
do {                                                    \
    if ((cmd == WR_RGE_INFO || cmd == RD_REG_INFO) &&   \
        size == 0) {                                    \
            goto set_error;                             \
        }                                               \
} while(0)

#define CTRL_REG_ARG_CHECK(cmd, arg)                    \
do {                                                    \
    if ((cmd == WR_RGE_INFO || cmd == RD_REG_INFO) &&   \
        arg == NULL) {                                  \
            goto set_error;                             \
    }                                                   \
} while(0)

/******************************************************************************/
/**
 * @brief 初始化灯驱寄存器
 *
 * @return ErrStatus
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
 */
ErrStatus control_register(const CtrlRegCmdType cmd,
                           const uint8_t regAddr,
                           void *arg,
                           uint8_t size);

#endif
