#include "evlk_cli.h"
#include "evlk_Shell.h"
#include "evlk_stand_syscli.h"
#include "HardwareSerial.h"
using namespace _EVLK_SHELL_;
#define cout sh.cout
#define endl String('\n')
#define cin sh.cin

/**
 * @param sh   shell对象接口
 * @param argc 传入参数个数
 * @param argv 传入参数数组
 * @param envp 环境参数数组,以NULL为结尾
 * @return 返回状态码 0-正常结束,-1-非正常结束
 * @note 其他重载方式：
 * @note int (Shell &sh);
 * @note int (Shell &sh, int argc);
 * @note int (Shell &sh, int argc, char *argv[]);
 * @note int (Shell &sh, int argc, char *argv[], char *envp[]);
 * **/
int custom_sum_function(Shell &sh, int argc, char *argv[], char *envp[])
{
    if (argc != 3)
    {
        cout << "参数无效，请传入两个参数" << endl;
        return -1;
    }
    else
    {
        // argv[0]是命令名
        int num1 = atoi(argv[1]);
        int num2 = atoi(argv[2]);
        if ((!num1 && (String)argv[0] != "0") || (!num2 && (String)argv[1] != "0"))
        {
            cout << "参数无效" << endl;
            return -1;
        }

        cout << (String)num1 + "+" + num2 + "=" + (num1 + num2) << endl;
        return 0;
    }
}

/**
 * @param Name
 * @param Function
 * @param Property
 * **/
static cli custom_sum("csum", custom_sum_function);

// Shell sh(Serial, sysh_cli_pool, sysh_var_pool);
Shell sh(Serial);

void setup()
{
    Serial.begin(115200);

    // evlk_stand_syscli.h中定义了一些初始默认关键字shell变量和cli
    _EVLK_SHELL_::load(sysh_cli_pool);
    _EVLK_SHELL_::load(sysh_var_pool);
    // sh.begin(sysh_cli_pool, sysh_var_pool);
    sh.begin();

    // 添加自定义的cli
    sh << custom_sum;

    /** shell通过run或push函数执行命令
     ** shell::run和shell::system的主要区别在于
     ** shell::run = system("_INPUT_STR_")
     ** _INPUT_STR_是自定义的输入框监听程序
     **/
    String cmd = "csum 111 222";
    Serial.println("---------------------------------");
    sh.system(cmd.c_str());
    Serial.print("---------------------------------");
}

void loop()
{
    sh.run(); // 可以通过流输入命令
}