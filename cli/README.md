# 通用嵌入式跨平台命令行接口（GM_CLI）

```cmd

-------------------------------------------------
[GM CLI] > help test
-------------------------------------------------

-------------------------------------------------

-------------------------------------------------

```

## 特性

> 1. 带有命令导出功能，修改添加命令无需修改命令行相关代码，只需要导出新的命令即可</br>
> 2. 支持命令重命名，可以使用英文符号作为命令名来精简指令</br>
> 3. 带有历史记录功能，用户可以配置记录条数</br>
> 4. 带有命令自动补全功能，输入时更加的方便快捷</br>
> 5. 代码占用少，执行效率高，无其他模块依赖</br>
> 6. 采用回调机制，用户只需要集中精力在命令的实现上，无需自己解析命令</br>
> 7. 带有系统默认命令，可以快速的查询和测试CLI系统</br>
> 8. 字符的发送采用注册机制，用户可以进行重定向和文件操作，完全自定义</br>
> 9. 字符的接收完全用户决定，可以进行文件操作或流读取，可以轻松实现脚本解释器的功能</br>
> 10. 支持静态命令注册功能，只需要用户定义一个全局数组即可</br>
> 11. 支持多种编译器自动识别生成相应的命令导出宏</br>

## 计划

> 1. 添加参数的自动补全功能
> 2. 添加模仿Linux命令的可选参数和复合参数等功能
> 3. 添加函数执行指令，便于代码调试

## 使用说明

1. 准备一个工程，带有字符输入输出功能或模拟数据输入输出功能即可</br>
2. 下载此代码到工程源码目录</br>
3. 添加 `gm_cli.c` 文件到工程，具体添加方式不同编译器不同</br>
4. 添加 `gm_cli.h` 所在的目录到编译头文件包含目录，具体添加方式不同编译器不同</br>
5. 采用静态命令注册功能需要在用户源文件定义如下数组，注意结尾一定要全部为NULL</br>

```C
/* 静态命令表 */
const gm_cli_cmd_t gm_cli_static_cmds[] =
{
    {
        .name  = "help",
        .usage = "help [cmd-name] -- list the command and usage",
        .cb    = gm_cli_internal_cmd_help,
        .link  = NULL,
    },
    {
        .name  = "?",
        .usage = NULL,
        .cb    = NULL,
        .link  = &gm_cli_static_cmds[0],
    },
    {
        .name  = "history",
        .usage = "history [num] -- list the history command",
        .cb    = gm_cli_internal_cmd_history,
        .link  = NULL,
    },
    {
        .name  = "test",
        .usage = "test [args] -- test the cli",
        .cb    = gm_cli_internal_cmd_test,
        .link  = NULL,
    },
    /* 下面添加自己的命令 */
    {
        .name  = "xxx",
        .usage = "xxx",
        .cb    = xxx_cb,
        .link  = xxx,
    },
    ......

    /* 数组末尾一定要以下面的元素结束 */
    {
        .name  = NULL,
        .usage = NULL,
        .cb    = NULL,
        .link  = NULL,
    },
};
```

6. 编译代码，解决一些因编译器不同产生的错误或警告，如有没法解决的错误或警告，请查询编译器手册或发起issue到本代码库
7. main函数while之前或主任务运行之前添加如下初始化代码（your_out_char_cb需要填入自己的字符输出驱动；your_prompt需要填入自定义的提示符，不调用此函数系统采用默认的提示符）

```C
/* 初始化CLI管理器 */
gm_cli_mgr_init();
/* 注册输出驱动 */
gm_cli_set_out_char_cb((gm_cli_out_char_cb_t*)your_out_char_cb);
/* 设置提示符 */
gm_cli_set_cmd_prompt("[your_prompt] > ");
/* 启动CLI */
gm_cli_start();
```

8. 初始化后先采用如下代码测试CLI是否正常

```C
const char str[] = "test 1 2 3\n";
unsigned int len = (unsigned int)strlen(str);
unsigned int i;
for (i = 0; i < len; i++)
{
    gm_cli_parse_char(str[i]);
}
```

如果输出如下表示CLI正常

```bash
cmd  -> test
arg1 -> 1
arg2 -> 2
arg3 -> 3
```

9. main函数while中或主任务或建立一个CLI任务，执行以下代码（_kbhit替换成自己的字符输入检测函数，_getch替换成自己的子符读取函数）

```C
/* 键盘检测或数据队列查询等 */
if (_kbhit())
{
    /* 读取字符 */
    ch = _getch();
    /* 解析字符 */
    gm_cli_parse_char((char)ch);
}
```

10. 编译代码，下载或进入调试，打开相应数据输入终端，按回车键查看是否有提示符输出，可以输入`test`或`help`指令检测CLI是否正常

## 添加命令

1. 命名回调函数格式如下：

```C
int command_callback(int argc, char* argv[]);
```

argc - 表示输入的命令行命令和参数的总个数，含有命令本身，所以此参数最小值为1

argv - 表示命令参数的内容，采用指针存放，类型都是`char`型的指针即字符串地址

2. 实现回调函数后只需要执行如下的导出命令即可

```C
GM_CLI_CMD_EXPORT(command_name, "command_usage_string", command_callback);
```

如果需要给这个命令设置别名，可在上面的指令后再使用以下命令（可重复使用设置多个别名）

```C
GM_CLI_CMD_ALIAS(command_name, "command_alias");
```

command_name - 命令的名字，这个名字就是命令的识别名，输入搜索的就是这个名字（不要加双引号，不能有空格等其它特殊字符）

command_usage_string - 命令的使用说明字符串，用于`help`命令给用户提示使用说明（需要加双引号，可以包含特殊字符，控制字符注意转义）

command_callback - 命令回调函数

command_alias - 命令别名字符串（需要加双引号，可以包含特殊字符，但不能包含空格和控制字符，必须是可显示字符）

3. 编译代码，下载调试即可使用此命令，命令详细使用说明可输入`help command_name`查看，也可直接使用`help`查看系统当前支持的所有命令

## 默认命令

<table>
  <tr>
    <th>命令名</th>
    <th>别名</th>
    <th>使用说明</th>
    <th>作用</th>
  </tr>
  <tr>
    <td rowspan="2">help</td>
    <td rowspan="2">?</td>
    <td>help</td>
    <td>列出全部已注册的命令</td>
  </tr>
  <tr>
    <td>help [cmd]</td>
    <td>列出指定命令的详细说明</td>
  </tr>
  <tr>
    <td rowspan="2">history</td>
    <td rowspan="2">无</td>
    <td>history</td>
    <td>查看全部存储的历史记录</td>
  </tr>
  <tr>
    <td>history [num]</td>
    <td>查看指定数量的历史记录</td>
  </tr>
  <tr>
    <td>test</td>
    <td>无</td>
    <td>test [...]</td>
    <td>测试CLI系统是否正常，后接可变长任意参数</td>
  </tr>
</table>

## 分支描述

分支 | 描述
:-: | :--
`master` | 初版命令行，支持VS/IAR/KEIL编译器，采用驼峰命名法，不支持静态命令注册，下一版会和`develop`分支合并
`develop` | 开发中命令行，主线版本，采用linux的全小写命名法，支持更多的编译器，支持静态命令注册，支持和计划支持更多新特性
`static-reg-cmd` | `master`分支上添加静态命令注册方式，由于`master`分支之后会被`develop`分支合并取代，所以独立一个分支，提供给功能需求较少和容量有限的单片机使用

## 特别感谢

用户名 | 开源地址首页 | 贡献内容
:-:  | :-: | :-- 
**徐煜** | https://gitee.com/dog_who_loves_cat_mint | `static-reg-cmd`分支添加命令静态注册方式

## 特殊说明

大家有什么建议或疑问请邮件或直接在仓库进行留言，谢谢大家的参与和代码的共同维护，也希望大家将自己的优秀的计划加入代码中并发出pull request
