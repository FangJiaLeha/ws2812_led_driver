#ifndef _TASK_SCH_H
#define _TASK_SCH_H

#include "common.h"

enum task_level_def
{
    TASK_1MS_LEVEL = 0x00,
    TASK_10MS_LEVEL,
    TASK_AUTO_SET_MS_LEVEL,
    TASK_DATA_DEAL_0MS_LEVEL
};

enum task_type
{
    WS2812_RENDER_TASK,
    DATA_ANALYSIS_TASK
};

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

void task_register(uint8_t task_type, uint8_t _level, void (*_callback)(void));
void task_1ms_tick_increase(void);
Rtv_Status task_ms_reset(uint8_t task_type, uint8_t task_level, uint16_t _new_task_ms);
void task_server(void);

#endif
