/***********************************************************************************************************************
** 文件描述：独立按键管理头文件
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
#ifndef __LETK_IBUTTON_H__
#define __LETK_IBUTTON_H__

#include "letk_ibutton_cfg.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* ms转ticks，用于计算相关ticks参数 */
#define LETK_IBUTTON_MS_TO_TICKS(ms)    ((ms) / (LETK_IBUTTON_POLL_INTERVAL))

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
    LETK_IBUTTON_STATUS_RELEASED,           /* 按键已释放状态 */
    LETK_IBUTTON_STATUS_PRESSED,            /* 按键已按下状态 */
    LETK_IBUTTON_STATUS_WAIT_PRESS_AGIN,    /* 按键松开，等待再次按下，
                                             * 用于判断是否连击 */
    LETK_IBUTTON_STATUS_PRESS_AGIN,         /* 按键再次按下状态，
                                             * 此时不能再响应长按 */
    LETK_IBUTTON_STATUS_WAIT_RELEASE,       /* 等待按键释放状态 */
} letk_ibutton_status_t;

/* 定义按键类，推荐采用初始化函数来初始化此对象实例，
 * 不要直接进行值操作，除非对此系统的实现源码特别理解 */
typedef struct _letk_ibutton_t letk_ibutton_t;

/* 读取IO口电平回调函数，返回true表示按下，false表示抬起 */
typedef bool letk_ibutton_read_cb_t(letk_ibutton_t* pbtn);
/* 按键事件处理回调函数 */
typedef void letk_ibutton_event_cb_t(letk_ibutton_t* pbtn);

/* 按键结构定义 */
struct _letk_ibutton_t
{
    /* 链表指针，内部管理器使用，不可写，只可读 */
    letk_ibutton_t*            next;
    /* IO读取函数，用户传入，需保证合法性，可读写，建议调用函数写入 */
    letk_ibutton_read_cb_t*    read_cb;
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
    /* 按键当前状态，内部枚举定义，不可写，只可读 */
    letk_ibutton_status_t      status;
    /* 按键事件，用户可以读取此值来处理事件，不可写，只可读 */
    letk_ibutton_event_t       event;
    /* IO状态，true：按下，false：松开，内部使用，不可写，只可读 */
    bool                       io_status;
};

/**
 * @brief 按键管理器参数初始化
 * @note 用于休眠时内存断电的场合，唤醒后需要调用此函数重新初始化内存数据
 *       此函数必须在按键设置相关的函数之前调用，不能在之后或中间调用
 */
void letk_ibutton_mgr_init(void);

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
                       letk_ibutton_event_cb_t* event_cb);

/**
 * @brief 设置读取IO回调函数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] read_cb 读取IO回调函数(不能为NULL)
 */
void letk_ibutton_set_read_io_cb(letk_ibutton_t* pbtn,
                                 letk_ibutton_read_cb_t* read_cb);

/**
 * @brief 设置事件处理回调函数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] event_cb 事件处理回调函数
 */
void letk_ibutton_set_event_cb(letk_ibutton_t* pbtn,
                               letk_ibutton_event_cb_t* event_cb);

/**
 * @brief 设置连击最大次数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] click_cnt_max 连击最大次数，范围[1-255]
 */
void letk_ibutton_set_click_cnt_max(letk_ibutton_t* pbtn,
                                    uint8_t click_cnt_max);

/**
 * @brief 设置防抖检测次数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] debounce_ticks 防抖检测次数，范围[1-255]
 */
void letk_ibutton_set_debounce_ticks(letk_ibutton_t* pbtn,
                                     uint8_t debounce_ticks);

/**
 * @brief 设置连击最大间隔时间周期次数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] hit_again_ticks 连击最大间隔时间周期次数
 */
void letk_ibutton_set_hit_again_ticks(letk_ibutton_t* pbtn,
                                      uint8_t hit_again_ticks);

/**
 * @brief 设置长按触发时间周期次数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] long_press_ticks 长按触发时间周期次数，范围[1-255]
 */
void letk_ibutton_set_long_press_ticks(letk_ibutton_t* pbtn,
                                       uint8_t long_press_ticks);

/**
 * @brief 设置长按重复按下事件触发时间周期次数
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @param[in] long_repeat_ticks 长按重复按下事件触发时间周期次数，范围[1-255]
 */
void letk_ibutton_set_long_repeat_ticks(letk_ibutton_t* pbtn,
                                        uint8_t long_repeat_ticks);

/**
 * @brief 添加按键对象到按键链表中
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @note 如果按键已在链表中不进行添加操作
 *       不要在事件处理回调函数中使用
 */
void letk_ibutton_add(letk_ibutton_t* pbtn);

/**
 * @brief 从按键链表中移除按键对象
 * @param[in] pbtn 按键对象指针(不能为NULL)
 * @note 如果按键未在链表中不进行删除操作
 *       不要在事件处理回调函数中使用
 */
void letk_ibutton_remove(letk_ibutton_t* pbtn);

/**
 * @brief 轮询所有按键
 * @note 此函数会依次每个按键进行轮询，有事件会执行相应事件的回调函数，
 *       需要放到定期执行的定时器回调或者定时任务中，尽量不要放到中断中，
 *       如果需要放到中断中，请对上面的按键初始化以及链表操作的函数进行
 *       临界段保护，且执行事件的回调函数体尽量短小，当然中间使用了全局
 *       或静态变量也要注意临界段保护
 */
void letk_ibutton_poll(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __LETK_IBUTTON_H__ */
