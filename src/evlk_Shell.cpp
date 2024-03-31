#include "evlk_cli.h"
#include "evlk_Shell.h"

namespace _EVLK_SHELL_
{
    // 特殊字符表
    const char *const spec_space = "[_SPC_";        // 空格
    const char *const spec_line = "[_LINE_";        // 换行
    const char *const spec_quoto = "[_QT_";         // "
    const char *const spec_singlrquoto = "[_SQT_";  // '
    const char *const spec_prebrace = "[_PBRC_";    // {
    const char *const spec_backbrace = "[_BBRC_";   // }
    const char *const spec_prebracket = "[_PBRK_";  // (
    const char *const spec_backbracket = "[_BBRK_"; // )
    const char *const spec_dollar = "[_DL_";        // $
    const char *const spec_backslash = "[_BS_";     // \

    cli_pool sysh_cli_pool;
    var_pool sysh_var_pool;

/**
 * @param _Src 源
 * @param _Dim 分割符
 * @returns ret,end
 */
#define strnsplit(_Src, _Len, _Dim)                                                                                           \
    for (const char *const __ttlen = _Src + _Len, *ret = _Src, *end = ret && strchr(ret, _Dim) ? strchr(ret, _Dim) : __ttlen; \
         ret;                                                                                                                 \
         ret = end < __ttlen ? end + 1 : nullptr, end = ret ? strchr(ret, _Dim) : nullptr, end = end ?: __ttlen)
#define strsplit(_Src, _Dim) strnsplit(_Src, strlen(_Src), _Dim)
    size_t str_find(const char *str, const char *subStr, size_t startIndex = 0)
    {
        const char *found = strstr(str + startIndex, subStr);
        return (found != NULL) ? (size_t)(found - str) : -1;
    }
    size_t str_find(const String &str, const char *subStr, size_t startIndex = 0)
    {
        return str_find(str.c_str(), subStr, startIndex);
    }
    void strnrep(String &str, size_t pos, size_t len, const char *replacement, size_t replacementLen)
    {
        const char *end = str.begin() + pos + len;
        str = String(str.begin(), pos) + String(replacement, replacementLen) + String(end, str.end() - end);
    }
    void strrep(String &str, size_t pos, size_t len, const char *replacement) { strnrep(str, pos, len, replacement, strlen(replacement)); }
    void strrsub(String &str, const char *key, const char *value) // 替换字串
    {
        size_t Start = str_find(str, key);
        while (Start != (size_t)-1)
        {
            strrep(str, Start, strlen(key), value);
            Start = str_find(str, key);
        }
    }

    void Trans(String &input) // 字符解译(表情解译)
    {
        strrsub(input, spec_space, " ");
        strrsub(input, spec_line, " ");
        strrsub(input, spec_quoto, "\"");
        strrsub(input, spec_singlrquoto, "\'");
        strrsub(input, spec_prebrace, "{");
        strrsub(input, spec_backbrace, "}");
        strrsub(input, spec_prebracket, "(");
        strrsub(input, spec_backbracket, ")");
        strrsub(input, spec_dollar, "$");
        strrsub(input, spec_backslash, "\\");
    }
    const char *Spec(char c)
    {
        switch (c)
        {
        case ' ':
            return spec_space;
        case 'n':
            return spec_line;
        case '\"':
            return spec_quoto;
        case '\'':
            return spec_singlrquoto;
        case '{':
            return spec_prebrace;
        case '}':
            return spec_backbrace;
        case '(':
            return spec_prebracket;
        case ')':
            return spec_backbracket;
        case '$':
            return spec_dollar;
        case '\\':
            return spec_backslash;
        default:
            return NULL;
        }
    }
    void specCharSupport(String &input)
    {
        for (size_t i = 0; i < input.length(); i++)
            if (input[i] == '\\')
            {
                String spec = Spec(input[i + 1]);
                strrep(input, i, 2, spec.c_str());
            }
    }
    void commentSupport(String &input) // 注释
    {
        size_t Start = str_find(input, "\"");
        size_t End = str_find(input, "\"", Start + 1);
        if (Start != -1 && End != -1)
        {
            String key(input.begin() + Start, End - 1 - Start);
            for (size_t i = 0; i < key.length(); i++)
            {
                String spec = Spec(key[i]);
                if (spec.length() && spec != spec_line)
                    strrep(key, i, 1, spec.c_str());
            }
            strrep(input, Start, End - Start + 1, key.c_str());
            commentSupport(input);
        }
    }
    void abcommentSupport(String &input) // 绝对注释
    {
        size_t Start = str_find(input, "\'");
        size_t End = str_find(input, "\'", Start + 1);
        if (Start != (size_t)-1 && End != (size_t)-1)
        {
            String key(input.begin() + Start, End - 1 - Start);
            for (size_t i = 0; i < key.length(); i++)
            {
                String spec = Spec(key[i]);
                if (spec.length() && spec != spec_line)
                    strrep(key, i, 1, spec.c_str());
            }
            strrep(input, Start, End - Start + 1, key.c_str());
            commentSupport(input);
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
            Start = str_find(input, "${");

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

            String key(input.begin() + Start + headlen - 1, End - Start - headlen);
            if (Type)
            {
                sh.push(key.c_str());
                key = sh.getCatch();
            }
            else
                sh.getVars()->get(key, key);
            strrep(input, Start, End - Start + 1, key.c_str());
            varSupport(input, sh);
        }
    }

    const cli *Shell::_foundCli(String name)
    {
        auto it = clis.begin();
        while (it != clis.end())
        {
            if ((*it)->name == name)
                return *it;
            it++;
        }
        return NULL;
    }
    String Shell::_tty() const
    {
        String str = "";
        size_t index = 0;
        char c = '\0';

        while (c != '\r')
        {
            if (Io->available())
            {
                c = (char)Io->read();

                if (!cin_format)
                    str += c;
                else
                {
                    if (c == '\0' || c == 0xE0 || c == -32)
                    {
                        c = (char)Io->read();
                        switch (c)
                        {
                        case 72: // up
                            break;
                        case 80: // down
                            break;
                        case 75: // left
                            if (index)
                            {
                                index--;
                                Io->write("\033[1D");
                            }
                            break;
                        case 77: // right
                            if (str.length() > index)
                            {
                                index++;
                                Io->write("\033[1C");
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    else if (c == '\b')
                    {
                        if (index)
                        {
                            str = String(str.c_str(), index - 1) + String(str.c_str() + index, str.length() - index);
                            index--;
                            Io->write("\033[1D\033[1P");
                        }
                    }
                    else if (c != '\r' && c != '\n')
                    {
                        str = String(str.c_str(), index) + c + String(str.c_str() + index, str.length() - index);
                        index++;
                        Io->write(str.c_str() + index - 1, str.length() - index + 1);
                        if (str.length() > index)
                        {
                            char offset[10] = "\033[";
                            itoa(str.length() - index, offset + 2, 10);
                            strcat(offset, "D");
                            Io->write(offset);
                        }
                    }
                }
            }
            Io->flush();
        }
        return str;
    }

    Shell::Shell(Stream &io, cli_pool &clipool, var_pool &varpool)
        : cout(*this), cin(*this),
          Io(&io), clis(clipool), vars(varpool), state(0) {}
    Shell &Shell::operator<<(cli &c)
    {
        clis.push_back(&c);
        return *this;
    }
    int Shell::_cout(const String &str)
    {
        if (state == 1)
            cout_catch += str;
        return echo ? Io->print(str) : 0;
    }
    int Shell::_cin(String &str) const
    {
        str = _tty();
        Io->write('\n');
        return str.length();
    }

    int Shell::system(const char *cmd, bool echo)
    {
        vector<String> cmds;
        strsplit(cmd, ' ')
            cmds.push_back(String(ret, end - ret));

        if (!cmds.size())
            return 0;

        auto it = cmds.begin();
        while (it != cmds.end())
        {
            Trans(*it);
            it++;
        }

        cli *exe = NULL;
        for (auto i = clis.begin(); i != clis.end(); i++)
            if ((*i)->name == cmds[0])
            {
                exe = *i;
                break;
            }
        if (!exe)
        {
            // Io->print(String("\033[33mERROR: not found the command named \"") + cmds[0] + "\"\033[0m");
            Io->print(String("ERROR: not found the command named \"") + cmds[0] + "\"");
            Io->flush();
            return 0;
        }

        vector<String> envs;
        char **envp = NULL;
        char **argv = NULL;
        int argc = cmds.size();
        switch (exe->getType())
        {
        case cli::cli_type_cve:
        {
            for (size_t i = 0; i < vars.lenth(); i++)
            {
                envs.push_back("");
                if (!vars.get(i, *(envs.end() - 1)))
                    return 0;
            }
            envp = new char *[envs.size() + 1];
            envp[envs.size()] = NULL;
            for (size_t i = 0; i < envs.size(); i++)
                envp[i] = envs[i].begin();
        }
        case cli::cli_type_cv:
        {
            argv = new char *[cmds.size()];
            for (size_t i = 0; i < cmds.size(); i++)
                argv[i] = cmds[i].begin();
        }
        case cli::cli_type_c:
        case cli::cli_type_s:
        {
            cout_catch = "";
            this->echo = echo;
            state = 1;
            int S = exe->run(*this, argc, argv, envp);
            this->echo = true;
            state = 0;
            if (argv)
                delete[] argv;
            if (envp)
                delete[] envp;
            return S;
        }

        default:
            return 0;
        }
    }
    int Shell::push(const char *cmd, bool echo)
    {
        strsplit(cmd, '\n')
        {
            String Cmd(ret, end - ret);
            strrsub(Cmd, spec_line, " ");     // 将换行表情符转空格
            specCharSupport(Cmd);             // 去转义字符
            abcommentSupport(Cmd);            // 去字符串(’‘中的所有特殊字符将会转换成表情符)
            varSupport(Cmd, *this);           // 去变量和引用(所有$()和${}会被解释)
            commentSupport(Cmd);              // 去字符串(""中的所有特殊字符将会转换成表情符)
            return system(Cmd.c_str(), echo); // 最后会剩下空格没被转译，输出到push_one后由空格分割，然后表情被转义成正常字符
        }
        return 0;
    }
    char *Shell::getenv_s(size_t *_ReturnSize, char *_DstBuf, size_t _DstSize, const char *_VarName)
    {
        String value;
        if (!vars.get(_VarName, value) || _DstSize <= value.length())
            return NULL;
        *_ReturnSize = value.length();
        strcpy(_DstBuf, value.c_str());
        _DstBuf[value.length()] = '\0';
        return _DstBuf;
    }
    const char *Shell::getenv(const char *envvar)
    {
        if (!vars.get(envvar, env_temp))
            return NULL;
        return env_temp.c_str();
    }
    int Shell::setenv(const char *name, const char *value, int overwrite) { return vars.set(name, value, overwrite); }
    int Shell::setenv(const char *string)
    {
        size_t l = strlen(string);
        const char *e = strchr(string, '=');
        String key(string, e - string);
        String value(e + 1, l - (e + 1 - string));
        return setenv(key.c_str(), value.c_str(), 1);
    }
    int Shell::putenv(const char *string) { return setenv(string); };
}