#include "board.h"
#include "test.h"
#include "iap.h"
#include "i2c_app.h"

// 语义化版本开始执行
/*
 * 当前程序版本
 * 版本编码规则参考@ref
 * @brief
 *      V1.0.0  the first release version
 *      V1.0.1  hotfix version that fix i2c overshoot bug by reducing gpio speed
 */
uint8_t version[3] = {1, 0, 1};

int main(void)
{
    #if (defined(_USING_TEST_EN) && _USING_TEST_EN == 0x01)
    test_func_enter();
    #else
    __set_PRIMASK(1);
    // Check the firmware whether is valid for software reset.
    check_firmware();

    __set_PRIMASK(0);
    __set_FAULTMASK(0);

    init_board();

    while(1)
    {
        i2c_data_recv_dispatch();
    }
    #endif
}
