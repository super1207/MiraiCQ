#include <thread>
#include <list>
#include <mutex>
#include <map>

#include "./log/MiraiLog.h"
#include "./tool/TimeTool.h"
#include "./tool/StrTool.h"

#include "../IPC/ipc.h"

 

static char * fun(const char * msg)
{
    MiraiLog::get_instance()->add_debug_log("IPCAPIRECV", msg);
    char  * ret = (char *)IPC_Malloc(1000);
    sprintf_s(ret,1000, "%s", "hello api");
    return ret;
}
#include <iostream>
int main() 
{
    /*std::thread([&]() {
        while (true)
        {
            IPC_ApiRecv(fun);
            TimeTool::sleep(15);
        }
    }).detach();
    TimeTool::sleep(1000);
    MiraiLog::get_instance()->add_debug_log("IPC_ApiSend_Recv", std::to_string(_getpid()));*/
    int i;
    std::cin >> i;
    while (true)
    {
        const char* ret_str = IPC_ApiSend(i, "ths msg");
        MiraiLog::get_instance()->add_debug_log("IPC_ApiSend_Recv", ret_str);
        TimeTool::sleep(1000);
    }
    while(true)
    {
        TimeTool::sleep(1000);
    }
}


