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

/* 日志等级 */
#define LETK_LOG_LEVEL_DEBUG    0   /* 调试信息 */
#define LETK_LOG_LEVEL_INFO     1   /* 关键信息 */
#define LETK_LOG_LEVEL_WARNING  2   /* 警告信息 */
#define LETK_LOG_LEVEL_ERROR    3   /* 错误信息 */
#define LETK_LOG_LEVEL_NONE     4   /* 不打印任何日志 */
#define LETK_LOG_LEVEL_NUM      4   /* 日志等级数量 */

#ifndef LETK_LOG_LEVEL
#define LETK_LOG_LEVEL          LETK_LOG_LEVEL_NONE
#endif  /* LETK_LOG_LEVEL */

/* 是否使能LOG功能 */
#define LETK_LOG_ENABLE         (LETK_LOG_LEVEL < LETK_LOG_LEVEL_NONE)

#if LETK_LOG_LEVEL < LETK_LOG_LEVEL_NONE

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* 日志等级数据类型 */
typedef int8_t letk_log_level_t;

#ifndef LETK_LOG_BUF_SIZE
#define LETK_LOG_BUF_SIZE       256
#endif  /* LETK_LOG_BUF_SIZE */

#ifndef LETK_LOG_USE_PRINTF
#define LETK_LOG_USE_PRINTF     0
#endif  /* LETK_LOG_USE_PRINTF */

#if !LETK_LOG_USE_PRINTF
/* 日志字符串输出回调函数 */
typedef void letk_log_puts_cb_t(const char* str);
/* 日志输出钩子回调函数，提供钩子，用户可以实现高级功能，例如命令行的再现 */
typedef void letk_log_hook_cb_t(void);

/**
 * @brief 日志系统初始化
 * @param[in] puts_cb 字符串输出回调函数
 * @param[in] end_cb 输出结束回调函数，不用可以置为NULL
 */
void letk_log_init(letk_log_puts_cb_t* puts_cb);
#endif  /* !LETK_LOG_USE_PRINTF */

/**
 * @brief 设置日志系统钩子回调函数
 * @param[in] start_cb 开始日志输出时的钩子回调函数，不用可以置为NULL
 * @param[in] end_cb 结束日志输出时的钩子回调函数，不用可以置为NULL
 */
void letk_log_set_hook_cb(letk_log_hook_cb_t* start_cb, letk_log_hook_cb_t* end_cb);

/**
 * @brief 输出一条日志，此函数内部宏使用，用户不要直接使用
 * @param[in] level 日志等级
 * @param[in] file 当前代码路径
 * @param[in] line 当前代码行号
 * @param[in] func 当前代码函数名
 * @param[in] fmt 格式化字符串
 * @param[in] ... 可变参数，fmt中的格式排列
 */
void letk_log_output(letk_log_level_t level, const char* file, int line, const char* func, const char* fmt, ...);

#ifdef __cplusplus
}   /* extern "C" */
#endif  /* __cplusplus */

#endif  /* LETK_LOG_LEVEL < LETK_LOG_LEVEL_NONE */

/* 日志输出宏 */
#if LETK_LOG_LEVEL <= LETK_LOG_LEVEL_DEBUG
#define LETK_LOG_DEBUG(...)     letk_log_output(LETK_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LETK_LOG_DEBUG(...)     (void)(0)
#endif

#if LETK_LOG_LEVEL <= LETK_LOG_LEVEL_INFO
#define LETK_LOG_INFO(...)      letk_log_output(LETK_LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LETK_LOG_INFO(...)      (void)(0)
#endif

#if LETK_LOG_LEVEL <= LETK_LOG_LEVEL_WARNING
#define LETK_LOG_WARNING(...)   letk_log_output(LETK_LOG_LEVEL_WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LETK_LOG_WARNING(...)   (void)(0)
#endif

#if LETK_LOG_LEVEL <= LETK_LOG_LEVEL_ERROR
#define LETK_LOG_ERROR(...)     letk_log_output(LETK_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LETK_LOG_ERROR(...)     (void)(0)
#endif

/* 日志打印 */
#define LETK_LOG(lvl_tag, ...)  LETK_LOG_ ## lvl_tag(__VA_ARGS__)

#endif  /* __LETK_LOG_H__ */
