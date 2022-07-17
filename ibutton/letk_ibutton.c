/***********************************************************************************************************************
** 文件描述：独立按键管理源文件
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

#include "letk_ibutton.h"
#include "../ainit/letk_ainit.h"
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

/* 按键链表头节点 */
static letk_ibutton_t* p_ibutton_head = NULL;

/**
 * @brief 按键管理器参数初始化
 * @note 用于休眠时内存断电的场合，唤醒后需要调用此函数重新初始化内存数据
 *       此函数必须在按键设置相关的函数之前调用，不能在之后或中间调用
 */
static void letk_ibutton_mgr_init(void)
{
    /* 头指针为空 */
    p_ibutton_head = NULL;
}
LETK_SOFT_INIT_EXPORT(letk_ibutton_mgr_init);

/**
 * @brief 按键对象初始化
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] read_cb 读取IO电平回调函数(不能为NULL)
 * @param[in] event_cb 事件处理回调函数
 * @note 在此函数调用之前需要初始化此按键相关的IO外设
 *       此函数会读取一次电平存放起来，作为初始电平
 *       按键参数使用配置文件里的默认值，需要修改请在此函数后调用相关函数设置
 */
void letk_ibutton_init(letk_ibutton_t* pbtn,
                       letk_ibutton_read_cb_t* read_cb,
                       letk_ibutton_event_cb_t* event_cb)
{
    if ((pbtn == NULL) || (read_cb == NULL))
    {
        return;
    }

    /* 初始化参数 */
    pbtn->next = NULL;
    pbtn->read_cb = read_cb;
    pbtn->event_cb = event_cb;
    pbtn->click_cnt_max = LETK_IBUTTON_CLICK_COUNT_MAX;
    pbtn->debounce_ticks = LETK_IBUTTON_DEBOUNCE_TICKS;
    pbtn->hit_again_ticks = LETK_IBUTTON_HIT_AGIN_TICKS;
    pbtn->long_press_ticks = LETK_IBUTTON_LONG_TICKS;
    pbtn->long_repeat_ticks = LETK_IBUTTON_REPEAT_TRIG_TICKS;
    pbtn->internal_cnt = 0;
    pbtn->debounce_cnt = 0;
    pbtn->click_cnt = 0;
    pbtn->status = LETK_IBUTTON_STATUS_RELEASED;
    pbtn->event = LETK_IBUTTON_EVENT_NONE;

    /* 读取一次IO状态并保存 */
    pbtn->io_status = pbtn->read_cb(pbtn);
}

/**
 * @brief 设置读取IO回调函数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] read_cb 读取IO回调函数(不能为NULL)
 */
void letk_ibutton_set_read_io_cb(letk_ibutton_t* pbtn,
                                 letk_ibutton_read_cb_t* read_cb)
{
    if ((pbtn == NULL) || (read_cb == NULL))
    {
        return;
    }

    pbtn->read_cb = read_cb;
}

/**
 * @brief 设置事件处理回调函数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] event_cb 事件处理回调函数
 */
void letk_ibutton_set_event_cb(letk_ibutton_t* pbtn,
                               letk_ibutton_event_cb_t* event_cb)
{
    if (pbtn == NULL)
    {
        return;
    }

    pbtn->event_cb = event_cb;
}

/**
 * @brief 设置连击最大次数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] click_cnt_max 连击最大次数，范围[1-255]
 */
void letk_ibutton_set_click_cnt_max(letk_ibutton_t* pbtn,
                                    uint8_t click_cnt_max)
{
    if ((pbtn == NULL) || (click_cnt_max == 0))
    {
        return;
    }

    pbtn->click_cnt_max = click_cnt_max;
}

/**
 * @brief 设置防抖检测次数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] debounce_ticks 防抖检测次数，范围[1-255]
 */
void letk_ibutton_set_debounce_ticks(letk_ibutton_t* pbtn,
                                     uint8_t debounce_ticks)
{
    if ((pbtn == NULL) || (debounce_ticks == 0))
    {
        return;
    }

    pbtn->debounce_ticks = debounce_ticks;
}

/**
 * @brief 设置连击最大间隔时间周期次数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] hit_again_ticks - 连击最大间隔时间周期次数，范围[1-255]
 */
void letk_ibutton_set_hit_again_ticks(letk_ibutton_t* pbtn,
                                      uint8_t hit_again_ticks)
{
    if ((pbtn == NULL) || (hit_again_ticks == 0))
    {
        return;
    }

    pbtn->hit_again_ticks = hit_again_ticks;
}

/**
 * @brief 设置长按触发时间周期次数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] long_press_ticks - 长按触发时间周期次数，范围[1-255]
 */
void letk_ibutton_set_long_press_ticks(letk_ibutton_t* pbtn,
                                       uint8_t long_press_ticks)
{
    if ((pbtn == NULL) || (long_press_ticks == 0))
    {
        return;
    }

    pbtn->long_press_ticks = long_press_ticks;
}

/**
 * @brief 设置长按重复按下事件触发时间周期次数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] long_repeat_ticks 长按重复按下事件触发时间周期次数，范围[1-255]
 */
void letk_ibutton_set_long_repeat_ticks(letk_ibutton_t* pbtn,
                                        uint8_t long_repeat_ticks)
{
    if ((pbtn == NULL) || (long_repeat_ticks == 0))
    {
        return;
    }

    pbtn->long_repeat_ticks = long_repeat_ticks;
}

/**
 * @brief 添加按键对象到按键链表中
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @note 如果按键已在链表中不进行添加操作
 *       不要在事件处理回调函数中使用
 */
void letk_ibutton_add(letk_ibutton_t* pbtn)
{
    letk_ibutton_t* target;

    if (pbtn == NULL)
    {
        /* 指针非法 */
        return;
    }

    /* 搜寻是否已存在于链表中 */
    target = p_ibutton_head;
    while (target != NULL)
    {
        if (target == pbtn)
        {
            /* 已存在 */
            return;
        }
        target = target->next;
    }
    /* 未搜索到，添加到头部 */
    pbtn->next = p_ibutton_head;
    /* 更新头节点 */
    p_ibutton_head = pbtn;
}

/**
 * @brief 从按键链表中移除按键对象
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @note 如果按键未在链表中不进行删除操作
 *       不要在事件处理回调函数中使用
 */
void letk_ibutton_remove(letk_ibutton_t* pbtn)
{
    letk_ibutton_t* ptemp;
    letk_ibutton_t* prev;

    if (pbtn == NULL)
    {
        /* 指针非法 */
        return;
    }

    if (p_ibutton_head == NULL)
    {
        /* 链表空，未搜索到 */
        return;
    }

    if (p_ibutton_head == pbtn)
    {
        /* 如果需要删除的节点位于链表头，需要更新链表头指针 */
        p_ibutton_head = p_ibutton_head->next;
        /* 保证删除节点的指针安全 */
        pbtn->next = NULL;
        return;
    }

    /* 从第二个开始搜索 */
    prev = p_ibutton_head;
    ptemp = p_ibutton_head->next;
    for (; ptemp != NULL; ptemp = ptemp->next)
    {
        if (ptemp == pbtn)
        {
            /* 搜索到，删除节点 */
            prev->next = ptemp->next;
            /* 保证删除节点的指针安全 */
            ptemp->next = NULL;
            return;
        }
        prev = ptemp;
    }
}

/**
 * @brief 轮询一个按键
 * @param[in] pbtn 按键对象指针(不能为NULL)
 */
static void letk_ibutton_poll_one_button(letk_ibutton_t* pbtn)
{
    /* 读取电平 */
    bool io_status = pbtn->read_cb(pbtn);

    /* 消抖 */
    if (io_status != pbtn->io_status)
    {
        if (++(pbtn->debounce_cnt) >= pbtn->debounce_ticks)
        {
            pbtn->io_status = io_status;
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
    switch (pbtn->status)
    {
    case LETK_IBUTTON_STATUS_RELEASED:
        if (pbtn->io_status)
        {
            /* 按键按下事件 */
            pbtn->event = LETK_IBUTTON_EVENT_PRESS;
            /* 清除计数器 */
            pbtn->internal_cnt = 0;
            /* 清除连击计数 */
            pbtn->click_cnt = 1;
            /* 切换到按键按下状态 */
            pbtn->status = LETK_IBUTTON_STATUS_PRESSED;
        }
        break;

    case LETK_IBUTTON_STATUS_PRESSED:
        if (!pbtn->io_status)
        {
            /* 未达到长按时间之前抬起，按键松开事件 */
            pbtn->event = LETK_IBUTTON_EVENT_RELEASE;
            /* 清除计数器 */
            pbtn->internal_cnt = 0;
            /* 切换到等待连击状态 */
            pbtn->status = LETK_IBUTTON_STATUS_WAIT_PRESS_AGIN;
        }
        else if (++pbtn->internal_cnt >= pbtn->long_press_ticks)
        {
            /* 超过长按时间还未松开，首次触发长按事件 */
            pbtn->event = LETK_IBUTTON_EVENT_LONG_FIRST;
            /* 切换到等待按键松开状态 */
            pbtn->status = LETK_IBUTTON_STATUS_WAIT_RELEASE;
        }
        break;

    case LETK_IBUTTON_STATUS_WAIT_PRESS_AGIN:
        if (pbtn->io_status)
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
            pbtn->status = LETK_IBUTTON_STATUS_PRESS_AGIN;
        }
        else if (++pbtn->internal_cnt >= pbtn->hit_again_ticks)
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
            pbtn->status = LETK_IBUTTON_STATUS_RELEASED;
        }
        break;

    case LETK_IBUTTON_STATUS_PRESS_AGIN:
        if (!pbtn->io_status)
        {
            /* 按键松开事件 */
            pbtn->event = LETK_IBUTTON_EVENT_RELEASE;
            /* 清除计数器 */
            pbtn->internal_cnt = 0;
            /* 切换到等待按键再次按下状态 */
            pbtn->status = LETK_IBUTTON_STATUS_WAIT_PRESS_AGIN;
        }
        break;

    case LETK_IBUTTON_STATUS_WAIT_RELEASE:
        if (!pbtn->io_status)
        {
            /* 按键松开，按键抬起事件 */
            pbtn->event = LETK_IBUTTON_EVENT_RELEASE;
            /* 清除计数器 */
            pbtn->internal_cnt = 0;
            /* 清除连击计数 */
            pbtn->click_cnt = 0;
            /* 回到释放状态 */
            pbtn->status = LETK_IBUTTON_STATUS_RELEASED;
        }
        else if (++pbtn->internal_cnt >= pbtn->long_repeat_ticks)
        {
            /* 清除计数 */
            pbtn->internal_cnt = 0;
            /* 继续按下，按键重复按下事件 */
            pbtn->event = LETK_IBUTTON_EVENT_REPEAT;
        }
        break;

    default:
        /* 异常，回到释放状态 */
        pbtn->internal_cnt = 0;
        pbtn->click_cnt = 0;
        pbtn->status = LETK_IBUTTON_STATUS_RELEASED;
        break;
    }

    /* 有事件产生，执行回调 */
    if ((pbtn->event != LETK_IBUTTON_EVENT_NONE) && (pbtn->event_cb != NULL))
    {
        pbtn->event_cb(pbtn);
    }
}

/**
 * @brief 轮询所有按键
 * @note 此函数会依次每个按键进行轮询，有事件会执行相应事件的回调函数，
 *       需要放到定期执行的定时器回调或者定时任务中，尽量不要放到中断中，
 *       如果需要放到中断中，请对上面的按键初始化以及链表操作的函数进行
 *       临界段保护，且执行事件的回调函数体尽量短小，当然中间使用了全局
 *       或静态变量也要注意临界段保护
 */
void letk_ibutton_poll(void)
{
    for (letk_ibutton_t* pbtn = p_ibutton_head; pbtn != NULL; pbtn = pbtn->next)
    {
        letk_ibutton_poll_one_button(pbtn);
    }
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */
