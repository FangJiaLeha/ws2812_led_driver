#ifndef _DRV_FLASH_H
#define _DRV_FLASH_H

//==============================================================================
#include "iap_config.h"

//==============================================================================
#if ( defined(_USING_FLASH) && (_USING_FLASH == 0x01u) )
/*------------------------------ Macro defination ----------------------------*/
#define IS_PRGADDR_VALID(prg_addr)      ( ((prg_addr) >= FLASH_BASE_ADDR) &&   \
                                          ((prg_addr) <= FLASH_END_ADDR) )
#define IS_PRGDATABUFF_VALID(prg_data_buff)     ( (prg_data_buff != NULL) )
#define IS_PRGSIZE_VALID(prg_addr, prg_size)    ( ((prg_addr + prg_size) >= FLASH_BASE_ADDR) &&  \
                                                  ((prg_addr + prg_size) <= FLASH_END_ADDR) )
#define IS_ERSADDR_VALID(ers_addr)      IS_PRGADDR_VALID(ers_addr)
#define IS_ERSSIZE_VALID(ers_addr, ers_size)      IS_PRGSIZE_VALID(ers_addr, ers_size)

//==============================================================================
/*--------------------------- Extern function prototypes ---------------------*/
void flash_program(uint32_t _PrgAddr, uint8_t *_PrgDataBuff, uint32_t _PrgSize);
void flash_erase(uint32_t _ErsAddr, uint32_t _ErsSize);
#endif

#endif
