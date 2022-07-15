/***********************************************************************************************************************
** 文件描述：日志模块头文件
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
#ifndef __LETK_LOG_H__
#define __LETK_LOG_H__

#include "letk_log_cfg.h"

/* 日志等级 */
#define LETK_LOG_LEVEL_DEBUG    0   /* 调试信息 */
#define LETK_LOG_LEVEL_INFO     1   /* 关键信息 */
#define LETK_LOG_LEVEL_WARNING  2   /* 警告信息 */
#define LETK_LOG_LEVEL_ERROR    3   /* 错误信息 */
#define LETK_LOG_LEVEL_NONE     4   /* 不打印任何日志 */

/* 取默认日志等级 */
#ifndef LETK_LOG_LEVEL
#define LETK_LOG_LEVEL          LETK_LOG_LEVEL_NONE
#endif  /* LETK_LOG_LEVEL */

/* 是否使能LOG功能 */
#define LETK_LOG_ENABLE         (LETK_LOG_LEVEL < LETK_LOG_LEVEL_NONE)
/* 是否使能DEBUG信息 */
#define LETK_LOG_DEBUG_ENABLE   (LETK_LOG_LEVEL <= LETK_LOG_LEVEL_DEBUG)
/* 是否使能INFO信息 */
#define LETK_LOG_INFO_ENABLE    (LETK_LOG_LEVEL <= LETK_LOG_LEVEL_INFO)
/* 是否使能WARNING信息 */
#define LETK_LOG_WARNING_ENABLE (LETK_LOG_LEVEL <= LETK_LOG_LEVEL_WARNING)
/* 是否使能ERROR信息 */
#define LETK_LOG_ERROR_ENABLE   (LETK_LOG_LEVEL <= LETK_LOG_LEVEL_ERROR)

#if LETK_LOG_ENABLE

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

#endif  /* LETK_LOG_ENABLE */

/* 日志输出宏 */
#if LETK_LOG_DEBUG_ENABLE
#define LETK_LOG_DEBUG(...)     letk_log_output(LETK_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else   /* LETK_LOG_DEBUG_ENABLE */
#define LETK_LOG_DEBUG(...)     (void)(0)
#endif  /* LETK_LOG_DEBUG_ENABLE */

#if LETK_LOG_INFO_ENABLE
#define LETK_LOG_INFO(...)      letk_log_output(LETK_LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else   /* LETK_LOG_INFO_ENABLE */
#define LETK_LOG_INFO(...)      (void)(0)
#endif  /* LETK_LOG_INFO_ENABLE */

#if LETK_LOG_WARNING_ENABLE
#define LETK_LOG_WARNING(...)   letk_log_output(LETK_LOG_LEVEL_WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else   /* LETK_LOG_WARNING_ENABLE */
#define LETK_LOG_WARNING(...)   (void)(0)
#endif  /* LETK_LOG_WARNING_ENABLE */

#if LETK_LOG_ERROR_ENABLE
#define LETK_LOG_ERROR(...)     letk_log_output(LETK_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else   /* LETK_LOG_ERROR_ENABLE */
#define LETK_LOG_ERROR(...)     (void)(0)
#endif  /* LETK_LOG_ERROR_ENABLE */

#if LETK_LOG_ASSERT_ENABLE && LETK_LOG_ERROR_ENABLE
/* 断言宏，归为错误，必须开启错误宏后才生效 */
#define LETK_LOG_ASSERT(expr)   do                  \
                                {                   \
                                    if (!(expr))    \
                                    {               \
                                        letk_log_output(LETK_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, #expr);\
                                        for (;;);   \
                                    }               \
                                } while (0)
#else   /* LETK_LOG_ASSERT_ENABLE && LETK_LOG_ERROR_ENABLE */
#define LETK_LOG_ASSERT(expr)   (void)(0)
#endif  /* LETK_LOG_ASSERT_ENABLE && LETK_LOG_ERROR_ENABLE */

/* 通用日志打印宏，lvl_tag：打印等级，可取：DEBUG/WARNING/INFO/ERROR */
#define LETK_LOG(lvl_tag, ...)  LETK_LOG_ ## lvl_tag(__VA_ARGS__)

#endif  /* __LETK_LOG_H__ */
