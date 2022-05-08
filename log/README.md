# 日志模块

## 介绍

提供通用的日志输出功能，主要特性如下：

- 可以输出日志的文件名，函数名和行号
- 可以输出5个等级的日志
- 可以宏配置允许输出的日志等级
- 可以注册输出的回调函数
- 可以使能或者禁用日志功能
- 可以直接使用printf打印日志，可以无需移植即可使用

## 配置

配置项 | 范围 | 描述
:-- | :-- | :--
LETK_LOG_ENABLE | 0/1 | 是否使能日志模块
LETK_LOG_LEVEL | 0-5 | 日志输出等级，只有大于这个等级的日志才会输出
LETK_LOG_BUF_SIZE | >0 | 日志输出buf大小，单位：字节
LETK_LOG_USE_PRINTF | 0/1 | 是否使用printf函数来打印日志，否则需要用户自己实现打印接口

【注意】

`LETK_LOG_LEVEL`不要直接使用数字来配置，要使用如下的宏

宏名称 | 值 | 描述
:-- | :-- | :--
LETK_LOG_LEVEL_ALL | 0 | 输出全部日志
LETK_LOG_LEVEL_DEBUG | 0 | 输出调试信息+关键信息+警告信息+错误信息+用户日志
LETK_LOG_LEVEL_INFO | 1 | 输出关键信息+警告信息+错误信息+用户日志
LETK_LOG_LEVEL_WARNING | 2 | 输出警告信息+错误信息+用户日志
LETK_LOG_LEVEL_ERROR | 3 | 输出错误信息+用户日志
LETK_LOG_LEVEL_USER | 4 | 仅输出用户日志（最高等级的日志，用于特别重要的地方）
LETK_LOG_LEVEL_NONE | 5 | 不输出任何日志

`LETK_LOG_ENABLE`设置为`0`和`LETK_LOG_LEVEL`设置为`LETK_LOG_LEVEL_NONE`一样的效果，未来可能会删除掉`LETK_LOG_ENABLE`

`LETK_LOG_BUF_SIZE`选取需要注意不能太小，可能输出会溢出被截断，但也不能太大，利用率低，浪费空间，建议【64-256】字节

## 移植

使用printf打印日志无需移植，直接使用，下面主要说明自定义输出的移植

1. 需要有一个支持输出的驱动，可以是串口、RAM区、文件、Flash等
2. 需要写一个日志输出的回调函数
3. 需要往日志系统注册此回调函数

## 使用

在需要输出日志的地方调用相应的API即可

## API

### 日志输出宏

- LETK_LOG_DEBUG(...)
- LETK_LOG_INFO(...)
- LETK_LOG_WARNING(...)
- LETK_LOG_ERROR(...)
- LETK_LOG_USER(...)

使用和printf一模一样，直接采用各种格式符输出信息即可

### 日志创建宏

- LETK_LOG_NEW(LVL_TAG, ...)，`LVL_TAG`可选{ `DEBUG`，`INFO`，`WARNING`，`ERROR`，`USER` }

可以采用`#define XXX_LOG_DEBUG(...) LETK_LOG_NEW(DEBUG, __VA_ARGS__)`创建新的日志宏

## 注意事项

- 合理输出日志，不要输出的太频繁，会占用系统资源
- 尽量分多等级输出，调试后尽量调低日志等级，屏蔽不重要的日志
- 不要打印太长的字符串，防止溢出截断
- 尽量不要打印中文，因为各种编码问题会让你崩溃（本库采用UTF8编码，建议客户代码和此库一直）

## 规划

- 提供更加简洁的接口
- 更加轻量，方便低速小容量MCU使用

## 缺点

- 不支持动态修改日志等级
- 为了兼容，没有输出颜色控制
- 依赖`stdio.h`的标准库中的打印相关函数，不是特别轻量和高效
