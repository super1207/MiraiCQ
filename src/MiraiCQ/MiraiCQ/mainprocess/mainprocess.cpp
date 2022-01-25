#include "mainprocess.h"
#include "../log/MiraiLog.h"
#include "../tool/StrTool.h"
#include "../center/center.h"
#include "../../IPC/ipc.h"
#include "../tool/TimeTool.h"
#include "../config/config.h"
#include "../tool/ThreadTool.h"
#include <jsoncpp/json.h>
#include <assert.h>


static void deal_api_thread(const std::string& sender,const std::string & flag,const std::string & msg)
{
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(msg, root))
	{
		/* Json解析失败 */
		MiraiLog::get_instance()->add_debug_log("TEST_IPC", "收到不规范的Json" + std::string(msg));
		IPC_ApiReply(sender.c_str(), flag.c_str(), NULL);
		return;
	}
	std::string action = StrTool::get_str_from_json(root, "action", "");
	Json::Value params = root.get("params", Json::Value());
	//if (!params.isObject())
	//{
	//	MiraiLog::get_instance()->add_debug_log("TEST_IPC", "收到不规范的Json" + std::string(msg));
	//	IPC_ApiReply(sender.c_str(), flag.c_str(), NULL);
	//	return;
	//}
	if (action == "CQ_addLog")
	{
		auto ret = Center::get_instance()->CQ_addLog(
			StrTool::get_int_from_json(params, "auth_code", 0),
			StrTool::get_int_from_json(params, "log_level", 0),
			StrTool::get_str_from_json(params, "category", "").c_str(),
			StrTool::get_str_from_json(params, "log_msg", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_sendGroupMsg")
	{
		auto ret = Center::get_instance()->CQ_sendGroupMsg(
			StrTool::get_int_from_json(params, "auth_code", 0),
			StrTool::get_int64_from_json(params, "group_id", 0),
			StrTool::get_str_from_json(params, "msg", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "heartbeat")
	{
		IPC_ApiReply(sender.c_str(), flag.c_str(), "OK");
	}
	else
	{
		IPC_ApiReply(sender.c_str(), flag.c_str(), NULL);
		// 无法处理的函数
	}
	
}

static void fun(const char* sender, const char* flag,const char* msg)
{
	assert(msg);
	assert(sender);
	assert(flag);
	MiraiLog::get_instance()->add_debug_log("IPCAPIRECV", msg);
	std::string msg_str = msg;
	std::string flag_str = flag;
	std::string sender_str = sender;
	ThreadTool::get_instance()->submit([sender, msg_str, flag_str, sender_str]() {
		deal_api_thread(sender_str, flag_str, msg_str);
	});
	
}

void mainprocess()
{
	// 初始化IPC服务
	if (IPC_Init("F6C3D957-BFDF-431a-997C-EE8857E595FF") != 0)
	{
		MiraiLog::get_instance()->add_fatal_log("TESTIPC", "IPC_Init 执行失败");
		exit(-1);
	}
	auto  net = MiraiNet::get_instance(Config::get_instance()->get_adapter());
	net->set_config("ws_url", Config::get_instance()->get_ws_url());
	net->set_config("access_token", Config::get_instance()->get_access_token());
	net->set_config("verifyKey", Config::get_instance()->get_verifyKey());
	net->set_config("http_url", Config::get_instance()->get_http_url());
	if (!net->connect()){
		MiraiLog::get_instance()->add_fatal_log("TESTIPC", "网络连接错误");
		exit(-1);
	}
	MiraiLog::get_instance()->add_debug_log("TESTIPC", "网络连接成功");
	Center::get_instance()->set_net(net);
	if (!Center::get_instance()->run()){
		MiraiLog::get_instance()->add_fatal_log("TESTIPC", "Center运行失败");
		exit(-1);
	}
	MiraiLog::get_instance()->add_debug_log("TEST_IPC",IPC_GetFlag());
	
	std::thread([&]() {
		while(true){
			IPC_ApiRecv(fun);
		}
	}).detach();
	while (true) {
		TimeTool::sleep(1000);
	}
	
}