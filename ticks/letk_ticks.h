/*******************************************************************************
** 文件名称：letk_ticks.h
** 文件作用：滴答时钟模块
** 编写作者：Tom Free 付瑞彪
** 编写时间：2022-05-02
** 文件备注：
** 更新记录：
**           2022-05-02 -> 创建文件
**                                                            <Tom Free 付瑞彪>
**
**           Copyright (c) 2018-2022 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/
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
