/*******************************************************************************
** @file   letk_cli.h
** @brief  命令行接口模块
** @author 付瑞彪
** @date   2022-05-29
**
**       Copyright (c) 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/
#ifndef __LETK_CLI_H__
#define __LETK_CLI_H__

#include "letk_cli_cfg.h"
#include "letk_cli_cc.h"

#if LETK_CLI_ENABLE

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* 输出字符回调函数 */
typedef void letk_cli_out_char_cb_t(const char);
/* 命令执行回调函数 */
typedef int letk_cli_cmd_cb_t(int, char*[]);

/* 命令结构定义 */
typedef struct _letk_cli_cmd_t
{
    char* name;                     /* 命令名 */
    char* usage;                    /* 使用简洁说明 */
    letk_cli_cmd_cb_t* cb;          /* 命令执行函数 */
    struct _letk_cli_cmd_t *link;   /* 链接到的命令，用于命令别名 */
} letk_cli_cmd_t;

#if LETK_CLI_CC
/* 字符串连接 */
#define LETK_CLI_CC_CONNECT2(a, b)      a ## b
#define LETK_CLI_CC_CONNECT3(a, b, c)   a ## b ## c

/* 导出命令 */
#define LETK_CLI_CMD_EXPORT(cmd_name, cmd_usage, cmd_cb)                       \
        LETK_CLI_CC_AT_CMD_SECTION static const letk_cli_cmd_t                 \
            LETK_CLI_CC_CONNECT2(letk_cli_ex_cmd_, cmd_name) =                 \
            {                                                                  \
                .name    = #cmd_name,                                          \
                .usage   = cmd_usage,                                          \
                .cb      = cmd_cb,                                             \
                .link    = NULL,                                               \
            };
/* 命令别名中转 */
#define LETK_CLI_CMD_ALIAS_NUM(cmd_name, cmd_alias_str, num)                   \
        LETK_CLI_CC_AT_CMD_SECTION static const letk_cli_cmd_t                 \
            LETK_CLI_CC_CONNECT3(letk_cli_ex_cmd_, cmd_name##_, num) =         \
            {                                                                  \
                .name    = cmd_alias_str,                                      \
                .usage   = NULL,                                               \
                .cb      = NULL,                                               \
                .link    = (letk_cli_cmd_t*)&letk_cli_ex_cmd_##cmd_name,       \
            };
/* 命令别名 */
#define LETK_CLI_CMD_ALIAS(cmd_name, cmd_alias_str) \
        LETK_CLI_CMD_ALIAS_NUM(cmd_name, cmd_alias_str, __LINE__)
#else   /* LETK_CLI_CC */
/* 以下宏用于兼容导出方式，免得需要手动删除代码 */
#define LETK_CLI_CMD_EXPORT(cmd_name, cmd_usage, cmd_cb)
#define LETK_CLI_CMD_ALIAS(cmd_name, cmd_alias_str)

/* 静态命令表，注意，结尾需要使用一个全部为NULL指针的元素来结束命令表 */
extern const letk_cli_cmd_t letk_cli_static_cmds[];

/* 以下命令需要用户手动放入letk_cli_static_cmds中 */
/* 内部命令-help */
int letk_cli_internal_cmd_help(int argc, char* argv[]);
#if LETK_CLI_HISTORY_ENABLE
/* 内部命令-history */
int letk_cli_internal_cmd_history(int argc, char* argv[]);
#endif  /* LETK_CLI_HISTORY_ENABLE */
/* 内部命令-test */
int letk_cli_internal_cmd_test(int argc, char* argv[]);
#endif  /* LETK_CLI_CMD_REG_BY_CC_SECTION */

/**
 * @brief 初始化CLI模块
 */
void letk_cli_mgr_init(void);

/**
 * @brief 设置输出字符回调函数
 * @param[in] out_char_cb 输出一个字符回调函数
 */
void letk_cli_set_out_char_cb(letk_cli_out_char_cb_t *out_char_cb);

/**
 * @brief 设置命令提示符
 * @param[in] prompt 新的命令提示符
 */
void letk_cli_set_prompt(const char* const prompt);

/**
 * @brief 启动CLI模块
 */
void letk_cli_start(void);

/**
 * @brief 打印字符
 * @param[in] ch 待打印的字符
 */
void letk_cli_put_char(const char ch);

/**
 * @brief 打印整数
 * @param[in] num 待打印的整形数据
 */
void letk_cli_put_int(const int num);

/**
 * @brief 打印字符串
 * @param[in] str 待打印的字符串
 */
void letk_cli_put_str(const char* const str);

/**
 * @brief 清除当前行的显示内容
 */
void letk_cli_clear_line(void);

/**
 * @brief 重新显示当前行的内容
 */
void letk_cli_put_line(void);

/**
 * @brief 解析一个字符
 * @param[in] ch 待解析字符
 */
void letk_cli_parse_char(const char ch);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#else   /* LETK_CLI_ENABLE */
/* 以下宏用于兼容导出方式，免得需要手动删除代码 */
#define LETK_CLI_CMD_EXPORT(cmd_name, cmd_usage, cmd_cb)
#define LETK_CLI_CMD_ALIAS(cmd_name, cmd_alias_str)
#endif  /* LETK_CLI_ENABLE */

#endif  /* __LETK_CLI_H__ */
