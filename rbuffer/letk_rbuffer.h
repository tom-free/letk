/***********************************************************************************************************************
** 文件描述：环形缓冲区头文件
** 创建作者：付瑞彪(Tom Free)
** 创建日期：2022年5月22日
** 编码格式：UTF-8编码
** 编程语言：C语言，C99标准
** 缩进格式：4个空格键
** 命名规范：下划线命名法(小写命名法)
** 开源许可：MIT许可证，参考：https://mit-license.org
** 版权信息：Copyright (c) 2013-2022, Tom Free, <tomfreefu@gmail.com>
**
** 修改记录
** 修改日期         修改作者        修改内容
** 2022年5月22日    付瑞彪          创建文件，初次版本
** 2022年7月15日    付瑞彪          修改代码注释规范
**
***********************************************************************************************************************/
#ifndef __LETK_RBUFFER_H__
#define __LETK_RBUFFER_H__

#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif  /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* 环形缓冲区管理器，用户不要去直接操作内部成员变量 */
typedef struct
{
     uint8_t* buf;      /* 环形缓冲区地址 */
     uint32_t size;     /* 环形缓冲区大小 */
     uint32_t front;    /* 头指针 */
     uint32_t rear;     /* 尾指针 */
} letk_rbuffer_t;

/**
 * @brief 初始化一个环形缓冲区
 * @param[in] rb 环形缓冲区管理器指针(必须非NULL）
 * @param[in] buf 数据缓冲区(必须非NULL)
 * @param[in] length buf长度(会向下裁剪到2的N次幂)
 */
void letk_rbuffer_init(letk_rbuffer_t* rb, uint8_t* buf, uint32_t length);

/**
 * @brief 清除环形缓冲区
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 */
void letk_rbuffer_clear(letk_rbuffer_t* rb);

/**
 * @brief 获取环形缓冲区当前的数据长度
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 * @return 当前的数据长度
 */
uint32_t letk_rbuffer_length(letk_rbuffer_t* rb);

/**
 * @brief 写入一字节到环形缓冲区
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 * @param[in] dat 数据字节
 * @return 是否写入成功
 */
bool letk_rbuffer_write_byte(letk_rbuffer_t* rb, uint8_t dat);

/**
 * @brief 从环形缓冲区读取一字节
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 * @param[out] pdat 数据字节指针(必须非NULL)
 * @return 是否读取成功
 */
bool letk_rbuffer_read_byte(letk_rbuffer_t* rb, uint8_t* pdat);

/**
 * @brief 写入多个字节到环形缓冲区
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 * @param[in] buf 数据存储(必须非NULL)
 * @param[in] length 数据存储长度
 * @return 写入成功的字节数
 */
uint32_t letk_rbuffer_write_bytes(letk_rbuffer_t* rb, const uint8_t* buf, uint32_t length);

/**
 * @brief 从环形缓冲区读取多个字节
 * @param[in] rb 缓冲区实例指针(必须非NULL)
 * @param[out] buf 数据存储(必须非NULL)
 * @param[in] length 需要读取的字节数
 * @return 实际读取的字节数
 */
uint32_t letk_rbuffer_read_bytes(letk_rbuffer_t* rb, uint8_t* buf, uint32_t length);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __LETK_RBUFFER_H__ */
