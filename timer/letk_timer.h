/***********************************************************************************************************************
** 文件描述：定时器管理(软件定时器)头文件
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
void letk_timer_poll(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __LETK_TIMER_H__ */
