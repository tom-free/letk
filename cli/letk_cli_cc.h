/***********************************************************************************************************************
** 文件描述：命令行接口模块编译器自动适配文件
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
#ifndef __LETK_CLI_CC_H__
#define __LETK_CLI_CC_H__

#include "letk_cli_cfg.h"

#if LETK_CLI_ENABLE

/* 编译器支持列举 */
#define LETK_CLI_CC_NULL            0x00u   /* 不支持的编译器 */
#define LETK_CLI_CC_MDK_ARM         0x01u   /* MDK for ARM (Keil) */
#define LETK_CLI_CC_IAR_ARM         0x02u   /* IAR for ARM */
#define LETK_CLI_CC_IAR_STM8        0x03u   /* IAR for STM8 */

#if LETK_CLI_CMD_REG_BY_CC_SECTION

/* 编译器自动识别，不能保证100%识别正确，需要不断优化 */
#if defined (__IAR_SYSTEMS_ICC__) && defined (__ICCARM__)
/* IAR for ARM */
#define LETK_CLI_CC                 LETK_CLI_CC_IAR_ARM
#elif defined (__IAR_SYSTEMS_ICC__) && defined (__ICCSTM8__)
/* IAR for STM8 */
#define LETK_CLI_CC                 LETK_CLI_CC_IAR_STM8
#elif defined (__CC_ARM) || defined (__CLANG_ARM)
/* MDK for ARM （AC5，采用ARMCC编译器） */
#define LETK_CLI_CC                 LETK_CLI_CC_MDK_ARM
#elif defined (__GNUC__) && defined (__arm__)
/* MDK for ARM (AC6，采用GCC编译器) */
#define LETK_CLI_CC                 LETK_CLI_CC_MDK_ARM
#else
/* 不支持的编译器，采用静态命令注册 */
#define LETK_CLI_CC                 LETK_CLI_CC_NULL
/* 错误提式 */
#error Not support this compiler, please register commands yourself
#endif

/* MDK for ARM，支持AC5/AC6 和 GCC for Linux，采用GCC扩展语法 */
#if LETK_CLI_CC == LETK_CLI_CC_MDK_ARM
/* 存放到CLI命令段 */
#define LETK_CLI_CC_AT_CMD_SECTION      \
    __attribute__((used)) __attribute__((section("letk_cli_cmd_section")))
#endif  /* LETK_CLI_CC == LETK_CLI_CC_MDK_ARM */

/* IAR for STM8/ARM，都是采用ICC编译器，且扩展规则一样 */
#if LETK_CLI_CC == LETK_CLI_CC_IAR_STM8 || LETK_CLI_CC == LETK_CLI_CC_IAR_ARM
/* 定义CLI命令段 */
#pragma section="letk_cli_cmd_section"
/* 存放到CLI命令段 */
#define LETK_CLI_CC_AT_CMD_SECTION      \
    _Pragma("location = \"letk_cli_cmd_section\"") __root
#endif  /* LETK_CLI_CC == LETK_CLI_CC_IAR_STM8 || LETK_CLI_CC == LETK_CLI_CC_IAR_ARM */

#else   /* LETK_CLI_CMD_REG_BY_CC_SECTION */

/* 静态命令注册 */
#define LETK_CLI_CC                 LETK_CLI_CC_NULL

#endif  /* LETK_CLI_CMD_REG_BY_CC_SECTION */

#endif  /* LETK_CLI_ENABLE */

#endif  /* __LETK_CLI_CC_H__ */
