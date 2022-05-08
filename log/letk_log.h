/*******************************************************************************
** 文件名称：letk_log.h
** 文件作用：日志模块头文件
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
#ifndef __LETK_LOG_H__
#define __LETK_LOG_H__

#include "letk_log_cfg.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* 日志等级 */
#define LETK_LOG_LEVEL_ALL      0   /* 输出全部日志 */
#define LETK_LOG_LEVEL_DEBUG    0   /* 调试信息 */
#define LETK_LOG_LEVEL_INFO     1   /* 关键信息 */
#define LETK_LOG_LEVEL_WARNING  2   /* 警告信息 */
#define LETK_LOG_LEVEL_ERROR    3   /* 错误信息 */
#define LETK_LOG_LEVEL_USER     4   /* 用户日志信息 */
#define LETK_LOG_LEVEL_NONE     5   /* 不输出任何日志 */

#ifndef LETK_LOG_ENABLE
#define LETK_LOG_ENABLE         0
#endif  /* LETK_LOG_ENABLE */

#ifndef LETK_LOG_LEVEL
#define LETK_LOG_LEVEL          LETK_LOG_LEVEL_NONE
#endif  /* LETK_LOG_LEVEL */

#if LETK_LOG_ENABLE && LETK_LOG_LEVEL < LETK_LOG_LEVEL_NONE

/* 日志等级数据类型 */
typedef int8_t letk_log_level_t;

#ifndef LETK_LOG_BUF_SIZE
#define LETK_LOG_BUF_SIZE       256
#endif  /* LETK_LOG_BUF_SIZE */

#ifndef LETK_LOG_USE_PRINTF
#define LETK_LOG_USE_PRINTF     0
#endif  /* LETK_LOG_USE_PRINTF */

#if !LETK_LOG_USE_PRINTF
/**
 * 日志打印回调函数
 * 输入参数依次为：日志等级、当前代码文件路径、当前代码行号、当前函数名称、日志描述
 */
typedef void letk_log_print_cb_t(letk_log_level_t level, const char* file, const uint32_t line, const char* func, const char* str);

/**
 * @brief       注册日志打印回调函数
 * @param[in]   print_cb - 打印日志回调函数指针
 * @return      无
 */
void letk_log_register_print_cb(letk_log_print_cb_t *print_cb);
#endif  /* !LETK_LOG_USE_PRINTF */

/**
 * @brief       输出一条日志，此函数内部宏使用，用户不要直接使用
 * @param[in]   level - 日志等级
 * @param[in]   file  - 当前代码路径
 * @param[in]   line  - 当前代码行号
 * @param[in]   func  - 当前代码函数名
 * @param[in]   fmt   - 格式化字符串
 * @param[in]   ...   - 可变参数，fmt中的格式排列
 * @return      无
 */
void _letk_log_output(letk_log_level_t level, const char* file, int line, const char* func, const char* fmt, ...);

/* 日志输出宏 */
#if LETK_LOG_LEVEL <= LETK_LOG_LEVEL_DEBUG
#define LETK_LOG_DEBUG(...)     _letk_log_output(LETK_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LETK_LOG_DEBUG(...)
#endif

#if LETK_LOG_LEVEL <= LETK_LOG_LEVEL_INFO
#define LETK_LOG_INFO(...)      _letk_log_output(LETK_LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LETK_LOG_INFO(...)
#endif

#if LETK_LOG_LEVEL <= LETK_LOG_LEVEL_WARNING
#define LETK_LOG_WARNING(...)   _letk_log_output(LETK_LOG_LEVEL_WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LETK_LOG_WARNING(...)
#endif

#if LETK_LOG_LEVEL <= LETK_LOG_LEVEL_ERROR
#define LETK_LOG_ERROR(...)     _letk_log_output(LETK_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LETK_LOG_ERROR(...)
#endif

#if LETK_LOG_LEVEL <= LETK_LOG_LEVEL_USER
#define LETK_LOG_USER(...)      _letk_log_output(LETK_LOG_LEVEL_USER, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LETK_LOG_USER(...)
#endif

#else   /* LETK_LOG_ENABLE && LETK_LOG_LEVEL < LETK_LOG_LEVEL_NONE */

/* 啥都不做 */
#define LETK_LOG_DEBUG(...)
#define LETK_LOG_INFO(...)
#define LETK_LOG_WARNING(...)
#define LETK_LOG_ERROR(...)
#define LETK_LOG_USER(...)

#endif  /* LETK_LOG_ENABLE && LETK_LOG_LEVEL < LETK_LOG_LEVEL_NONE */

/* 创建一个新的log */
#define LETK_LOG_NEW(LVL_TAG, ...)  LETK_LOG_ ## LVL_TAG(__VA_ARGS__)

/* 创建相应的日志 */
#define LETK_LOG_DEBUG_NEW(...)     LETK_LOG_NEW(DEBUG, __VA_ARGS__)
#define LETK_LOG_INFO_NEW(...)      LETK_LOG_NEW(INFO, __VA_ARGS__)
#define LETK_LOG_WARNING_NEW(...)   LETK_LOG_NEW(WARNING, __VA_ARGS__)
#define LETK_LOG_ERROR_NEW(...)     LETK_LOG_NEW(ERROR, __VA_ARGS__)
#define LETK_LOG_USER_NEW(...)      LETK_LOG_NEW(USER, __VA_ARGS__)

#ifdef __cplusplus
}   /* extern "C" */
#endif  /* __cplusplus */

#endif  /* __LETK_LOG_H__ */
