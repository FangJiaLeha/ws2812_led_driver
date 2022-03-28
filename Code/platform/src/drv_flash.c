/****************************** Copyright (c) **********************************

**------------------------------ File Info -------------------------------------
** File name: flash.c
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
#include "drv_flash.h"

//==============================================================================
/*------------------------------ Private typedef -----------------------------*/
typedef struct flash_attr_type
{
    uint32_t flash_size;
    uint32_t page_base_size;
    uint32_t page_num;
    uint32_t page_base_addr;
}FlashAttrType;

typedef struct flash_device_type
{
    FlashAttrType *flash_attr_sct;
    void (*write)(uint32_t _toPrgAddr, uint8_t *_toPrgData, uint32_t _toPrgSize);
    void (*erase)(uint32_t _toErsAddr, uint32_t _toErsSize);
}FlashDeviceType;

//==============================================================================
#if ( defined(_USING_FLASH) && (_USING_FLASH == 0x01u) )
/*----------------------- Private user function prototypes -------------------*/
static void _write(uint32_t _toPrgAddr, uint8_t *_toPrgDataBuff, uint32_t _toPrgSize);
static void _erase(uint32_t _toErsAddr, uint32_t _toErsSize);

/*-------------------------- Private variable defination ---------------------*/
static FlashAttrType FlashAttrStruct = 
{
    .flash_size       = FLASH_SIZE,
    .page_base_size   = FLASH_PAGE_BASE_SIZE,
    .page_num         = FLASH_SIZE / FLASH_PAGE_BASE_SIZE,
    .page_base_addr   = FLASH_BASE_ADDR
};
static FlashDeviceType flash_device = 
{
    .flash_attr_sct = &FlashAttrStruct,
    .write = _write,
    .erase = _erase
};

/*--------------------- Private user function implementation -----------------*/
/**
 * @Brief   _write() func
 * @Call    Internal
 * @Param   _toPrgAddr The flash address of being write data.
 *          The paramter must between @FLASH_APP_ADDR to @FLASH_END_ADDR
 * @Param   _toPrgDataBuff The buff address of being write data.
 *          The paramter must valid.
 * @Param   _toPrgSize The size of being write data.
 * @Note    The user can write data to flash through this function.
 * @RetVal  None
 */
static void _write(uint32_t _toPrgAddr, uint8_t *_toPrgDataBuff, uint32_t _toPrgSize)
{
    uint32_t wrDataCnt;
    uint8_t remainBytes;
    DDL_ASSERT(IS_PRGADDR_VALID(_toPrgAddr));
    DDL_ASSERT(IS_PRGDATABUFF_VALID(_toPrgDataBuff));
    DDL_ASSERT(IS_PRGSIZE_VALID(_toPrgAddr, _toPrgSize));
    
    // program addr must 4 bytes alignment
    _toPrgAddr = (_toPrgAddr + 0x03u) & (~0x03u);

    /* Unlock EFM. */
    fmc_unlock();

    // calculate the @_toPrgSize is whther 4 bytes alignment
    remainBytes = _toPrgSize % 4;
    
    // program data to the flash by 4 bytes alignment
    for(wrDataCnt = 0u; wrDataCnt < _toPrgSize / 0x04u; wrDataCnt++)
    {
        fmc_word_program(_toPrgAddr,*(uint32_t *)_toPrgDataBuff);
        _toPrgAddr += 0x04u;
        _toPrgDataBuff += 0x04u;
    }

    // program data to the flash by no 4 bytes alignment
    if (remainBytes) {
        memset(_toPrgDataBuff + remainBytes, 0, 4 - remainBytes);
        fmc_word_program(_toPrgAddr, *(uint32_t *)_toPrgDataBuff);
    }

    /* Lock EFM. */
    fmc_lock();

set_error:
    return;
}
/**
 * @Brief   _erase() func
 * @Call    Internal
 * @Param   _toErsAddr The flash address of being erasing
 *          The paramter must between @FLASH_APP_ADDR to @FLASH_END_ADDR
 * @Param   _toErsSize The flash size of being erasing
 * @Note    The user can erase flash through this function.
 * @RetVal  None
 */
static void _erase(uint32_t _toErsAddr, uint32_t _toErsSize)
{
    uint32_t pageBaseSize = FlashAttrStruct.page_base_size;
    uint8_t ersPageNum, ersPageCnt = 0;

    // the @_toErsAddr and @_toErsSize valid check
    DDL_ASSERT(IS_ERSADDR_VALID(_toErsAddr));
    DDL_ASSERT(IS_ERSSIZE_VALID(_toErsAddr, _toErsSize));

    if (_toErsSize  == 0)
        return;

    // calculte the need erase sector number
    ersPageNum = _toErsSize / pageBaseSize;
    if (_toErsSize % pageBaseSize != 0) {
        ersPageNum += 1;
    }

    /* Unlock EFM. */
    fmc_unlock();

    /* Clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    // erase the sector according to the @ersPageNum
    fmc_page_erase(_toErsAddr);
    while(++ersPageCnt < ersPageNum) {
        fmc_page_erase(_toErsAddr + ersPageCnt * pageBaseSize);
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }

    /* Lock EFM. */
    fmc_lock();

set_error:
    return;
}

//==============================================================================
/**
 * @Brief   flash_program() func
 * @Call    External
 * @Param   _PrgAddr The program address.
 *          The paramter must between @FLASH_APP_ADDR to @FLASH_END_ADDR
 * @Param   _toPrgDataBuff The buff address of being programing.
 *          The paramter must valid.
 * @Param   _PrgSize The size of being program.
 * @Note    None
 * @RetVal  None
 */
void flash_program(uint32_t _PrgAddr, uint8_t *_PrgDataBuff, uint32_t _PrgSize)
{
    flash_device.write(_PrgAddr, _PrgDataBuff, _PrgSize);
}

/**
 * @Brief   flash_erase() func
 * @Call    External
 * @Param   _ErsAddr The erase address.
 *          The paramter must between @FLASH_APP_ADDR to @FLASH_END_ADDR
 * @Param   _ErsSize The flash address of being erasing.
 * @Note    None
 * @RetVal  None
 */
void flash_erase(uint32_t _ErsAddr, uint32_t _ErsSize)
{
    flash_device.erase(_ErsAddr, _ErsSize);
}

#endif
