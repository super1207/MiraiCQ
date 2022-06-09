#include "MiraiPlus.h"

#include <Windows.h>
#include <mutex>
#include <fstream>
#include <algorithm>

#include "../log/MiraiLog.h"
#include "../tool/PathTool.h"
#include "../tool/StrTool.h"
#include "../tool/TimeTool.h"
#include "../tool/ThreadTool.h"
#include "../tool/IPCTool.h"

#include <jsoncpp/json.h>
#include <assert.h>
#include <websocketpp/base64/base64.hpp>


using namespace std;
MiraiPlus::MiraiPlus()
{
}

MiraiPlus::~MiraiPlus()
{
}


bool MiraiPlus::load_plus(const std::string& dll_name, std::string & err_msg) 
{
	err_msg.clear();
	{
		// 设置dll搜索目录
		std::string path_str = PathTool::get_exe_dir() + "bin\\";
		PathTool::create_dir(path_str);
		SetDllDirectoryA(path_str.c_str());
	}

	//判断CQP.dll是否存在
	HMODULE hHandle = LoadLibraryA("CQP.dll");
	if (hHandle == NULL) {
		err_msg = "CQP.dll加载失败，code:" + std::to_string(GetLastError());
		return false;
	}
	else {
		FreeLibrary(hHandle);
	}
	/* 创建图片目录 */ 
	PathTool::create_dir(PathTool::get_exe_dir() + "data\\");
	PathTool::create_dir(PathTool::get_exe_dir() + "data\\image");

	std::string dll_path = PathTool::get_exe_dir() + "app\\" + dll_name + ".dll";
	std::string json_path = PathTool::get_exe_dir() + "app\\" + dll_name + ".json";
	if (!PathTool::is_file_exist(dll_path))
	{
		err_msg = "模块dll文件不存在";
		return false;
	}
	if (!PathTool::is_file_exist(json_path))
	{
		err_msg = "模块json文件不存在";
		return false;
	}

	std::string json_file;
	try {
		json_file = PathTool::read_biniary_file(json_path);
		if (StrTool::is_utf8(json_file)) {
			json_file = StrTool::to_ansi(json_file);
		}
	}
	catch (...) {
		err_msg = "模块json文件读取失败";
		return false;
	}
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(json_file, root))
	{
		err_msg = "模块json文件解析失败";
		return false;
	}

	std::shared_ptr<PlusDef> plus_def(new PlusDef);

	plus_def->filename = dll_name;

	Json::Value def_str = "";
	{
		auto name_json = root.get("name", def_str);
		if (name_json.isString() && name_json.asString() != "")
		{
			plus_def->name = name_json.asString();
		}
		else
		{
			err_msg = "模块json文件解析失败,缺少name";
			return false;
		}
	}

	{
		auto version_json = root.get("version", def_str);
		if (version_json.isString() && version_json.asString() != "")
		{
			plus_def->version = version_json.asString();
		}
		else
		{
			err_msg = "模块json文件解析失败,缺少version";
			return false;
		}
	}

	// 是否接收额外的event
	{
		auto recive_ex_event_json = root.get("recive_ex_event", def_str);
		if (recive_ex_event_json.isBool())
		{
			if (recive_ex_event_json.asBool()) {
				plus_def->recive_ex_event = true;
			}
			else {
				plus_def->recive_ex_event = false;
			}
		}
		else if (recive_ex_event_json.isInt()) {
			if (recive_ex_event_json.asInt() != 0) {
				plus_def->recive_ex_event = true;
			}
			else {
				plus_def->recive_ex_event = false;
			}
		}
		else
		{
			plus_def->recive_ex_event = false;
		}
	}

	{
		auto author_json = root.get("author", def_str);
		if (author_json.isString() && author_json.asString() != "")
		{
			plus_def->author = author_json.asString();
		}
		else
		{
			err_msg = "模块json文件解析失败,缺少author";
			return false;
		}
	}

	{
		auto description_json = root.get("description", def_str);
		if (description_json.isString())
		{
			plus_def->description = description_json.asString();
		}
		else
		{
			err_msg = "模块json文件解析失败,缺少description";
			return false;
		}
	}

	auto event_json_arr = root.get("event","");
	if (event_json_arr.isArray())
	{
		for (auto& node : event_json_arr)
		{
			auto event = make_shared<PlusDef::Event>();
			{
				auto fun_name_json = node.get("function", def_str);
				if (fun_name_json.isString() && fun_name_json.asString() != "")
				{
					event->fun_name = fun_name_json.asString();
				}
				else
				{
					err_msg = "模块json文件解析失败,缺少function";
					return false;
				}
			}
			{
				auto type_json = node.get("type", def_str);
				if (type_json.isInt())
				{
					event->type = type_json.asInt();
				}
				else
				{
					err_msg = "模块json文件解析失败,event函数`"+ event->fun_name +"`缺少type";
					return false;
				}
			}
			{
				auto priority_json = node.get("priority", def_str);
				if (priority_json.isInt())
				{
					event->priority = priority_json.asInt();
				}
				else
				{
					event->priority = 30000;
					std::string msg = "函数`"+ event->fun_name +"`缺少priority,使用默认值30000";
					MiraiLog::get_instance()->add_debug_log("load_plus", msg);
				}
			}
		
			plus_def->event_vec.push_back(event);
		}
	}
	auto meun_json_arr = root.get("menu", "");
	if (meun_json_arr.isArray())
	{
		for (auto& node : meun_json_arr)
		{
			auto menu = make_shared<PlusDef::Menu>();
			{
				auto fun_name_json = node.get("function", def_str);
				if (fun_name_json.isString() && fun_name_json.asString() != "")
				{
					menu->fun_name = fun_name_json.asString();
				}
				else
				{
					err_msg = "模块json文件解析失败,缺少function";
					return false;
				}
				auto name_json = node.get("name", def_str);
				if (name_json.isString() && name_json.asString() != "")
				{
					menu->name = name_json.asString();
				}
				else
				{
					err_msg = "模块json文件解析失败,缺少name";
					return false;
				}
			}
			plus_def->menu_vec.push_back(menu);
		}
	}
	plus_def->dll_name = dll_name;
	plus_def->ac = StrTool::gen_ac();

	{
		unique_lock<shared_mutex> lock(this->mx_plus_map);
		plus_map[plus_def->ac] = plus_def;
	}

	return true;
}

bool MiraiPlus::enable_plus(int ac, std::string & err_msg) 
{
	err_msg.clear();
	std::shared_ptr<PlusDef> plus = get_plus(ac);
	if (!plus) 
	{
		err_msg = "无效的ac";
		return false;
	}
	
	{
		// 插件进程存在，说明插件已经启用
		shared_lock<shared_mutex> lock(plus->mx_plus_def);
		if (plus->process) {
			return true;
		}
	}

	// 创建UUID,先设置uuid再启动进程，可以让插件始终能顺利调用API
	std::string uuid = StrTool::gen_uuid();
	plus->set_uuid(uuid);
	assert(uuid != "");

	/* 创建插件进程 */
	std::shared_ptr<MiraiPlus::PlusDef::Process> proc = std::make_shared<MiraiPlus::PlusDef::Process>(plus->dll_name,uuid);
	
	{
		unique_lock<shared_mutex> lock(plus->mx_plus_def);
		plus->process = proc;
	}

	// 等待插件进程加载
	Json::Value to_send;
	to_send["action"] = "is_load";
	bool is_load = false;
	for (int i = 0; i < 100; ++i)
	{
		const char* ret = IPC_ApiSend(uuid.c_str(), Json::FastWriter().write(to_send).c_str(), 100);
		if (strcmp(ret, "OK") == 0)
		{
			is_load = true;
			break;
		}
	}
	if (!is_load)
	{
		err_msg = "插件无响应";
		plus->set_uuid("");
		{
			unique_lock<shared_mutex> lock(plus->mx_plus_def);
			plus->process = nullptr;
		}
		return false;
	}
	return true;
}

void MiraiPlus::disable_plus(int ac) 
{
	std::shared_ptr<PlusDef> plus = get_plus(ac);
	
	if (!plus) {
		// 错误的AC
		return ;
	}

	// 插件未启用
	if (!plus->is_enable()) {
		return ;
	}

	Json::Value to_send;
	to_send["event_type"] = "exit";
	// 发送退出事件
	IPC_SendEvent(plus->get_uuid().c_str(), to_send.toStyledString().c_str());
	// 等待进程退出(5s)
	{
		shared_lock<shared_mutex> lock(mx_plus_map);
		plus->process->wait_process_quit(5000);
	}
	{
		unique_lock<shared_mutex> lock(mx_plus_map);
		plus->process = nullptr;
	}
	plus->set_uuid("");

}

bool MiraiPlus::is_enable(int ac) 
{
	std::shared_ptr<PlusDef> plus = get_plus(ac);
	if (!plus) {
		return false;
	}
	return plus->is_enable();
}

std::shared_ptr<MiraiPlus::PlusDef> MiraiPlus::get_plus(int ac) 
{
	shared_lock<shared_mutex> lock(mx_plus_map);
	auto iter = plus_map.find(ac);
	if (iter == plus_map.end())
	{
		return nullptr;
	}
	return iter->second;
	
}

std::map<int, std::shared_ptr<MiraiPlus::PlusDef>> MiraiPlus::get_all_plus() 
{
	shared_lock<shared_mutex> lock(mx_plus_map);
	return plus_map;
}

std::vector<std::pair<int,std::weak_ptr<MiraiPlus::PlusDef>>> MiraiPlus::get_all_ac() 
{
	shared_lock<shared_mutex> lock(mx_plus_map);
	std::vector<std::pair<int, std::weak_ptr<MiraiPlus::PlusDef>>> ret_vec;
	for (auto& it : plus_map)
	{
		ret_vec.push_back({ it.first,it.second });
	}
	return ret_vec;
}

MiraiPlus* MiraiPlus::get_instance() 
{
	static MiraiPlus mirai_plus;
	return &mirai_plus;
}

MiraiPlus::PlusDef::~PlusDef()
{

}

const std::shared_ptr<const MiraiPlus::PlusDef::Event> MiraiPlus::PlusDef::get_event_fun(int type) 
{
	for (const auto& fun : event_vec)
	{
		if (fun->type == type)
		{
			return fun;
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<const MiraiPlus::PlusDef::Menu>> MiraiPlus::PlusDef::get_menu_vec() 
{
	return menu_vec;
}

std::string MiraiPlus::PlusDef::get_name() 
{
	return this->name;
}

std::string MiraiPlus::PlusDef::get_filename() 
{
	return this->filename;
}

std::string MiraiPlus::PlusDef::get_uuid()
{
	std::shared_lock<std::shared_mutex> lk(this->mx_plus_def);
	return this->uuid;
}

void MiraiPlus::PlusDef::set_uuid(const std::string & uuid)
{
	std::unique_lock<std::shared_mutex> lk(this->mx_plus_def);
	this->uuid = uuid;
}

bool MiraiPlus::PlusDef::is_enable()
{
	shared_lock<shared_mutex> lock(mx_plus_def);
	return (process != nullptr);
}



bool MiraiPlus::PlusDef::is_process_exist()
{
	shared_lock<shared_mutex> lock(mx_plus_def);
	if (process && process->is_exist()) {
		return true;
	}
	return false;
}

bool MiraiPlus::PlusDef::is_recive_ex_event()
{
	shared_lock<shared_mutex> lock(mx_plus_def);
	return this->recive_ex_event;
}

MiraiPlus::PlusDef::Process::Process(const std::string& dll_name,const std::string & uuid)
{
	char path_str[MAX_PATH + 1] = { 0 };
	if (GetModuleFileNameA(NULL, path_str, MAX_PATH) == 0)
	{
		MiraiLog::get_instance()->add_fatal_log("PLUSLOAD", "获得当前exe名称失败");
		exit(-1); 
	}
	std::string cmd = "\"" + string(path_str) + "\"";
	cmd += " ";
	cmd += IPC_GetFlag();
	cmd += " ";
	cmd += uuid;
	cmd += " ";
	cmd += websocketpp::base64_encode(dll_name);
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	MiraiLog::get_instance()->add_debug_log("PLUSLOAD", cmd);
	BOOL bRet = CreateProcessA(path_str, (LPSTR)cmd.c_str(), NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi);
	if (bRet != TRUE) {
		MiraiLog::get_instance()->add_fatal_log("PLUSLOAD", "创建插件进程失败");
		exit(-1);
	}
	CloseHandle(pi.hThread);
	this->process_handle = pi.hProcess;
	//绑定主进程，确保主进程结束后子进程能强制退出
	HANDLE job = CreateJobObjectA(NULL, NULL);
	if (job == NULL) {
		MiraiLog::get_instance()->add_fatal_log("PLUSLOAD", "CreateJobObjectA失败");
		exit(-1);
	}
	this->job_handle = job;
	BOOL ret = AssignProcessToJobObject(job, this->process_handle);
	if (ret != TRUE) {
		MiraiLog::get_instance()->add_fatal_log("PLUSLOAD", "将子进程附加到主进程失败1");
		exit(-1);
	}
	assert(ret == TRUE);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION limit_info = {0};
	limit_info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	ret = SetInformationJobObject(
		job,
		JobObjectExtendedLimitInformation,
		&limit_info,
		sizeof(limit_info)
	);
	if (ret != TRUE) {
		MiraiLog::get_instance()->add_fatal_log("PLUSLOAD", "设置子进程自动结束失败");
		exit(-1);
	}
}

bool MiraiPlus::PlusDef::Process::is_exist()
{
	assert(process_handle != NULL);
	DWORD dw = WaitForSingleObject(process_handle, 1);
	if (dw == WAIT_OBJECT_0 || dw == WAIT_FAILED) {
		return false;
	}
	return true;
}

void MiraiPlus::PlusDef::Process::wait_process_quit(int timeout)
{
	DWORD dw = WaitForSingleObject(process_handle, timeout);
}

MiraiPlus::PlusDef::Process::~Process()
{
	//TerminateProcess((HANDLE)process_handle, 0);
	CloseHandle(process_handle);
	CloseHandle(job_handle);
}
