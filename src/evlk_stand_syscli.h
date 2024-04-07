#ifndef _EVLK_SYSCLI_H_
#define _EVLK_SYSCLI_H_
#include "evlk_cli.h"
#include "evlk_shell.h"

namespace _EVLK_SHELL_
{
    using namespace _EVLK_SHELL_;
#include "ecli_begin.h"

    static cli cli__INPUT_STR_(
        "_INPUT_STR_",
        [](Shell &sh) -> int
        {
            String user = getenv("USER"), dir = getenv("DIR"), input;
            // cout << String("\33[36m[@") + user + ' ' + dir + "]>\033[0m";
            // cout << "\33[36m>\33[0m";
            cout << String('\n');
            cout << ">";
            cin >> input;
            int S = system(input.c_str());
            return S; },
        {.display = false, .script = true});

    static cli cli_sys_set(
        "set",
        [](Shell &sh, int argc, char *argv[]) -> int
        {
            if (argc == 3)
                return setenv((String(argv[1]) + '=' + String(argv[2])).c_str());
            return 0;
        },
        {.display = false, .script = true});

    static cli cli_echo(
        "echo",
        [](Shell &sh, int argc, char *argv[]) -> int
        {
            for (size_t i = 1; i < argc; i++)
                cout << argv[i] << String(' ');
            return 1; },
        {.display = false, .script = true});

    static cli cli_climan(
        "climan",
        [](Shell &sh, int argc, char *argv[]) -> int
        {
            if (argc < 2)
                return 0;
            if (String(argv[1]) == "-Q")
            {
                bool isunvis = false;
                if (argc == 3)
                {
                    if (String(argv[2]) == "--unvisible")
                        isunvis = true;
                    else
                        return 0;
                }
                else if (argc != 2)
                    return 0;

                for (auto i = sh.getClis()->begin(); i < sh.getClis()->end(); i++)
                    if (isunvis)
                        cout << (*i)->name << endl;
                    else if ((*i)->property.display)
                        cout << (*i)->name << endl;
                return 1;
            }
            return 0;
        });

    static cli cli_tput(
        "tput",
        [](Shell &sh, int argc, char *argv[]) -> int
        {
            if (argc < 2)
                return 0;
            String option = argv[1];
            size_t n = argc - 2;
            int prarams[n] = {0};
            for (int i = 0; i < n; i++)
                prarams[i] = atoi(argv[i + 2]);
            if (n == 0)
            {
                const char *listm[9] = {"sgr0", "bold", "dim", "smso", "", "blink", "", "rev", "invis"};
                for (size_t i = 0; i < 9; i++)
                {
                    if (i == 4 && option == "smul" || option == "rmul")
                    {
                        cout << "\33[4m";
                        return 1;
                    }
                    else if (i == 6)
                        continue;
                    else if (option == listm[i])
                    {
                        cout << String("\33[") + int(i) + "m";
                        return 1;
                    }
                }
                const char *listk[3] = {"el", "el1", "el2"};
                for (size_t i = 0; i < 3; i++)
                {
                    if (option == listk[i])
                    {
                        cout << String("\33[") + int(i) + "K";
                        return 1;
                    }
                }
                if (option == "civis")
                {
                    cout << "\33[?25l";
                    return 1;
                }
                if (option == "cvvis")
                {
                    cout << "\33[?25h";
                    return 1;
                }

                return 0;
            }
            if (n == 1 && 0 <= prarams[0] && prarams[0] <= 9 && prarams[0] != 8)
            {
                if (option == "setaf")
                    cout << String("\33[") + int(30 + prarams[0]) + 'm';
                else if (option == "setab")
                    cout << String("\33[") + int(40 + prarams[0]) + 'm';
                else
                    return 0;
                return 1;
            }
            if (n == 2 && option == "cup")
            {
                cout << String("\33[") + prarams[0] + ';' + prarams[1] + 'H';
                return 1;
            }

            return 0;
        });
#include "ecli_end.h"

    static void load(cli_pool &pool)
    {
        pool.push_back(&cli__INPUT_STR_);
        pool.push_back(&cli_sys_set);
        pool.push_back(&cli_echo);
        pool.push_back(&cli_climan);
        pool.push_back(&cli_tput);
    };
    static void load(var_pool &pool)
    {
        pool.set("USER", "root");
        pool.set("DIR", "/");
    };
}

#endif