/*******************************************************************************
** 文件名称：letk_cli.c
** 文件作用：命令行接口
** 编写作者：Tom Free 付瑞彪
** 编写时间：2020-08-09
** 文件备注：
** 更新记录：
**           2020-08-09 -> 创建文件
**                                                             <Tom Free 付瑞彪>
**           2021-03-18 -> 修改宏来适配不同编译器
**                                                             <Tom Free 付瑞彪>
**           2021-06-17 -> 增加自动命令注册和静态注册选项
**                                                             <Tom Free 付瑞彪>
**
**              Copyright (c) 2018-2022 付瑞彪 All Rights Reserved
**
**       1 Tab == 4 Spaces     UTF-8     ANSI C Language(C99)
*******************************************************************************/

#include "letk_cli.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#if LETK_CLI_ENABLE

/* 输入状态定义 */
typedef enum
{
    LETK_CLI_INPUT_WAIT_NORMAL,       /* 等待正常字符 */
    LETK_CLI_INPUT_WAIT_SPEC_KEY,     /* 等待特殊字符 */
    LETK_CLI_INPUT_WAIT_FUNC_KEY,     /* 等待功能字符 */
} letk_cli_input_status_t;

/* CLI管理器 */
typedef struct
{
    char                    line[LETK_CLI_LINE_CHAR_MAX];   /* 一行字符串存储 */
    unsigned int            input_count;                    /* 输入的字符数量 */
    unsigned int            input_cusor;                    /* 输入的光标位置 */
    letk_cli_input_status_t input_status;                   /* 当前输入的状态 */
    letk_cli_out_char_cb_t *pf_outchar;                     /* 输出字符回调函数 */
    const int*              p_cmd_start;                    /* 命令存储区起始指针 */
    const int*              p_cmd_end;                      /* 命令存储区结束指针 */
    const char*             prompt;                         /* 命令提示符 */
#if LETK_CLI_HISTORY_ENABLE
    /* 备份字符串，用于翻历史记录时保存当前 */
    char                    backup_str[LETK_CLI_LINE_CHAR_MAX];
    char                    history_str[LETK_CLI_HISTORY_LINE_MAX][LETK_CLI_LINE_CHAR_MAX];
    unsigned int            history_total;                  /* 历史总记录条数 */
    unsigned int            history_index;                  /* 历史存储索引 */
    unsigned int            history_inquire_index;          /* 历史查询索引 */
    unsigned int            history_inquire_count;          /* 历史查询数量计数器 */
#endif  /* LETK_CLI_HISTORY_ENABLE */
} letk_cli_mgr_t;

/* CLI控制 */
static letk_cli_mgr_t letk_cli_mgr;

/* 读取下一个命令 */
static const letk_cli_cmd_t* letk_cli_get_next_cmd(const int* const addr)
{
    const int *ptr = (const int*)((char*)addr + sizeof(letk_cli_cmd_t));
    if (ptr < letk_cli_mgr.p_cmd_end)
    {
        return (const letk_cli_cmd_t*)ptr;
    }

    return NULL;
}

/* 搜索命令 */
static const letk_cli_cmd_t* letk_cli_search_cmd(const char* const cmd_name)
{
    const letk_cli_cmd_t* p_ret = NULL;
    const letk_cli_cmd_t* p_temp;

    p_temp = (letk_cli_cmd_t*)letk_cli_mgr.p_cmd_start;

    while (p_temp != NULL)
    {
        if (strcmp(p_temp->name, cmd_name) == 0)
        {
            p_ret = p_temp;
            break;
        }
        p_temp = letk_cli_get_next_cmd((const int*)p_temp);
    }

    return p_ret;
}

/* 初始化cli管理器 */
void letk_cli_mgr_init(void)
{
#if (LETK_CLI_CC == LETK_CLI_CC_MDK_ARM)
    extern const int letk_cli_cmd_section$$Base;
    extern const int letk_cli_cmd_section$$Limit;
    letk_cli_mgr.p_cmd_start = (const int*)&letk_cli_cmd_section$$Base;
    letk_cli_mgr.p_cmd_end   = (const int*)&letk_cli_cmd_section$$Limit;
#elif ((LETK_CLI_CC == LETK_CLI_CC_IAR_ARM) || (LETK_CLI_CC == LETK_CLI_CC_IAR_STM8))
    letk_cli_mgr.p_cmd_start = (const int*)__section_begin("letk_cli_cmd_section");
    letk_cli_mgr.p_cmd_end = (const int*)__section_end("letk_cli_cmd_section");
    letk_cli_mgr.p_cmd_start = (const int*)&letk_cli_static_cmds[0];
    const letk_cli_cmd_t *ptr = &letk_cli_static_cmds[0];
    while (ptr->name != NULL) ptr++;
    letk_cli_mgr.p_cmd_end = (const int*)ptr;
#endif

    letk_cli_mgr.input_count = 0;
    letk_cli_mgr.input_cusor = 0;
    letk_cli_mgr.input_status = LETK_CLI_INPUT_WAIT_NORMAL;
    letk_cli_mgr.pf_outchar = NULL;
    letk_cli_mgr.prompt = LETK_CLI_DEFAULT_CMD_PROMPT;

    memset(letk_cli_mgr.line, 0, sizeof(letk_cli_mgr.line));
#if LETK_CLI_HISTORY_ENABLE
    memset(letk_cli_mgr.backup_str, 0, sizeof(letk_cli_mgr.backup_str));
    memset(letk_cli_mgr.history_str, 0, sizeof(letk_cli_mgr.history_str));
#endif  /* LETK_CLI_HISTORY_ENABLE */
}

/* 设置输出字符回调函数 */
void letk_cli_set_out_char_cb(letk_cli_out_char_cb_t *out_char_cb)
{
    if (out_char_cb != NULL)
    {
        letk_cli_mgr.pf_outchar = out_char_cb;
    }
}

/* 设置命令提示符 */
void letk_cli_set_prompt(const char* const prompt)
{
    if (prompt != NULL)
    {
        letk_cli_mgr.prompt = prompt;
    }
    else
    {
        letk_cli_mgr.prompt = LETK_CLI_DEFAULT_CMD_PROMPT;
    }
}

/* 启动命令行 */
void letk_cli_start(void)
{
    letk_cli_put_str("\r\n");
    letk_cli_put_str(letk_cli_mgr.prompt);
}

/* 打印字符 */
void letk_cli_put_char(const char ch)
{
    if (letk_cli_mgr.pf_outchar != NULL)
    {
        letk_cli_mgr.pf_outchar(ch);
    }
}

/* 打印整数 */
void letk_cli_put_int(const int num)
{
    char buf[10];
    int i = 0;
    int temp = num;

    if (temp < 0)
    {
        letk_cli_put_char('-');
        temp = -temp;
    }

    do
    {
        buf[i++] = (temp % 10) + '0';
        temp /= 10;
    } while (temp);

    while (i)
    {
        letk_cli_put_char(buf[--i]);
    }
}

/* 打印字符串 */
void letk_cli_put_str(const char* const str)
{
    const char* ptemp = str;
    if (str == NULL)
    {
        return;
    }
    if (letk_cli_mgr.pf_outchar != NULL)
    {
        while (*ptemp)
        {
            letk_cli_mgr.pf_outchar(*ptemp);
            ptemp++;
        }
    }
}

/**
 * @brief 保存当前上下文内容并清除当前行的显示内容
 */
void letk_cli_save_context(void)
{
    unsigned int length = letk_cli_mgr.input_cusor + strlen(letk_cli_mgr.prompt);

    while (length--)
    {
        letk_cli_put_str("\b \b");
    }
}

/**
 * @brief 恢复上下文并重新显示当前行的内容
 */
void letk_cli_restore_context(void)
{
    unsigned int length;

    letk_cli_put_str(letk_cli_mgr.prompt);
    if (letk_cli_mgr.input_count)
    {
        letk_cli_put_str(letk_cli_mgr.line);
    }
    if (letk_cli_mgr.input_cusor < letk_cli_mgr.input_count)
    {
        length = letk_cli_mgr.input_count - letk_cli_mgr.input_cusor;
        while (length--)
        {
            letk_cli_put_char('\b');
        }
    }
}

#if LETK_CLI_HISTORY_ENABLE
/* 上键处理 */
static void letk_cli_parse_up_key(void)
{
    unsigned int len;

    if (letk_cli_mgr.history_total == 0)
    {
        /* 无记录 */
        return;
    }

    if (letk_cli_mgr.history_inquire_count == 0)
    {
        /* 从未上翻记录，备份当前输入 */
        memcpy(letk_cli_mgr.backup_str, letk_cli_mgr.line, sizeof(letk_cli_mgr.line));
        /* 搜索记录位置设置到当前记录处 */
        letk_cli_mgr.history_inquire_index = letk_cli_mgr.history_index;
    }

    /* 查看是否已经搜索完成 */
    if (letk_cli_mgr.history_inquire_count < letk_cli_mgr.history_total)
    {
        /* 搜索编号往前找一个记录编号 */
        if (letk_cli_mgr.history_inquire_index == 0)
        {
            letk_cli_mgr.history_inquire_index = LETK_CLI_HISTORY_LINE_MAX - 1;
        }
        else
        {
            letk_cli_mgr.history_inquire_index--;
        }
        /* 搜索数量加1 */
        letk_cli_mgr.history_inquire_count++;
        /* 光标之后的行数据清除掉 */
        len = letk_cli_mgr.input_count - letk_cli_mgr.input_cusor;
        for (unsigned int i = 0; i < len; i++)
        {
            letk_cli_put_char(' ');
        }
        /* 清除所有的行数据 */
        for (unsigned int i = 0; i < letk_cli_mgr.input_count; i++)
        {
            letk_cli_put_str("\b \b");
        }

        /* 导入历史输入 */
        memcpy(letk_cli_mgr.line, letk_cli_mgr.history_str[letk_cli_mgr.history_inquire_index], sizeof(letk_cli_mgr.line));
        letk_cli_mgr.input_count = (unsigned int)strlen(letk_cli_mgr.line);
        letk_cli_mgr.input_cusor = letk_cli_mgr.input_count;
        /* 显示历史记录 */
        letk_cli_put_str(letk_cli_mgr.line);
    }
}

/* 下键处理 */
static void letk_cli_parse_down_key(void)
{
    unsigned int len;

    if ((letk_cli_mgr.history_total == 0) ||
        (letk_cli_mgr.history_inquire_count == 0))
    {
        /* 无记录或无上翻 */
        return;
    }

    /* 查询数量减一 */
    letk_cli_mgr.history_inquire_count--;
    /* 删除当前行内容 */
    len = letk_cli_mgr.input_count - letk_cli_mgr.input_cusor;
    for (unsigned int i = 0; i < len; i++)
    {
        letk_cli_put_char(' ');
    }
    for (unsigned int i = 0; i < letk_cli_mgr.input_count; i++)
    {
        letk_cli_put_str("\b \b");
    }

    if (letk_cli_mgr.history_inquire_count == 0)
    {
        /* 恢复备份的输入 */
        memcpy(letk_cli_mgr.line, letk_cli_mgr.backup_str, sizeof(letk_cli_mgr.line));
    }
    else
    {
        /* 更新索引 */
        letk_cli_mgr.history_inquire_index++;
        letk_cli_mgr.history_inquire_index %= LETK_CLI_HISTORY_LINE_MAX;
        /* 取出历史 */
        memcpy(letk_cli_mgr.line, letk_cli_mgr.history_str[letk_cli_mgr.history_inquire_index], sizeof(letk_cli_mgr.line));
    }

    /* 重新更新坐标 */
    letk_cli_mgr.input_count = (unsigned int)strlen(letk_cli_mgr.line);
    letk_cli_mgr.input_cusor = letk_cli_mgr.input_count;
    /* 显示输入行 */
    letk_cli_put_str(letk_cli_mgr.line);
}
#endif  /* LETK_CLI_HISTORY_ENABLE */

/* 左键处理 */
static void letk_cli_parse_left_key(void)
{
    if (letk_cli_mgr.input_cusor > 0)
    {
        letk_cli_put_char('\b');
        letk_cli_mgr.input_cusor--;
    }
}

/* 右键处理 */
static void letk_cli_parse_right_key(void)
{
    if ((letk_cli_mgr.input_count > 0) &&
        (letk_cli_mgr.input_cusor < letk_cli_mgr.input_count))
    {
        letk_cli_put_char(letk_cli_mgr.line[letk_cli_mgr.input_cusor]);
        letk_cli_mgr.input_cusor++;
    }
}

/* 功能键处理返回：0 - 已处理的功能字符，-1 - 未处理的字符 */
static int letk_cli_parse_func_key(const char ch)
{
    /* XSHELL终端，超级终端等功能码 */
    if (ch == (char)0x1B)
    {
        letk_cli_mgr.input_status = LETK_CLI_INPUT_WAIT_SPEC_KEY;
        return 0;
    }
    else if (letk_cli_mgr.input_status == LETK_CLI_INPUT_WAIT_SPEC_KEY)
    {
        if (ch == (char)0x5b)
        {
            letk_cli_mgr.input_status = LETK_CLI_INPUT_WAIT_FUNC_KEY;
            return 0;
        }

        letk_cli_mgr.input_status = LETK_CLI_INPUT_WAIT_NORMAL;
    }
    else if (letk_cli_mgr.input_status == LETK_CLI_INPUT_WAIT_FUNC_KEY)
    {
        letk_cli_mgr.input_status = LETK_CLI_INPUT_WAIT_NORMAL;

        if (ch == (char)0x41)      /* 上 */
        {
#if LETK_CLI_HISTORY_ENABLE
            letk_cli_parse_up_key();
#endif  /* LETK_CLI_HISTORY_ENABLE */
            return 0;
        }
        else if (ch == (char)0x42) /* 下 */
        {
#if LETK_CLI_HISTORY_ENABLE
            letk_cli_parse_down_key();
#endif  /* LETK_CLI_HISTORY_ENABLE */
            return 0;
        }
        else if (ch == (char)0x44) /* 左 */
        {
            letk_cli_parse_left_key();
            return 0;
        }
        else if (ch == (char)0x43) /* 右 */
        {
            letk_cli_parse_right_key();
            return 0;
        }
    }

    return -1;
}

/* 判断字符串是否空，返回：0 - 空，-1 - 非空 */
static int letk_cli_str_empty_check(const char* const str)
{
    const char* p_tmp = str;
    while (*p_tmp)
    {
        if ((*p_tmp) != ' ')
        {
            return -1;
        }
        p_tmp++;
    }
    return 0;
}

/* 字符串匹配 */
static int letk_cli_str_completion(const char* const str, const char* const substr)
{
    unsigned int len1 = (unsigned int)strlen(str);
    unsigned int len2 = (unsigned int)strlen(substr);
    unsigned int i = 0;

    if (len2 > len1)
    {
        return -1;
    }
    for (i = 0; i < len2; i++)
    {
        if (str[i] != substr[i])
        {
            return -1;
        }
    }
    return 0;
}

/* 删除开头的空格 */
static const char* letk_cli_delete_start_space(const char* const str)
{
    unsigned int i = 0;
    unsigned int len = (unsigned int)strlen(str);
    while ((str[i] == ' ') && (str[i] != '\0') && (i < len))
    {
        i++;
    }
    return (const char*)(str + i);
}

/* tab键处理 */
static void letk_cli_parse_tab_key(void)
{
    const letk_cli_cmd_t *p_temp, *p_find_first_cmd = NULL;
    unsigned int find_count = 0, len;
    const char *p_line_start;

    /* 检测是否是空白行 */
    if (letk_cli_str_empty_check(letk_cli_mgr.line) == 0)
    {
        return;
    }

    /* 取消前面的空白 */
    p_line_start = letk_cli_delete_start_space(letk_cli_mgr.line);

    p_temp = (letk_cli_cmd_t*)letk_cli_mgr.p_cmd_start;

    /* 查询命令 */
    while (p_temp != NULL)
    {
        if (letk_cli_str_completion(p_temp->name, p_line_start) == 0)
        {
            if (find_count == 0)
            {
                p_find_first_cmd = p_temp;
            }
            else if (find_count == 1)
            {
                letk_cli_put_str("\r\n");
                letk_cli_put_str(p_find_first_cmd->name);
                letk_cli_put_str("\r\n");
                letk_cli_put_str(p_temp->name);
                letk_cli_put_str("\r\n");
            }
            else
            {
                letk_cli_put_str(p_temp->name);
                letk_cli_put_str("\r\n");
            }
            find_count++;
        }
        p_temp = letk_cli_get_next_cmd((const int*)p_temp);
    }

    if (find_count == 1)
    {
        /* 删除当前行内容 */
        len = letk_cli_mgr.input_count - letk_cli_mgr.input_cusor;
        for (unsigned int i = 0; i < len; i++)
        {
            letk_cli_put_char(' ');
        }
        for (unsigned int i = 0; i < letk_cli_mgr.input_count; i++)
        {
            letk_cli_put_str("\b \b");
        }

        /* 自动填充行 */
        memset(letk_cli_mgr.line, '\0', sizeof(letk_cli_mgr.line));
        memcpy(letk_cli_mgr.line, p_find_first_cmd->name, strlen(p_find_first_cmd->name));
        /* 重新更新坐标 */
        letk_cli_mgr.input_count = (unsigned int)strlen(letk_cli_mgr.line);
        letk_cli_mgr.input_cusor = letk_cli_mgr.input_count;
        /* 显示输入行 */
        letk_cli_put_str(letk_cli_mgr.line);
    }
    else if (find_count > 1)
    {
        /* 显示提示符 */
        letk_cli_put_str(letk_cli_mgr.prompt);
        /* 重新更新坐标 */
        letk_cli_mgr.input_count = (unsigned int)strlen(letk_cli_mgr.line);
        letk_cli_mgr.input_cusor = letk_cli_mgr.input_count;
        /* 显示输入行 */
        letk_cli_put_str(letk_cli_mgr.line);
    }
}

/* 退格键处理 */
static void letk_cli_parse_backspace_key(void)
{
    int i, count;

    if (letk_cli_mgr.input_cusor == 0)
    {
        /* 未输入，直接跳过 */
        return;
    }

    /* 更新位置 */
    letk_cli_mgr.input_cusor--;
    letk_cli_mgr.input_count--;

    if (letk_cli_mgr.input_cusor == letk_cli_mgr.input_count)
    {
        /* 末尾置0 */
        letk_cli_mgr.line[letk_cli_mgr.input_count] = '\0';
        /* 光标在最后 */
        letk_cli_put_str("\b \b");
    }
    else
    {
        /* 计算需要搬移的字符数 */
        count = letk_cli_mgr.input_count - letk_cli_mgr.input_cusor;
        /* 搬移 */
        for (i = 0; i < count; i++)
        {
            letk_cli_mgr.line[letk_cli_mgr.input_cusor + i] = letk_cli_mgr.line[letk_cli_mgr.input_cusor + i + 1];
        }
        /* 末尾置0 */
        letk_cli_mgr.line[letk_cli_mgr.input_count] = '\0';
        /* 重新刷新显示 */
        letk_cli_put_char('\b');
        letk_cli_put_str(&letk_cli_mgr.line[letk_cli_mgr.input_cusor]);
        letk_cli_put_str(" \b");
        /* 光标回位 */
        for (i = 0; i < count; i++)
        {
            letk_cli_put_char('\b');
        }
    }
}

/* 回车换行键处理 */
static void letk_cli_parse_enter_key(void)
{
    unsigned int i;
    int argc = 0;
    char* argv[LETK_CLI_CMD_ARGS_NUM_MAX];
    const letk_cli_cmd_t* p_cmd;

    /* 回车，处理命令时可能有输出 */
    letk_cli_put_str("\r\n");

    if (letk_cli_mgr.input_count > 0)
    {
#if LETK_CLI_HISTORY_ENABLE
        /* 备份进入历史记录 */
        memcpy(letk_cli_mgr.history_str[letk_cli_mgr.history_index++], letk_cli_mgr.line, sizeof(letk_cli_mgr.line));
        letk_cli_mgr.history_index %= LETK_CLI_HISTORY_LINE_MAX;
        if (letk_cli_mgr.history_total < LETK_CLI_HISTORY_LINE_MAX)
        {
            letk_cli_mgr.history_total++;
        }
        letk_cli_mgr.history_inquire_index = 0;
        letk_cli_mgr.history_inquire_count = 0;
#endif  /* LETK_CLI_HISTORY_ENABLE */

        /* 分析字符串 */
        for (i = 0; i < letk_cli_mgr.input_count;)
        {
            /* 跳过空格并替换为0 */
            while ((letk_cli_mgr.line[i] == ' ') && (i < letk_cli_mgr.input_count))
            {
                letk_cli_mgr.line[i++] = '\0';
            }
            if (i >= letk_cli_mgr.input_count)
            {
                break;
            }

            if (argc >= LETK_CLI_CMD_ARGS_NUM_MAX)
            {
                letk_cli_put_str("Too many args! Line will replace follow:\r\n  < ");
                for (int j = 0; j < argc; j++)
                {
                    letk_cli_put_str(argv[j]);
                    letk_cli_put_char(' ');
                }
                letk_cli_put_str(">\r\n");
                break;
            }

            argv[argc++] = &letk_cli_mgr.line[i];
            /* 跳过中间的字符串 */
            while ((letk_cli_mgr.line[i] != ' ') && (i < letk_cli_mgr.input_count))
            {
                i++;
            }
            if (i >= letk_cli_mgr.input_count)
            {
                break;
            }
        }

        if (argc > 0)
        {
            p_cmd = letk_cli_search_cmd(argv[0]);
            if (p_cmd != NULL)
            {
                if (p_cmd->link != NULL)
                {
                    p_cmd = p_cmd->link;
                }
                if (p_cmd->cb)
                {
                    p_cmd->cb(argc, argv);
                }
            }
            else
            {
                letk_cli_put_str("Not found command \"");
                letk_cli_put_str(argv[0]);
                letk_cli_put_str("\"\r\n");
            }
        }
    }

    /* 清空行，为下一次输入准备 */
    letk_cli_put_str(letk_cli_mgr.prompt);
    memset(letk_cli_mgr.line, 0, sizeof(letk_cli_mgr.line));
    letk_cli_mgr.input_cusor = letk_cli_mgr.input_count = 0;
}

/* 通用可显示字符处理 */
static void letk_cli_parse_common_char(const char ch)
{
    int i, count;

    if (letk_cli_mgr.input_count >= (LETK_CLI_LINE_CHAR_MAX))
    {
        return;
    }
    if (letk_cli_mgr.input_cusor == letk_cli_mgr.input_count)
    {
        /* 光标在最后 */
        letk_cli_mgr.line[letk_cli_mgr.input_cusor++] = ch;
        letk_cli_mgr.input_count++;
        letk_cli_put_char(ch);
    }
    else
    {
        /* 光标不在最后，计算需要搬移的字符数 */
        count = letk_cli_mgr.input_count - letk_cli_mgr.input_cusor;
        /* 搬移 */
        for (i = count; i > 0; i--)
        {
            letk_cli_mgr.line[letk_cli_mgr.input_cusor + i] = letk_cli_mgr.line[letk_cli_mgr.input_cusor + i - 1];
        }
        letk_cli_mgr.line[letk_cli_mgr.input_cusor] = ch;
        letk_cli_put_char(ch);
        letk_cli_mgr.input_count++;
        letk_cli_mgr.input_cusor++;
        letk_cli_put_str(&letk_cli_mgr.line[letk_cli_mgr.input_cusor]);
        /* 光标回位 */
        for (i = 0; i < count; i++)
        {
            letk_cli_put_char('\b');
        }
    }
}

/* 解析一个字符 */
void letk_cli_parse_char(const char ch)
{
    /* 过滤无效字符 */
    if ((ch == (char)0x00) ||
        (ch == (char)0xFF))
    {
        return;
    }

    /* 功能码 */
    if (letk_cli_parse_func_key(ch) == 0)
    {
        return;
    }

    /* 字符解析 */
    if (ch == '\t')
    {
        /* Tab */
        letk_cli_parse_tab_key();
    }
    else if ((ch == (char)0x7F) || (ch == (char)0x08))
    {
        /* 退格 */
        letk_cli_parse_backspace_key();
    }
    if ((ch == '\r') || (ch == '\n'))
    {
        /* 回车或换行 */
        letk_cli_parse_enter_key();
    }
    else if ((ch >= ' ') && (ch <= '~'))
    {
        letk_cli_parse_common_char(ch);
    }
}

#if LETK_CLI_CC
/* 编译器命令导出方式可以防耦合，将函数声明为局部函数 */
#define CMD_CB_CALL_PREFIX  static
#else   /* LETK_CLI_CC */
/* 静态注册方式需要导出默认命令函数，不能加static */
#define CMD_CB_CALL_PREFIX
#endif  /* LETK_CLI_CC */

/* 内部命令-help */
CMD_CB_CALL_PREFIX void letk_cli_internal_cmd_help(int argc, char* argv[])
{
    const letk_cli_cmd_t* p_temp;
    const letk_cli_cmd_t* p_temp1;
    int   found_flag = 0;

    if (argc == 1)
    {
        p_temp = (letk_cli_cmd_t*)letk_cli_mgr.p_cmd_start;

        letk_cli_put_str("System all command:\r\n");
        while (p_temp != NULL)
        {
            letk_cli_put_str("    ");
            letk_cli_put_str(p_temp->name);
            if (p_temp->link != NULL)
            {
                letk_cli_put_str(" -> ");
                p_temp1 = p_temp->link;
                letk_cli_put_str(p_temp1->name);
            }
            letk_cli_put_str("\r\n");
            p_temp = letk_cli_get_next_cmd((const int*)p_temp);
        }
    }
    else if (argc == 2)
    {
        p_temp = (letk_cli_cmd_t*)letk_cli_mgr.p_cmd_start;
        found_flag = 0;
        while (p_temp != NULL)
        {
            if (strcmp(p_temp->name, argv[1]) == 0)
            {
                letk_cli_put_str("command:");
                letk_cli_put_str(p_temp->name);
                if (p_temp->link != NULL)
                {
                    letk_cli_put_str(" -> ");
                    p_temp = p_temp->link;
                    letk_cli_put_str(p_temp->name);
                }
                letk_cli_put_str("\r\n  usage:");
                letk_cli_put_str(p_temp->usage);
                letk_cli_put_str("\r\n");
                found_flag = 1;
                break;
            }
            p_temp = letk_cli_get_next_cmd((const int*)p_temp);
        }
        if (found_flag == 0)
        {
            letk_cli_put_str("Not found command \"");
            letk_cli_put_str(argv[1]);
            letk_cli_put_str("\"\r\n");
        }
    }
    else
    {
        letk_cli_put_str("Too many args! Only support less then 2 args\r\n");
    }
}
/* 导出help命令 */
LETK_CLI_CMD_EXPORT(help,
                  "help [cmd-name] -- list the command and usage",
                  letk_cli_internal_cmd_help);
/* 设置help的别名'?' */
LETK_CLI_CMD_ALIAS(help, "?");

#if LETK_CLI_HISTORY_ENABLE
/* 内部命令-history */
CMD_CB_CALL_PREFIX void letk_cli_internal_cmd_history(int argc, char* argv[])
{
    unsigned int i, count, num;

    if (argc == 1)
    {
        num = letk_cli_mgr.history_total;
    }
    else if (argc == 2)
    {
        count = atoi(argv[1]);
        if (letk_cli_mgr.history_total > count)
        {
            num = count;
        }
        else
        {
            num = letk_cli_mgr.history_total;
        }
    }
    else
    {
        letk_cli_put_str("Too many args! Only support less then 2 args\r\n");
        return;
    }

    for (i = 0, count = letk_cli_mgr.history_index; i < num; i++)
    {
        if (count == 0)
        {
            count = LETK_CLI_HISTORY_LINE_MAX - 1;
        }
        else
        {
            count--;
        }
        letk_cli_put_str("    ");
        letk_cli_put_str(letk_cli_mgr.history_str[count]);
        letk_cli_put_str("\r\n");
    }
}
/* 导出history命令 */
LETK_CLI_CMD_EXPORT(history,
                  "history [num] -- list the history command",
                  letk_cli_internal_cmd_history);
#endif  /* LETK_CLI_HISTORY_ENABLE */

/* 内部命令-test */
CMD_CB_CALL_PREFIX void letk_cli_internal_cmd_test(int argc, char* argv[])
{
    for (int i = 0; i < argc; i++)
    {
        letk_cli_put_str("arg[");
        letk_cli_put_int(i);
        letk_cli_put_str("] = ");
        letk_cli_put_str(argv[i]);
        letk_cli_put_str("\r\n");
    }
}
/* 导出test命令 */
LETK_CLI_CMD_EXPORT(test,
                  "test [args] -- test the cli",
                  letk_cli_internal_cmd_test);

#endif  /* LETK_CLI_ENABLE */
