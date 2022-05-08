/*******************************************************************************
** 文件名称：letk_timer.h
** 文件作用：定时器模块
** 编写作者：Tom Free 付瑞彪
** 编写时间：2022-05-04
** 文件备注：
** 更新记录：
**           2022-05-04 -> 创建文件
**                                                            <Tom Free 付瑞彪>
**
**           Copyright (c) 2018-2022 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/
#ifndef __LETK_TIMER_H__
#define __LETK_TIMER_H__

#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif  /* __cplusplus */

#ifdef __cplusplus
extern 'C' {
#endif  /* __cplusplus */

/* 定时器类型定义 */
typedef struct _letk_timer_t letk_timer_t;

/* 定时器回调函数原型定义 */
typedef void letk_timer_cb_t(letk_timer_t*);

/* 定时器结构 */
struct _letk_timer_t
{
    letk_timer_t* next;         /* 下一个节点指针，不要随意摆弄*/
    letk_timer_cb_t* cb;        /* 回调函数 */
    void* user_data;            /* 用户数据指针 */
    int32_t repeat;             /* >0：重复次数，0：运行结束，<0：无限循环 */
    uint32_t interval;          /* 间隔周期 */
    uint32_t last;              /* 上一次运行时间 */
    uint8_t status;             /* 定时器状态，0：停止，1：启动 */
};

/**
 * @brief 初始化定时器参数
 * @param[in] ptimer 定时器指针
 * @param[in] repeat >0：运行次数，0：运行结束，<0：无限循环
 * @param[in] interval 空闲间隔时间
 * @param[in] cb 回调函数
 * @param[in] user_data 用户数据指针
 */
void letk_timer_init(letk_timer_t* ptimer,
                     int32_t repeat,
                     uint32_t interval,
                     letk_timer_cb_t *cb,
                     void* user_data);

/**
 * @brief 添加定时器到链表
 * @param[in] ptimer 待添加的定时器指针
 */
void letk_timer_add(letk_timer_t* ptimer);

/**
 * @brief 从链表移除定时器
 * @param[in] ptimer 待移除的定时器指针
 */
void letk_timer_remove(letk_timer_t* ptimer);

/**
 * @brief 移除全部的定时器
 */
void letk_timer_remove_all(void);

/**
 * @brief 启动定时器
 * @param[in] ptimer 定时器指针
 */
void letk_timer_start(letk_timer_t* ptimer);

/**
 * @brief 停止定时器
 * @param[in] ptimer 定时器指针
 */
void letk_timer_stop(letk_timer_t* ptimer);

/**
 * @brief 定时器事件轮询处理
 */
void letk_timer_poll_event(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __LETK_TIMER_H__ */
