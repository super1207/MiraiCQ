#include "MiraiPlus.h"

#include <Windows.h>
#include <mutex>
#include <fstream>
#include <algorithm>

#include "../log/MiraiLog.h"
#include "../tool/PathTool.h"
#include "../tool/StrTool.h"
#include "../tool/TimeTool.h"
#include <jsoncpp/json.h>
#include <assert.h>
#include "../../IPC/ipc.h"
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
	std::string bin_path = PathTool::get_exe_dir() + "";
	PathTool::create_dir(bin_path);
	std::string cqp_path = bin_path + "CQP.dll";
	if (!PathTool::is_file_exist(cqp_path.c_str()))
	{
		err_msg = "CQP.dll不在目录下";
		return false;
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
	ifstream json_fp;
	json_fp.open(json_path);
	if (!json_fp.is_open())
	{
		err_msg = "模块json文件打开失败";
		return false;
	}
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(json_fp, root))
	{
		err_msg = "模块json文件解析失败";
		return false;
	}
	json_fp.close();
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
	plus_def->uuid = StrTool::gen_uuid();
	plus_def->ac = StrTool::gen_ac();

	char path_str[MAX_PATH + 1] = { 0 };
	if (GetModuleFileNameA(NULL, path_str, MAX_PATH) == 0)
	{
		err_msg = "获得当前进程名失败";
		return false;
	}
	IPC_Init("");
	std::string cmd = path_str;
	cmd += " ";
	cmd += IPC_GetFlag();
	cmd += " ";
	cmd += plus_def->uuid;
	cmd += " ";
	cmd += websocketpp::base64_encode(dll_name);
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	MiraiLog::get_instance()->add_debug_log("PLUSLOAD", cmd);
	BOOL bRet = CreateProcessA(path_str, (LPSTR)cmd.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (bRet != TRUE) {
		err_msg = "创建插件进程失败";
		return false;
	}
	else
	{
		/* 关闭不需要的句柄,MiraiCQ通过uuid与共享内存与插件进程通信，不需要句柄 */
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		Json::Value to_send;
		to_send["action"] = "is_load";
		bool is_load = false;
		for (int i = 0;i < 100;++i)
		{
			const char* ret = IPC_ApiSend(plus_def->uuid.c_str(), Json::FastWriter().write(to_send).c_str(), 100);
			if (strcmp(ret, "OK") == 0)
			{
				is_load = true;
				break;
			}
		}
		if (!is_load)
		{
			err_msg = "插件无响应";
			return false;
		}
	}

	{
		unique_lock<shared_mutex> lock(mx_plus_map);
		plus_map[plus_def->ac] = plus_def;
	}

	return true;
}

bool MiraiPlus::enable_plus(int ac, std::string & err_msg) 
{
	err_msg.clear();
	auto sptr = this->get_plus(ac);
	if (!sptr)
	{
		err_msg = "插件不存在";
		return false;
	}
	{
		Json::Value to_send;
		auto fptr = sptr->get_event_fun(1001);
		if (fptr) {
			to_send["action"] = "start";
			to_send["params"]["fun_name"] = fptr->fun_name;
			std::string ret = IPC_ApiSend(sptr->uuid.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			if (ret != "OK")
			{
				err_msg = "插件拒绝启用或无响应";
				return false;
			}
		}
	}

	{
		Json::Value to_send;
		auto fptr = sptr->get_event_fun(1003);
		if (fptr) {
			to_send["action"] = "start";
			to_send["params"]["fun_name"] = fptr->fun_name;
			std::string ret = IPC_ApiSend(sptr->uuid.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			if (ret != "OK")
			{
				err_msg = "插件拒绝启用或无响应";
				return false;
			}
		}
	}
	sptr->is_enable = true;
	return true;
}

bool MiraiPlus::disable_plus(int ac, std::string & err_msg) 
{
	err_msg.clear();
	return true;
}

bool MiraiPlus::is_enable(int ac) 
{
	return true;
}

bool MiraiPlus::del_plus(int ac) 
{
	return true;
}

std::shared_ptr<MiraiPlus::PlusDef> MiraiPlus::get_plus(int ac) 
{
	shared_lock<shared_mutex> lock(mx_plus_map);
	auto iter = plus_map.find(ac);
	if (iter == plus_map.end())
	{
		/* ac不存在，说明已经被卸载 */
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

