/*******************************************************************************
** 文件名称：letk_log_cfg.h
** 文件作用：日志模块配置文件
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
#ifndef __LETK_LOG_CFG_H__
#define __LETK_LOG_CFG_H__

/* 是否使能日志模块 */
#define LETK_LOG_ENABLE         1
/* 日志输出等级，只有大于这个等级的日志才会输出 */
#define LETK_LOG_LEVEL          LETK_LOG_LEVEL_ALL
/* 日志输出buf大小，单位：字节 */
#define LETK_LOG_BUF_SIZE       256
/* 是否使用printf函数来打印日志，否则需要用户自己实现打印接口 */
#define LETK_LOG_USE_PRINTF     1

#endif  /* __LETK_LOG_CFG_H__ */
