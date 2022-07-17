/***********************************************************************************************************************
** 文件描述：自动初始化管理头文件
** 创建作者：付瑞彪(Tom Free)
** 创建日期：2022年7月16日
** 编码格式：UTF-8编码
** 编程语言：C语言，C99标准
** 缩进格式：4个空格键
** 命名规范：下划线命名法(小写命名法)
** 开源许可：MIT许可证，参考：https://mit-license.org
** 版权信息：Copyright (c) 2013-2022, Tom Free, <tomfreefu@gmail.com>
**
** 修改记录
** 修改日期         修改作者        修改内容
** 2022年7月16日    付瑞彪          创建文件，初次版本
**
***********************************************************************************************************************/
#ifndef __LETK_AINIT_H__
#define __LETK_AINIT_H__

#if defined(__CC_ARM) || defined(__GNUC__)
/* Keil, GCC */
#define LETK_AINIT_CC_SECTION(x)    __attribute__((section(x)))
#define LETK_AINIT_CC_UNUSED        __attribute__((unused))
#define LETK_AINIT_CC_USED          __attribute__((used))
#define LETK_AINIT_CC_ALIGN(n)      __attribute__((aligned(n)))
#define LETK_AINIT_CC_WEAK          __attribute__((weak))
#elif defined (__ICCARM__)
/* IAR */
#define LETK_AINIT_CC_SECTION(x)    @ x
#define LETK_AINIT_CC_UNUSED
#define LETK_AINIT_CC_USED          __root
#define LETK_AINIT_CC_WEAK          __weak
#else
#error Not support the compiler
#endif

/* 自动初始化函数原型 */
typedef void letk_ainit_fn_t(void);

/* 自动初始化函数等级导出 */
#define LETK_AINIT_LEVEL(fn, level)     \
    LETK_AINIT_CC_USED static letk_ainit_fn_t* _letk_ainit_##fn LETK_AINIT_CC_SECTION(".letk_ainit." #level) = fn;

/* 最开始初始化，用于特殊场合，例如关中断、设置芯片运行环境等 */
#define LETK_FIRST_INIT_EXPORT(fn)      LETK_AINIT_LEVEL(fn, 1)
/* 软件包初始化 */
#define LETK_SOFT_INIT_EXPORT(fn)       LETK_AINIT_LEVEL(fn, 2)
/* 板级初始化 */
#define LETK_BOARD_INIT_EXPORT(fn)      LETK_AINIT_LEVEL(fn, 3)
/* 外围设备初始化 */
#define LETK_DEVICE_INIT_EXPORT(fn)     LETK_AINIT_LEVEL(fn, 4)
/* 组件初始化 */
#define LETK_COMPONENT_INIT_EXPORT(fn)  LETK_AINIT_LEVEL(fn, 5)
/* 应用层初始化 */
#define LETK_APP_INIT_EXPORT(fn)        LETK_AINIT_LEVEL(fn, 6)
/* 最后期初始化，用于特殊场合，例如开中断、启动看门狗等 */
#define LETK_FINALLY_INIT_EXPORT(fn)    LETK_AINIT_LEVEL(fn, 7)

/**
 * @brief 自动初始化调用
 */
void letk_auto_init(void);

#endif  /* __LETK_AINIT_H__ */
