/***********************************************************************************************************************
** 文件描述：自动初始化管理源文件
** 创建作者：付瑞彪(Tom Free)
** 创建日期：2022年7月16日
** 编码格式：UTF-8编码
** 编程语言：C语言，C99标准
** 缩进格式：4个空格键
** 命名规范：下划线命名法(小写命名法)
** 开源许可：MIT许可证，参考：https://mit-license.org
** 版权信息：Copyright (c) 2013-2022, Tom Free, <tomfreefu@gmail.com>
**
** 修改记录
** 修改日期         修改作者        修改内容
** 2022年7月16日    付瑞彪          创建文件，初次版本
**
***********************************************************************************************************************/

#include "letk_ainit.h"
#include <stddef.h>

/* 定义两个虚拟的初始化函数指示头和尾 */
static void ai_start(void) {}
LETK_AINIT_LEVEL(ai_start, 0);
static void ai_end(void) {}
LETK_AINIT_LEVEL(ai_end, 8);

/**
 * @brief 自动初始化调用
 */
void letk_auto_init(void)
{
    letk_ainit_fn_t** pfn;

    for (pfn = &_letk_ainit_ai_start; pfn < &_letk_ainit_ai_end; pfn++)
    {
        (*pfn)();
    }
}
