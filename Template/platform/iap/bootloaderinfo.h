#ifndef _BOOTLOADER_INFO_H
#define _BOOTLOADER_INFO_H

#include "iap_config.h"

//==============================================================================
/*------------------------------ Extern typedef ------------------------------*/
typedef enum rdwr_bootloaderinfo_type
{
    SOFTWARE_VERSION = 0x01u,
    FILE_SIZE,
    DOWNLOAD_BYTES,
    CRT_BLOCK_SIZE,
    CRT_BLOCK_NUM,
    CRT_ADDR,
}RDWRBootloaderinfoType;

//==============================================================================
#if ( defined(_USING_BOOTLOADERINFO) && (_USING_BOOTLOADERINFO == 0x01u) )
/*------------------------------ Macro defination ----------------------------*/
#define IS_RDWRBOOTINFO_VALID(rdwr_bootinfo)    IS_PARAMETER_VALID(rdwr_bootinfo)
#define IS_RDWRTYPE_VALID(rdwr_type)    ( ((rdwr_type) == SOFTWARE_VERSION) || \
                                          ((rdwr_type) == FILE_SIZE)        || \
                                          ((rdwr_type) == DOWNLOAD_BYTES)   || \
                                          ((rdwr_type) == CRT_BLOCK_SIZE)   || \
                                          ((rdwr_type) == CRT_BLOCK_NUM)    || \
                                          ((rdwr_type) == CRT_ADDR) )

#define IS_RDWRVALUE_VALID(rdwr_value)  IS_PARAMETER_VALID(rdwr_value)

//==============================================================================
/*--------------------------- Extern function prototypes ---------------------*/
void write_bootloaderinfo(RDWRBootloaderinfoType _writeType, const void *_writeValue);
void read_bootloaderinfo(RDWRBootloaderinfoType _readType, void *_readValue);
#endif

#endif
