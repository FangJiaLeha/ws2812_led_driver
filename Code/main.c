/**
 * @file main.c
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "board.h"
#include "task_sch.h"

int main(void)
{
    init_board();
    task_register(WS2812_RENDER_TASK, TASK_10MS_LEVEL, ws2812_render);
    task_register(DATA_ANALYSIS_TASK, TASK_DATA_DEAL_0MS_LEVEL, data_analysis_task);
    while (1)
    {
        task_server();
        // 执行喂狗
        clear_wdgt();
    }
}
