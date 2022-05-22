/*******************************************************************************
** @file   letk_ibutton_cfg.h
** @brief  独立按键管理配置文件
** @author 付瑞彪
** @date   2022-05-22
**
**       Copyright (c) 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/
#ifndef __LETK_IBUTTON_CFG_H__
#define __LETK_IBUTTON_CFG_H__

/* 按键轮询时间间隔，用于系统的相关时间计算，单位:ms */
#define LETK_IBUTTON_POLL_INTERVAL      10
/* 默认的消抖时间滴答数，以上面的间隔为基准 */
#define LETK_IBUTTON_DEBOUNCE_TICKS     2
/* 默认的连击最长间隔时间滴答数，超过此时间认为连击被终结，换算成滴答数，以上面的间隔为基准 */
#define LETK_IBUTTON_HIT_AGIN_TICKS     30
/* 默认的长按触发时间，超过此时间认为进入长按，换算成滴答数，以上面的间隔为基准 */
#define LETK_IBUTTON_LONG_TICKS         100
/* 默认的长按重复触发时间，即隔多长时间触发一次，换算成滴答数，以上面的间隔为基准 */
#define LETK_IBUTTON_REPEAT_TRIG_TICKS  10
/* 默认的连击最大次数，注意不要超过数据最大范围（8位无符号） */
#define LETK_IBUTTON_CLICK_COUNT_MAX    200

#endif	/* __LETK_IBUTTON_CFG_H__ */
