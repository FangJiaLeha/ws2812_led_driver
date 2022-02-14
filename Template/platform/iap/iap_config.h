#ifndef _USER_CONFIG_H
#define _USER_CONFIG_H

#include "common.h"

//==============================================================================
/* bootloader config */
#define _USING_FLASH        (0x01u)
#define FLASH_BASE_ADDR     (0x08000000ul)
#define FLASH_BOOT_MAXSIZE  (1024 * 20)
#define FLASH_APP_ADDR      (FLASH_BASE_ADDR + FLASH_BOOT_MAXSIZE)

//==============================================================================
/* bootloader info config */
#define _USING_BOOTLOADERINFO           (0x01u)

//==============================================================================
/* flash config */
#define FLASH_END_ADDR                (0x0801FFFFul)
#define FLASH_SIZE                    (FLASH_END_ADDR - FLASH_BASE_ADDR + 0x01u)
#define FLASH_PAGE_BASE_SIZE          (1 * 1024ul)             // 1kbytes/page

//==============================================================================
/* IAP config */
#define DEBUG_MODE              (0x00u)
#define _USING_BOOTLOADER       (0x01u)
#define _USING_IAP              (0x01u)

//==============================================================================
/* I2C_app config */
#define RECV_DATA_BASE_SIZE          (1024u)
#define I2C_CMD_NUM                  (0x07u)
#define IAP_CMD                      (0x16u)
#define PROGRAM_BYTES_ONCE           (0x05u)

//==============================================================================
/* soft CRC32 config */
#define _USING_SOFT_CRC32            (0x01u)

#endif
