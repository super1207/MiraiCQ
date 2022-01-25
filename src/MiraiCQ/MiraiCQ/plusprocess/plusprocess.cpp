#include "plusprocess.h"
#include "../../IPC/ipc.h"
#include<thread>
#include "../log/MiraiLog.h"
#include "../tool/TimeTool.h"
#include <jsoncpp/json.h>
void plusprocess(const std::string& main_flag, const std::string& plus_flag, const std::string& plus_name)
{
	if (IPC_Init(plus_flag.c_str()) != 0)
	{
		MiraiLog::get_instance()->add_fatal_log("TESTPLUS", "IPC_Init Ö´ÐÐÊ§°Ü");
		exit(-1);
	}
	std::thread([main_flag]() {
		while (true)
		{
			const char * evt = IPC_GetEvent(main_flag.c_str());
			MiraiLog::get_instance()->add_debug_log("TESTPLUS", evt);
		}
	}).detach();
	std::thread([main_flag]() {
		Json::Value to_send;
		to_send["action"] = "heartbeat";
		while (true)
		{
			const char* ret = IPC_ApiSend(main_flag.c_str(), Json::FastWriter().write(to_send).c_str(),3000);
			if (strcmp(ret, "") == 0)
			{
				MiraiLog::get_instance()->add_warning_log("PLUS", "exit...");
				exit(-1);
			}
			else
			{
				MiraiLog::get_instance()->add_warning_log("PLUSRET", ret);
			}
			TimeTool::sleep(5000);
		}
	}).detach();
	while (true) {
		TimeTool::sleep(5000);
		Json::Value to_send;
		to_send["action"] = "CQ_addLog";
		to_send["params"]["auth_code"] = _getpid(); 
		to_send["params"]["log_level"] = 2;
		to_send["params"]["category"] = "my_category";
		to_send["params"]["log_msg"] = "my_log_msg";
		const char * ret_str = IPC_ApiSend(main_flag.c_str(), Json::FastWriter().write(to_send).c_str(),15000);
		MiraiLog::get_instance()->add_debug_log("TESTPLUS IPC_ApiSend_Recv", ret_str);
	}
}