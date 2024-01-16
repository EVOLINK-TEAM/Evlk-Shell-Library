#include "evlk_Shell.h"

// 特殊字符表
#define spec_space "[_SPC_"        // 空格
#define spec_line "[_LINE_"        // 换行
#define spec_quoto "[_QT_"         // "
#define spec_singlrquoto "[_SQT_"  // '
#define spec_prebrace "[_PBRC_"    // {
#define spec_backbrace "[_BBRC_"   // }
#define spec_prebracket "[_PBRK_"  // (
#define spec_backbracket "[_BBRK_" // )
#define spec_dollar "[_DL_"        // $
#define spec_backslash "[_BS_"     // \

namespace _EVLK_SHELL_API
{
    sh_Cli_pool sysh_cli_pool;
    sh_Var_pool sysh_var_pool;

    size_t str_find(const char *str, const char *subStr, size_t startIndex = 0)
    {
        const char *found = strstr(str + startIndex, subStr);
        return (found != NULL) ? (size_t)(found - str) : -1;
    }
    size_t str_find(const String &str, const char *subStr, size_t startIndex = 0)
    {
        return str_find(str.c_str(), subStr, startIndex);
    }

    void str_replace(char *&str, size_t pos, size_t len, const char *replacement)
    {
        size_t strLen = strlen(str);
        size_t replacementLen = strlen(replacement);

        if (pos < strLen)
        {
            size_t replaceLen = len;
            if (pos + len > strLen)
            {
                replaceLen = strLen - pos;
            }

            if (replaceLen != replacementLen)
            {
                size_t newSize = strLen + replacementLen - replaceLen + 1;
                if (newSize > strLen)
                {
                    char *tempStr = new char[newSize];
                    memcpy(tempStr, str, pos);
                    memcpy(tempStr + pos, replacement, replacementLen);
                    memcpy(tempStr + pos + replacementLen, str + pos + replaceLen, strLen - pos - replaceLen + 1);
                    delete[] str;
                    str = tempStr;
                }
                else
                {
                    memmove(str + pos + replacementLen, str + pos + replaceLen, strLen - pos - replaceLen + 1);
                    memcpy(str + pos, replacement, replacementLen);
                }
            }
        }
    }
    void str_replace(String &str, size_t pos, size_t len, const char *replacement)
    {
        char *Str = new char[str.length() + 1];
        strcpy(Str, str.c_str());
        str_replace(Str, pos, len, replacement);
        str = Str;
        delete[] Str;
    }
    char *str_substr(const char *str, size_t start, size_t length) //! 需要释放内存
    {
        size_t strLen = strlen(str);

        if (start >= strLen)
        {
            return NULL; // 起始索引超出字符串长度，返回 NULL
        }

        size_t subStrLen = (length < (strLen - start)) ? length : (strLen - start);
        char *result = (char *)malloc((subStrLen + 1) * sizeof(char));

        if (result != NULL)
        {
            strncpy(result, str + start, subStrLen);
            result[subStrLen] = '\0'; // 添加 null 终止符
        }

        return result;
    }
    String str_substr(const String &str, size_t start, size_t length)
    {
        char *Str = str_substr(str.c_str(), start, length);
        if (Str != NULL)
        {
            String s = Str;
            free(Str);
            return s;
        }
        return "";
    }

    void strReplaceSub(String &str, const char *key, const char *value) // 替换字串
    {
        size_t Start = str_find(str, key);
        while (Start != -1)
        {
            str_replace(str, Start, strlen(key), value);
            Start = str_find(str, key);
        }
    }
    std::vector<String> strSplit(const char *str, const char *split)
    {
        std::vector<String> dq;
        char Str[strlen(str) + 1];
        strcpy(Str, str);
        char *ret = NULL;
        char *saveptr;
        for (ret = strtok_r(Str, " ", &saveptr); ret != NULL; ret = strtok_r(NULL, " ", &saveptr)) // 分割换行符
        {
            dq.push_back(ret);
        }
        return dq;
    };

    void charTrans(String &input) // 字符解译(表情解译)
    {
        strReplaceSub(input, spec_space, " ");
        strReplaceSub(input, spec_line, " ");
        strReplaceSub(input, spec_quoto, "\"");
        strReplaceSub(input, spec_singlrquoto, "\'");
        strReplaceSub(input, spec_prebrace, "{");
        strReplaceSub(input, spec_backbrace, "}");
        strReplaceSub(input, spec_prebracket, "(");
        strReplaceSub(input, spec_backbracket, ")");
        strReplaceSub(input, spec_dollar, "$");
        strReplaceSub(input, spec_backslash, "\\");
    }
    void patternSpec(String &str, char c)
    {
        switch (c)
        {
        case ' ':
            str = spec_space;
            break;
        case 'n':
            str = spec_line;
            break;
        case '\"':
            str = spec_quoto;
            break;
        case '\'':
            str = spec_singlrquoto;
            break;
        case '{':
            str = spec_prebrace;
            break;
        case '}':
            str = spec_backbrace;
            break;
        case '(':
            str = spec_prebracket;
            break;
        case ')':
            str = spec_backbracket;
            break;
        case '$':
            str = spec_dollar;
            break;
        case '\\':
            str = spec_backslash;
            break;
        default:
            break;
        }
    }

    void specCharSupport(String &input)
    {
        for (size_t i = 0; i < input.length(); i++)
        {
            if (input[i] == '\\')
            {
                String spec;
                patternSpec(spec, input[i + 1]);
                str_replace(input, i, 2, spec.c_str());
            }
        }
    }
    void varSupport(String &input, Shell &sh) // 仅替换变量
    {
        bool Type = false; // 0-var,1-func
        size_t Start = -1;
        size_t End = str_find(input, "}");
        size_t temp = str_find(input, ")");
        if (temp < End) // 找到第一个反括号
        {
            Start = str_find(input, "$(");
            End = temp;
            Type = true;
        }
        else
        {
            Start = str_find(input, "${");
        }

        if (Start != -1 && End != -1)
        {
            const char *head = Type ? "$(" : "${";
            size_t headlen = strlen(head);
            temp = str_find(input, head, Start + 1);
            while (temp < End) // 找到与反括号匹配的正括号
            {
                Start = temp;
                temp = str_find(input, head, Start + 1);
            }

            String key = str_substr(input, Start + headlen, End - Start - headlen);
            if (Type)
            {
                key = sh.push(key.c_str()).value;
            }
            else
            {
                sh.getVar(key, key);
            }
            str_replace(input, Start, End - Start + 1, key.c_str());
            varSupport(input, sh);
        }
    }
    void commentSupport(String &input) // 注释
    {
        size_t Start = str_find(input, "\"");
        size_t End = str_find(input, "\"", Start + 1);
        if (Start != -1 && End != -1)
        {
            String key = str_substr(input, Start + 1, End - Start - 1);
            for (size_t i = 0; i < key.length(); i++)
            {
                String spec;
                patternSpec(spec, key[i]);
                if (spec.length() && spec != spec_line)
                {
                    str_replace(key, i, 1, spec.c_str());
                }
            }
            str_replace(input, Start, End - Start + 1, key.c_str());
            commentSupport(input);
        }
    }
    void abcommentSupport(String &input) // 绝对注释
    {
        size_t Start = str_find(input, "\'");
        size_t End = str_find(input, "\'", Start + 1);
        if (Start != -1 && End != -1)
        {
            String key = str_substr(input, Start + 1, End - Start - 1);
            for (size_t i = 0; i < key.length(); i++)
            {
                String spec;
                patternSpec(spec, key[i]);
                if (spec.length() && spec != spec_line)
                {
                    str_replace(key, i, 1, spec.c_str());
                }
            }
            str_replace(input, Start, End - Start + 1, key.c_str());
            commentSupport(input);
        }
    }
    std::list<cli *> &sh_Cli_pool::getpool()
    {
        return pool;
    };
    bool sh_Cli_pool::add(cli &cli)
    {
        pool.push_back(&cli);
        return true;
    };
    int sh_Cli_pool::length()
    {
        return pool.size();
    }

    sh_Var_pool::sh_Var_pool(){};
    sh_Var_pool::sh_Var_pool(std::map<String, String> vars) : pool(vars){};
    std::map<String, String> &sh_Var_pool::getpool()
    {
        return pool;
    };
    bool sh_Var_pool::set(String key, String value)
    {
        String v;
        if (this->get(key, v))
        {
            erase(key);
        }
        pool.insert(std::pair<String, String>(key, value));
        return true;
    }
    bool sh_Var_pool::get(String key, String &value) const
    {
        std::map<String, String>::const_iterator it = pool.find(key);
        if (it != pool.end())
        {
            value = (*it).second;
            return true;
        }
        else
        {
            return false;
        }
    }
    bool sh_Var_pool::erase(String key)
    {
        return pool.erase(key);
    }

    shPoolMng::shPoolMng(sh_Cli_pool &clipool, sh_Var_pool &varpool) : clipool(&clipool), varpool(varpool){};
    sh_Cli_pool &shPoolMng::getCliPool()
    {
        return *clipool;
    }
    sh_Var_pool &shPoolMng::getVarPool()
    {
        return varpool;
    }
    bool shPoolMng::getVar(String key, String &value) const
    {
        return varpool.get(key, value);
    }
    bool shPoolMng::exportVar(String key, String value)
    {
        return varpool.set(key, value);
    }

    Shell::Shell(Stream &io, sh_Cli_pool &cli, sh_Var_pool &var) : shPoolMng(cli, var), Io(&io){};
    cli::func_return Shell::push_one(const char *cmd)
    {
        std::vector<String> cmds = strSplit(cmd, " ");

        std::vector<String>::iterator it = cmds.begin();
        while (it != cmds.end())
        {
            charTrans(*it);
            it++;
        }

        if (cmds.size())
        {
            sh_Cli_pool &Pool = getCliPool();
            std::list<cli *> &pool = Pool.getpool();
            String cmdname = cmds.front();
            cmds.erase(cmds.begin());

            size_t i = cmds.size();
            std::list<cli *>::iterator it = pool.begin();
            while (it != pool.end())
            {
                cli *cli = *it;
                if (cli->cliName == cmdname)
                {
                    cli::func_return r = cli->func({.params = cmds, .sh = *this});
                    String v = r.value;
                    strReplaceSub(v, "\n", spec_line);
                    r.value = v;
                    return r;
                }
                it++;
            }
            String str = "\33[33mERROR: not found the command named \"" + cmdname + "\"\33[0m";
            println(str.c_str());
        }
        return {.state = 0, .value = ""};
    }
    cli::func_return Shell::push(const char *cmd)
    {
        char str[strlen(cmd) + 1];
        strcpy(str, cmd);
        char *ret = NULL;
        char *saveptr;
        for (ret = strtok_r(str, "\n", &saveptr); ret != NULL; ret = strtok_r(NULL, "\n", &saveptr)) // 分割换行符
        {
            String Cmd = ret;
            strReplaceSub(Cmd, spec_line, " "); // 将换行表情符转空格
            specCharSupport(Cmd);               // 去转义字符
            abcommentSupport(Cmd);              // 去字符串(’‘中的所有特殊字符将会转换成表情符)
            varSupport(Cmd, *this);             // 去变量和引用(所有$()和${}会被解释)
            commentSupport(Cmd);                // 去字符串(""中的所有特殊字符将会转换成表情符)
            return push_one(Cmd.c_str());       // 最后会剩下空格没被转译，输出到push_one后由空格分割，然后表情被转义成正常字符
        }
        return {.state = 0, .value = ""};
    }
    int Shell::run(const char *cmd)
    {
        String INPUT_STRATEGY;
        getVar("_INPUT_STRATEGY_", INPUT_STRATEGY);
        print(push(INPUT_STRATEGY.c_str()).value.c_str());

        String Cmd = cmd;
        Cmd = '\'' + Cmd + '\'';
        abcommentSupport(Cmd);
        println(cmd);

        cli::func_return r = push(cmd);
        strReplaceSub(r.value, spec_line, "\n");
        Io->println(r.value);

        return r.state;
    }
    int Shell::run()
    {
        String INPUT_STRATEGY;
        getVar("_INPUT_STRATEGY_", INPUT_STRATEGY);
        print(push(INPUT_STRATEGY.c_str()).value.c_str());

        String cmd = getCommand();
        String Cmd = '\'' + cmd + '\'';
        abcommentSupport(Cmd);
        println("");

        cli::func_return r = push(cmd.c_str());
        strReplaceSub(r.value, spec_line, "\n");
        Io->println(r.value);

        return r.state;
    }

    int Shell::getState() const
    {
        return state;
    }
    int Shell::print(const char *str) const
    {
        return Io->print(str);
    }
    int Shell::println(const char *str) const
    {
        return Io->println(str);
    }
    String Shell::getCommand() const
    {
        String cmd;
        uint8_t curpos = 0;
        while (1)
        {
            if (Io->available())
            {
                char c = Io->read();
                if (c == '\r')
                    break;
                Io->print(c);
                cmd += c;
            }
        }
        return cmd;
    }
}