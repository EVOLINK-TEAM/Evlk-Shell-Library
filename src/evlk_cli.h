#include "WString.h"
#include "vector"

namespace _EVLK_SHELL_API
{
    class Shell;
    struct cli
    {
    public:
        struct func_return
        {
            int state;
            String value;
            // func_return(int state, String value) : state(state), value(value){};
            // func_return(){};
        };
        struct func_param
        {
            std::vector<String> &params;
            Shell &sh;
        };
        typedef func_return (*cli_Func)(func_param params);

        struct Property
        {
            bool display;
            // Property(bool display) : display(display){};
            // Property(){};
        } property;

        const cli_Func func;
        const String cliName;
        // const String (*getHelp)();
        // const String (*getDescribe)();
        cli(String name, cli_Func func, Property property = {.display = true}) : func(func), cliName(name), property(property){};
    };
}