#include "board.h"
#include "test.h"
#include "iap.h"
#include "i2c_app.h"

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
