/*******************************************************************************
** @file   letk_ibutton.c
** @brief  独立按键管理
** @author 付瑞彪
** @date   2022-05-22
**
**       Copyright (c) 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/

#include "letk_ibutton.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* 轮询检测间隔时间，单位：ms，一般5-20ms为宜，
 * 太长反应迟钝，太快占用CPU时间 */
#ifndef LETK_IBUTTON_POLL_INTERVAL
#define LETK_IBUTTON_POLL_INTERVAL      (5u)
#endif  /* LETK_IBUTTON_POLL_INTERVAL */

/* 防抖动滴答数量，时间等于上门的数乘以数量，一般在10-20ms为宜，
 * 太少消抖不完全，太长反应迟钝 */
#ifndef LETK_IBUTTON_DEBOUNCE_TICKS
#define LETK_IBUTTON_DEBOUNCE_TICKS     (3u)
#endif  /* LETK_IBUTTON_DEBOUNCE_TICKS */

/* 连击最长间隔时间，超过此时间认为连击被终结，换算成滴答数 */
#ifndef LETK_IBUTTON_HIT_AGIN_TICKS
#define LETK_IBUTTON_HIT_AGIN_TICKS     (300u / LETK_IBUTTON_POLL_INTERVAL)
#endif  /* LETK_IBUTTON_HIT_AGIN_TICKS */

/* 长按触发时间，超过此时间认为进入长按，换算成滴答数 */
#ifndef LETK_IBUTTON_LONG_TICKS
#define LETK_IBUTTON_LONG_TICKS         (1000u / LETK_IBUTTON_POLL_INTERVAL)
#endif  /* LETK_IBUTTON_LONG_TICKS */

/* 长按重复触发时间，即隔多长时间触发一次，换算成滴答数 */
#ifndef LETK_IBUTTON_REPEAT_TRIG_TICKS
#define LETK_IBUTTON_REPEAT_TRIG_TICKS  (100u / LETK_IBUTTON_POLL_INTERVAL)
#endif  /* LETK_IBUTTON_REPEAT_TRIG_TICKS */

/* 连击最大次数，主要是怕数据溢出 */
#ifndef LETK_IBUTTON_CLICK_COUNT_MAX
#define LETK_IBUTTON_CLICK_COUNT_MAX    UINT8_MAX
#endif  /* LETK_IBUTTON_CLICK_COUNT_MAX */

/* 按键管理器 */
typedef struct
{
    /* 按键链表头节点 */
    letk_ibutton_t *p_list_head;
    /* 链表长度，仅用于统计节点个数 */
    uint16_t        list_length;
} letk_ibutton_mgr_t;

/* 执行事件回调 */
#define LETK_IBUTTON_EVENT_PROC(pbtn)           \
    do                                          \
    {                                           \
        if (pbtn->event_cb != NULL)             \
        {                                       \
            pbtn->event_cb(pbtn);               \
        }                                       \
    } while (0)

/* 管理器参数 */
static letk_ibutton_mgr_t ibutton_mgr;

/* 按键管理器初始化 */
void letk_ibutton_mgr_init(void)
{
    /* 初始化头指针为空 */
    ibutton_mgr.p_list_head = NULL;
    /* 初始化长度为0 */
    ibutton_mgr.list_length = 0;
}

/* 按键对象初始化 */
letk_ibutton_result_t letk_ibutton_init(letk_ibutton_t* pbtn,
                                        letk_ibutton_read_io_cb_t* read_io_cb,
                                        letk_ibutton_event_cb_t* event_cb)
{
    if ((pbtn == NULL) ||
        (read_io_cb == NULL) ||
        (event_cb == NULL))
    {
        return LETK_IBUTTON_RESULT_ARGS_ERR;
    }

    /* 初始化参数 */
    pbtn->next = NULL;
    pbtn->read_io_cb = read_io_cb;
    pbtn->event_cb = event_cb;
    pbtn->click_cnt_max = LETK_IBUTTON_CLICK_COUNT_MAX;
    pbtn->debounce_ticks = LETK_IBUTTON_DEBOUNCE_TICKS;
    pbtn->hit_again_ticks = LETK_IBUTTON_HIT_AGIN_TICKS;
    pbtn->long_press_ticks = LETK_IBUTTON_LONG_TICKS;
    pbtn->long_repeat_ticks = LETK_IBUTTON_REPEAT_TRIG_TICKS;
    pbtn->internal_cnt = 0;
    pbtn->debounce_cnt = 0;
    pbtn->click_cnt = 0;
    pbtn->fsm_status = LETK_IBUTTON_FSM_STATUS_RELEASE;
    pbtn->event = LETK_IBUTTON_EVENT_NONE;

    /* 读取一次电平并保存 */
    pbtn->level = pbtn->read_io_cb(pbtn);

    return LETK_IBUTTON_RESULT_OK;
}

/* 设置读取IO回调 */
letk_ibutton_result_t letk_ibutton_set_read_io_cb(letk_ibutton_t* pbtn, letk_ibutton_read_io_cb_t* read_io_cb)
{
    if ((pbtn == NULL) ||
        (read_io_cb == NULL))
    {
        return LETK_IBUTTON_RESULT_ARGS_ERR;
    }

    pbtn->read_io_cb = read_io_cb;

    return LETK_IBUTTON_RESULT_OK;
}

/* 设置事件处理回调 */
letk_ibutton_result_t letk_ibutton_set_event_cb(letk_ibutton_t* pbtn, letk_ibutton_event_cb_t* event_cb)
{
    if ((pbtn == NULL) ||
        (event_cb == NULL))
    {
        return LETK_IBUTTON_RESULT_ARGS_ERR;
    }

    pbtn->event_cb = event_cb;

    return LETK_IBUTTON_RESULT_OK;
}

/* 设置连击最大次数 */
letk_ibutton_result_t letk_ibutton_set_click_cnt_max(letk_ibutton_t* pbtn, uint8_t click_cnt_max)
{
    if ((pbtn == NULL) ||
        (click_cnt_max == 0))
    {
        return LETK_IBUTTON_RESULT_ARGS_ERR;
    }

    pbtn->click_cnt_max = click_cnt_max;

    return LETK_IBUTTON_RESULT_OK;
}

/* 设置防抖周期 */
letk_ibutton_result_t letk_ibutton_set_debounce_ticks(letk_ibutton_t* pbtn, uint8_t debounce_ticks)
{
    if ((pbtn == NULL) ||
        (debounce_ticks == 0))
    {
        return LETK_IBUTTON_RESULT_ARGS_ERR;
    }

    pbtn->debounce_ticks = debounce_ticks;

    return LETK_IBUTTON_RESULT_OK;
}

/* 设置连击最大间隔时间周期 */
letk_ibutton_result_t letk_ibutton_set_hit_again_ticks(letk_ibutton_t* pbtn, uint8_t hit_again_ticks)
{
    if ((pbtn == NULL) ||
        (hit_again_ticks == 0))
    {
        return LETK_IBUTTON_RESULT_ARGS_ERR;
    }

    pbtn->hit_again_ticks = hit_again_ticks;

    return LETK_IBUTTON_RESULT_OK;
}

/* 设置长按触发时间周期 */
letk_ibutton_result_t letk_ibutton_set_long_press_ticks(letk_ibutton_t* pbtn, uint8_t long_press_ticks)
{
    if ((pbtn == NULL) ||
        (long_press_ticks == 0))
    {
        return LETK_IBUTTON_RESULT_ARGS_ERR;
    }

    pbtn->long_press_ticks = long_press_ticks;

    return LETK_IBUTTON_RESULT_OK;
}

/* 设置长按重复按下事件触发时间周期 */
letk_ibutton_result_t letk_ibutton_set_long_repeat_ticks(letk_ibutton_t* pbtn, uint8_t long_repeat_ticks)
{
    if ((pbtn == NULL) ||
        (long_repeat_ticks == 0))
    {
        return LETK_IBUTTON_RESULT_ARGS_ERR;
    }

    pbtn->long_repeat_ticks = long_repeat_ticks;

    return LETK_IBUTTON_RESULT_OK;
}

/* 添加按键对象到按键链表中 */
letk_ibutton_result_t letk_ibutton_add(letk_ibutton_t* pbtn)
{
    letk_ibutton_t* target;

    if (pbtn == NULL)
    {
        /* 指针非法，返回参数错误 */
        return LETK_IBUTTON_RESULT_ARGS_ERR;
    }

    /* 搜寻是否已存在于链表中 */
    target = ibutton_mgr.p_list_head;
    while (target != NULL)
    {
        if (target == pbtn)
        {
            /* 已存在 */
            return LETK_IBUTTON_RESULT_EXIST;
        }
        target = target->next;
    }
    /* 未搜索到，添加到头部 */
    pbtn->next = ibutton_mgr.p_list_head;
    /* 更新头节点 */
    ibutton_mgr.p_list_head = pbtn;
    /* 链表数量加1 */
    ibutton_mgr.list_length++;

    return LETK_IBUTTON_RESULT_OK;
}

/* 从按键链表中移除按键对象 */
letk_ibutton_result_t letk_ibutton_remove(letk_ibutton_t* pbtn)
{
    letk_ibutton_t* ptemp;
    letk_ibutton_t* prev;

    if (pbtn == NULL)
    {
        /* 指针非法 */
        return LETK_IBUTTON_RESULT_ARGS_ERR;
    }

    if ((ibutton_mgr.list_length == 0) ||
        (ibutton_mgr.p_list_head == NULL))
    {
        /* 链表空，未搜索到 */
        return LETK_IBUTTON_RESULT_NOT_FOUND;
    }

    if (ibutton_mgr.p_list_head == pbtn)
    {
        /* 如果需要删除的节点位于链表头，需要更新链表头指针 */
        ibutton_mgr.p_list_head = ibutton_mgr.p_list_head->next;
        /* 保证删除节点的指针安全 */
        pbtn->next = NULL;

        return LETK_IBUTTON_RESULT_OK;
    }

    /* 从第二个开始搜索 */
    prev = ibutton_mgr.p_list_head;
    ptemp = ibutton_mgr.p_list_head->next;
    for (; ptemp != NULL; ptemp = ptemp->next)
    {
        if (ptemp == pbtn)
        {
            /* 搜索到，删除节点 */
            prev->next = ptemp->next;
            /* 保证删除节点的指针安全 */
            ptemp->next = NULL;

            return LETK_IBUTTON_RESULT_OK;
        }
        prev = ptemp;
    }

    /* 未搜索到 */
    return LETK_IBUTTON_RESULT_NOT_FOUND;
}

/* 轮询一个按键 */
static void letk_ibutton_poll_one_button(letk_ibutton_t* pbtn)
{
    /* 读取电平 */
    letk_ibutton_level_t btn_level = pbtn->read_io_cb(pbtn);

    /* 消抖 */
    if (btn_level != pbtn->level)
    {
        if (++(pbtn->debounce_cnt) >= pbtn->debounce_ticks)
        {
            pbtn->level = btn_level;
            pbtn->debounce_cnt = 0;
        }
    }
    else
    {
        pbtn->debounce_cnt = 0;
    }

    /* 默认按键无事件 */
    pbtn->event = LETK_IBUTTON_EVENT_NONE;

    /* 状态机 */
    switch (pbtn->fsm_status)
    {
    case LETK_IBUTTON_FSM_STATUS_RELEASE:
        if (pbtn->level == LETK_IBUTTON_LEVEL_PRESS)
        {
            /* 按键按下事件 */
            pbtn->event = LETK_IBUTTON_EVENT_PRESS;
            /* 清除计数器 */
            pbtn->internal_cnt = 0;
            /* 清除连击计数 */
            pbtn->click_cnt = 1;
            /* 切换到按键按下状态 */
            pbtn->fsm_status = LETK_IBUTTON_FSM_STATUS_PRESS;
        }
        break;

    case LETK_IBUTTON_FSM_STATUS_PRESS:
        if (pbtn->level != LETK_IBUTTON_LEVEL_PRESS)
        {
            /* 未达到长按时间之前抬起，按键松开事件 */
            pbtn->event = LETK_IBUTTON_EVENT_RELEASE;
            /* 清除计数器 */
            pbtn->internal_cnt = 0;
            /* 切换到等待连击状态 */
            pbtn->fsm_status = LETK_IBUTTON_FSM_STATUS_WAIT_PRESS_AGIN;
        }
        else if (++(pbtn->internal_cnt) > pbtn->long_press_ticks)
        {
            /* 超过长按时间还未松开，首次触发长按事件 */
            pbtn->event = LETK_IBUTTON_EVENT_LONG_FIRST;
            /* 切换到等待按键松开状态 */
            pbtn->fsm_status = LETK_IBUTTON_FSM_STATUS_WAIT_RELEASE;
        }
        break;

    case LETK_IBUTTON_FSM_STATUS_WAIT_PRESS_AGIN:
        if (pbtn->level == LETK_IBUTTON_LEVEL_PRESS)
        {
            /* 按键按下事件 */
            pbtn->event = LETK_IBUTTON_EVENT_PRESS;
            /* 连击计数加1 */
            if (pbtn->click_cnt < pbtn->click_cnt_max)
            {
                pbtn->click_cnt++;
            }
            /* 清除计数器 */
            pbtn->internal_cnt = 0;
            /* 切换到按键再次按下状态 */
            pbtn->fsm_status = LETK_IBUTTON_FSM_STATUS_PRESS_AGIN;
        }
        else if (++(pbtn->internal_cnt) > pbtn->hit_again_ticks)
        {
            /* 超过连击间隔时间还未再次按下按键 */
            if (pbtn->click_cnt == 1)
            {
                /* 仅点击了一次 */
                pbtn->event = LETK_IBUTTON_EVENT_SINGLE_CLICK;
            }
            else if (pbtn->click_cnt == 2)
            {
                /* 双击 */
                pbtn->event = LETK_IBUTTON_EVENT_DOUBLE_CLICK;
            }
            else
            {
                /* 多次连击 */
                pbtn->event = LETK_IBUTTON_EVENT_MULTI_CLICK;
            }
            /* 切换到按键抬起状态，此次按键流程结束 */
            pbtn->fsm_status = LETK_IBUTTON_FSM_STATUS_RELEASE;
        }
        break;

    case LETK_IBUTTON_FSM_STATUS_PRESS_AGIN:
        if (pbtn->level != LETK_IBUTTON_LEVEL_PRESS)
        {
            /* 按键松开事件 */
            pbtn->event = LETK_IBUTTON_EVENT_RELEASE;
            /* 清除计数器 */
            pbtn->internal_cnt = 0;
            /* 切换到等待按键再次按下状态 */
            pbtn->fsm_status = LETK_IBUTTON_FSM_STATUS_WAIT_PRESS_AGIN;
        }
        break;

    case LETK_IBUTTON_FSM_STATUS_WAIT_RELEASE:
        if (pbtn->level == LETK_IBUTTON_LEVEL_PRESS)
        {
            if (++(pbtn->internal_cnt) > pbtn->long_repeat_ticks)
            {
                /* 清除计数 */
                pbtn->internal_cnt = 0;
                /* 继续按下，按键重复按下事件 */
                pbtn->event = LETK_IBUTTON_EVENT_REPEAT;
            }
        }
        else
        {
            /* 按键松开，按键抬起事件 */
            pbtn->event = LETK_IBUTTON_EVENT_RELEASE;
            /* 清除计数器 */
            pbtn->internal_cnt = 0;
            /* 清除连击计数 */
            pbtn->click_cnt = 0;
            /* 回到释放状态 */
            pbtn->fsm_status = LETK_IBUTTON_FSM_STATUS_RELEASE;
        }
        break;

    default:
        /* 异常，回到释放状态 */
        pbtn->internal_cnt = 0;
        pbtn->click_cnt = 0;
        pbtn->fsm_status = LETK_IBUTTON_FSM_STATUS_RELEASE;
        break;
    }

    /* 有事件产生，执行回调 */
    if (pbtn->event != LETK_IBUTTON_EVENT_NONE)
    {
        LETK_IBUTTON_EVENT_PROC(pbtn);
    }
}

/* 轮询所有按键 */
void letk_ibutton_poll(void)
{
    letk_ibutton_t* pbtn;

    for (pbtn = ibutton_mgr.p_list_head; pbtn != NULL; pbtn = pbtn->next)
    {
        letk_ibutton_poll_one_button(pbtn);
    }
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */
