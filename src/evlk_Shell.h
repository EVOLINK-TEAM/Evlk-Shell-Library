#ifndef _EVLK_SHELL_H_
#define _EVLK_SHELL_H_

#include "evlk_cli.h"
#include "Stream.h"
#include "list"
#include "map"

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

namespace _EVLK_SHELL_API
{
    struct sh_Cli_pool
    {
    private:
        std::list<cli *> pool;

    public:
        std::list<cli *> &getpool();
        bool add(cli &cli);
        int length();
        bool remove();
    };

    struct sh_Var_pool
    {
    private:
        std::map<String, String> pool;

    public:
        sh_Var_pool();
        sh_Var_pool(std::map<String, String>);
        std::map<String, String> &getpool();
        bool set(String key, String value);
        bool get(String key, String &value) const;
        bool erase(String key);
    };

    extern sh_Cli_pool sysh_cli_pool;
    extern sh_Var_pool sysh_var_pool;

    class shPoolMng
    {
    protected:
        sh_Cli_pool *clipool; // 方法池
        sh_Var_pool varpool;  // 变量池

    public:
        shPoolMng(sh_Cli_pool &clipool, sh_Var_pool &varpool);
        sh_Cli_pool &getCliPool();
        sh_Var_pool &getVarPool();
        bool getVar(String key, String &value) const;
        bool exportVar(String key, String value);
        bool unsetVar(String kill_dependency);
    };

    class Shell : public shPoolMng
    {
    private:
        Stream *Io; // 输入输出(Terminal)
        int state;  // TODO
        // -1 未启动
        // 0 待机
        // 1 运行占用
        // 等待用户输入

    public:
        String path;
        Shell(Stream &io, sh_Cli_pool &clipool = sysh_cli_pool, sh_Var_pool &varpool = sysh_var_pool);

        cli::func_return push_one(const char *cmd); // 以空格为分隔符运行命令
        cli::func_return push(const char *cmd);     // 解释器

        int run(const char *cmd);
        int run();

        // 以下是cli程序使用的命令
        int getState() const;
        int print(const char *) const;
        int println(const char *) const;
        String getCommand() const;
        /////////////////////
    };
}

#endif