/**
 * @file task_sch.c
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "task_sch.h"

//==============================================================================
#define TASK_AUTO_SET_MS_DEFAULT            10
static uint32_t task_1ms_tick = 0;

//==============================================================================
struct task_list
{
    const TaskLevelType task_level;
    uint32_t task_ms_value;
    void (*task_callback)(void);
};

struct task_status
{
    TaskLevelType cur_task_level;
    void (*cur_task_func)(void);
};

struct task_list tasks[] = 
{
    /**
     * @brief 1ms 任务
     */
    {TASK_1MS_LEVEL, 1, NULL},

    /**
     * @brief 10ms 任务
     */
    {TASK_10MS_LEVEL, 10, NULL},

    /**
     * @brief atuo_set ms 任务
     */
    {TASK_AUTO_SET_MS_LEVEL, TASK_AUTO_SET_MS_DEFAULT, NULL},

    /**
     * @brief 数据解析任务
     */
    {TASK_DATA_DEAL_0MS_LEVEL, 0, NULL},
};

static struct task_status task_types[] = 
{
    // WS2812 task status
    {
        .cur_task_level = TASK_1MS_LEVEL,
        .cur_task_func = NULL
    },
    {
        .cur_task_level = 0,
        .cur_task_func = NULL
    },
};

/**
 * @brief 根据任务等级 调用其回调函数
 *
 * @param _level                任务等级@task_level
 * @return struct task_list*    返回当前任务句柄
 */
static struct task_list *find_task_proccess(const TaskLevelType _level)
{
    uint8_t tasks_num = ITEM_NUM(tasks);

    for (uint8_t tasks_cnt = 0; tasks_cnt < tasks_num; tasks_cnt++) {
        if (_level == tasks[tasks_cnt].task_level) {
            if (NULL == tasks[tasks_cnt].task_callback) {
                return NULL;
            } else {
                return &tasks[tasks_cnt];
            }
        }
    }
    return NULL;
}

//==============================================================================
/**
 * @brief Set the task ms tick object
 *
 * @param cur_tick 设置当前任务时间戳
 *
 * @note  主要用来保证当前任务执行后 下一个任务能顺利执行
 */
static void set_task_ms_tick(const uint32_t cur_tick)
{
    task_1ms_tick = cur_tick;
}
/**
 * @brief Get the max task ms tick object
 *
 * @return uint16_t 返回任务列表中设置的最大任务时间
 *
 * @note   主要比较初始10ms任务时间 和 自定义任务时间
 */
static uint32_t get_max_task_ms_tick(void)
{
    uint32_t task_10ms_value = tasks[TASK_10MS_LEVEL].task_ms_value;
    uint32_t task_auto_set_ms_value = tasks[TASK_AUTO_SET_MS_LEVEL].task_ms_value;
    return (task_10ms_value > task_auto_set_ms_value ? task_10ms_value : task_auto_set_ms_value);
}
/**
 * @brief Get the task ms value object
 *
 * @param _level        任务等级@task_level
 * @return uint16_t     返回当前任务的任务时间
 */
static uint32_t get_task_ms_value(const TaskLevelType _level)
{
    return tasks[_level].task_ms_value;
}
//==============================================================================
void task_1ms_tick_increase(void)
{
    task_1ms_tick++;
}

uint32_t get_task_ms_tick(void)
{
    return task_1ms_tick;
}

void task_register(const TaskType task_type,
                   const TaskLevelType _level,
                   void (*_callback)(void))
{
    TASK_TYPE_CHECK(task_type);
    TASK_LEVEL_CHECK(_level);
    TASK_CALLBACK_CHECK(_callback);

    tasks[_level].task_callback = _callback;
    task_types[task_type].cur_task_level = _level;
    task_types[task_type].cur_task_func = tasks[_level].task_callback;
set_error:
    return;
}

Rtv_Status task_ms_reset(const TaskType task_type,
                         const TaskLevelType task_level,
                         const uint32_t _new_task_ms)
{
    TASK_TYPE_CHECK(task_type);
    TASK_LEVEL_CHECK(task_level);
    if (_new_task_ms == 0) {
        goto set_error;
    }

    if (task_level == task_types[task_type].cur_task_level) {
    } else {
        tasks[task_types[task_type].cur_task_level].task_callback = NULL;
        task_types[task_type].cur_task_level = task_level;
        tasks[task_level].task_callback = task_types[task_type].cur_task_func;
    }
    tasks[task_level].task_ms_value = _new_task_ms;
    return SUCCESS;
set_error:
    return EINVAL;
}

void task_server(void)
{
    struct task_list *tsk_lt = NULL;
    uint32_t ms_tick = get_task_ms_tick(), save_enter_tick;
    // 获取当前任务列表中任务最大调度时间
    uint32_t max_ms_tick = get_max_task_ms_tick();

    if( (ms_tick % max_ms_tick) ==
        (max_ms_tick == get_task_ms_value(TASK_AUTO_SET_MS_LEVEL) ? 0 : get_task_ms_value(TASK_AUTO_SET_MS_LEVEL)) ) {
        // 记录进入任务调度时的tick
        save_enter_tick = get_task_ms_tick();
        tsk_lt = find_task_proccess(TASK_AUTO_SET_MS_LEVEL);
        if (tsk_lt != NULL) {
            tsk_lt->task_callback();
        }
        // 执行任务调度后 恢复tick
        set_task_ms_tick(save_enter_tick);
    } else if ( (ms_tick % max_ms_tick) == get_task_ms_value(TASK_1MS_LEVEL)) {
        // 记录进入任务调度时的tick
        save_enter_tick = get_task_ms_tick();
        tsk_lt = find_task_proccess(TASK_1MS_LEVEL);
        if (tsk_lt != NULL) {
            tsk_lt->task_callback();
        }
        // 执行任务调度后 恢复tick
        set_task_ms_tick(save_enter_tick);
    } else if( (ms_tick % max_ms_tick) == 
        (max_ms_tick == get_task_ms_value(TASK_10MS_LEVEL) ? 0 : get_task_ms_value(TASK_10MS_LEVEL)) ) {
        // 记录进入任务调度时的tick
        save_enter_tick = get_task_ms_tick();
        tsk_lt = find_task_proccess(TASK_10MS_LEVEL);
        if (tsk_lt != NULL) {
            tsk_lt->task_callback();
        }
        // 执行任务调度后 恢复tick
        set_task_ms_tick(save_enter_tick);
    } else {
        // 记录进入任务调度时的tick
        save_enter_tick = get_task_ms_tick();
        tsk_lt = find_task_proccess(TASK_DATA_DEAL_0MS_LEVEL);
        if (tsk_lt != NULL) {
            tsk_lt->task_callback();
        }
        // 执行任务调度后 恢复tick
        set_task_ms_tick(save_enter_tick);
    }
}
