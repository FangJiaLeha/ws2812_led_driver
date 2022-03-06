#ifndef _REGISTER_INFO_H
#define _REGISTER_INFO_H

#include "common.h"
#include "drv_i2c.h"

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
    GET_REG_NUM_INFO
}CtrlRegCmdType;

typedef enum driver_dev_type
{
    TLC59108DEV = 0x01,
    WS2812DEV
}DriverDevType;

#define CTRL_REG_CMD_CHECK(cmd)                 \
do {                                            \
    if (cmd != WR_RGE_INFO &&                   \
        cmd != RD_REG_INFO &&                   \
        cmd != RESET_REG_INFO &&                \
        cmd != GET_TLC59108REG_NUM_INFO &&      \
        cmd != GET_WS2812REG_NUM_INFO &&        \
        cmd != GET_REG_NUM_INFO ) {             \
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

#define SET_DRIVER_TYPE_CHECK(type)         \
do {                                        \
    if (type != TLC59108DEV &&              \
        type != WS2812DEV) {                \
            goto set_error;                 \
    }                                       \
} while(0)

/**
 * @brief 初始化灯驱寄存器
 *
 */
void init_register(void);

/**
 * @brief 操作灯驱寄存器
 *
 * @param cmd     操作命令
 * @param regAddr 寄存器地址
 * @param arg     指针参数 用于传入读写buff
 * @param size    读写字节数
 */
void control_register(const CtrlRegCmdType cmd,
                      const uint8_t regAddr,
                      void *arg,
                      const uint8_t size);

#endif
