/*******************************************************************************
** @file   letk_rbuffer.h
** @brief  环形缓冲区
** @author 付瑞彪
** @date   2022-05-22
**
**       Copyright (c) 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/
#ifndef __LETK_RBUFFER_H__
#define __LETK_RBUFFER_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* 尺寸类型定义 */
typedef uint32_t letk_rbuffer_size_t;

/* 环形缓冲区管理器，用户不要去直接操作内部成员变量 */
typedef struct
{
     uint8_t*            buf;       /* 环形缓冲区地址 */
     letk_rbuffer_size_t size;      /* 环形缓冲区大小 */
     letk_rbuffer_size_t front;     /* 头指针 */
     letk_rbuffer_size_t rear;      /* 尾指针 */
} letk_rbuffer_t;

/**
 * @brief 初始化环形缓冲区
 * @param[in] rb 缓冲区实例指针
 * @param[in] buf 缓冲区存储区
 * @param[in] rb 缓冲区存储长度
 */
bool letk_rbuffer_init(letk_rbuffer_t* rb, uint8_t* buf, letk_rbuffer_size_t length);

/**
 * @brief 清除环形缓冲区
 * @param[in] rb 缓冲区实例指针
 */
void letk_rbuffer_clear(letk_rbuffer_t* rb);

/**
 * @brief 获取环形缓冲区当前的数据长度
 * @param[in] rb 缓冲区实例指针
 * @return 当前的数据长度
 */
letk_rbuffer_size_t letk_rbuffer_length(letk_rbuffer_t* rb);

/**
 * @brief 写入一字节到环形缓冲区
 * @param[in] rb 缓冲区实例指针
 * @param[in] dat 数据字节
 * @return 是否写入成功
 */
bool letk_rbuffer_write_byte(letk_rbuffer_t* rb, uint8_t dat);

/**
 * @brief 从环形缓冲区读取一字节
 * @param[in] rb 缓冲区实例指针
 * @param[out] pdat 数据字节指针
 * @return 是否读取成功
 */
bool letk_rbuffer_read_byte(letk_rbuffer_t* rb, uint8_t* pdat);

/**
 * @brief 写入多个字节到环形缓冲区
 * @param[in] rb 缓冲区实例指针
 * @param[in] buf 数据存储
 * @param[in] length 数据存储长度
 * @return 写入成功的字节数
 */
letk_rbuffer_size_t letk_rbuffer_write_bytes(letk_rbuffer_t* rb, uint8_t* buf, letk_rbuffer_size_t length);

/**
 * @brief 从环形缓冲区读取多个字节
 * @param[in] rb 缓冲区实例指针
 * @param[out] buf 数据存储
 * @param[in] length 需要读取的字节数
 * @return 实际读取的字节数
 */
letk_rbuffer_size_t letk_rbuffer_read_bytes(letk_rbuffer_t* rb, uint8_t* buf, letk_rbuffer_size_t length);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __LETK_RBUFFER_H__ */
