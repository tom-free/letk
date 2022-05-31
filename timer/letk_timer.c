/*******************************************************************************
** @file   letk_timer.c
** @brief  定时器模块
** @author 付瑞彪
** @date   2022-05-29
**
**       Copyright (c) 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/

#include "letk_timer.h"
#include "letk_ticks.h"
#include <stddef.h>

#ifdef __cplusplus
extern 'C' {
#endif  /* __cplusplus */

/* 定时器链表头指针 */
static letk_timer_t* p_timer_head = NULL;
/* 标识运行回调期间链表是否发生变化 */
static bool list_changed = false;

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
                     void* user_data)
{
    if (ptimer != NULL)
    {
        ptimer->next = NULL;
        ptimer->repeat = repeat;
        ptimer->interval = interval;
        ptimer->status = 0;
        ptimer->cb = cb;
        ptimer->user_data = user_data;
    }
}

/**
 * @brief 添加定时器到链表
 * @param[in] ptimer 待添加的定时器指针
 */
void letk_timer_add(letk_timer_t* ptimer)
{
    const letk_timer_t* pt = p_timer_head;

    if (ptimer == NULL)
    {
        return;
    }

    /* 搜索是否已经存在于链表中 */
    while (pt != NULL)
    {
        if (pt == ptimer)
        {
            /* 已存在 */
            return;
        }
        pt = pt->next;
    }

    /* 添加到链表头部 */
    ptimer->next = p_timer_head;
    p_timer_head = ptimer;

    list_changed = true;
}

/**
 * @brief 从链表移除定时器
 * @param[in] ptimer 待移除的定时器指针
 */
void letk_timer_remove(letk_timer_t* ptimer)
{
    letk_timer_t* pt;

    if ((ptimer == NULL) || (p_timer_head == NULL))
    {
        return;
    }

    /* 判断是否是链表头节点 */
    if (p_timer_head == ptimer)
    {
        /* 头直接指向下一个 */
        p_timer_head = p_timer_head->next;
        /* 释放指针，防止别人摆弄 */
        ptimer->next = NULL;

        list_changed = true;
        return;
    }

    /* 搜索链表中timer位置的前一个 */
    pt = p_timer_head;
    while (pt != NULL)
    {
        if (pt->next == ptimer)
        {
            /* 断链 */
            pt->next = ptimer->next;
            /* 释放指针，防止别人摆弄 */
            ptimer->next = NULL;

            list_changed = true;
            return;
        }
        pt = pt->next;
    }
}

/**
 * @brief 移除全部的定时器
 */
void letk_timer_remove_all(void)
{
    letk_timer_t *ptemp = p_timer_head;
    letk_timer_t *pnext = ptemp;

    while (ptemp != NULL)
    {
        /* 备份下一个 */
        pnext = ptemp->next;
        /* 释放指针，防止别人摆弄 */
        ptemp->next = NULL;
        /* 移到下一个 */
        ptemp = pnext;
    }

    /* 头节点为空 */
    p_timer_head = NULL;

    list_changed = true;
}

/**
 * @brief 启动定时器
 * @param[in] ptimer 定时器指针
 */
void letk_timer_start(letk_timer_t* ptimer)
{
    if (ptimer != NULL)
    {
        /* 切换为运行态 */
        ptimer->status = 1;
        /* 保存当前ticks */
        ptimer->last = letk_ticks_get_ms();
    }
}

/**
 * @brief 停止定时器
 * @param[in] ptimer 定时器指针
 */
void letk_timer_stop(letk_timer_t* ptimer)
{
    if (ptimer != NULL)
    {
        /* 切换为挂起态 */
        ptimer->status = 0;
    }
}

/**
 * @brief 定时器事件轮询处理
 */
void letk_timer_poll(void)
{
    letk_timer_t* pt;
    do
    {
        list_changed = false;
        pt = p_timer_head;
        while (pt != NULL)
        {
            /* 备份下一个节点 */
            if (pt->status && pt->repeat &&
                letk_ticks_is_timeout(pt->last, pt->interval))
            {
                /* 进行倒计数 */
                if (pt->repeat > 0)
                {
                    --pt->repeat;
                }
                /* 重置起始时间 */
                pt->last = pt->last + pt->interval;
                /* 运行回调 */
                if (pt->cb != NULL)
                {
                    /* 如果内部调用了add和remove等函数会导致循环异常 */
                    pt->cb(pt);
                    /* 链表已发生变化，遍历路径已经被破坏了 */
                    if (list_changed)
                    {
                        break;
                    }
                }
            }
            /* 下一个 */
            pt = pt->next;
        }
    } while (pt != NULL);
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */
