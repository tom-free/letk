# 系统滴答时钟模块

## 介绍

用于产生系统的滴答时钟，保证整个系统有一个标准的参考时基，特性如下：

- 时基单位为ms，易读易移植
- 32位时基值，足以保证系统长时间运行，减少溢出现象
- 时基溢出能继续正常工作，不会对系统定时造成影响
- 保证读取时基的原子性，8/16位机中断时出现多字节撕裂现象时不会导致读取异常
- 提供读取流逝时间和判断时间超时的接口

## 移植

需要用户将 `letk_ticks_inc_ms` 函数放到一个周期运行的硬件定时器或者周期运行的任务中，定时周期可以用户自由设置，最小为1ms，最大建议不要超过20ms

例如启动一个10ms的硬件定时器中断，在中断函数中调用如下（伪代码）：

```C
#include "letk_ticks.h"

/* 定时器x中断，周期10ms */
ISR_FUNC void hal_timerx_isr(void)
{
    letk_ticks_inc_ms(10);
}
```
## API

函数 | 描述
:-- | :--
void letk_ticks_inc_ms(uint32_t ms) | 系统时钟增加指定的ms数
uint32_t letk_ticks_get_ms(void) | 获取系统当前的ms数
uint32_t letk_ticks_elapsed_ms(uint32_t last_ms) | 获取相对于上一时刻流逝的ms数
bool letk_ticks_is_timeout(uint32_t last_ms, uint32_t interval) | 判断相对于上一时刻是否超过间隔的ms数

## 注意事项

为了程序的可移植性和程序可读性，我们不采用ticks作为系统滴答单位，而是采用ms，这样应用程序在进行平台移植时就不会因为ticks长度不一致导致的时序错误问题
