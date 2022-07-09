/*******************************************************************************
** @file   letk_ibutton.h
** @brief  独立按键管理
** @author 付瑞彪
** @date   2022-05-22
**
**       Copyright (c) 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/
#ifndef __LETK_IBUTTON_H__
#define __LETK_IBUTTON_H__

#include "letk_ibutton_cfg.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* ms转ticks，用于计算相关ticks参数 */
#define LETK_IBUTTON_MS_TO_TICKS(ms)    ((ms) / (LETK_IBUTTON_POLL_INTERVAL))

/* 操作结果定义，用于操作的返回提示 */
typedef enum
{
    /* 成功或正确 */
    LETK_IBUTTON_RESULT_OK = 0,
    /* 传入参数错误，例如指针为空 */
    LETK_IBUTTON_RESULT_ARGS_ERR = -1,
    /* 添加时发现已经在链表了 */
    LETK_IBUTTON_RESULT_EXIST = -2,
    /* 删除时未在链表中找到对象 */
    LETK_IBUTTON_RESULT_NOT_FOUND = -3,
} letk_ibutton_result_t;

/* 按键电平定义，抽象电平概念，用户硬件可以使用任意的电平，包括ADC按键，
 * 只需要提供回调函数是按照下面的抽象电平传入即可，达到高度自定义和扩展 */
typedef enum
{
    /* 按下 */
    LETK_IBUTTON_LEVEL_PRESS,
    /* 释放 */
    LETK_IBUTTON_LEVEL_RELEASE,
} letk_ibutton_level_t;

/* 按键事件，标识当前发生的按键事件，
 * 用户可按需处理，只需要关心需求的部分事件 */
typedef enum
{
    /* 无事件 */
    LETK_IBUTTON_EVENT_NONE,
    /* 按下事件，按下边沿触发 */
    LETK_IBUTTON_EVENT_PRESS,
    /* 释放事件，松开边沿触发 */
    LETK_IBUTTON_EVENT_RELEASE,
    /* 首次触发长按，按下达到长按时间时触发 */
    LETK_IBUTTON_EVENT_LONG_FIRST,
    /* 重复按下，按下时一直触发 */
    LETK_IBUTTON_EVENT_REPEAT,
    /* 单击，短暂按下并松开 */
    LETK_IBUTTON_EVENT_SINGLE_CLICK,
    /* 双击，因为常用，所以单独列一个，其实可以归为连击 */
    LETK_IBUTTON_EVENT_DOUBLE_CLICK,
    /* 连击，短暂的连续点击 */
    LETK_IBUTTON_EVENT_MULTI_CLICK,
    /* 事件数量总数 */
    LETK_IBUTTON_EVENT_NUM,
} letk_ibutton_event_t;

/* 按键状态定义 */
typedef enum
{
    /* 按键已释放状态 */
    LETK_IBUTTON_FSM_STATUS_RELEASE,
    /* 按键已按下状态 */
    LETK_IBUTTON_FSM_STATUS_PRESS,
    /* 按键松开，等待再次按下，用于判断是否连击 */
    LETK_IBUTTON_FSM_STATUS_WAIT_PRESS_AGIN,
    /* 按键再次按下状态，此时不能再响应长按，
     * 所以和LETK_IBUTTON_FSM_STATUS_PRESS处理不同 */
    LETK_IBUTTON_FSM_STATUS_PRESS_AGIN,
    /* 等待按键释放状态 */
    LETK_IBUTTON_FSM_STATUS_WAIT_RELEASE,
    /* 总状态数量 */
    LETK_IBUTTON_FSM_STATUS_NUM,
} letk_ibutton_fsm_status_t;

/* 定义按键类型体，推荐采用初始化函数来初始化此对象实例，
 * 不要直接进行值操作，除非对此系统的实现源码特别理解 */
typedef struct letk_ibutton_t letk_ibutton_t;

/* 读取IO口电平回调函数，用户按照此函数模板去实现回调 */
typedef letk_ibutton_level_t letk_ibutton_read_io_cb_t(letk_ibutton_t* pbtn);
/* 按键事件处理回调，用户按照此函数模板去实现回调 */
typedef void letk_ibutton_event_cb_t(letk_ibutton_t* pbtn);

/* 按键结构定义 */
struct letk_ibutton_t
{
    /* 链表指针，内部管理器使用，不可写，只可读 */
    letk_ibutton_t*            next;
    /* IO读取函数，用户传入，需保证合法性，可读写，建议调用函数写入 */
    letk_ibutton_read_io_cb_t* read_io_cb;
    /* 事件处理回调，用户传入，需保证合法性，可读写，建议调用函数写入 */
    letk_ibutton_event_cb_t*   event_cb;
    /* 连击最大次数，超过此数量不再连击计数，
     * 以设置的扫描间隔为基准，可读写，>=1，建议调用函数写入 */
    uint8_t                    click_cnt_max;
    /* 消抖滴答个数，每次读取电平必须保证此时间内的数据稳定才进入状态机，
     * 以设置的扫描间隔为基准，可读写，>=1，建议调用函数写入 */
    uint8_t                    debounce_ticks;
    /* 连击间隔滴答个数，时间在此范围内的多击才有效，超时此次连击结束，
     * 以设置的扫描间隔为基准，可读写，>=1，建议调用函数写入 */
    uint16_t                   hit_again_ticks;
    /* 长按首次触发间隔滴答个数，第一次进入长按的时间超过此时间触发长按，
     * 以设置的扫描间隔为基准，可读写，>=1，建议调用函数写入 */
    uint16_t                   long_press_ticks;
    /* 长按重复触发间隔滴答个数，触发长按后不松手，
     * 按照此时间间隔产生重复按下事件，
     * 以设置的扫描间隔为基准，可读写，>=1，建议调用函数写入 */
    uint16_t                   long_repeat_ticks;
    /* 按键内部计数器，不可写，只可读 */
    uint16_t                   internal_cnt;
    /* 按键内部消抖计数，不可写，只可读 */
    uint8_t                    debounce_cnt;
    /* 连续点击计数，用户可以读取此值来判断连击次数，不可写，只可读 */
    uint8_t                    click_cnt;
    /* 按键状态机状态，内部枚举定义，不可写，只可读 */
    uint8_t                    fsm_status;
    /* 按键事件，用户可以读取此值来处理事件，不可写，只可读 */
    letk_ibutton_event_t       event;
    /* IO电平，内部使用，不可写，只可读 */
    letk_ibutton_level_t       level;
};

/*******************************************************************************
** 函数名称：letk_ibutton_mgr_init
** 函数作用：按键管理器初始化
** 输入参数：无
** 输出参数：无
** 使用样例：letk_ibutton_mgr_init();
** 函数备注：可以不使用此函数，此函数要在其他函数之前调用，
**           主要用到低功耗内存会掉电的情况下重新设置内存初始值
*******************************************************************************/
void letk_ibutton_mgr_init(void);

/*******************************************************************************
** 函数名称：letk_ibutton_init
** 函数作用：初始化按键
** 输入参数：pbtn      - 按键指针对象
**           read_io_cb - 按键读取IO电平函数指针
**           event_cb   - 事件回调函数
** 输出参数：操作结果
** 使用样例：letk_ibutton_init(&btn1, btn1_read, btn1_enevt);
** 函数备注：此函数会读取一次电平存放起来，作为初始化电平，
**           请在此函数调用之前初始化IO外设；
**           此函数只初始化回调函数指针，其它参数使用默认值，需要设置请采用相应函数
*******************************************************************************/
letk_ibutton_result_t letk_ibutton_init(letk_ibutton_t* pbtn,
                                        letk_ibutton_read_io_cb_t* read_io_cb,
                                        letk_ibutton_event_cb_t* event_cb);

/*******************************************************************************
** 函数名称：letk_ibutton_set_read_io_cb
** 函数作用：设置读取IO回调
** 输入参数：pbtn      - 按键指针对象
**           read_io_cb - 按键读取IO电平函数指针
** 输出参数：操作结果
** 使用样例：letk_ibutton_result_t res = letk_ibutton_set_read_io_cb(&btn1, btn1_read);
** 函数备注：
*******************************************************************************/
letk_ibutton_result_t letk_ibutton_set_read_io_cb(letk_ibutton_t* pbtn, letk_ibutton_read_io_cb_t* read_io_cb);

/*******************************************************************************
** 函数名称：letk_ibutton_set_event_cb
** 函数作用：设置事件处理回调
** 输入参数：pbtn      - 按键指针对象
**           event_cb   - 事件回调函数
** 输出参数：操作结果
** 使用样例：letk_ibutton_result_t res = letk_ibutton_set_event_cb(&btn1, btn1_enevt);
** 函数备注：
*******************************************************************************/
letk_ibutton_result_t letk_ibutton_set_event_cb(letk_ibutton_t* pbtn, letk_ibutton_event_cb_t* event_cb);

/*******************************************************************************
** 函数名称：letk_ibutton_set_click_cnt_max
** 函数作用：设置连击最大次数
** 输入参数：pbtn         - 按键指针对象
**           click_cnt_max - 连击最大次数
** 输出参数：操作结果
** 使用样例：letk_ibutton_result_t res = letk_ibutton_set_click_cnt_max(&btn1, 10);
** 函数备注：
*******************************************************************************/
letk_ibutton_result_t letk_ibutton_set_click_cnt_max(letk_ibutton_t* pbtn, uint8_t click_cnt_max);

/*******************************************************************************
** 函数名称：letk_ibutton_set_debounce_ticks
** 函数作用：设置防抖周期
** 输入参数：pbtn          - 按键指针对象
**           debounce_ticks - 防抖周期
** 输出参数：操作结果
** 使用样例：letk_ibutton_result_t res = letk_ibutton_set_debounce_ticks(&btn1, 10);
** 函数备注：
*******************************************************************************/
letk_ibutton_result_t letk_ibutton_set_debounce_ticks(letk_ibutton_t* pbtn, uint8_t debounce_ticks);

/*******************************************************************************
** 函数名称：letk_ibutton_set_hit_again_ticks
** 函数作用：设置连击最大间隔时间周期
** 输入参数：pbtn           - 按键指针对象
**           hit_again_ticks - 连击间隔最大周期
** 输出参数：操作结果
** 使用样例：letk_ibutton_result_t res = letk_ibutton_set_hit_again_ticks(&btn1, 10);
** 函数备注：
*******************************************************************************/
letk_ibutton_result_t letk_ibutton_set_hit_again_ticks(letk_ibutton_t* pbtn, uint8_t hit_again_ticks);

/*******************************************************************************
** 函数名称：letk_ibutton_set_long_press_ticks
** 函数作用：设置长按触发时间周期
** 输入参数：pbtn            - 按键指针对象
**           long_press_ticks - 长按触发时间周期
** 输出参数：操作结果
** 使用样例：letk_ibutton_result_t res = letk_ibutton_set_long_press_ticks(&btn1, 10);
** 函数备注：
*******************************************************************************/
letk_ibutton_result_t letk_ibutton_set_long_press_ticks(letk_ibutton_t* pbtn, uint8_t long_press_ticks);

/*******************************************************************************
** 函数名称：letk_ibutton_set_long_repeat_ticks
** 函数作用：设置长按重复按下事件触发时间周期
** 输入参数：pbtn             - 按键指针对象
**           long_repeat_ticks - 重复按下事件触发时间周期
** 输出参数：操作结果
** 使用样例：letk_ibutton_result_t res = letk_ibutton_set_long_repeat_ticks(&btn1, 10);
** 函数备注：
*******************************************************************************/
letk_ibutton_result_t letk_ibutton_set_long_repeat_ticks(letk_ibutton_t* pbtn, uint8_t long_repeat_ticks);

/*******************************************************************************
** 函数名称：letk_ibutton_add
** 函数作用：添加一个按键进入链表中
** 输入参数：pbtn - 按键指针对象
** 输出参数：操作结果
** 使用样例：letk_ibutton_result_t res = letk_ibutton_add(&btn1);
** 函数备注：pbtn非法或已存在链表均会返回失败
*******************************************************************************/
letk_ibutton_result_t letk_ibutton_add(letk_ibutton_t* pbtn);

/*******************************************************************************
** 函数名称：letk_ibutton_remove
** 函数作用：从按键链表中移除一个按键对象
** 输入参数：pbtn - 按键指针对象
** 输出参数：操作结果
** 使用样例：letk_ibutton_result_t res = letk_ibutton_remove(&btn1);
** 函数备注：pbtn非法或不存在均会返回失败
*******************************************************************************/
letk_ibutton_result_t letk_ibutton_remove(letk_ibutton_t* pbtn);

/*******************************************************************************
** 函数名称：letk_ibutton_poll
** 函数作用：执行按键事件轮询
** 输入参数：无
** 输出参数：无
** 使用样例：letk_ibutton_poll();
** 函数备注：此函数会依次每个按键进行轮询，有事件会执行相应事件的回调函数；
**           需要放到定期执行的定时器回调或者定时任务中，尽量不要放到中断中；
**           如果需要放到中断中，请对上面的按键初始化以及链表操作的函数进行
**           临界段保护，且执行事件的回调函数体尽量短小，当然中间使用了全局
**           或静态变量也要注意临界段保护
*******************************************************************************/
void letk_ibutton_poll(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __LETK_IBUTTON_H__ */
