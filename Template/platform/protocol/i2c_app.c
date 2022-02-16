/****************************** Copyright (c) **********************************

**------------------------------ File Info -------------------------------------
** File name: can_app.c
** Last modified Date: None
** Last Version: None
** Descriptions: 
**
**------------------------------------------------------------------------------
** Created by:               FJL
** Created date:             2022/02/10
** Version:                  1.0
** Descriptions:
**----------------------------------------------------------------------------*/
#include "i2c_app.h"
#include "bootloaderinfo.h"
#include "drv_flash.h"
#include "crc.h"
#include "drv_i2c.h"

//==============================================================================
/*------------------------------ Private typedef -----------------------------*/
typedef enum mcu_work_mode_type
{
    WORK_IN_APP,
    WORK_IN_BOOT
}McuWorkModeType;

typedef struct crc_1k_ram_type
{
    uint8_t header[6];
    uint8_t u8_1K_downloadDataRam[RECV_DATA_BASE_SIZE];
}Crc1KRamType;

typedef enum i2c_cmd_list_type
{
    CmdSoftWareReset = 0x01u,
    CmdCheckBootOrApp,
    CmdGetSoftWareVersion,
    CmdUpdateFileInfo = 0x05u,
    CmdBlockInfo,
    CmdUpdateData,
    CmdSectionCRC,
    CmdJumpToApp
}I2cCmdListType;

typedef struct cmd_list_type
{
    I2cCmdListType i2c_cmd;
    void (*cmd_callback)(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg);
}CmdListType;


typedef struct i2c_app_object
{
    I2CRxMsgType i2c_rx_msg;
    Crc1KRamType CRC_1K_RAM;
    McuWorkModeType mcu_work_mode;
    CmdListType pcmd[I2C_CMD_NUM];
}I2CAppObject;

//==============================================================================
/*----------------------- Private user function prototypes -------------------*/
static uint8_t get_check_xor(const uint8_t *i2cBuff, uint8_t i2cDataLen);
static void i2c_ack(I2CRxMsgType *_i2c_rx_msg);
static void cmd_check_in_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg);
static void cmd_get_soft_version_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg);
static void cmd_update_fileinfo_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg);
static void cmd_block_info_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg);
static void cmd_update_data_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg);
static void cmd_section_crc_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg);
static void cmd_jump_app_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg);
static void cmd_software_reset_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg);

/*-------------------------- Private variable defination ---------------------*/
static I2CAppObject i2c_app = 
{
    .i2c_rx_msg = {0},
    .CRC_1K_RAM = {0},
    .pcmd       =
    {
        {CmdUpdateData,         cmd_update_data_handler},
        {CmdSectionCRC,         cmd_section_crc_handler},
        {CmdCheckBootOrApp,     cmd_check_in_handler},
        {CmdGetSoftWareVersion, cmd_get_soft_version_handler},
        {CmdUpdateFileInfo,     cmd_update_fileinfo_handler},
        {CmdBlockInfo,          cmd_block_info_handler},
        {CmdJumpToApp,          cmd_jump_app_handler},
        {CmdSoftWareReset,      cmd_software_reset_handler},
    },
};

static uint16_t recv_data_len = 0;
static uint8_t *set_send_buff = NULL;

/*--------------------- Private user function implementation -----------------*/
/**
 * @Brief   get_check_xor() func
 * @Call    Internal
 * @Param   i2cBuff The value buff of checking
 * @Param   i2cDataLen The length of to check buff
 *          The parameter must be set (length - 1) of checking buff
 * @Note    check xor func
 * @RetVal  None
 */
static uint8_t get_check_xor(const uint8_t *i2cBuff, uint8_t i2cDataLen)
{
    uint8_t checkXor = 0;
    if (NULL == i2cBuff || i2cDataLen == 0) {
        return 0;
    }

    for(uint8_t data_len_cnt = 0; data_len_cnt < i2cDataLen - 1; data_len_cnt++)
    {
        checkXor ^= *i2cBuff++;
    }
    return checkXor;
}

/**
 * @Brief   i2c_ack() func
 * @Call    Internal
 * @Param   _i2c_rx_msg The i2c rx msg
 * @Note    Used to ack can that PC
 * @RetVal  None
 */
static void i2c_ack(I2CRxMsgType *_i2c_rx_msg)
{
    _i2c_rx_msg->rxData[8] = get_check_xor(_i2c_rx_msg->rxData, 9);
    memmove(set_send_buff, _i2c_rx_msg->rxData, 9);
}

/**
 * @Brief   cmd_check_in_handler() func
 * @Call    Internal
 * @Param   _i2c_app The address of @i2c_app
 * @Param   _i2c_rx_msg The recieved i2c msg
 * @Note    I2C cmd to mcu for checking work in mode
 * @RetVal  None
 */
static void cmd_check_in_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg)
{
    uint8_t checkXor;
    DDL_ASSERT(IS_I2CRX_MSG_VALID(_i2c_rx_msg));

    checkXor = get_check_xor(_i2c_rx_msg->rxData, 9);

    if ( COMPARE_VALUE(checkXor, _i2c_rx_msg->rxData[8]) ) {
        _i2c_rx_msg->rxData[3] = WORK_IN_BOOT;  // set to mcu in boot mode(0x01u)
        i2c_ack(_i2c_rx_msg);
    }
set_error:
    return;
}

static void cmd_get_soft_version_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg)
{
    uint8_t checkXor;
    DDL_ASSERT(IS_I2CRX_MSG_VALID(_i2c_rx_msg));

    checkXor = get_check_xor(_i2c_rx_msg->rxData, 9);

    if ( COMPARE_VALUE(checkXor, _i2c_rx_msg->rxData[8]) ) {
        i2c_ack(_i2c_rx_msg);
    }
set_error:
    return;
}

/**
 * @Brief   cmd_update_fileinfo_handler() func
 * @Call    Internal
 * @Param   _i2c_app The address of @i2c_app
 * @Param   _i2c_rx_msg The recieved i2c msg
 * @Note    i2c cmd to mcu for saving the update file size to @Bootloader
 * @RetVal  None
 */
static void cmd_update_fileinfo_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg)
{
    uint32_t toUpdateFileSize;
    uint8_t checkXor;
    DDL_ASSERT(IS_I2CRX_MSG_VALID(_i2c_rx_msg));

    checkXor = get_check_xor(_i2c_rx_msg->rxData, 9);

    if ( COMPARE_VALUE(checkXor, _i2c_rx_msg->rxData[8]) ) {
        // Get the update file size
        toUpdateFileSize = (_i2c_rx_msg->rxData[3] << 16) + 
                         (_i2c_rx_msg->rxData[4] << 8) + _i2c_rx_msg->rxData[5];
        // Write the update file size to @Bootloader
        write_bootloaderinfo(FILE_SIZE, &toUpdateFileSize);
        // Erase the flash according to the @toUpdateFileSize
        flash_erase(FLASH_APP_ADDR, toUpdateFileSize);
        //i2c_ack(_i2c_rx_msg);
    }
set_error:
    return;
}

/**
 * @Brief   cmd_block_info_handler() func
 * @Call    Internal
 * @Param   _i2c_app The address of @i2c_app
 * @Param   _i2c_rx_msg The recieved can msg
 * @Note    I2C cmd to mcu for saving the block info to @Bootloader
 *          The block info include: 
 *                                 The Current Block Size @curBlockSize
 *                                 The Current Block Num  @curBlockNum
 *                                 The Current Addr       @curAddr
 * @RetVal  None
 */
static void cmd_block_info_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg)
{
    uint16_t curBlockSize, curBlockNum, curAddr = 0x00u;
    uint8_t checkXor;
    DDL_ASSERT(IS_I2CRX_MSG_VALID(_i2c_rx_msg));

    checkXor = get_check_xor(_i2c_rx_msg->rxData, 9);

    if ( COMPARE_VALUE(checkXor, _i2c_rx_msg->rxData[8]) ) {
        // Get the current block size and block num
        curBlockSize = (_i2c_rx_msg->rxData[3] << 8) + _i2c_rx_msg->rxData[4];
        curBlockNum  = (_i2c_rx_msg->rxData[5] << 8) + _i2c_rx_msg->rxData[6];
        // Write the block info to @Bootloader
        write_bootloaderinfo(CRT_BLOCK_SIZE, &curBlockSize);
        write_bootloaderinfo(CRT_BLOCK_NUM,  &curBlockNum);
        write_bootloaderinfo(CRT_ADDR,       &curAddr);
        i2c_ack(_i2c_rx_msg);
    }
set_error:
    return;
}

/**
 * @Brief   cmd_update_data_handler() func
 * @Call    Internal
 * @Param   _i2c_app The address of @i2c_app
 * @Param   _i2c_rx_msg The recieved i2c msg
 * @Note    I2c cmd to mcu for saving the block data to @CRC_1K_RAM buff.
 *          The block data is 1K and Copy 5 bytes @PROGRAM_BYTES_ONCE at a time.
 * @RetVal  None
 */
static void cmd_update_data_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg)
{
    I2CAppObject *I2CApp = NULL;
    uint16_t curAddr, curBlockSize;
    DDL_ASSERT(IS_I2CRX_MSG_VALID(_i2c_rx_msg));

    // Get the current addr and block size from @Bootloader
    read_bootloaderinfo(CRT_ADDR, &curAddr);
    read_bootloaderinfo(CRT_BLOCK_SIZE, &curBlockSize);
    I2CApp = (I2CAppObject *)_i2c_app;

    if (curAddr >= curBlockSize) {
        return;
    } else {
        // Memery cop
        memmove(&(I2CApp->CRC_1K_RAM.u8_1K_downloadDataRam[curAddr]), 
                (const void *)&(_i2c_rx_msg->rxData[3]), PROGRAM_BYTES_ONCE);
    }
    // Copy addr offset
    curAddr += PROGRAM_BYTES_ONCE;
    // Write the current addr to @Bootloader
    write_bootloaderinfo(CRT_ADDR, &(curAddr));
set_error:
    return;
}

/**
 * @Brief   cmd_section_crc_handler() func
 * @Call    Internal
 * @Param   _i2c_app The address of @i2c_app
 * @Param   _i2c_rx_msg The recieved i2c msg
 * @Note    I2C cmd to mcu for softwareCrc32 the @CRC_1K_RAM buff data.
 * @RetVal  None
 */
static void cmd_section_crc_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg)
{
    I2CAppObject *I2CApp = NULL;
    uint32_t i2c_msg_crc, crc_cal_value, file_size;
    uint16_t curBlockNum, curBlockSize;
    DDL_ASSERT(IS_I2CRX_MSG_VALID(_i2c_rx_msg));

    I2CApp = (I2CAppObject *)_i2c_app;
    // Get the current block num and block size from @Bootloader
    read_bootloaderinfo(CRT_BLOCK_NUM, &curBlockNum);
    read_bootloaderinfo(CRT_BLOCK_SIZE, &curBlockSize);
    // Get the crc value according to can rx msg
    i2c_msg_crc = (_i2c_rx_msg->rxData[4] << 24) + (_i2c_rx_msg->rxData[5] << 16) +
                  (_i2c_rx_msg->rxData[6] << 8 ) + _i2c_rx_msg->rxData[7] ;
    // Set value the the @I2CApp->CRC_1K_RAM.header for the CRC32
    I2CApp->CRC_1K_RAM.header[0] = curBlockNum >> 0x08u;
    I2CApp->CRC_1K_RAM.header[1] = curBlockNum & 0xffu;
    *(uint32_t *)&(I2CApp->CRC_1K_RAM.header[2]) = *(uint32_t *)&_i2c_rx_msg->rxData;

    // Calculate the CRC32 value of recieve data
    crc_cal_value = SoftwareCRC32(I2CApp->CRC_1K_RAM.header, curBlockSize + 0x06u);

    _i2c_rx_msg->rxData[4] = 0x00u;
    _i2c_rx_msg->rxData[5] = 0x00u;
    _i2c_rx_msg->rxData[6] = 0x00u;

    // CRC32 value compare
    if (COMPARE_VALUE(crc_cal_value, i2c_msg_crc)) {
        _i2c_rx_msg->rxData[3] = 0x01u;
        // Get the file size from @Bootloader
        read_bootloaderinfo(FILE_SIZE, &file_size);

        // Write the @I2CApp->CRC_1K_RAM.u8_1K_downloadDataRam data to flash
        flash_program(FLASH_APP_ADDR + curBlockNum * 1024u,
                      I2CApp->CRC_1K_RAM.u8_1K_downloadDataRam, curBlockSize);

        // clear the 1k buff
        memset(I2CApp->CRC_1K_RAM.u8_1K_downloadDataRam, 0, curBlockSize);

    } else {
        _i2c_rx_msg->rxData[3] = 0x00u;
    }

    i2c_ack(_i2c_rx_msg);
set_error:
    return;
}

/**
 * @Brief   cmd_jump_app_handler() func
 * @Call    Internal
 * @Param   _i2c_app The address of @i2c_app
 * @Param   _i2c_rx_msg The recieved i2c msg
 * @Note    I2c cmd to mcu from boot jump to app.
 * @RetVal  None
 */
static void cmd_jump_app_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg)
{
    uint8_t checkXor;
    DDL_ASSERT(IS_I2CRX_MSG_VALID(_i2c_rx_msg));

    checkXor = get_check_xor(_i2c_rx_msg->rxData, 9);

    if ( COMPARE_VALUE(checkXor, _i2c_rx_msg->rxData[8]) ) {
        // software reset access the mcu from boot to app
        NVIC_SystemReset();
    }
set_error:
    return;
}

/**
 * @Brief   cmd_software_reset_handler() func
 * @Call    Internal
 * @Param   _i2c_app The address of @i2c_app
 * @Param   _i2c_rx_msg The recieved i2c msg
 * @Note    I2c cmd to mcu software reset.
 * @RetVal  None
 */
static void cmd_software_reset_handler(void *_i2c_app, I2CRxMsgType *_i2c_rx_msg)
{
    uint8_t checkXor;
    DDL_ASSERT(IS_I2CRX_MSG_VALID(_i2c_rx_msg));

    checkXor = get_check_xor(_i2c_rx_msg->rxData, 9);

    if ( COMPARE_VALUE(checkXor, _i2c_rx_msg->rxData[8]) ) {
        i2c_ack(_i2c_rx_msg);
    }
set_error:
    return;
}

/**
 * @Brief   find_processer() func
 * @Call    Internal
 * @Param   _cmd The type of can cmd list
 * @Note    Get the callback according to the parameter @_cmd
 * @RetVal  None
 */
static const CmdListType *find_processer(const I2cCmdListType _cmd)
{
    uint8_t cmds_num = ITEM_NUM(i2c_app.pcmd);

    for (uint8_t cmds_cnt = 0; cmds_cnt < cmds_num; cmds_cnt++) {
        if (i2c_app.pcmd[cmds_cnt].i2c_cmd == _cmd) {
            return &i2c_app.pcmd[cmds_cnt];
        }
    }
    return NULL;
}

/**
 * @Brief   iap_cmd_handler() func
 * @Call    Internal
 * @Param   None
 * @Note    Get the func i2c cmd byte,then call the func that @cmdFuncType
 * @RetVal  None
 */
static void iap_cmd_handler(void)
{
    const CmdListType *cmd = NULL;
    I2cCmdListType cmdHeaderType = (I2cCmdListType)i2c_app.i2c_rx_msg.rxData[0];
    uint8_t i2c_dev_id = i2c_app.i2c_rx_msg.rxData[1];
    I2cCmdListType cmdFuncType;

    DDL_ASSERT(IS_I2C_CMD_HEADER_VALID((uint8_t)cmdHeaderType));
    DDL_ASSERT(IS_I2C_DEV_ID_VALID(i2c_dev_id));

    // function bytes analysis
    cmdFuncType = (I2cCmdListType)i2c_app.i2c_rx_msg.rxData[2];
    cmd = find_processer(cmdFuncType);
    if (NULL != cmd) {
        cmd->cmd_callback((void *)&i2c_app, &(i2c_app.i2c_rx_msg));
    }

set_error:
    return;
}

//==============================================================================
/*-------------------------- User function implementation --------------------*/
/**
 * @Brief   i2c_data_recv_dispatch() func
 * @Call    External
 * @Param   None
 * @Note    The func is used to dispatch i2c recieve frame
 * @RetVal  None
 */
void i2c_data_recv_dispatch(void)
{
    control_i2c(I2C0_DEV, I2C_GET_RECV_DATA_LEN, (void *)&recv_data_len);
    if (recv_data_len != 0) {
        control_i2c(I2C0_DEV, I2C_RESET_SEND_BUFF, NULL);
        control_i2c(I2C0_DEV, I2C_RESET_SEND_DATA_LEN, NULL);
        control_i2c(I2C0_DEV, I2C_GET_RECV_BUFF, (void *)i2c_app.i2c_rx_msg.rxData);
        control_i2c(I2C0_DEV, I2C_GET_SEND_BUFF, (void *)&set_send_buff);
        iap_cmd_handler();
        control_i2c(I2C0_DEV, I2C_RESET_RECV_BUFF, NULL);
        control_i2c(I2C0_DEV, I2C_RESET_RECV_DATA_LEN, NULL);
    }
}
