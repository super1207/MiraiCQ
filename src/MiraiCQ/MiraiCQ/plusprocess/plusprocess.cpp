#include "plusprocess.h"


#include<thread>
#include <jsoncpp/json.h>
#include <Windows.h>
#include <assert.h>
#include <FL/Fl.H>

#include "../../IPC/ipc.h"
#include "../log/MiraiLog.h"
#include "../tool/TimeTool.h"
#include "../tool/StrTool.h"
#include "../tool/PathTool.h"
#include "../tool/ThreadTool.h"


extern std::string g_main_flag;
static std::string g_dll_path;

static std::atomic_bool g_close_heartbeat = false;

/* 用于从插件dll中获取函数地址 */
static void* get_fun_ptr(const std::string& dll_path, const std::string& fun_name)
{
	HMODULE hand = GetModuleHandleA(dll_path.c_str());
	if (hand == NULL)
	{
		hand = LoadLibraryA(dll_path.c_str());
	}
	if (hand == NULL)
	{
		return NULL;
	}
	return GetProcAddress(hand, fun_name.c_str());
}

/* 用于向主进程查询函数名字 */
static std::string get_fun_name(int funtype)
{
	static std::map<int, std::string> mmap;
	static std::mutex mx;
	{
		std::lock_guard<std::mutex>lk(mx);
		if (mmap.find(funtype) != mmap.end()) {
			return mmap.at(funtype);
		}
	}
	
	Json::Value to_send;
	to_send["action"] = "get_fun_name";
	to_send["params"] = funtype;
	const char* ret = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 3000);
	if (strcmp(ret, "") != 0) {
		std::lock_guard<std::mutex>lk(mx);
		mmap[funtype] = ret;
	}
	return ret;
}

/* 用于判断主进程是否还存在，若不存在，则结束当前进程 */
static void do_heartbeat(const std::string& main_flag)
{
	Json::Value to_send;
	to_send["action"] = "heartbeat";
	while (true)
	{
		const char* ret = IPC_ApiSend(main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 3000);
		if (strcmp(ret, "") == 0)
		{
			if (g_close_heartbeat){
				break;
			}
			MiraiLog::get_instance()->add_warning_log("do_heartbeat", "检测到主进程无响应，所以插件进程强制退出");
			exit(-1);
		}
		TimeTool::sleep(5000);
	}
}


/* 用于加载插件dll,并且设置静态全局变量`g_dll_path` */
static void load_plus(const std::string& plus_name)
{
	/* 获得插件绝对路径，不含后缀 */
	std::string plus_path = PathTool::get_exe_dir() + "app\\" + plus_name;
	/* 获得CQP的绝对路径 */
	std::string cqp_path = PathTool::get_exe_dir() + "CQP.dll";
	/* 加载CQP.dll */
	if (LoadLibraryA(cqp_path.c_str()) == NULL)
	{
		MiraiLog::get_instance()->add_fatal_log("LOADPLUS", "CQP.dll加载失败");
		exit(-1);
	}
	/* 加载插件dll */
	std::string plus_dll_path = plus_path + ".dll";
	/* 全局变量赋值 */
	g_dll_path = plus_dll_path;
	HMODULE hand = LoadLibraryA(plus_dll_path.c_str());
	if (hand == NULL)
	{
		MiraiLog::get_instance()->add_fatal_log("LOADPLUS", plus_dll_path + "加载失败");
		exit(-1);
	}
	/* 调用插件的Initialize函数 */
	typedef __int32(__stdcall* fun_ptr_type_1)(__int32);
	fun_ptr_type_1 fun_ptr1 = (fun_ptr_type_1)GetProcAddress(hand, "Initialize");
	if (!fun_ptr1)
	{
		MiraiLog::get_instance()->add_fatal_log("LOADPLUS", plus_dll_path + ":函数Initialize获取失败");
		exit(-1);
	}
	fun_ptr1(1); //这里ac直接传1
}

/* 用于调用插件的start和enable函数 */
void call_start(void* user_data)
{
	typedef __int32(__stdcall* fun_ptr_type_1)();
	int ret = ((fun_ptr_type_1)user_data)();
	if (ret != 0) {
		MiraiLog::get_instance()->add_fatal_log("START","插件拒绝启用, 强制退出");
		exit(-1);
	}
}

/* 用于调用插件菜单 */
void call_menu(void* user_data)
{
	typedef __int32(__stdcall* fun_ptr_type_1)();
	((fun_ptr_type_1)user_data)();
}

/* IPC_ApiRecv的回调函数，用于接收主进程的指令 */
static void fun(const char* sender, const char* flag, const char* msg)
{
	assert(msg);
	assert(sender);
	assert(flag);
	std::string msg_str = msg;
	std::string flag_str = flag;
	std::string sender_str = sender;
	/* 在新的线程里面处理API调用 */
	std::thread([msg_str, flag_str, sender_str]() {
		try
		{
			Json::Value root;
			Json::Reader reader;
			if (!reader.parse(msg_str, root))
			{
				MiraiLog::get_instance()->add_debug_log("PLUS_API_FUN", "收到不规范的Json" + std::string(msg_str));
				IPC_ApiReply(sender_str.c_str(), flag_str.c_str(), "");
				return;
			}
			std::string action = StrTool::get_str_from_json(root, "action", "");
			if (action == "is_load")
			{
				// 用于主进程判断插件进程是否正常启动
				IPC_ApiReply(sender_str.c_str(), flag_str.c_str(), "OK");
				return;
			}
			else if (action == "start")
			{
				// CQ的启动函数
				void* fptr = get_fun_ptr(g_dll_path, root["params"]["fun_name"].asString());
				if (!fptr) {
					IPC_ApiReply(sender_str.c_str(), flag_str.c_str(), "");
					return;
				}
				IPC_ApiReply(sender_str.c_str(), flag_str.c_str(), "OK");
				Fl::awake(call_start, fptr);
				return;
			}
			else if (action == "enable")
			{
				// 插件启用函数
				void* fptr = get_fun_ptr(g_dll_path, root["params"]["fun_name"].asString());
				if (!fptr) {
					IPC_ApiReply(sender_str.c_str(), flag_str.c_str(), "");
					return;
				}
				IPC_ApiReply(sender_str.c_str(), flag_str.c_str(), "OK");
				Fl::awake(call_start, fptr);
				return;
			}
			else if (action == "call_menu") 
			{
				// 插件菜单
				void* fptr = get_fun_ptr(g_dll_path, root["params"]["fun_name"].asString());
				if (!fptr) {
					IPC_ApiReply(sender_str.c_str(), flag_str.c_str(), "");
					return;
				}
				IPC_ApiReply(sender_str.c_str(), flag_str.c_str(), "OK");
				Fl::awake(call_menu, fptr);
				return;
			}
			else if (action == "heartbeat")
			{
				// 心跳，用于主进程判断插件进程是否存活（意外崩溃）
				IPC_ApiReply(sender_str.c_str(), flag_str.c_str(), "OK");
				return;
			}
			IPC_ApiReply(sender_str.c_str(), flag_str.c_str(), "");
		}
		catch (const std::exception& e)
		{
			MiraiLog::get_instance()->add_warning_log("PLUS_API_FUN", "抛出异常" + std::string(e.what()));
		}
		
	}).detach();
}

/* 用于处理主进程传来的事件 */
static void do_event(Json::Value & root) {
	std::string event_type = StrTool::get_str_from_json(root, "event_type", "");
	MiraiLog::get_instance()->add_debug_log("PLUS","收到主进程的事件类型："+ event_type);
	if (event_type == "cq_event_group_message")
	{
		std::string fun_name = get_fun_name(2);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* cq_event_group_message)(int sub_type, int msg_id, int64_t from_group, int64_t from_qq, const char* anonymous, const char* msg, int font);
			((cq_event_group_message)fun_ptr)(
				root["data"]["sub_type"].asInt(),
				root["data"]["msg_id"].asInt(),
				root["data"]["from_group"].asInt64(),
				root["data"]["from_qq"].asInt64(),
				root["data"]["anonymous"].asCString(),
				root["data"]["msg"].asCString(),
				root["data"]["font"].asInt()
				);
		}
	}
	else if (event_type == "cq_1207_event")
	{
		std::string fun_name = get_fun_name(1207);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* cq_1207_event)(const char* msg);
			((cq_1207_event)fun_ptr)(
				root["data"]["msg"].asCString()
				);
		}
	}
	else if (event_type == "cq_event_private_message")
	{
		std::string fun_name = get_fun_name(21);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* cq_event_private_message)(int sub_type, int msg_id, int64_t from_qq, const char* msg, int font);
			((cq_event_private_message)fun_ptr)(
				root["data"]["sub_type"].asInt(),
				root["data"]["msg_id"].asInt(),
				root["data"]["from_qq"].asInt64(),
				root["data"]["msg"].asCString(),
				root["data"]["font"].asInt()
				);
		}
	}
	else if (event_type == "cq_event_group_request")
	{
		std::string fun_name = get_fun_name(302);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* cq_event_group_request)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, const char* msg, const char* response_flag);
			((cq_event_group_request)fun_ptr)(
				root["data"]["sub_type"].asInt(),
				root["data"]["send_time"].asInt(),
				root["data"]["from_group"].asInt64(),
				root["data"]["from_qq"].asInt64(),
				root["data"]["msg"].asCString(),
				root["data"]["response_flag"].asCString()
				);
		}
	}
	else if (event_type == "cq_event_friend_request")
	{
		std::string fun_name = get_fun_name(301);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* cq_event_friend_request)(__int32 sub_type, __int32 send_time, __int64 from_qq, const char* msg, const char* response_flag);
			((cq_event_friend_request)fun_ptr)(
				root["data"]["sub_type"].asInt(),
				root["data"]["send_time"].asInt(),
				root["data"]["from_qq"].asInt64(),
				root["data"]["msg"].asCString(),
				root["data"]["response_flag"].asCString()
				);
		}
	}
	else if (event_type == "cq_event_friend_add")
	{
		std::string fun_name = get_fun_name(201);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* cq_event_friend_add)(__int32 sub_type, __int32 send_time, __int64 from_qq);
			((cq_event_friend_add)fun_ptr)(
				root["data"]["sub_type"].asInt(),
				root["data"]["send_time"].asInt(),
				root["data"]["from_qq"].asInt64()
				);
		}
	}
	else if (event_type == "cq_event_group_ban")
	{
		std::string fun_name = get_fun_name(104);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* cq_event_group_ban)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, __int64 being_operate_qq, __int64 duration);
			((cq_event_group_ban)fun_ptr)(
				root["data"]["sub_type"].asInt(),
				root["data"]["send_time"].asInt(),
				root["data"]["from_group"].asInt64(),
				root["data"]["from_qq"].asInt64(),
				root["data"]["being_operate_qq"].asInt64(),
				root["data"]["duration"].asInt64()
				);
		}
	}
	else if (event_type == "cq_event_group_member_increase")
	{
		std::string fun_name = get_fun_name(103);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* cq_event_group_member_increase)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, __int64 being_operate_qq);
			((cq_event_group_member_increase)fun_ptr)(
				root["data"]["sub_type"].asInt(),
				root["data"]["send_time"].asInt(),
				root["data"]["from_group"].asInt64(),
				root["data"]["from_qq"].asInt64(),
				root["data"]["being_operate_qq"].asInt64()
				);
		}
	}
	else if (event_type == "cq_event_group_member_decrease")
	{
		std::string fun_name = get_fun_name(102);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* cq_event_group_member_decrease)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, __int64 being_operate_qq);
			((cq_event_group_member_decrease)fun_ptr)(
				root["data"]["sub_type"].asInt(),
				root["data"]["send_time"].asInt(),
				root["data"]["from_group"].asInt64(),
				root["data"]["from_qq"].asInt64(),
				root["data"]["being_operate_qq"].asInt64()
				);
		}
	}
	else if (event_type == "cq_event_group_admin")
	{
		std::string fun_name = get_fun_name(101);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* cq_event_group_admin)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 being_operate_qq);
			((cq_event_group_admin)fun_ptr)(
				root["data"]["sub_type"].asInt(),
				root["data"]["send_time"].asInt(),
				root["data"]["from_group"].asInt64(),
				root["data"]["being_operate_qq"].asInt64()
				);
		}
	}
	else if (event_type == "cq_event_group_upload")
	{
		std::string fun_name = get_fun_name(11);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* cq_event_group_upload)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, const char* file_base64);
			((cq_event_group_upload)fun_ptr)(
				root["data"]["sub_type"].asInt(),
				root["data"]["send_time"].asInt(),
				root["data"]["from_group"].asInt64(),
				root["data"]["from_qq"].asInt64(),
				root["data"]["file_base64"].asCString()
				);
		}
	}
	else if (event_type == "exit")
	{
		g_close_heartbeat = true;
		MiraiLog::get_instance()->add_info_log("PLUS", "接收到主进程发送的退出事件，正在安全退出...");
		std::thread([]() {
			TimeTool::sleep(5000);
			/* 安全退出(指强制结束进程 */
			exit(-1);
		}).detach();
		std::string fun_name = get_fun_name(1004);
		void* fun_ptr = get_fun_ptr(g_dll_path, fun_name);
		if (fun_ptr)
		{
			typedef int(__stdcall* exit_event)();
			((exit_event)fun_ptr)();
		}
		exit(-1);
	}
	else { 
		MiraiLog::get_instance()->add_warning_log("EVENTRECV", "收到未知的事件类型:" + root.toStyledString());
	}
}
void plusprocess(const std::string& main_flag, const std::string& plus_flag, const std::string& plus_name)
{
	try
	{
		/* 初始化IPC */
		if (IPC_Init(plus_flag.c_str()) != 0)
		{
			MiraiLog::get_instance()->add_fatal_log("TESTPLUS", "IPC_Init 执行失败");
			exit(-1);
		}

		/* 加载插件，加载失败会强制退出进程 */
		load_plus(plus_name);

		/* 用于处理主进程下发的事件 */
		std::thread([plus_flag]() {
			while (true)
			{
				const char* evt = IPC_GetEvent(plus_flag.c_str());
				
				Json::Value root;
				Json::Reader reader;
				if (!reader.parse(evt, root))
				{
					/* Json解析失败 */
					MiraiLog::get_instance()->add_warning_log("EVENTRECV", "收到不规范的Json" + std::string(evt));
					continue;
				}
				try {
					ThreadTool::get_instance()->submit([=]() {
						Json::Value root_ = root;
						do_event(root_);
					});
					// 目前事件先不做多线程，防止莫些插件不能正确处理
					// do_event(root);
				}
				catch (const std::exception& e) {
					MiraiLog::get_instance()->add_warning_log("EVENTRECV", std::string("do_event发生异常：") + e.what());
				}
			}
		}).detach();

		/* 用于判断主进程是否结束，来结束自己 */
		std::thread([main_flag]() {
			do_heartbeat(main_flag);
		}).detach();

		/* 处理主进程的API调用 */
		std::thread([&]() {
			while (true) {
				IPC_ApiRecv(fun);
			}
		}).detach();

		/* 窗口循环 */
		while (true) {
			TimeTool::sleep(20);
			Fl::wait(1e20);
		}
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_fatal_log("PLUS", e.what());
	}


}