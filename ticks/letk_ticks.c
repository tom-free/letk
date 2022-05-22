/*******************************************************************************
** 文件名称：letk_ticks.c
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

#include "letk_ticks.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* 系统运行总的ms数 */
static uint32_t sys_all_ms = 0;
/* 滴答中断标志，因为系统时钟滴答是多字节（32位）的，为了保证原子操作，
 * 防止8位或16位CPU下，系统时钟滴答读取操作时被中断出现字节撕裂现象
 * 此处必须是一个byte类型，防止这个标志也因为是多字节被撕裂，
 * 且必须加volatile关键字，用于每次都进行内存操作而非使用寄存器中的值 */
static volatile uint8_t tick_irq_flag;

/**
 * @brief 系统增加指定的ms数
 * @param[in] ms 一个tick流逝的ms数
 */
void letk_ticks_inc_ms(uint32_t ms)
{
    tick_irq_flag = 0;
    sys_all_ms += ms;
}

/**
 * @brief 获取系统当前的ms数
 * @return 系统当前的ms数
 */
uint32_t letk_ticks_get_ms(void)
{
    uint32_t result;

    do
    {
        tick_irq_flag = 1;
        result = sys_all_ms;
    } while (!tick_irq_flag);

    return result;
}

/**
 * @brief 获取相对于上一时刻流逝的ms数
 * @param[in] last_ms 上一时刻的ms值
 * @return 流逝的ms数
 */
uint32_t letk_ticks_elapsed_ms(uint32_t last_ms)
{
    return (letk_ticks_get_ms() - last_ms);
}

/**
 * @brief 判断相对于上一时刻是否超过间隔的ms数
 * @param[in] last_ms 上一时刻的ms值
 * @param[in] interval 间隔的ms数
 * @return 是否超时
 */
bool letk_ticks_is_timeout(uint32_t last_ms, uint32_t interval)
{
    return (letk_ticks_get_ms() - last_ms >= interval);
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */
