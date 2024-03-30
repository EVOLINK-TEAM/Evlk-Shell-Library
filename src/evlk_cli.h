#ifndef _EVLK_CLI_H_
#define _EVLK_CLI_H_

#include "WString.h"

namespace _EVLK_SHELL_
{
    class Shell;
    struct cli
    {
    public:
        enum cli_type_t
        {
            cli_type_s = 1,
            cli_type_c,
            cli_type_cv,
            cli_type_cve
        };
        struct Property
        {
            bool display;
            bool script;
        } property;
        typedef int (*cli_func_s)(Shell &);
        typedef int (*cli_func_c)(Shell &, int);
        typedef int (*cli_func_cv)(Shell &, int, char *[]);
        typedef int (*cli_func_cve)(Shell &, int, char *[], char *[]);
        const cli_func_s func;
        const cli_func_c func_c;
        const cli_func_cv func_cv;
        const cli_func_cve func_cve;
        const String name;
        size_t getType()
        {
            if (func)
                return cli_type_s;
            if (func_c)
                return cli_type_c;
            if (func_cv)
                return cli_type_cv;
            if (func_cve)
                return cli_type_cve;
            return 0;
        };
        int run(Shell &sh, int argc, char *argv[], char *envp[])
        {
            size_t type = getType();
            switch (type)
            {
            case cli_type_s:
                return func(sh);
            case cli_type_c:
                return func_c(sh, argc);
            case cli_type_cv:
                return func_cv(sh, argc, argv);
            case cli_type_cve:
                return func_cve(sh, argc, argv, envp);
            default:
                return 0;
            }
        }
        cli(const cli &c)
            : name(c.name), func(c.func), func_c(c.func_c), func_cv(c.func_cv), func_cve(c.func_cve) {}
        cli(cli_func_s func, String name, Property property = {.display = true})
            : func(func), func_c(nullptr), func_cv(nullptr), func_cve(nullptr), name(name), property(property){};
        cli(cli_func_c func, String name, Property property = {.display = true})
            : func(nullptr), func_c(func), func_cv(nullptr), func_cve(nullptr), name(name), property(property){};
        cli(cli_func_cv func, String name, Property property = {.display = true})
            : func(nullptr), func_c(nullptr), func_cv(func), func_cve(nullptr), name(name), property(property){};
        cli(cli_func_cve func, String name, Property property = {.display = true})
            : func(nullptr), func_c(nullptr), func_cv(nullptr), func_cve(func), name(name), property(property){};
    };
}
#endif