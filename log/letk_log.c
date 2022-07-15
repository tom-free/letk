/***********************************************************************************************************************
** 文件描述：日志模块源文件
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

#include "letk_log.h"

#if LETK_LOG_LEVEL < LETK_LOG_LEVEL_NONE

#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern 'C' {
#endif  /* __cplusplus */

/* 日志前缀 */
static const char* const log_prefix[LETK_LOG_LEVEL_NUM] =
{
    [LETK_LOG_LEVEL_DEBUG]   = "[D] ",
    [LETK_LOG_LEVEL_INFO]    = "[I] ",
    [LETK_LOG_LEVEL_WARNING] = "[W] ",
    [LETK_LOG_LEVEL_ERROR]   = "[E] ",
};
/* 日志的缓存，用于输出日志 */
static char log_buf[LETK_LOG_BUF_SIZE];

/* 日志打印回调函数指针 */
static letk_log_puts_cb_t* letk_log_puts_cb = NULL;
/* 日志输出开始回调函数 */
static letk_log_hook_cb_t* letk_log_start_cb = NULL;
/* 日志输出结束回调函数 */
static letk_log_hook_cb_t* letk_log_end_cb = NULL;

/**
 * @brief 日志系统初始化
 * @param[in] puts_cb 字符串输出回调函数
 * @param[in] end_cb 输出结束回调函数，不用可以置为NULL
 */
void letk_log_init(letk_log_puts_cb_t* puts_cb)
{
    letk_log_puts_cb = puts_cb;
}

/**
 * @brief 设置日志系统钩子回调函数
 * @param[in] start_cb 开始日志输出时的钩子回调函数，不用可以置为NULL
 * @param[in] end_cb 结束日志输出时的钩子回调函数，不用可以置为NULL
 */
void letk_log_set_hook_cb(letk_log_hook_cb_t* start_cb, letk_log_hook_cb_t* end_cb)
{
    letk_log_start_cb = start_cb;
    letk_log_end_cb = end_cb;
}

/**
 * @brief 输出一条日志，此函数内部宏使用，用户不要直接使用
 * @param[in] level 日志等级
 * @param[in] file 当前代码路径
 * @param[in] line 当前代码行号
 * @param[in] func 当前代码函数名
 * @param[in] fmt 格式化字符串
 * @param[in] ... 可变参数，fmt中的格式排列
 */
void letk_log_output(letk_log_level_t level, const char* file, int line, const char* func, const char* fmt, ...)
{
    int index, length, remain, posit;
    va_list args;

    if (level >= LETK_LOG_LEVEL_NUM)
    {
        return;
    }

    /* 日志开始钩子回调函数 */
    if (letk_log_start_cb)
    {
        letk_log_start_cb();
    }

    /* 初始化参数 */
    index = 0;
    remain = sizeof(log_buf) - 1;

    /* 输出前缀 */
    length = snprintf(&log_buf[index], remain, log_prefix[level]);
    index += length;
    remain -= length;

    /* 寻找文件名，忽略路径 */
    length = strlen(file);
    for (posit = length; posit > 0; posit--)
    {
        if ((file[posit] == '/') || (file[posit] == '\\'))
        {
            posit++;
            break;
        }
    }

    /* 输出文件信息 */
    length = snprintf(&log_buf[index], remain, "[%s:%d %s] ", &file[posit], line, func);
    index += length;
    remain -= length;

    /* 输出打印内容 */
    va_start(args, fmt);
    length = vsnprintf(&log_buf[index], remain, fmt, args);
    va_end(args);
    index += length;
    remain -= length;

    /* 输出换行 */
    snprintf(&log_buf[index], remain, "\r\n");

    /* 最后一字节补充0，防止结束符异常 */
    log_buf[sizeof(log_buf) - 1] = '\0';

#if LETK_LOG_USE_PRINTF
    /* 打印 */
    printf((const char*)log_buf);
#else   /* LETK_LOG_USE_PRINTF */
    /* 自定义打印 */
    if (letk_log_puts_cb)
    {
        letk_log_puts_cb((const char*)log_buf);
    }
#endif  /* LETK_LOG_USE_PRINTF */

    /* 接收调用钩子回调函数 */
    if (letk_log_end_cb)
    {
        letk_log_end_cb();
    }
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* LETK_LOG_LEVEL >= LETK_LOG_LEVEL_NONE */
