/**
 * @file drv_wdgt.c
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "drv_wdgt.h"
#include "task_sch.h"

Rtv_Status init_wdgt(const uint16_t timeout_s)
{
    uint32_t cur_tick;
    if (timeout_s == 0) {
        return EINVAL;
    }
    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);

    cur_tick = get_task_ms_tick();
    /* wait till IRC40K is ready */
    while(ERROR == rcu_osci_stab_wait(RCU_IRC40K)) {
        // 等待超时 自动退出
        if (get_task_ms_tick() - cur_tick > 5000) {
            return ETIMEOUT;
        }
    }

    /* enable write access to FWDGT_PSC and FWDGT_RLD registers */
    fwdgt_config(WDGT_RELOAD_VALUE(timeout_s), FWDGT_PSC_DIV64);
    fwdgt_enable();
    return SUCCESS;
}

void clear_wdgt(void)
{
    fwdgt_counter_reload();
}