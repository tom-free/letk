/*******************************************************************************
** @file   letk_rbuffer.c
** @brief  环形缓冲区
** @author 付瑞彪
** @date   2022-05-22
**
**       Copyright (c) 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/

#include "letk_rbuffer.h"
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* 计算最小值 */
#define LETK_RBUFFER_GET_MIN(a, b) ((a) < (b)) ? (a) : (b)

/**
 * @brief 向下裁剪到2的N次幂
 * @param[in] x 数值
 * @return 向下裁剪到2的N次幂后的值
 * @example 输入17，输出16，输入4，输出4
 */
static uint32_t letk_rbuffer_trim_to_2_pow_n(uint32_t x)
{
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return (x + 1) >> 1;
}

/**
 * @brief 初始化一个环形缓冲区
 * @param[in] rb 环形缓冲区管理器指针(必须非NULL）
 * @param[in] buf 数据缓冲区(必须非NULL)
 * @param[in] length buf长度(会向下裁剪到2的N次幂)
 */
void letk_rbuffer_init(letk_rbuffer_t* rb, uint8_t* buf, uint32_t length)
{
    if (rb == NULL)
    {
        return;
    }

    if (buf == NULL)
    {
        rb->size = 0;
        rb->front = rb->rear = 0;
        return;
    }

    rb->buf = buf;
    rb->size = letk_rbuffer_trim_to_2_pow_n(length);
    rb->front = rb->rear = 0;
}

/**
 * @brief 清除环形缓冲区
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 */
void letk_rbuffer_clear(letk_rbuffer_t* rb)
{
    rb->front = rb->rear = 0;
}

/**
 * @brief 获取环形缓冲区当前的数据长度
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 * @return 当前的数据长度
 */
uint32_t letk_rbuffer_length(letk_rbuffer_t* rb)
{
    return (uint32_t)(rb->rear - rb->front);
}

/**
 * @brief 写入一字节到环形缓冲区
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 * @param[in] dat 数据字节
 * @return 是否写入成功
 */
bool letk_rbuffer_write_byte(letk_rbuffer_t* rb, uint8_t dat)
{
    uint32_t left;
    left = rb->size + rb->front - rb->rear;
    if (left)
    {
        *(uint8_t*)(rb->buf + (rb->rear & (rb->size - 1))) = dat;
        rb->rear++;
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief 从环形缓冲区读取一字节
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 * @param[out] pdat 数据字节指针(必须非NULL)
 * @return 是否读取成功
 */
bool letk_rbuffer_read_byte(letk_rbuffer_t* rb, uint8_t* pdat)
{
    uint32_t left;
    left = rb->rear - rb->front;
    if (left)
    {
        *pdat = *(uint8_t*)(rb->buf + (rb->front & (rb->size - 1)));
        rb->front++;
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief 写入多个字节到环形缓冲区
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 * @param[in] buf 数据存储(必须非NULL)
 * @param[in] length 数据存储长度
 * @return 写入成功的字节数
 */
uint32_t letk_rbuffer_write_bytes(letk_rbuffer_t* rb, const uint8_t* buf, uint32_t length)
{
    uint32_t i;
    uint32_t left;
    left = rb->size + rb->front - rb->rear;
    length = LETK_RBUFFER_GET_MIN(length, left);
    i = LETK_RBUFFER_GET_MIN(length, rb->size - (rb->rear & (rb->size - 1)));
    memcpy(rb->buf + (rb->rear & (rb->size - 1)), buf, i);
    memcpy(rb->buf, buf + i, length - i);
    rb->rear += length;
    return length;
}

/**
 * @brief 从环形缓冲区读取多个字节
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 * @param[out] buf 数据存储(必须非NULL)
 * @param[in] length 需要读取的字节数
 * @return 实际读取的字节数
 */
uint32_t letk_rbuffer_read_bytes(letk_rbuffer_t* rb, uint8_t* buf, uint32_t length)
{
    uint32_t i;
    uint32_t left;
    left = rb->rear - rb->front;
    length = LETK_RBUFFER_GET_MIN(length, left);
    i = LETK_RBUFFER_GET_MIN(length, rb->size - (rb->front & (rb->size - 1)));
    memcpy(buf, rb->buf + (rb->front & (rb->size - 1)), i);
    memcpy(buf + i, rb->buf, length - i);
    rb->front += length;
    return length;
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */
