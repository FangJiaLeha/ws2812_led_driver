/**
 * @file drv_i2c.h
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _DRV_I2C_H
#define _DRV_I2C_H

#include "common.h"

//==============================================================================
#define USING_I2C (0x01)
#define USING_I2C0 (0x01)
#if (defined(USING_I2C) && (USING_I2C == 0x01))
#define I2C_CLK_FRE (100000) // default set i2c clk is 100Khz
#define I2C_DEF_ADDR (0x41)  // default set i2c addr is 0x41
enum i2c_device_index
{
    I2C0_DEV = 0x01,
    I2C1_DEV,
};

typedef enum i2c_device_ctl_cmd
{
    I2C_GET_RECV_DATA_LEN = 0x01,
    I2C_RESET_RECV_DATA_LEN,
    I2C_GET_RECV_BUFF,
    I2C_RESET_RECV_BUFF,
    I2C_GET_SEND_BUFF
} CtrlI2CDevCmdType;
#endif

#define I2C_DEV_NUM_CHECK(_num) \
    do                          \
    {                           \
        if (_num == 0)          \
        {                       \
            goto set_error;     \
        }                       \
    } while (0)

#define I2C_ADDR_CHECK(_addr) \
    do                        \
    {                         \
        if (_addr == 0)       \
        {                     \
            goto set_error;   \
        }                     \
    } while (0)

#define I2C_DEV_INDEX_CHECK(_i2c_dev_index, i2c_dev_num) \
    do                                                   \
    {                                                    \
        if (_i2c_dev_index > i2c_dev_num ||              \
            _i2c_dev_index == 0)                         \
        {                                                \
            goto set_error;                              \
        }                                                \
    } while (0)

#define I2C_DEV_CTL_CMD_CHECK(_i2c_dev_ctl_cmd)            \
    do                                                     \
    {                                                      \
        if (_i2c_dev_ctl_cmd != I2C_GET_RECV_DATA_LEN &&   \
            _i2c_dev_ctl_cmd != I2C_RESET_RECV_DATA_LEN && \
            _i2c_dev_ctl_cmd != I2C_GET_RECV_BUFF &&       \
            _i2c_dev_ctl_cmd != I2C_RESET_RECV_BUFF &&     \
            _i2c_dev_ctl_cmd != I2C_GET_SEND_BUFF)         \
        {                                                  \
            goto set_error;                                \
        }                                                  \
    } while (0)

//==============================================================================
/**
 * @brief ?????????I2C
 *
 * @param i2c_addr          I2C??????????????????
 * @param i2c_buff_size     I2C?????????????????????Buff????????????????????????
 * @return RtvStatus       @SUCCESS:??????????????? @ERROR:???????????????
 */
RtvStatus init_i2c(const uint8_t i2c_addr,
                    const uint8_t i2c_buff_size);

/**
 * @brief ??????I2C
 *
 * @param i2c_index         I2C????????????(?????????I2C0 0x01)
 * @param cmd               ????????????@CtrlI2CDevCmdType
 * @param arg               ????????????
 * @return RtvStatus       @SUCCESS:??????i2c?????? @??????:??????i2c??????
 *
 * @note  ??????????????????:
 *        @cmd I2C_GET_RECV_DATA_LEN(??????i2c?????????????????????) arg(??????????????????)
 *             I2C_RESET_RECV_DATA_LEN(??????i2c?????????????????????) arg(????????????)
 *             I2C_GET_RECV_BUFF(??????i2c?????????buff) arg(????????????)
 *             I2C_RESET_RECV_BUFF(??????i2c?????????buff) arg(????????????)
 *             I2C_GET_SEND_BUFF(??????i2c?????????buff) arg(????????????)
 */
RtvStatus control_i2c(const uint8_t i2c_index,
                       const CtrlI2CDevCmdType cmd,
                       void *arg);
#endif
