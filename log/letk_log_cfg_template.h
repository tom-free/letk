/*******************************************************************************
** @file   letk_log_cfg.h
** @brief  日志模块配置文件
** @author 付瑞彪
** @date   2022-05-29
**
**       Copyright (c) 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/
#ifndef __LETK_LOG_CFG_H__
#define __LETK_LOG_CFG_H__

/* 日志输出等级，只有不小于这个等级的日志才会输出 */
#define LETK_LOG_LEVEL          LETK_LOG_LEVEL_DEBUG
/* 是否使能断言功能，用于给LETK组件进行断言参数 */
#define LETK_LOG_ASSERT_ENABLE  1
/* 日志输出buf大小，单位：字节 */
#define LETK_LOG_BUF_SIZE       256
/* 是否使用printf函数来打印日志，否则需要用户自己实现打印接口 */
#define LETK_LOG_USE_PRINTF     0

#endif  /* __LETK_LOG_CFG_H__ */
