#include "test.h"

void test_func_enter(void)
{
    // write and read flash test
    #if (defined(_RDWR_BOOTLOADERINFO_TEST_EN) && _FLASH_WRERS_TEST_EN == 0x01)
    uint32_t writeAddr = FLASH_APP_ADDR;
    uint8_t writeBuff[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    flash_erase(writeAddr, sizeof(writeBuff));
    flash_program(writeAddr, writeBuff, sizeof(writeBuff));
    #endif

    // write and read bootloader test
    #if (_RDWR_BOOTLOADERINFO_TEST_EN)
    uint32_t file_size1 = 0x12345678ul, file_size2;
    write_bootloaderinfo(FILE_SIZE, (const void *)&file_size1);
    read_bootloaderinfo(FILE_SIZE, (void *)&file_size2);
    
    uint32_t download_bytes1 = 0x12345678ul, download_bytes2;
    write_bootloaderinfo(DOWNLOAD_BYTES, (const void *)&download_bytes1);
    read_bootloaderinfo(DOWNLOAD_BYTES, (void *)&download_bytes2);
    
    uint16_t crt_block_size1 = 0x5678ul, crt_block_size2;
    write_bootloaderinfo(CRT_BLOCK_SIZE, (const void *)&crt_block_size1);
    read_bootloaderinfo(CRT_BLOCK_SIZE, (void *)&crt_block_size2);
    
    uint16_t crt_block_num1 = 0x5678ul, crt_block_num2;
    write_bootloaderinfo(CRT_BLOCK_NUM, (const void *)&crt_block_num1);
    read_bootloaderinfo(CRT_BLOCK_NUM, (void *)&crt_block_num2);

    uint16_t crt_addr1 = 0x5678ul, crt_addr2;
    write_bootloaderinfo(CRT_ADDR, (const void *)&crt_addr1);
    read_bootloaderinfo(CRT_ADDR, (void *)&crt_addr2);
    #endif
}
