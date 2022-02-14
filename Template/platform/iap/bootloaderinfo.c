/****************************** Copyright (c) **********************************

**------------------------------ File Info -------------------------------------
** File name: bootloaderinfo.c
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
#include "bootloaderinfo.h"

//==============================================================================
/*------------------------------ Private typedef -----------------------------*/
typedef struct bootloaderinfo_type
{
    uint32_t FileSize;
    uint32_t DownloadBytesSum;                  
    uint16_t CurrentBlockSize;
    uint16_t CurrentBlockNum;
    uint16_t CurrentAddr;
}BootLoaderInfoType;

typedef struct bootloader_type
{
    BootLoaderInfoType bootloader_info;
    void (*write)(BootLoaderInfoType *_toWriteBootInfo,
                  RDWRBootloaderinfoType _toWriteType, const void *_toWriteValue);
    void (*read)(BootLoaderInfoType *_toReadBootInfo,
                 RDWRBootloaderinfoType _toReadType, void *_toReadValue);
}BootloaderType;

//==============================================================================
#if ( defined(_USING_BOOTLOADERINFO) && (_USING_BOOTLOADERINFO == 0x01u) )
/*----------------------- Private user function prototypes -------------------*/
static void _write(BootLoaderInfoType *_toWriteBootInfo, 
                   RDWRBootloaderinfoType _toWriteType, const void *_toWriteValue);
static void _read(BootLoaderInfoType *_toReadBootInfo, 
                  RDWRBootloaderinfoType _toReadType, void *_toReadValue);

/*-------------------------- Private variable defination ---------------------*/
static BootloaderType Bootloader = 
{
    .bootloader_info = {0},
    .write = _write,
    .read  = _read
};

/*--------------------- Private user function implementation -----------------*/
/**
 * @Brief   _write() func
 * @Call    Internal
 * @Param   _toWriteBootInfo The address of @Bootloader
 * @Param   _toWriteType The type of write @Bootloader
 *          The Parameter must be set @RDWRBootloaderinfoType
 * @Param   _toWriteValue The address of set value
 * @Note    Write bootloader segnment according to the @_toWriteType
 * @RetVal  None
 */
static void _write(BootLoaderInfoType *_toWriteBootInfo, 
                   RDWRBootloaderinfoType _toWriteType, const void *_toWriteValue)
{
    DDL_ASSERT(IS_RDWRBOOTINFO_VALID(_toWriteBootInfo));
    DDL_ASSERT(IS_RDWRTYPE_VALID(_toWriteType));
    DDL_ASSERT(IS_RDWRVALUE_VALID(_toWriteValue));
    
    switch((uint8_t)_toWriteType)
    {
        case FILE_SIZE:
            _toWriteBootInfo->FileSize = *(uint32_t *)_toWriteValue;
        break;
        case DOWNLOAD_BYTES:
            _toWriteBootInfo->DownloadBytesSum = *(uint32_t *)_toWriteValue;
        break;
        case CRT_BLOCK_SIZE:
            _toWriteBootInfo->CurrentBlockSize = *(uint16_t *)_toWriteValue;
        break;
        case CRT_BLOCK_NUM:
            _toWriteBootInfo->CurrentBlockNum = *(uint16_t *)_toWriteValue;
        break;
        case CRT_ADDR:
            _toWriteBootInfo->CurrentAddr = *(uint16_t *)_toWriteValue;
        break;
        default:
        break;
    }

set_error:
    return;
}

/**
 * @Brief   _read() func
 * @Call    Internal
 * @Param   _toReadBootInfo The address of @Bootloader
 * @Param   _toReadType The type of read @Bootloader
 *          The parameter must be set @RDWRBootloaderinfoType
 * @Param   _toReadValue The address of saving value
 * @Note    Read bootloader segnment according to the @_toReadType
 * @RetVal  None
 */
static void _read(BootLoaderInfoType *_toReadBootInfo, 
                  RDWRBootloaderinfoType _toReadType, void *_toReadValue)
{
    DDL_ASSERT(IS_RDWRBOOTINFO_VALID(_toReadBootInfo));
    DDL_ASSERT(IS_RDWRTYPE_VALID(_toReadType));
    DDL_ASSERT(IS_RDWRVALUE_VALID(_toReadValue));
    
    switch((uint8_t)_toReadType)
    {
        case FILE_SIZE:
            *(uint32_t *)_toReadValue = _toReadBootInfo->FileSize;
        break;
        case DOWNLOAD_BYTES:
             *(uint32_t *)_toReadValue = _toReadBootInfo->DownloadBytesSum;
        break;
        case CRT_BLOCK_SIZE:
             *(uint16_t *)_toReadValue = _toReadBootInfo->CurrentBlockSize;
        break;
        case CRT_BLOCK_NUM:
             *(uint16_t *)_toReadValue = _toReadBootInfo->CurrentBlockNum;
        break;
        case CRT_ADDR:
             *(uint16_t *)_toReadValue = _toReadBootInfo->CurrentAddr;
        break;
        default:
        break;
    }

set_error:
    return;
}

//==============================================================================
/*-------------------------- User function implementation --------------------*/
/**
 * @Brief   write_bootloaderinfo() func
 * @Call    External
 * @Param   _writeType The write type to bootloader.
 * @Param   _writeValue The value writed according to the write type.
 * @Note    User can write data to the bootloader according to the @_writeType
 * @RetVal  None
 */
void write_bootloaderinfo(RDWRBootloaderinfoType _writeType, const void *_writeValue)
{
    Bootloader.write(&(Bootloader.bootloader_info), _writeType, _writeValue);
}

/**
 * @Brief   read_bootloaderinfo() func
 * @Call    External
 * @Param   _readType The read type to bootloader.
 * @Param   _readValue The value read according to the read type.
 * @Note    User can read data to the bootloader according to the @_readType
 * @RetVal  None
 */
void read_bootloaderinfo(RDWRBootloaderinfoType _readType, void *_readValue)
{
    Bootloader.read(&(Bootloader.bootloader_info), _readType, _readValue);
}

#endif
