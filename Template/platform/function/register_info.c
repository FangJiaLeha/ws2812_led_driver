/**
 * @file register_info.c
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-02-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "register_info.h"
#include "string.h"

/* 寄存器中段数据域长度管理 */
enum reg_seg_data_len
{
    APPID_REG_DATA_LEN = 0x01,
    VERSION_REG_DATA_LEN = 0x03,
    CMD_REG_DATA_LEN = 0x01,
    PARA_REG_DATA_LEN = 0x01,
    CONFIRM_REG_DATA_LEN = 0x01
};

/* 寄存器地址管理 */
#define REGSTER_BASE_ADDR           (0x0800A000)
#define APPID_REG_ADDR              (REGSTER_BASE_ADDR)
#define VERSION_REG_ADDR            (APPID_REG_ADDR + APPID_REG_DATA_LEN)
#define CMD_REG_ADDR                (VERSION_REG_ADDR + VERSION_REG_DATA_LEN)
#define PARA_REG0_ADDR              (CMD_REG_ADDR + CMD_REG_DATA_LEN)
#define PARA_REG1_ADDR              (PARA_REG0_ADDR + PARA_REG_DATA_LEN)
#define PARA_REG2_ADDR              (PARA_REG1_ADDR + PARA_REG_DATA_LEN)
#define PARA_REG3_ADDR              (PARA_REG2_ADDR + PARA_REG_DATA_LEN)
#define PARA_REG4_ADDR              (PARA_REG3_ADDR + PARA_REG_DATA_LEN)
#define CONFIRM_REG_ADDR            (PARA_REG4_ADDR + PARA_REG_DATA_LEN)

typedef struct seg_info
{
	uint32_t addr;
	uint8_t data_len;
}SegInfoType;

typedef struct reg_info
{
	RegType seg_id;
    SegInfoType seg_info;
	uint8_t *seg_buff;
    RegStatusType seg_status;
}RegInfoType;
typedef RegInfoType* RegInfoType_t;

/* 寄存器类型及其回调函数结构体声明 */
typedef struct reg_cmd_list
{
    RegType reg_cmd_type;
    void (*callback)(const ControlRegCmdType ctrl_reg_type, const RegType reg_type, void *arg);
}RegCmdListType;
typedef RegCmdListType* RegCmdListType_t;

static void ctrl_appid_reg(const ControlRegCmdType ctrl_reg_type, const RegType reg_type, void *arg);
static void ctrl_version_reg(const ControlRegCmdType ctrl_reg_type, const RegType reg_type, void *arg);
static void ctrl_cmd_reg(const ControlRegCmdType ctrl_reg_type, const RegType reg_type, void *arg);
static void ctrl_para_reg(const ControlRegCmdType ctrl_reg_type, const RegType reg_type, void *arg);
static void ctrl_confirm_reg(const ControlRegCmdType ctrl_reg_type, const RegType reg_type, void *arg);
static uint8_t *set_send_buff = NULL;

/* 寄存器种类管理 */
static RegInfoType register_info[] = {
    /**
     * @brief appid register config
     * 
     */
    {
        .seg_id = APPID_REG,
        .seg_info = {
            .addr = APPID_REG_ADDR,
            .data_len = APPID_REG_DATA_LEN,
        },
        .seg_buff = NULL
    },
    
    /**
     * @brief version register config
     * 
     */
    {
        .seg_id = VERSION_REG,
        .seg_info = {
            .addr = VERSION_REG_ADDR,
            .data_len = VERSION_REG_DATA_LEN,
        },
        .seg_buff = NULL
    },
    
    /**
     * @brief cmd register config
     * 
     */
    {
        .seg_id = CMD_REG,
        .seg_info = {
            .addr = CMD_REG_ADDR,
            .data_len = CMD_REG_DATA_LEN,
        },
        .seg_buff = NULL
    },
    
    /**
     * @brief para0 register config
     * 
     */
    {
        .seg_id = PARA_REG0,
        .seg_info = {
            .addr = PARA_REG0_ADDR,
            .data_len = PARA_REG_DATA_LEN,
        },
        .seg_buff = NULL
    },
    
    /**
     * @brief para1 register config
     * 
     */
    {
        .seg_id = PARA_REG1,
        .seg_info = {
            .addr = PARA_REG1_ADDR,
            .data_len = PARA_REG_DATA_LEN,
        },
        .seg_buff = NULL
    },
    
    /**
     * @brief para2 register config
     * 
     */
    {
        .seg_id = PARA_REG2,
        .seg_info = {
            .addr = PARA_REG2_ADDR,
            .data_len = PARA_REG_DATA_LEN,
        },
        .seg_buff = NULL,
    },
    
    /**
     * @brief para3 register config
     * 
     */
    {
        .seg_id = PARA_REG3,
        .seg_info = {
            .addr = PARA_REG3_ADDR,
            .data_len = PARA_REG_DATA_LEN,
        },
        .seg_buff = NULL
    },
    
    /**
     * @brief para4 register config
     * 
     */
    {
        .seg_id = PARA_REG4,
        .seg_info = {
            .addr = PARA_REG4_ADDR,
            .data_len = PARA_REG_DATA_LEN,
        },
        .seg_buff = NULL
    },
    
    /**
     * @brief confirm register config
     * 
     */
    {
        .seg_id = CONFIRM_REG,
        .seg_info = {
            .addr = CONFIRM_REG_ADDR,
            .data_len = CONFIRM_REG_DATA_LEN,
        },
        .seg_buff = NULL
    }
};

static const RegCmdListType reg_cmd_lists[] = 
{
    /**
     * @brief 
     * 
     */
    {APPID_REG, ctrl_appid_reg},

    /**
     * @brief 
     * 
     */
    {VERSION_REG, ctrl_version_reg},

    /**
     * @brief 
     * 
     */
    {CMD_REG, ctrl_cmd_reg},

    /**
     * @brief 
     * 
     */
    {PARA_REG0, ctrl_para_reg},

    /**
     * @brief 
     * 
     */
    {PARA_REG1, ctrl_para_reg},

    /**
     * @brief 
     * 
     */
    {PARA_REG2, ctrl_para_reg},

    /**
     * @brief 
     * 
     */
    {PARA_REG3, ctrl_para_reg},

    /**
     * @brief 
     * 
     */
    {PARA_REG4, ctrl_para_reg},

    /**
     * @brief 
     * 
     */
    {CONFIRM_REG, ctrl_confirm_reg},
};

/**
 * @brief 
 * 
 * @param ctrl_reg_type 
 * @param reg_type 
 * @param arg 
 */
static void ctrl_appid_reg(const ControlRegCmdType ctrl_reg_type, const RegType reg_type, void *arg)
{
    if (ctrl_reg_type == RD_REG) {
        control_i2c(I2C0_DEV, I2C_RESET_SEND_BUFF, (void *)&set_send_buff);
        memmove(set_send_buff, register_info[0].seg_buff, register_info[0].seg_info.data_len);
        control_i2c(I2C0_DEV, I2C_RESET_SEND_DATA_LEN, NULL);
    }
}

/**
 * @brief 
 * 
 * @param ctrl_reg_type 
 * @param reg_type 
 * @param arg 
 */
static void ctrl_version_reg(const ControlRegCmdType ctrl_reg_type, const RegType reg_type, void *arg)
{
    control_i2c(I2C0_DEV, I2C_RESET_SEND_BUFF, (void *)&set_send_buff);
    if (ctrl_reg_type == RD_REG) {
        memmove(set_send_buff, register_info[1].seg_buff, register_info[1].seg_info.data_len);
    }
    control_i2c(I2C0_DEV, I2C_RESET_SEND_DATA_LEN, NULL);
}

/**
 * @brief 
 * 
 * @param ctrl_reg_type 
 * @param reg_type 
 * @param arg 
 */
static void ctrl_cmd_reg(const ControlRegCmdType ctrl_reg_type, const RegType reg_type, void *arg)
{
    control_i2c(I2C0_DEV, I2C_RESET_SEND_BUFF, (void *)&set_send_buff);
    if (ctrl_reg_type == WR_REG) {
        register_info[2].seg_buff[0] = *(uint8_t *)arg;
        register_info[2].seg_status = STAT_ACCEPTED;
        memmove(set_send_buff, &register_info[2].seg_status, 0x01);
    } else {
        memmove(set_send_buff, register_info[2].seg_buff, register_info[2].seg_info.data_len);
    }
    control_i2c(I2C0_DEV, I2C_RESET_SEND_DATA_LEN, NULL);
}

/**
 * @brief 
 * 
 * @param ctrl_reg_type 
 * @param reg_type 
 * @param arg 
 */
static void ctrl_para_reg(const ControlRegCmdType ctrl_reg_type, const RegType reg_type, void *arg)
{
    control_i2c(I2C0_DEV, I2C_RESET_SEND_BUFF, (void *)&set_send_buff);
    if (ctrl_reg_type == WR_REG) {
        register_info[reg_type - 0x1D].seg_buff[0] = *(uint8_t *)arg;
        register_info[reg_type - 0x1D].seg_status = STAT_ACCEPTED;
        memmove(set_send_buff, &register_info[reg_type - 0x1D].seg_status, 0x01);
    } else {
        memmove(set_send_buff, register_info[reg_type - 0x1D].seg_buff, register_info[reg_type - 0x1D].seg_info.data_len);
    }
    control_i2c(I2C0_DEV, I2C_RESET_SEND_DATA_LEN, NULL);
}

/**
 * @brief 
 * 
 * @param ctrl_reg_type 
 * @param reg_type 
 * @param arg 
 */
static void ctrl_confirm_reg(const ControlRegCmdType ctrl_reg_type, const RegType reg_type, void *arg)
{
    control_i2c(I2C0_DEV, I2C_RESET_SEND_BUFF, (void *)&set_send_buff);
    if (ctrl_reg_type == WR_REG) {
        // 确认配置 并烧录参数到工作page区
        if (*(uint8_t *)arg == 0xFF) {
            register_info[8].seg_buff[0] = *(uint8_t *)arg;
            register_info[8].seg_status = STAT_ACCEPTED;
            // 烧录参数接口
        } else {
            register_info[8].seg_status = STAT_ERR_CONFIG;
        }
        memmove(set_send_buff, &register_info[8].seg_status, 0x01);
    } else {
        memmove(set_send_buff, register_info[8].seg_buff, register_info[8].seg_info.data_len);
    }
    control_i2c(I2C0_DEV, I2C_RESET_SEND_DATA_LEN, NULL);
}

/**
 * @brief 
 * 
 * @param reg_cmd_type 
 * @return const RegCmdListType_t 
 */
static const RegCmdListType_t find_processor(const RegType reg_cmd_type)
{
    for (uint8_t reg_cmd_cnt = 0; reg_cmd_cnt < ITEM_NUM(reg_cmd_lists); reg_cmd_cnt++) {
        if (reg_cmd_type == reg_cmd_lists[reg_cmd_cnt].reg_cmd_type) {
            return &reg_cmd_lists[reg_cmd_cnt];
        }
    }
    return NULL;
}

/**
 * @brief 
 * 
 * @param reg_info_para 
 */
static void _load_register_info(RegInfoType_t reg_info_para)
{
    if (reg_info_para == NULL) {
        return;
    }

    // 获取flash中对应地址段的数据
    for (uint8_t seg_data_len_cnt = 0; seg_data_len_cnt < reg_info_para->seg_info.data_len; seg_data_len_cnt++) {
        reg_info_para->seg_buff[seg_data_len_cnt] = (*(uint32_t *)reg_info_para->seg_info.addr) >> (8 * seg_data_len_cnt);
    }
}

/**
 * @brief 
 * 
 * @param reg_type_para 
 * @param arg 
 */
static void _write_register_info(const RegType wr_reg_type_para, void *arg)
{
    WR_REG_TYPE_CHECK(wr_reg_type_para);

    RegCmdListType_t reg_cmd = NULL;
    reg_cmd = find_processor(wr_reg_type_para);
    if (reg_cmd != NULL) {
        reg_cmd->callback(WR_REG, wr_reg_type_para, arg);
    }
set_error:
    return;
}

/**
 * @brief 
 * 
 * @param reg_type_para 
 */
static void _read_register_info(const RegType rd_reg_type_para)
{
    RD_REG_TYPE_CHECK(rd_reg_type_para);

    RegCmdListType_t reg_cmd = NULL;
    reg_cmd = find_processor(rd_reg_type_para);
    if (reg_cmd != NULL) {
        reg_cmd->callback(RD_REG, rd_reg_type_para, NULL);
    }

set_error:
    return;
}

/**
 * @brief 
 * 
 * @return Rtv_Status 
 */
Rtv_Status init_register_info(void)
{
    for (uint8_t reg_cnt = 0; reg_cnt < ITEM_NUM(register_info); reg_cnt++) {
        register_info[reg_cnt].seg_buff = (uint8_t *)malloc(sizeof(uint8_t) * register_info[reg_cnt].seg_info.data_len);
        if (register_info[reg_cnt].seg_buff == NULL) {
            return ENOMEM;
        }
        memset(register_info[reg_cnt].seg_buff, 0, register_info[reg_cnt].seg_info.data_len);
        register_info[reg_cnt].seg_status = STAT_OK;
        _load_register_info(&register_info[reg_cnt]);
    }
    control_i2c(I2C0_DEV, I2C_GET_SEND_BUFF, (void *)&set_send_buff);
    return SUCCESS;
}

/**
 * @brief 
 * 
 * @param ctrl_reg_cmd_para 
 * @param reg_type_para 
 * @param arg 
 */
void control_register_info(const ControlRegCmdType ctrl_reg_cmd_para, const RegType reg_type_para, void *arg)
{
    CTRL_REG_CMD_CHECK(ctrl_reg_cmd_para);

    if (ctrl_reg_cmd_para == WR_REG) {
        _write_register_info(reg_type_para, arg);
    } else {
        _read_register_info(reg_type_para);
    }

set_error:
    return;
}
