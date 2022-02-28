#ifndef _REGISTER_INFO_H
#define _REGISTER_INFO_H

#include "common.h"
#include "drv_i2c.h"

typedef enum reg_type
{
	APPID_REG = 0x00,
	VERSION_REG,
	CMD_REG = 0x08,
	PARA_REG0 = 0x20,
	PARA_REG1 = 0x21,
	PARA_REG2 = 0x22,
	PARA_REG3 = 0x23,
	PARA_REG4 = 0x24,
	CONFIRM_REG = 0xAA,
}RegType;

typedef enum reg_status_type
{
	STAT_OK = 0x00,
	STAT_ACCEPTED,
	STAT_ERR_CONFIG,
	STAT_ERR_UNKNOWN_CMD,
}RegStatusType;

typedef enum control_reg_cmd
{
	WR_REG = 0x01,
	RD_REG
}ControlRegCmdType;

#define CTRL_REG_CMD_CHECK(_cmd)	\
do {								\
	if (_cmd != WR_REG &&			\
		_cmd != RD_REG ) {			\
			goto set_error;			\
		}							\
} while(0)

#define RD_REG_TYPE_CHECK(_rd_regtype)	\
do {									\
	if (_rd_regtype != APPID_REG &&		\
		_rd_regtype != VERSION_REG &&	\
		_rd_regtype != CMD_REG &&		\
		_rd_regtype != PARA_REG0 &&		\
		_rd_regtype != PARA_REG1 &&		\
		_rd_regtype != PARA_REG2 &&		\
		_rd_regtype != PARA_REG3 &&		\
		_rd_regtype != PARA_REG4 &&		\
		_rd_regtype != CONFIRM_REG ) {	\
			goto set_error;				\
		}								\
} while(0)

#define WR_REG_TYPE_CHECK(_wr_regtype)	\
do {									\
	if (_wr_regtype != VERSION_REG &&	\
		_wr_regtype != CMD_REG &&		\
		_wr_regtype != PARA_REG0 &&		\
		_wr_regtype != PARA_REG1 &&		\
		_wr_regtype != PARA_REG2 &&		\
		_wr_regtype != PARA_REG3 &&		\
		_wr_regtype != PARA_REG4 &&		\
		_wr_regtype != CONFIRM_REG ) {	\
			goto set_error;				\
		}								\
} while(0)
Rtv_Status init_register_info(void);
void control_register_info(const ControlRegCmdType ctrl_reg_cmd_para, const RegType reg_type_para, void *arg);

#endif
