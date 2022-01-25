#include <thread>
#include <list>
#include <mutex>
#include <map>

#include "./log/MiraiLog.h"
#include "./tool/TimeTool.h"
#include "./tool/StrTool.h"
#include "./net/MiraiNet.h"
#include "./config/config.h"
#include "./center/center.h"
#include <iostream>

#include "../IPC/ipc.h"
#include "mainprocess/mainprocess.h"
#include "plusprocess/plusprocess.h"
#include <assert.h>
#include <websocketpp/base64/base64.hpp>






int main(int argc,char * argv[])
{
	//判断是否center
	//0:
	//1:center uuid
	//2:plus_uuid
	//3:plus name base64
	bool is_mainprocess = (argc != 4); 
	if (is_mainprocess)
	{
		mainprocess();
	}
	else
	{
		int main_pid = std::stoi(argv[1]);
		std::string plus_file = websocketpp::base64_decode(argv[3]);
		plusprocess(main_pid, plus_file);
	}
	while (true) {
		TimeTool::sleep(1000);
	}


	
	//if (!is_center)
	//{
	//	//插件中，检测center是否存在，若不存在，则退出
	//	int center_pid = std::stoi(argv[1]);
	//	std::thread([center_pid]() {
	//		Json::Value to_send;
	//		to_send["action"] = "heartbeat";
	//		while (true)
	//		{
	//			const char* ret = IPC_ApiSend(center_pid, Json::FastWriter().write(to_send).c_str());
	//			if (strcmp(ret, "") == 0)
	//			{
	//				MiraiLog::get_instance()->add_warning_log("PLUS", "exit...");
	//				exit(-1);
	//			}
	//			else
	//			{
	//				MiraiLog::get_instance()->add_warning_log("PLUSRET", ret);
	//			}
	//			TimeTool::sleep(1000);
	//		}
	//	}).detach();
	//}
	//if (is_center) 
	//{
	//	center_fun();
	//}
	//else
	//{
	//	int i = std::stoi(argv[1]);
	//	std::thread([i]() {
	//		while (true)
	//		{
	//			TimeTool::sleep(5000);
	//			Json::Value to_send;
	//			to_send["action"] = "CQ_addLog";
	//			to_send["params"]["auth_code"] = _getpid(); 
	//			to_send["params"]["log_level"] = 2;
	//			to_send["params"]["category"] = "my_category";
	//			to_send["params"]["log_msg"] = "my_log_msg";
	//			const char * ret = IPC_ApiSend(i, Json::FastWriter().write(to_send).c_str());
	//			MiraiLog::get_instance()->add_debug_log("IPCPLUS", ret);
	//		}
	//	}).detach();
	//	while (true)
	//	{
	//		const char* msg = IPC_GetEvent(i);
	//		if (std::string(msg) != "")
	//		{
	//			MiraiLog::get_instance()->add_debug_log("TEST_IPC", msg);
	//		}
	//		else
	//		{
	//			TimeTool::sleep(10);
	//		}
	//	}
	//	while (1)
	//	{
	//		TimeTool::sleep(5000);
	//	}
	//}

	/*std::thread([&]() {
		while (true)
		{
			IPC_ApiRecv(fun);
			TimeTool::sleep(15);
		}
	}).detach();
	TimeTool::sleep(1000);
	MiraiLog::get_instance()->add_debug_log("IPC_ApiSend_Recv", std::to_string(_getpid()));*/
	/* int i;
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
	 }*/
}


