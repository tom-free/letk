/***********************************************************************************************************************
** 文件描述：滴答时钟管理头文件
** 创建作者：付瑞彪(Tom Free)
** 创建日期：2022年5月29日
** 编码格式：UTF-8编码
** 编程语言：C语言，C99标准
** 缩进格式：4个空格键
** 命名规范：下划线命名法(小写命名法)
** 开源许可：MIT许可证，参考：https://mit-license.org
** 版权信息：Copyright (c) 2013-2022, Tom Free, <tomfreefu@gmail.com>
**
** 修改记录
** 修改日期         修改作者        修改内容
** 2022年5月29日    付瑞彪          创建文件，初次版本
** 2022年7月15日    付瑞彪          修改代码注释规范
**
***********************************************************************************************************************/
#ifndef __LETK_TICKS_H__
#define __LETK_TICKS_H__

#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif  /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief 系统增加指定的ms数
 * @param[in] ms 一个tick流逝的ms数
 */
void letk_ticks_inc_ms(uint32_t ms);

/**
 * @brief 获取系统当前的ms数
 * @return 系统当前的ms数
 */
uint32_t letk_ticks_get_ms(void);

/**
 * @brief 获取相对于上一时刻流逝的ms数
 * @param[in] last_ms 上一时刻的ms值
 * @return 流逝的ms数
 */
uint32_t letk_ticks_elapsed_ms(uint32_t last_ms);

/**
 * @brief 判断相对于上一时刻是否超过间隔的ms数
 * @param[in] last_ms 上一时刻的ms值
 * @param[in] interval 间隔的ms数
 * @return 是否超时
 */
bool letk_ticks_is_timeout(uint32_t last_ms, uint32_t interval);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __LETK_TICKS_H__ */
