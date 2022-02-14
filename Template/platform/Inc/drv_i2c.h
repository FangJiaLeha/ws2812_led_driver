#ifndef _DRV_I2C_H
#define _DRV_I2C_H

#include "gd32f3x0.h"
#include "common.h"

//==============================================================================
#define USING_I2C                   (0x01)
#define USING_I2C0                  (0x01)
#if ( defined(USING_I2C) && (USING_I2C == 0x01) )
#define I2C_CLK_FRE                 (400000)    // default set i2c clk is 400Khz
#define I2C_DEF_ADDR                (0x41)      // default set i2c addr is 0x41
enum i2c_device_index
{
    I2C0_DEV = 0x01,
    I2C1_DEV,
};

enum i2c_device_ctl_cmd
{
    I2C_GET_RECV_DATA_LEN = 0x01,
    I2C_RESET_RECV_DATA_LEN,
    I2C_GET_RECV_BUFF,
    I2C_RESET_RECV_BUFF,
    I2C_GET_SEND_DATA_LEN,
    I2C_RESET_SEND_DATA_LEN,
    I2C_GET_SEND_BUFF,
    I2C_RESET_SEND_BUFF
};
#endif

#define I2C_DEV_NUM_CHECK(_num)                             \
do {                                                        \
    if (_num == 0) {                                        \
        goto set_error;                                     \
    }                                                       \
} while(0)

#define I2C_ADDR_CHECK(_addr)                               \
do {                                                        \
    if (_addr == 0) {                                       \
        goto set_error;                                     \
    }                                                       \
} while(0)

#define I2C_DEV_INDEX_CHECK(_i2c_dev_index)                 \
do {                                                        \
    if (_i2c_dev_index != I2C0_DEV &&                       \
        _i2c_dev_index != I2C1_DEV ) {                      \
        goto set_error;                                     \
    }                                                       \
} while(0)

#define I2C_DEV_CTL_CMD_CHECK(_i2c_dev_ctl_cmd)             \
do {                                                        \
    if (_i2c_dev_ctl_cmd != I2C_GET_RECV_DATA_LEN &&        \
        _i2c_dev_ctl_cmd != I2C_RESET_RECV_DATA_LEN &&      \
        _i2c_dev_ctl_cmd != I2C_GET_RECV_BUFF &&            \
        _i2c_dev_ctl_cmd != I2C_RESET_RECV_BUFF &&          \
        _i2c_dev_ctl_cmd != I2C_GET_SEND_DATA_LEN &&        \
        _i2c_dev_ctl_cmd != I2C_RESET_SEND_DATA_LEN &&      \
        _i2c_dev_ctl_cmd != I2C_GET_SEND_BUFF &&            \
        _i2c_dev_ctl_cmd != I2C_RESET_SEND_BUFF) {          \
            goto set_error;                                 \
        }                                                   \
} while(0)

//==============================================================================
void init_i2c(uint8_t i2c_addr, uint8_t i2c_buff_size);
void control_i2c(uint8_t i2c_index, int cmd, void *arg);

#endif

