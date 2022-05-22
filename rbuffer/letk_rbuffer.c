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
 * @brief 初始化一个环形缓冲区
 * @param[in] rb 环形缓冲区管理器指针(必须非NULL）
 * @param[in] buf 数据缓冲区(必须非NULL，否则会初始化失败)
 * @param[in] length buf长度(必须是2的N次幂，否则会初始化失败)
 * @return 是否初始化成功
 */
bool letk_rbuffer_init(letk_rbuffer_t* rb, uint8_t* buf, letk_rbuffer_size_t length)
{
    rb->buf = buf;
    rb->size = length;
    rb->front = rb->rear = 0;
    return ((buf != NULL) && ((length & (length - 1)) == 0));
}

/**
 * @brief   清空环形缓冲区 
 * @param   rb - 待清空的环形缓冲区管理器(必须非NULL，否则挂批)
 * @return  无
 */
void letk_rbuffer_clear(letk_rbuffer_t* rb)
{
    rb->front = rb->rear = 0;
}

/**
 * @brief   获取环形缓冲区数据长度
 * @param   rb - 环形缓冲区管理器(必须非NULL，否则挂批)
 * @return  环形缓冲区中有效字节数
 */
letk_rbuffer_size_t letk_rbuffer_length(letk_rbuffer_t* rb)
{
    return (letk_rbuffer_size_t)(rb->rear - rb->front);
}

/**
 * @brief   将一个字节数据放到环形缓冲区中
 * @param   rb - 环形缓冲区管理器(必须非NULL，否则挂批)
 * @param   dat - 待写入的一个字节数据
 * @return  是否写入成功
 */
bool letk_rbuffer_write_byte(letk_rbuffer_t* rb, uint8_t dat)
{
    letk_rbuffer_size_t left;
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
 * @brief   从环形缓冲区中读取一个字节数据
 * @param   rb - 环形缓冲区管理器(必须非NULL，否则挂批)
 * @param   pdat - 待读取的一个字节数据存储地址(必须非NULL，否则挂批)
 * @return  是否读取成功
 */
bool letk_rbuffer_read_byte(letk_rbuffer_t* rb, uint8_t* pdat)
{
    letk_rbuffer_size_t left;
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
 * @brief   将指定长度的数据放到环形缓冲区中
 * @param   rb - 环形缓冲区管理器(必须非NULL，否则挂批)
 * @param   buf - 数据缓冲区(必须非NULL，否则挂批)
 * @param   length - 缓冲区长度
 * @return  实际写入的数据长度
 */
letk_rbuffer_size_t letk_rbuffer_write_bytes(letk_rbuffer_t* rb, uint8_t* buf, letk_rbuffer_size_t length)
{
    letk_rbuffer_size_t i;
    letk_rbuffer_size_t left;
    left = rb->size + rb->front - rb->rear;
    length = LETK_RBUFFER_GET_MIN(length, left);
    i = LETK_RBUFFER_GET_MIN(length, rb->size - (rb->rear & (rb->size - 1)));
    memcpy(rb->buf + (rb->rear & (rb->size - 1)), buf, i);
    memcpy(rb->buf, buf + i, length - i);
    rb->rear += length;
    return length;
}

/**
 * @brief   从环形缓冲区中读取指定长度的数据
 * @param   rb - 环形缓冲区管理器(必须非NULL，否则挂批)
 * @param   buf - 数据缓冲区(必须非NULL，否则挂批)
 * @param   length - 缓冲区长度
 * @return  实际读取的数据长度
 */
letk_rbuffer_size_t letk_rbuffer_read_bytes(letk_rbuffer_t* rb, uint8_t* buf, letk_rbuffer_size_t length)
{
    letk_rbuffer_size_t i;
    letk_rbuffer_size_t left;
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
