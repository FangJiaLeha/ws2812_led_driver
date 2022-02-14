#ifndef _CRC_H
#define _CRC_H

//==============================================================================
#include "iap_config.h"

//==============================================================================
#if ( defined(_USING_SOFT_CRC32) && (_USING_SOFT_CRC32 == 0x01u) )
/*--------------------------- Extern function prototypes ---------------------*/
uint32_t SoftwareCRC32( uint8_t *pData, uint16_t Length );
#endif

#endif
