#ifndef _TEST_H
#define _TEST_H

#include "drv_flash.h"
#include "bootloaderinfo.h"

//==============================================================================
/* debug config */
#define _USING_TEST_EN                  (0x00u)

#if (defined(_USING_TEST_EN) && _USING_TEST_EN == 0x01)
#define _FLASH_WRERS_TEST_EN            (0x01u)
#define _RDWR_BOOTLOADERINFO_TEST_EN    (0x01u)
#endif

void test_func_enter(void);

#endif
