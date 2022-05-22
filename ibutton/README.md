# 通用按键管理器

## 介绍

通用多功能按键管理器，主要特性如下：

> 1. 代码量小，占用内存和程序空间少，内存只占用一个指针变量空间，程序仅几个函数
> 2. 按键采用链表管理，可动态增删按键
> 3. 无按键个数限制，只取决于系统内存大小
> 4. 使用简单，就几个函数的调用，无其他系统依赖
> 5. 采用回调机制，可自定义按键IO读取和事件处理
> 6. 可以扩展成矩阵键盘或者组合按键，自定义程度高
> 7. 内部采用状态机实现，无延时，自带消抖功能，可以设置消抖时间
> 8. 支持短按、长按、单击和多击功能，一键多用，节省按键个数

## 软件架构

读取IO操作的回调函数原型

```c
/* 读取IO口电平回调函数，用户按照此函数模板去实现回调 */
typedef gm_multi_key_level_t gm_multi_key_read_io_cb_t(gm_multi_key_t *p_key);
```

按键事件处理的回调函数原型

```c
/* 按键事件处理回调，用户按照此函数模板去实现回调 */
typedef int gm_multi_key_event_cb_t(gm_multi_key_t *p_key);
```

内部采用状态机实现，状态如下：

```c
/* 按键状态定义 */
typedef enum _gm_multi_key_fsm_status_t
{
    /* 按键已释放状态 */
    GM_MULTI_KEY_FSM_STATUS_RELEASE,
    /* 按键已按下状态 */
    GM_MULTI_KEY_FSM_STATUS_PRESS,
    /* 按键松开，等待再次按下，用于判断是否连击 */
    GM_MULTI_KEY_FSM_STATUS_WAIT_PRESS_AGIN,
    /* 按键再次按下状态，此时不能再响应长按，
     * 所以和GM_MULTI_KEY_FSM_STATUS_PRESS处理不同 */
    GM_MULTI_KEY_FSM_STATUS_PRESS_AGIN,
    /* 等待按键释放状态 */
    GM_MULTI_KEY_FSM_STATUS_WAIT_RELEASE,
    /* 总状态数量 */
    GM_MULTI_KEY_FSM_STATUS_NUM,
} gm_multi_key_fsm_status_t;
```

按键采用链表管理，按键参数定义如下：

```c
/* 按键结构定义 */
struct _gm_multi_key_t
{
    /* 链表指针，内部管理器使用，不可写，只可读 */
    struct _gm_multi_key_t    *next;
    /* IO读取函数，用户传入，需保证合法性，可读写，建议调用函数写入 */
    gm_multi_key_read_io_cb_t *read_io_cb;
    /* 事件处理回调，用户传入，需保证合法性，可读写，建议调用函数写入 */
    gm_multi_key_event_cb_t   *event_proc_cb;
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
    gm_multi_key_event_t       event;
    /* IO电平，内部使用，不可写，只可读 */
    gm_multi_key_level_t       level;
};
```

> 1. `next`指针用来进行链表管理
> 2. `read_io_cb`函数指针用来保存读取IO操作的回调函数
> 3. `event_proc_cb`函数指针用来保存按键事件回调函数
> 4. `click_cnt_max`设置连击最大次数，超过此次数的连击都默认为最大次数，范围：1-255
> 5. `debounce_ticks`设置消抖滴答个数，每次读取IO电平变化必须在此滴答数内保持稳定才有效，范围：1-255
> 6. `hit_again_ticks`设置连击有效的时间间隔滴答数，连击事件必须发生在此时间范围内才能认为是连击，范围：1-65535
> 7. `long_press_ticks`设置长按时间间隔滴答数，首次按下超过此时间间隔认为长按，触发长按事件，之后周期性触发重复按下事件，范围：1-65535
> 8. `long_repeat_ticks`设置长按后不松开周期性触发重复按下事件时间间隔滴答数，范围：1-65535
> 9. `internal_cnt`按键内部的计数器，用户不可使用
> 10. `debounce_cnt`按键内部消抖的计数器，用户不可使用
> 11. `click_cnt`用来标识连击次数
> 12. `fsm_status`按键状态机状态，用于内部状态切换，用户不可使用
> 13. `event`按键事件，标识当前的按键事件
> 14. `level`按键IO电平，内部记录电平变化使用，用户不可使用

## 配置项

配置项放在头文件`gm_multi_key_cfg.h`中

配置项名称 | 配置描述 | 范围 | 单位
:-- | :-- | :-- | :-- 
GM_MULTI_KEY_POLL_INTERVAL | 按键轮询时间间隔，用于系统的相关时间计算 | 1~65535 | ms
GM_MULTI_KEY_DEBOUNCE_TICKS | 默认的消抖时间滴答数，以上面的间隔为基准 | 1~255 | 滴答
GM_MULTI_KEY_HIT_AGIN_TICKS | 默认的连击最长间隔时间滴答数，超过此时间认为连击被终结，以上面的间隔为基准 | 1~65535 | 滴答
GM_MULTI_KEY_LONG_TICKS | 默认的长按触发时间，超过此时间认为进入长按，以上面的间隔为基准 | 1~65535 | 滴答
GM_MULTI_KEY_REPEAT_TRIG_TICKS | 默认的长按重复触发时间，即隔多长时间触发一次，以上面的间隔为基准 | 1~65535 | 滴答
GM_MULTI_KEY_CLICK_COUNT_MAX | 默认的连击最大次数 | 1~255 | 次

所有的时间参数全部依赖于`GM_MULTI_KEY_POLL_INTERVAL`参数，为了方便转换，可以采用头文件中的`GM_MULTI_KEY_MS_TO_TICKS(xms)`宏函数来自动计算相关参数，传入的单位均为`ms`

## 安装教程

> 1. 准备一个工程
> 2. 克隆此仓库到工程目录
> 3. 添加文件进工程页面
> 4. 添加头文件进工程设置
> 5. 按照自己的需求修改配置文件`gm_multi_key_cfg.h`
> 6. 编译查错
> 7. 添加读取IO和事件处理回调函数
> 8. 调用`gm_multi_key_mgr_init`初始化管理器和`gm_multi_key_init`初始化按键（多个按键多次调用此函数），调用`gm_multi_key_add`添加按键到链表中（多个按键多次调用此函数），最后周期调用`gm_multi_key_poll`轮询，可放到定时器回调中或定时任务中，中断中调用请保证临界资源访问互斥，系统内部没有互斥实现

## 使用伪代码例程

```c
/* 包含芯片平台头文件 */
#include "chip.h"
/* 按键头文件 */
#include "gm_multi_key.h"

/* 按键对象 */
static gm_multi_key_t key1, key2;

/* 按键1读取 */
static gm_multi_key_level_t key1_read_level(gm_multi_key_t *p_key)
{
    if (/* 读取IO的实际硬件操作函数或寄存器 */)
    {
        return GM_MULTI_KEY_LEVEL_RELEASE;
    }
    else
    {
        return GM_MULTI_KEY_LEVEL_PRESS;
    }
}

/* 按键2读取 */
static gm_multi_key_level_t key2_read_level(gm_multi_key_t *p_key)
{
    if (/* 读取IO的实际硬件操作函数或寄存器 */)
    {
        return GM_MULTI_KEY_LEVEL_RELEASE;
    }
    else
    {
        return GM_MULTI_KEY_LEVEL_PRESS;
    }
}

/* 按键事件处理 */
static int key1_event_proc(gm_multi_key_t *pkey)
{
    /* 处理事件 */
    switch (pkey->event)
    {
    case GM_MULTI_KEY_EVENT_PRESS:
        
        break;

    case GM_MULTI_KEY_EVENT_RELEASE:
        
        break;

    case GM_MULTI_KEY_EVENT_MULTI_CLICK:

        break;

    case GM_MULTI_KEY_EVENT_xxx:
        
        break;

    default:
        break;
    }

    return 0;
}

/* 主函数 */
int main(int argc, void* argv[])
{
    /* 初始化按键IO */
    hal_key_io_init();

    /* 初始化各种其他系统外设 */
    hal_others_init();

    /* 初始化管理器 */
    gm_multi_key_mgr_init();

    ////////////////////////////////////////////////////////////////////////////
    /* 初始化按键1 */
    gm_multi_key_init(&key1, key1_read_level, key1_event_proc);

    /* 下面的5个函数如果想使用系统默认值可以不用调用配置 */
    /* 设置连击最大次数为10次 */
    gm_multi_key_set_click_cnt_max(&key1, 10);
    /* 设置消抖时间为200ms */
    gm_multi_key_set_debounce_ticks(&key1, GM_MULTI_KEY_MS_TO_TICKS(50));
    /* 设置连击间隔时间为500ms */
    gm_multi_key_set_hit_again_ticks(&key1, GM_MULTI_KEY_MS_TO_TICKS(500));
    /* 设置长按触发时间为1000ms */
    gm_multi_key_set_long_press_ticks(&key1, GM_MULTI_KEY_MS_TO_TICKS(1000));
    /* 设置长按重复触发时间为250ms */
    gm_multi_key_set_long_repeat_ticks(&key1, GM_MULTI_KEY_MS_TO_TICKS(250));

    /* 添加按键1到链表 */
    gm_multi_key_add(&key1);
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    /* 初始化按键2 */
    gm_multi_key_init(&key2, key_read_level, key_event_proc);

    /* 下面的5个函数如果想使用系统默认值可以不用调用配置 */
    /* 设置连击最大次数为15次 */
    gm_multi_key_set_click_cnt_max(&key2, 15);
    /* 设置消抖时间为20ms */
    gm_multi_key_set_debounce_ticks(&key2, GM_MULTI_KEY_MS_TO_TICKS(20));
    /* 设置连击间隔时间为300ms */
    gm_multi_key_set_hit_again_ticks(&key2, GM_MULTI_KEY_MS_TO_TICKS(300));
    /* 设置长按触发时间为500ms */
    gm_multi_key_set_long_press_ticks(&key2, GM_MULTI_KEY_MS_TO_TICKS(500));
    /* 设置长按重复触发时间为100ms */
    gm_multi_key_set_long_repeat_ticks(&key2, GM_MULTI_KEY_MS_TO_TICKS(100));

    /* 添加按键2按键进链表 */
    gm_multi_key_add(&key2);
    ////////////////////////////////////////////////////////////////////////////

    /* 多个按键请多次调用上面的函数 */
    // ...

    while (1)
    {
        /* 轮询 */
        gm_multi_key_poll();
        /* 模拟延时，实际应用不可如此使用 */
        delay_ms(GM_MULTI_KEY_POLL_INTERVAL);
    }

    return 0;
}
```

## 参与贡献

1. clone 本仓库
2. 新建 dev_xxx 分支
3. 提交代码
4. 新建 pull request
