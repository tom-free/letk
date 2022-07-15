/***********************************************************************************************************************
** 文件描述：自动初始化管理头文件
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
#ifndef __LETK_AINIT_H__
#define __LETK_AINIT_H__

#define  ANONY_CONN(type, var, line)  type  var##line
#define  ANONY_DEF(type,prefix,line)  ANONY_CONN(type, prefix, line)
#define  ANONY_TYPE(type,prefix)      ANONY_DEF(type, prefix, __LINE__)

#define container_of(ptr, type, member) ( \
    (type *)( (char *)(ptr) - offsetof(type,member) ))

#if defined(__CC_ARM) || defined(__GNUC__) /* ARM,GCC*/
#define LETK_AINIT_CC_SECTION(x)    __attribute__((section(x)))
#define LETK_AINIT_CC_UNUSED        __attribute__((unused))
#define LETK_AINIT_CC_USED          __attribute__((used))
#define LETK_AINIT_CC_ALIGN(n)      __attribute__((aligned(n)))
#define LETK_AINIT_CC_WEAK          __attribute__((weak))
#elif defined (__ICCARM__)              /*IAR */
#define LETK_AINIT_CC_SECTION(x)    @ x
#define LETK_AINIT_CC_UNUSED
#define LETK_AINIT_CC_USED          __root
#define LETK_AINIT_CC_WEAK          __weak
#else
#error "Current tool chain haven't supported yet!"
#endif

#define LETK_AINIT_CC_USED              __attribute__((__used__))
#define LETK_AINIT_CC_SECTION(name)     __attribute__((__section__(name)))

#define LETK_AINIT_LEVEL(fn, level) \
    LETK_AINIT_CC_USED static const void* _letk_ainit_##fn LETK_AINIT_CC_SECTION(".letk_ainit." #level) = fn;

#define pure_initcall(fn)       LETK_AINIT_LEVEL(fn, 0) //可用作系统时钟初始化
#define fs_initcall(fn)         LETK_AINIT_LEVEL(fn, 1) //tick和调试接口初始化
#define device_initcall(fn)     LETK_AINIT_LEVEL(fn, 2) //驱动初始化
#define late_initcall(fn)       LETK_AINIT_LEVEL(fn, 3) //传感器初始化


/*模块初始化项*/
typedef struct {
    const char *name;               //模块名称
    void(*init)(void);             //初始化接口
}init_item_t;

#define __module_initialize(name,func,level)           \
    LETK_AINIT_CC_USED ANONY_TYPE(const init_item_t, init_tbl_##func)\
    LETK_AINIT_CC_SECTION("init.item."level) = {name,func}

/**
 * @brief 自动初始化调用
 */
void letk_auto_init(void);

pure_initcall(letk_auto_init);
__module_initialize("init", letk_auto_init, 0);

#endif  /* __LETK_AINIT_H__ */
