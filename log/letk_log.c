/*******************************************************************************
** 文件名称：letk_log.c
** 文件作用：日志模块源文件
** 编写作者：Tom Free 付瑞彪
** 编写时间：2022-05-03
** 文件备注：
** 更新记录：
**           2022-05-03 -> 创建文件
**                                                            <Tom Free 付瑞彪>
**
**           Copyright (c) 2018-2022 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/

#include "letk_log.h"

#if LETK_LOG_ENABLE && LETK_LOG_LEVEL < LETK_LOG_LEVEL_NONE

#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#if LETK_LOG_USE_PRINTF
#include <stdio.h>
#endif  /* LETK_LOG_USE_PRINTF */

#ifdef __cplusplus
extern 'C' {
#endif  /* __cplusplus */

/* 日志的缓存，用于输出日志 */
static char log_buf[LETK_LOG_BUF_SIZE];

#if !LETK_LOG_USE_PRINTF
/* 日志打印回调函数指针 */
static letk_log_print_cb_t *user_print_cb = NULL;

/**
 * @brief 注册日志打印回调函数
 * @param[in] print_cb 打印日志回调函数指针
 */
void letk_log_register_print_cb(letk_log_print_cb_t *print_cb)
{
    user_print_cb = print_cb;
}
#endif  /* !LETK_LOG_USE_PRINTF */

/**
 * @brief 输出一条日志，此函数内部宏使用，用户不要直接使用
 * @param[in] level 日志等级
 * @param[in] file 当前代码路径
 * @param[in] line 当前代码行号
 * @param[in] func 当前代码函数名
 * @param[in] fmt 格式化字符串
 * @param[in] ... 可变参数，fmt中的格式排列
 */
void _letk_log_output(letk_log_level_t level, const char* file, int line, const char* func, const char* fmt, ...)
{
    if (level >= LETK_LOG_LEVEL_NONE)
    {
        /* 无效的等级 */
        return;
    }

    if (level >= LETK_LOG_LEVEL)
    {
        va_list args;
        va_start(args, fmt);
        vsnprintf(log_buf, sizeof(log_buf), fmt, args);
        log_buf[LETK_LOG_BUF_SIZE - 1] = 0;
        va_end(args);

#if LETK_LOG_USE_PRINTF
        /* 寻找文件名，忽略路径 */
        size_t p, len;
        for (p = strlen(file); p > 0; p--)
        {
            if ((file[p] == '/') || (file[p] == '\\'))
            {
                p++;    /* 忽略斜杠 */
                break;
            }
        }
        static const char* prefix[] = { ".", "*", "!", "?", "-" };
        printf("%s %s", prefix[level], log_buf);
        len = strlen(log_buf);
        if (len < 32)
        {
            memset(log_buf, ' ', 32 - len);
            log_buf[32 - len] = 0;
            printf(log_buf);
        }
        printf("[%s:%d:%s]\r\n", &file[p], line, func);
#else   /* LETK_LOG_USE_PRINTF */
        /* 自定义打印 */
        if (user_print_cb)
        {
            user_print_cb(level, file, line, func, log_buf);
        }
#endif  /* LETK_LOG_USE_PRINTF */
    }
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* LETK_LOG_ENABLE && LETK_LOG_LEVEL < LETK_LOG_LEVEL_NONE */
