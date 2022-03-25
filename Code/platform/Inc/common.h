/**
 * @file common.h
 * @author {fangjiale}
 * @brief
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _COMMON_H
#define _COMMON_H

#include "gd32f3x0.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t RtvStatus;
typedef uint32_t SizeType;
typedef uint32_t OffsetType;

struct dev_ops
{
    RtvStatus (*init)(void *dev);
    RtvStatus (*open)(void *dev, uint16_t oflag);
    RtvStatus (*close)(void *dev);
    SizeType (*read)(void *dev, OffsetType pos, void *buffer, SizeType size);
    SizeType (*write)(void *dev, OffsetType pos, const void *buffer, SizeType size);
    RtvStatus (*control)(void *dev, const int cmd, void *args);
};

/* test macro */
#define _TEST_ 0x00

#define EINVAL 2   /* Invalid argument */
#define ETIMEOUT 3 /* Timed out */
#define EFULL 4    /* The resource is full */
#define EEMPTY 5   /* The resource is empty */
#define ENOMEM 6   /* No memory */
#define ENOSYS 7   /* No system */
#define EBUSY 8    /* Busy */
#define EIO 9      /* IO error */
#define EINTR 10   /* Interrupted system call */

#define ITEM_NUM(_item) (sizeof(_item) / sizeof(_item[0]))

#endif
