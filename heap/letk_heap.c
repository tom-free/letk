/***********************************************************************************************************************
** 文件描述：内存堆管理(动态内存管理)源文件
** 创建作者：付瑞彪(Tom Free)
** 创建日期：2022年7月5日
** 编码格式：UTF-8编码
** 编程语言：C语言，C99标准
** 缩进格式：4个空格键
** 命名规范：下划线命名法(小写命名法)
** 开源许可：MIT许可证，参考：https://mit-license.org
** 版权信息：Copyright (c) 2013-2022, Tom Free, <tomfreefu@gmail.com>
**
** 修改记录
** 修改日期         修改作者        修改内容
** 2022年7月5日     付瑞彪          创建文件，初次版本
**
***********************************************************************************************************************/

#include "letk_heap_cfg.h"
#include "letk_heap.h"
#include "letk_log.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/* 创建本模块的日志打印 */
#if LETK_HEAP_LOG_ENABLE
#define LETK_HEAP_LOG_DEBUG(...)    LETK_LOG(DEBUG, __VA_ARGS__)
#define LETK_HEAP_LOG_ERROR(...)    LETK_LOG(ERROR, __VA_ARGS__)
#else   /* LETK_HEAP_LOG_ENABLE */
#define LETK_HEAP_LOG_DEBUG(...)
#define LETK_HEAP_LOG_ERROR(...)
#endif  /* LETK_HEAP_LOG_ENABLE */

/* 内存块标志 */
enum
{
    LETK_HEAP_FLAG_FREE    = (uint8_t)0,    /* 空闲标志 */
    LETK_HEAP_FLAG_USED    = (uint8_t)1,    /* 使用标志 */
    LETK_HEAP_FLAG_PADDING = (uint8_t)2,    /* 使用填充标志 */
};

/* 内存区域 */
static uint8_t letk_heap_buf[LETK_HEAP_BLOCK_NUM][LETK_HEAP_BLOCK_SIZE];
/* map区 */
static uint8_t letk_heap_flag_map[LETK_HEAP_BLOCK_NUM];
/* 初始化标志 */
static bool letk_heap_init_flag = false;

/**
 * @brief   初始化内存堆
 * @return  初始化结果
 */
bool letk_heap_init(void)
{
    /* 清空map表 */
    memset(letk_heap_flag_map, LETK_HEAP_FLAG_FREE, sizeof(letk_heap_flag_map));
    letk_heap_init_flag = true;

    LETK_HEAP_LOG_DEBUG("letk_heap_init ok");

    return true;
}

/**
 * @brief   内存申请
 * @param   size 内存大小
 * @return  内存地址
 */
void* letk_heap_alloc(size_t size)
{
    size_t want_blk_num;    /* 期望申请的块数 */
    size_t free_cnt;        /* 连续空闲块数 */

    LETK_HEAP_LOG_DEBUG("malloc size = %d", size);

    /* 申请长度不能为0或者大于总长度 */
    if ((size == 0) || (size > sizeof(letk_heap_buf)) || !letk_heap_init_flag)
    {
        LETK_HEAP_LOG_ERROR("malloc failed, size or letk_heap_init_flag error");
        return NULL;
    }

    /* 搜索空闲内存 */
    want_blk_num = (size + LETK_HEAP_BLOCK_SIZE - 1) / LETK_HEAP_BLOCK_SIZE;
    free_cnt = 0;
    for (size_t i = 0; i < LETK_HEAP_BLOCK_NUM; i++)
    {
        if (letk_heap_flag_map[i] == LETK_HEAP_FLAG_FREE)
        {
            free_cnt++;
        }
        else
        {
            free_cnt = 0;
        }
        if (free_cnt == want_blk_num)
        {
            i -= (free_cnt - 1);
            letk_heap_flag_map[i] = LETK_HEAP_FLAG_USED;
            for (size_t j = 1; j < free_cnt; j++)
            {
                letk_heap_flag_map[i + j] = LETK_HEAP_FLAG_PADDING;
            }
            LETK_HEAP_LOG_DEBUG("malloc ptr = 0x%x", &letk_heap_buf[i][0]);
            return &letk_heap_buf[i][0];
        }
    }

    LETK_HEAP_LOG_ERROR("malloc failed, memory not enough");
    return NULL;
}

/**
 * @brief   内存释放
 * @param   ptr 内存指针
 */
void letk_heap_free(void* const ptr)
{
    size_t offset;
    size_t blk;

    LETK_HEAP_LOG_DEBUG("free ptr = 0x%x", ptr);

    if (!letk_heap_init_flag)
    {
        LETK_HEAP_LOG_ERROR("free failed, letk_heap_init_flag error");
        return;
    }

    if ((uint8_t*)ptr < (uint8_t*)&letk_heap_buf[0][0])
    {
        LETK_HEAP_LOG_ERROR("free failed, ptr < start");
        return;
    }
    offset = (uint8_t*)ptr - (uint8_t*)&letk_heap_buf[0][0];
    if (offset >= sizeof(letk_heap_buf))
    {
        LETK_HEAP_LOG_ERROR("free failed, ptr > end");
        return;
    }
    blk = offset / LETK_HEAP_BLOCK_SIZE;
    letk_heap_flag_map[blk++] = LETK_HEAP_FLAG_FREE;
    while (letk_heap_flag_map[blk] == LETK_HEAP_FLAG_PADDING)
    {
        letk_heap_flag_map[blk++] = LETK_HEAP_FLAG_FREE;
    }
    LETK_HEAP_LOG_DEBUG("free ok");
}
