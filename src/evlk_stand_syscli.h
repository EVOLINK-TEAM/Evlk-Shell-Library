#ifndef _EVLK_SYSCLI_H_
#define _EVLK_SYSCLI_H_
#include "evlk_Shell.h"

namespace _EVLK_STAND_SYSCLI
{
    using namespace _EVLK_SHELL_API;
    static cli cli_sys_set(
        "set",
        [](cli::func_param param) -> cli::func_return
        {
            bool code;
            std::vector<String> &params = param.params;
            Shell &sh = param.sh;
            if (params.size() == 2)
            {
                sh_Var_pool &vars = sh.getVarPool();
                code = vars.set(params[0], params[1]);
            }
            return {code, ""};
        },
        {.display = false});

    static cli cli_sys_export(
        "export",
        [](cli::func_param param) -> cli::func_return
        {
            if (param.params.size() == 2)
            {
                std::vector<String>::iterator it = param.params.begin();
                param.sh.exportVar((*it), (*it++));
            }
            return {.state = 1, .value = ""};
        },
        {.display = false});

    static cli cli_echo(
        "echo",
        [](cli::func_param param) -> cli::func_return
        {
            std::vector<String>::iterator it = param.params.begin();
            String value;
            while (it != param.params.end())
            {
                value += *it + ' ';
                it++;
            }
            return {.state = 1, .value = value};
        });

    static cli cli_climng(
        "climng",
        [](cli::func_param param) -> cli::func_return
        {
            std::vector<String> &params = param.params;
            Shell &sh = param.sh;
            if (!params.empty())
            {
                const String param1 = params.front();
                const String param2 = params[1];
                if (param1 == "-Q")
                {
                    bool isunvis = false;
                    if (param2 == "--unvisible")
                    {
                        isunvis = true;
                    }

                    std::list<cli *> &Pool = sysh_cli_pool.getpool();
                    std::list<cli *>::iterator it = Pool.begin();
                    String val;
                    while (it != Pool.end())
                    {
                        if (isunvis)
                        {
                            val += (**it).cliName + '\n';
                        }
                        else if ((**it).property.display)
                        {
                            val += (**it).cliName + '\n';
                        }
                        it++;
                    }
                    return {.state = 1, .value = val};
                }
            }
            return {.state = 0, .value = ""};
        });

    static void load(sh_Cli_pool &pool)
    {
        pool.add(cli_sys_set);
        pool.add(cli_sys_export);
        pool.add(cli_echo);
        pool.add(cli_climng);
    };

    static void load(sh_Var_pool &pool)
    {
        pool.set("USER", "root");
        pool.set("DIR", "/");
        pool.set("_INPUT_FORMAT_", "[@${USER} ${DIR}]> ");
        pool.set("_INPUT_STRATEGY_", "echo ${_INPUT_FORMAT_}");
    };
}

#endif