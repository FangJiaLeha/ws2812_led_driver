/****************************** Copyright (c) **********************************

**------------------------------ File Info -------------------------------------
** File name: system_common.c
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
#include "system_common.h"

/*------------------------------- Variable defination ------------------------*/
volatile uint32_t ResetSignature[2] __attribute__( ( at( RESET_SIGNATURE_ADDRESS + 0x00u) ) );

#if ( defined( _USING_BOOTLOADER ) &&  (_USING_BOOTLOADER != 0))
const uint32_t *FirmwareSignature = (uint32_t *)(FLASH_APP_ADDR + FIRMWARE_SIGNATURE_OFFSET + 0x00u);
const uint32_t *FirmwareSize      = (uint32_t *)(FLASH_APP_ADDR + FIRMWARE_SIGNATURE_OFFSET + 0x08u);
const uint8_t  *mcu_type          = (uint8_t  *)(FLASH_APP_ADDR + FIRMWARE_SIGNATURE_OFFSET + 0x0Cu);
#else
const uint32_t FirmwareSignature[2] __attribute__( ( at( FLASH_APP_ADDR + FIRMWARE_SIGNATURE_OFFSET + 0x00u ) ) ) = 
{
    FIRMWARE_SIGNATURE0_DEBUG,
    FIRMWARE_SIGNATURE1_DEBUG
};
#endif

