#ifndef _EVLK_SHELL_H_
#define _EVLK_SHELL_H_

#include "Stream.h"
#include "evlk_dictman/evlk_dictman.h"
#include <vector>

/******语法支持*********
 *
 * 空格 区分命令与参数
 * ${var}获取变量值
 * $(cmd)获取命令返回值
 * "abc"取消语法规则(${}与$()不生效)
 * cmd1|cmd2将命令cmd1返回值作为命令cmd2的输入=$()、//TODO
 * && & || 逻辑与、逻辑或//TODO
 *
 * *******************/
using std ::vector;

namespace _EVLK_SHELL_
{
    struct cli;
    typedef vector<cli *> cli_pool;
    typedef _EVLK_DICTMAN_::dictman var_pool;
    extern cli_pool sysh_cli_pool;
    extern var_pool sysh_var_pool;

    class Shell
    {
    private:
        cli_pool clis; // 函数池
        var_pool vars; // 变量池
        Stream *Io;    // 输入输出(Terminal)
        bool echo = true;
        String cout_catch;
        // String cin_catch;
        String env_temp; // 给getenv()存放返回值
        int state;       // TODO
        // 0 待机
        // 1 运行占用

        const cli *_foundCli(String name);
        // 输入格式(是否处理控制字符)
        bool cin_format = true;
        String _tty() const;
        int _cout(const String &);
        int _cin(String &) const;

        class outModule
        {
        private:
            Shell &sh;

        public:
            outModule(Shell &sh) : sh(sh){};
            outModule &operator<<(String str)
            {
                sh._cout(str);
                return *this;
            };
        };
        class inModule
        {
        private:
            const Shell &sh;

        public:
            inModule(Shell &sh) : sh(sh){};
            int operator>>(String &str) { return sh._cin(str); };
        };

    public:
        Shell(Stream &io, cli_pool &clipool = sysh_cli_pool, var_pool &varpool = sysh_var_pool);
        void begin(cli_pool &clipool = sysh_cli_pool, var_pool &varpool = sysh_var_pool);
        Shell &operator<<(cli &); // 函数注册

        int system(const char *cmd, bool echo = true); // 以空格为分隔符运行命令
        int push(const char *cmd, bool echo = true);   // shell脚本解释机
        int run() { return system("_INPUT_STR_"); };   // 执行外部输入策略

        //! cli used
        const String &getCatch() const { return cout_catch; };
        const cli_pool *getClis() const { return &clis; };
        const var_pool *getVars() const { return &vars; };
        int getState() const { return state; };
        char *getenv_s(size_t *_ReturnSize, char *_DstBuf, size_t _DstSize, const char *_VarName);
        const char *getenv(const char *envvar);
        int setenv(const char *name, const char *value, int overwrite);
        int setenv(const char *string);
        int putenv(const char *string);

        outModule cout;
        inModule cin;
    };
}

#endif