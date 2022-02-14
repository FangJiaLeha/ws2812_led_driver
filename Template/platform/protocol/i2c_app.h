#ifndef _I2C_APP_H
#define _I2C_APP_H

#include "iap_config.h"

//==============================================================================
/*------------------------------ Extern typedef ------------------------------*/
typedef struct i2c_rx_msg_type
{
    uint32_t Identifier;
    uint8_t rxData[16];
}I2CRxMsgType;

//==============================================================================
/*------------------------------ Macro defination ----------------------------*/
#define IS_I2CRX_MSG_VALID(i2c_rx_msg)            IS_PARAMETER_VALID(i2c_rx_msg)
#define IS_I2C_CMD_HEADER_VALID(i2c_cmd_header)   (i2c_cmd_header == IAP_CMD ? 0x01 : 0x00)
#define IS_I2C_DEV_ID_VALID(i2c_id)               (i2c_id == I2C0_DEV ? 0x01 : 0x00)
#define COMPARE_VALUE(value1, value2)       ((value1 == value2) ? 0x01u : 0x00u)

//==============================================================================
/*--------------------------- Extern function prototypes ---------------------*/
void i2c_data_recv_dispatch(void);

#endif
