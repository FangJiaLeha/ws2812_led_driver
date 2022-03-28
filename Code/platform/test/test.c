/**
 * @file test.c
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "test.h"

#if defined(_USING_TEST) && (_USING_TEST == 0x01)

typedef struct
{
    uint8_t *cmd_name;       // 测试结点命令名
    void (*cmd_callback)(void); // 测试结点命令对应的执行函数
} TestCmdNodeType;
typedef TestCmdNodeType *pTestCmdNodeType;

typedef struct
{
    /**
     * @brief 指向一个测试任务的TEST_TASK_NUM个测试结点
     *
     */
    TestCmdNodeType (*cmdNodes)[TEST_TASK_NUM];
    /**
     * @brief 指向一个测试任务的空闲测试结点总数
     *
     */
    const uint8_t (*freeNodesAll)[1];
    /**
     * @brief 指向一个测试任务的空闲测试结点计数
     *
     */
    uint8_t (*freeNodesCnt)[1];
    void (*init_task)(void *dev,
                      const uint8_t test_node_cmd_len);
    void (*regist_task)(void *dev,
                        const TestType test_type,
                        const char *test_cmd_name,
                        void (*test_callback)(void));
    void (*schedule_task)(void *dev,
                          const TestType test_type);
} TestDevType;
typedef TestDevType *pTestDevType;


static void _init(void *dev,
                  const uint8_t test_node_cmd_len);
static void _regist(void *dev,
                    const TestType test_type,
                    const char *test_cmd_name,
                    void (*test_callback)(void));
static void _schedule(void *dev,
                      const TestType test_type);

/**
 * @brief 静态编译定义测试设备
 *
 */
static TestDevType test_dev =
{
    .cmdNodes = NULL,
    .freeNodesAll = NULL,
    .freeNodesCnt = NULL,
    .init_task = _init,
    .regist_task = _regist,
    .schedule_task = _schedule
};

/**
 * @brief 初始化测试设备
 *
 * @param dev   设备地址
 * @param test_node_cmd_len     测试结点命令长度
 */
static void _init(void *dev,
                  const uint8_t test_node_cmd_len)
{
     pTestDevType test_dev_temp = (pTestDevType)dev;
     uint8_t test_dev_node_all = 0;
     pTestCmdNodeType pNode = NULL;
     if (test_dev_temp == NULL ||
         test_node_cmd_len == 0)
     {
         return;
     }

    // 若初始测试结点已经分配了内存 需释放
    if (test_dev_temp->cmdNodes != NULL)
    {
        free(test_dev_temp->cmdNodes);
        test_dev_temp->cmdNodes = NULL;
    }
    // 重新给测试结点分配内存
    test_dev_node_all = (INTERFACE_TEST + 1) * TEST_TASK_NUM;
    test_dev_temp->cmdNodes =
    (TestCmdNodeType (*)[TEST_TASK_NUM])malloc(test_dev_node_all * sizeof(TestCmdNodeType));
    if (test_dev_temp->cmdNodes == NULL)
    {
        return;
    }
    memset(test_dev_temp->cmdNodes,
           0,
           test_dev_node_all * sizeof(TestCmdNodeType));

    // 初始化单个测试结点对应的命令名内存
    pNode = test_dev_temp->cmdNodes[FUNC_TEST];
    for (uint8_t test_node_cnt = 0; test_node_cnt < test_dev_node_all; test_node_cnt++)
    {
        pNode[test_node_cnt].cmd_name =
        (uint8_t *)malloc(sizeof(uint8_t) * test_node_cmd_len);
        if (pNode[test_node_cnt].cmd_name == NULL)
        {
            return;
        }
        memset(pNode[test_node_cnt].cmd_name,
               0,
               sizeof(uint8_t) * test_node_cmd_len);
    }

    // 若初始化测试结点总数已经分配了内存 需释放
    if (test_dev_temp->freeNodesAll != NULL)
    {
        free((void *)test_dev_temp->freeNodesAll);
    }
    test_dev_temp->freeNodesAll = (const uint8_t (*)[1])malloc( (INTERFACE_TEST + 1) * 1 * sizeof(uint8_t));
    if (test_dev_temp->freeNodesAll == NULL)
    {
        return;
    }
    memset((void *)test_dev_temp->freeNodesAll,
           TEST_TASK_NUM,
           (INTERFACE_TEST + 1) * 1 * sizeof(uint8_t));

    // 若初始化测试结点计数变量已经分配了内存 需释放
    if (test_dev_temp->freeNodesCnt != NULL)
    {
        free(test_dev_temp->freeNodesCnt);
    }
    test_dev_temp->freeNodesCnt = (uint8_t (*)[1])malloc( (INTERFACE_TEST + 1) * 1 * sizeof(uint8_t));
    if (test_dev_temp->freeNodesCnt == NULL)
    {
        return;
    }
    memset(test_dev_temp->freeNodesCnt,
           TEST_TASK_NUM,
           (INTERFACE_TEST + 1) * 1 * sizeof(uint8_t));
}

/**
 * @brief 注册测试任务
 *
 * @param dev               测试设备
 * @param test_type         测试类型@TestType
 * @param test_cmd_name     测试命令(字符串)
 * @param test_callback     测试命令对应的执行函数
 */
static void _regist(void *dev,
                    const TestType test_type,
                    const char *test_cmd_name,
                    void (*test_callback)(void))
{
    pTestDevType test_dev_temp = (pTestDevType)dev;
    uint8_t freeNodesCnt = 0;
    uint8_t freeNodesAll = 0;
    uint8_t cal_pos = 0;

    // 传入参数有效性判断
    if (test_dev_temp == NULL)
    {
        return;
    }

    // 获取当前测试设备的空闲结点个数 并判断其有效性
    freeNodesCnt = test_dev_temp->freeNodesCnt[test_type][0];
    if (freeNodesCnt == 0)
    {
        return;
    }

    // 在现有测试结点中 匹配已有测试任务 若存在该任务 则替换执行函数
    for (uint8_t node_cnt = 0; node_cnt < TEST_TASK_NUM; node_cnt++)
    {
        if (strncmp(test_cmd_name,
                    (const void *)test_dev_temp->cmdNodes[test_type][node_cnt].cmd_name,
                    strlen(test_cmd_name)) == 0)
        {
            test_dev_temp->cmdNodes[test_type][node_cnt].cmd_callback =
            test_callback;
        }
    }

    // 获取当前测试设备的空闲结点总数 并计算即将插入测试任务的结点位置
    freeNodesAll = test_dev_temp->freeNodesAll[test_type][0];
    cal_pos = freeNodesAll - freeNodesCnt;

    // 任务注册
    if (test_dev_temp->cmdNodes[test_type][cal_pos].cmd_name == NULL)
    {
        return;
    }
    // 若注册的任务命令长度大于单个测试结点预留的任务命令长度
    if (strlen(test_cmd_name) > TEST_NODE_CMD_LEN)
    {
        memcpy(test_dev_temp->cmdNodes[test_type][cal_pos].cmd_name,
            test_cmd_name,
            TEST_NODE_CMD_LEN);
    }
    else
    {
        memcpy(test_dev_temp->cmdNodes[test_type][cal_pos].cmd_name,
            test_cmd_name,
            strlen(test_cmd_name));
    }
    test_dev_temp->cmdNodes[test_type][cal_pos].cmd_callback = test_callback;

    // 空闲结点数更新
    (test_dev_temp->freeNodesCnt[test_type][0])--;
}

/**
 * @brief 测试任务调度
 *
 * @param dev           测试设备
 * @param test_type     测试任务类型
 */
static void _schedule(void *dev,
                      const TestType test_type)
{
    pTestDevType test_dev_temp = (pTestDevType)dev;
    uint8_t need_test_nodes = 0;
    if (test_dev_temp == NULL)
    {
        return;
    }

    need_test_nodes = test_dev_temp->freeNodesAll[test_type][0] -
                      test_dev_temp->freeNodesCnt[test_type][0];
    // 判断各测试类型有无需调度的测试结点
    if (need_test_nodes == 0 ||
        (int)need_test_nodes < 0) {
        return;
    }

    // 遍历测试任务列表中各结点
    for (uint8_t test_nodes_cnt = 0; test_nodes_cnt < need_test_nodes; test_nodes_cnt++)
    {
        if (test_dev_temp->cmdNodes[test_type][test_nodes_cnt].cmd_callback != NULL)
        {
            test_dev_temp->cmdNodes[test_type][test_nodes_cnt].cmd_callback();
        }
    }
}

//=========================================================================
void init_test_task(void)
{
    test_dev.init_task((void *)&test_dev, TEST_NODE_CMD_LEN);
}

void regist_test_task(const TestType test_type,
                      const char *test_cmd_name,
                      void (*test_func)(void))
{
    if ((test_type != FUNC_TEST &&
        test_type != INTERFACE_TEST) ||
        test_cmd_name == NULL ||
        test_func == NULL)
    {
        return;
    }
    test_dev.regist_task((void *)&test_dev,
                         test_type,
                         test_cmd_name,
                         test_func);
}

void schedule_test_task(const TestType test_type)
{
    if (test_type != FUNC_TEST &&
        test_type != INTERFACE_TEST)
    {
        return;
    }
    test_dev.schedule_task((void *)&test_dev,
                            test_type);
}

#endif
