#include "evlk_Shell.h"
#include "evlk_stand_syscli.h"
#include "HardwareSerial.h"
using namespace _EVLK_SHELL_API;
using namespace _EVLK_STAND_SYSCLI;

/**
 * @param cli::func_param
 * struct
 *  - std::vector<String> &params;  //sh对象传递过来的参数
 *  - Shell &sh;                    //用于调用sh对象的控制函数
 *
 * @return cli::func_return
 * struct
 * - int state = 0;                 //状态码返回值
 * - String value = "";             //字符串型返回值
 * **/
cli::func_return custom_sum_function(cli::func_param param)
{
    std::vector<String>::iterator it = param.params.begin();
    if (param.params.size() != 2)
    {
        return {
            .state = 0,
            .value = " 参数仅能为两个整数 "};
    }
    else
    {
        param.sh.println("自定义cli正在运行");
        int num1 = atoi((*it).c_str());
        it++;
        int num2 = atoi((*it).c_str());
        char buffer[10];
        return {
            .state = 1,
            .value = itoa(num1 + num2, buffer, 10)};
    }
}

/**
 * @param String name
 * @param function
 * @param Property
 * struct
 *  - bool display = true;   //cli的显隐性
 * **/
static cli custom_sum("custom_sum", custom_sum_function, {});

void setup()
{
    Serial.begin(115200);

    //_EVLK_STAND_SYSCLI::load中定义了一些初始关键字shell变量和cli
    _EVLK_STAND_SYSCLI::load(sysh_cli_pool);
    _EVLK_STAND_SYSCLI::load(sysh_var_pool);

    // 向默认cli池中加入自定义的cli
    sysh_cli_pool.add(custom_sum);

    // Shell sh(Serial, sysh_cli_pool);
    Shell sh(Serial);

    /** shell通过run或push函数执行命令
     ** shell::run和shell::push的主要区别在于
     ** shell::run = shell::push("${_INPUT_STRATEGY_}") + shell::push(cmd)
     ** _INPUT_STRATEGY_是关键字变量，它总是会在run函数前先被运行
     ** shell::run的返回值只有int，shell::push的返回值为cli::func_return
     ** shell::run会打印shell::push(cmd)的返回值，而shell::push只有返回值而不会打印结果
     **/
    String cmd = "custom_sum 111 222";
    Serial.println("---------------------------------");
    String v = sh.push(cmd.c_str()).value;
    Serial.println(v);
    Serial.println("---------------------------------");
    sh.run(cmd.c_str());
    Serial.println("---------------------------------");
    sh.run(); // 可以通过流输入命令
}

void loop()
{
}