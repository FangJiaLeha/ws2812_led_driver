#ifndef _SYSTEM_COMMON_H
#define _SYSTEM_COMMON_H

//==============================================================================
/*------------------------------ Headfile include ----------------------------*/
#include "bootloaderinfo.h"
#include "signature_code.h"

//==============================================================================
/*------------------------------ Macro defination ----------------------------*/
#define FIRMWARE_MAX_SIZE   (FLASH_SIZE - FLASH_BOOT_MAXSIZE)

//==============================================================================
/*-------------------------- Extern ariable statement ------------------------*/
extern volatile uint32_t ResetSignature[2];
#if ( defined(_USING_BOOTLOADER) && (_USING_BOOTLOADER != 0x00u))
extern const uint32_t * FirmwareSignature;
extern const uint32_t *FirmwareSize;
extern const uint8_t  *mcu_type;
#else
extern const uint32_t FirmwareSignature[2];
#endif

#endif
