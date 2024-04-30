#include "evlk_stand_syscli.h"

// ********************CLI_TOOLS*********************

#define cli_s [](Shell & sh) -> int
#define cli_c [](Shell & sh, int argc) -> int
#define cli_cv [](Shell & sh, int argc, char *argv[]) -> int
#define cli_cve [](Shell & sh, int argc, char *argv[], char *envp[]) -> int
#define clib(name) cli cli_##name(#name,cli_cv
#define clie )
#define set_eat                                                             \
    auto eat = [&argc, &argv]() -> char * {if (!argc) return NULL;argc--;argv++;return *(argv - 1); };                              \
    auto eat_ = [&argc, &argv, eat](const char *key) -> bool {if (!argc || String(argv[0]) != key)return false;return eat(); };           \
    auto Arg = [&eat_, &argc, &argv](const char *key, int num) -> bool {if (eat_(key)){if (num == argc)return true;else{argc++;argv--;return false;}}else return false; }; \
    eat()

// ***********************CLI************************
namespace _EVLK_SHELL_
{
#include "ecli_begin.h"
    clib(_INPUT_STR_)
    {
        String user = getenv("USER"), dir = getenv("DIR"), input;
        // cout << String("\33[36m[@") + user + ' ' + dir + "]>\033[0m";
        // cout << "\33[36m>\33[0m";
        cout << String('\n');
        cout << ">";
        cin >> input;
        int S = system(input.c_str());
        if (S == 127)
            cout << (String) "Error:Command not found" << endl;

        return S;
    }
    , {.display = false, .script = true} clie;

    clib(set)
    {
        auto help = [&sh]() -> int
        {
            cout << "usage" << endl;
            cout << "  set [key] [value]" << endl;
            return -1;
        };
        set_eat;
        if (argc != 2)
            return help();
        else
            return setenv(((String)argv[0] + '=' + argv[1]).c_str()) ? 0 : -1;
    }
    , {.display = false, .script = true} clie;

    clib(echo)
    {
        for (size_t i = 1; i < argc; i++)
            cout << argv[i] << String(' ');
        return 0;
    }
    , {.display = false, .script = true} clie;

    clib(tput)
    {
        auto help = [&sh]() -> int
        {
            cout << "usage" << endl;
            cout << "  tput [options]" << endl;
            cout << "options" << endl;
            cout << "  srg0" << endl;
            cout << "  bold" << endl;
            cout << "  dim" << endl;
            cout << "  smso" << endl;
            cout << "  smul/rmul" << endl;
            cout << "  blink" << endl;
            cout << "  rev" << endl;
            cout << "  invis" << endl;
            cout << "  setaf 0~9" << endl;
            cout << "  setab 0~9" << endl;
            cout << "  civis/cvvis" << endl;
            cout << "  el/el1/el2" << endl;
            cout << "  cup row col" << endl;
            return -1;
        };
        set_eat;
        String option = eat();
        size_t n = argc;
        int prarams[n] = {0};
        for (int i = 0; i < n; i++)
            prarams[i] = atoi(argv[i]);

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
                return 0;
            }

            return -1;
        }
        if (n == 1 && 0 <= prarams[0] && prarams[0] <= 9 && prarams[0] != 8)
        {
            if (option == "setaf")
                cout << String("\33[") + int(30 + prarams[0]) + 'm';
            else if (option == "setab")
                cout << String("\33[") + int(40 + prarams[0]) + 'm';
            else
                return -1;
            return 0;
        }
        if (n == 2 && option == "cup")
        {
            cout << String("\33[") + prarams[0] + ';' + prarams[1] + 'H';
            return 0;
        }

        return -1;
    }
    , {.display = false, .script = true} clie;

    clib(climan)
    {
        auto help = [&sh]() -> int
        {
            cout << "usage" << endl;
            cout << "  climan [options]" << endl;
            cout << "options" << endl;
            cout << "  -H,--help         =show help" << endl;
            cout << "  -Q [--unvisible]  =show usage commands" << endl;
            return -1;
        };
        set_eat;
        auto showcmds = [&sh](bool isunvis) -> int
        {
            for (auto i = sh.getClis()->begin(); i < sh.getClis()->end(); i++)
                if (isunvis)
                    cout << (*i)->name << endl;
                else if ((*i)->property.display)
                    cout << (*i)->name << endl;
            return 0;
        };
        if (Arg("-H", 0) || Arg("--help", 0))
        {
            help();
            return 0;
        }
        if (Arg("-Q", 0))
            return showcmds(false);
        if (Arg("-Q", 1) && Arg("--unvisible", 0))
            return showcmds(true);
        return help();
    }
    clie;
#include "ecli_end.h"
    void load(cli_pool &pool)
    {
        pool.push_back(&cli__INPUT_STR_);
        pool.push_back(&cli_set);
        pool.push_back(&cli_echo);
        pool.push_back(&cli_climan);
        pool.push_back(&cli_tput);
    };
    void load(var_pool &pool)
    {
        pool.set("USER", "root");
        pool.set("DIR", "/");
    };
}