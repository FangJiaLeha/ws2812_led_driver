#ifndef _SIGNATURE_CODE_H
#define _SIGNATURE_CODE_H

/***************************************************************************************************/
#define RESET_SIGNATURE_ADDRESS                   ( 0x20000000 )
#define FIRMWARE_SIGNATURE_OFFSET                 ( 0x0F80 )
/***************************************************************************************************
                                        
***************************************************************************************************/
#define RESET_SIGNATURE0_UPDATE_FIRMWARE          ( 0XFD002244 )
#define RESET_SIGNATURE1_UPDATE_FIRMWARE          ( 0XFD113355 )

#define BOOTLOADER_SIGNATURE0_VALID               ( 0XBDDEEDAB )
#define BOOTLOADER_SIGNATURE1_VALID               ( 0XBDAAEEDD )

#define FIRMWARE_SIGNATURE0_DEBUG                 ( 0XFDEB0044 )
#define FIRMWARE_SIGNATURE1_DEBUG                 ( 0XFDEB1155 )
#define FIRMWARE_SIGNATURE0_RELEASE               ( 0XFE00AACC )
#define FIRMWARE_SIGNATURE1_RELEASE               ( 0XFE117799 )
/**************************************************************************************************/

#endif
