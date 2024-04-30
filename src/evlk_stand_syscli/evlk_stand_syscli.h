#ifndef _EVLK_SYSCLI_H_
#define _EVLK_SYSCLI_H_
#include "evlk_cli.h"
#include "evlk_shell.h"

namespace _EVLK_SHELL_
{
    extern cli cli__INPUT_STR_;
    extern cli cli_set;
    extern cli cli_echo;
    extern cli cli_climan;
    extern cli cli_tput;
    void load(cli_pool &pool);
    void load(var_pool &pool);
}

#endif