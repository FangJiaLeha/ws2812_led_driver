/**
 * @file test.h
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _TEST_H
#define _TEST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief 测试宏
 *
 * @note  当该宏值为1时 则启动测试模块/当该宏值为0时 则禁用测试模块
 */
#define _USING_TEST (0x00)

#if defined(_USING_TEST) && (_USING_TEST == 0x01)

typedef unsigned char uint8_t;

/**
 * @brief 定义每个测试类型有 TEST_TASK_NUM 个测试任务
 *
 * @note  此处设置任务为10
 */
#define TEST_TASK_NUM (0x0A)

/**
 * @brief 定义每个测试结点命令的长度为20
 *
 */
#define TEST_NODE_CMD_LEN   (0x14)

/**
 * @brief 测试类型
 *
 */
typedef enum
{
    FUNC_TEST,              // 功能性测试
    INTERFACE_TEST          // 内部接口测试
} TestType;

/**
 * @brief 测试任务初始化
 *
 */
void init_test_task(void);

/**
 * @brief 注册测试任务
 *
 * @param test_type         测试类型@TestType
 * @param test_cmd_name     测试命令(字符串)
 * @param test_func         测试命令对应的执行函数
 *
 */
void regist_test_task(const TestType test_type,
                      const char *test_cmd_name,
                      void (*test_func)(void));

/**
 * @brief 测试任务调度
 *
 * @param test_type         测试类型@TestType
 */
void schedule_test_task(const TestType test_type);
#endif

#endif
