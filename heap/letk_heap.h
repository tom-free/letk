/***********************************************************************************************************************
** 文件描述：内存堆管理(动态内存管理)头文件
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
#ifndef __LETK_HEAP_H__
#define __LETK_HEAP_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief   初始化内存堆
 * @return  初始化结果
 */
bool letk_heap_init(void);

/**
 * @brief   内存申请
 * @param   size 内存大小
 * @return  内存地址
 */
void* letk_heap_alloc(size_t size);

/**
 * @brief   内存释放
 * @param   ptr 内存指针
 */
void letk_heap_free(void* const ptr);

#endif  /* __LETK_HEAP_H__ */
