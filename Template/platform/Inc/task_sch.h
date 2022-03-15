/**
 * @file task_sch.h
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _TASK_SCH_H
#define _TASK_SCH_H

#include "common.h"

typedef enum task_level
{
    TASK_1MS_LEVEL = 0x00,
    TASK_10MS_LEVEL,
    TASK_AUTO_SET_MS_LEVEL,
    TASK_DATA_DEAL_0MS_LEVEL
}TaskLevelType;

typedef enum task_type
{
    WS2812_RENDER_TASK,
    DATA_ANALYSIS_TASK
}TaskType;

#define TASK_TYPE_CHECK(_type)                          \
do {                                                    \
    if (_type != WS2812_RENDER_TASK &&                  \
        _type != DATA_ANALYSIS_TASK) {                  \
        goto set_error;                                 \
    }                                                   \
} while(0)

#define TASK_LEVEL_CHECK(_level)                        \
do {                                                    \
    if (_level != TASK_1MS_LEVEL &&                     \
        _level != TASK_10MS_LEVEL &&                    \
        _level != TASK_AUTO_SET_MS_LEVEL &&             \
        _level != TASK_DATA_DEAL_0MS_LEVEL) {           \
        goto set_error;                                 \
    }                                                   \
} while(0)

#define TASK_CALLBACK_CHECK(_callback)                  \
do {                                                    \
    if (_callback == NULL) {                            \
        goto set_error;                                 \
    }                                                   \
} while(0)

//==============================================================================
/**
 * @brief 任务tick递增 放在系统中断函数中
 *
 */
void task_1ms_tick_increase(void);

/**
 * @brief Get the task ms tick object
 *
 * @return uint32_t 返回当前系统时钟tick
 */
uint32_t get_task_ms_tick(void);

/**
 * @brief 任务注册接口
 *
 * @param task_type         任务类型@task_type
 * @param _level            任务等级@task_level_def
 * @param _callback         任务执行函数
 */
void task_register(const TaskType task_type,
                   const TaskLevelType _level,
                   void (*_callback)(void));

/**
 * @brief 任务调度时间重置接口
 *
 * @param task_type         任务类型@task_type
 * @param task_level        任务等级@task_level_def
 * @param _new_task_ms      任务新调度时间
 * @return Rtv_Status       @SUCCESS:重置任务调度时间成功 @其他值:重置任务调度时间失败
 */
Rtv_Status task_ms_reset(const TaskType task_type,
                         const TaskLevelType task_level,
                         const uint16_t _new_task_ms);

/**
 * @brief 任务调度接口额
 *
 */
void task_server(void);

#endif
