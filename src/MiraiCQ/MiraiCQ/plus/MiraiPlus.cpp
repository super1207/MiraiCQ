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

using namespace std;
MiraiPlus::MiraiPlus()
{
}

MiraiPlus::~MiraiPlus()
{
}

bool MiraiPlus::load_plus(const std::string& dll_name, std::string & err_msg) noexcept
{
	err_msg.clear();
	std::string bin_path = PathTool::get_exe_dir() + "";
	PathTool::create_dir(bin_path);
	//SetDllDirectoryA(bin_path.c_str()); 
	std::string cqp_path = bin_path + "CQP.dll";
	if (!PathTool::is_file_exist(cqp_path.c_str()))
	{
		err_msg = "CQP.dll不在目录下";
		return false;
	}
	if (GetModuleHandleA(cqp_path.c_str()) == NULL)
	{
		if (!LoadLibraryA(cqp_path.c_str()))
		{
			err_msg = "CQP.dll加载失败";
			return false;
		}
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
	plus_def->module_hand = LoadLibraryA(dll_path.c_str());
	if (!plus_def->module_hand)
	{
		err_msg = "模块dll文件加载失败";
		return false;
	}

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
			
			{
				void* dll_func = GetProcAddress((HMODULE)plus_def->module_hand, event->fun_name.c_str());
				if (dll_func == NULL)
				{
					err_msg = "函数`"+ event->fun_name +"`加载失败";
					return false;
				}
				event->function = dll_func;
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
					menu->name = fun_name_json.asString();
				}
				else
				{
					err_msg = "模块json文件解析失败,缺少function";
					return false;
				}
			}
			{
				void* dll_func = GetProcAddress((HMODULE)plus_def->module_hand, menu->name.c_str());
				if (dll_func == NULL)
				{
					err_msg = "函数`" + menu->name + "`加载失败";
					return false;
				}
				menu->function = dll_func;
			}
			plus_def->menu_vec.push_back(menu);
		}
	}

	plus_def->ac = StrTool::gen_ac();
	{
		typedef __int32(__stdcall* fun_ptr_type_1)(__int32);
		fun_ptr_type_1 fun_ptr1 = (fun_ptr_type_1)GetProcAddress((HMODULE)plus_def->module_hand, "Initialize");
		if (!fun_ptr1)
		{
			err_msg = "函数Initialize获取失败";
			return false;
		}
		fun_ptr1(plus_def->ac);
	}

	{
		unique_lock<shared_mutex> lock(mx_plus_map);
		plus_map[plus_def->ac] = plus_def;
	}

	return true;
}

bool MiraiPlus::enable_plus(int ac, std::string & err_msg) noexcept
{
	err_msg.clear();
	auto plus = get_plus(ac);
	if (!plus)
	{
		err_msg = "ac不存在";
		return false;
	}

	if (plus->is_enable) /* 插件已经被启用 */
	{
		return true;
	}
	else
	{
		plus->is_enable = true;
	}
	
	/* 插件第一次启用 */
	if (plus->is_first_enable)
	{
		plus->is_first_enable = false;
		auto fun =  plus->get_event_fun(1001); /* 框架启动 */
		if (fun)
		{ 
			/* shared_lock<shared_mutex> lock(plus->mx); */
			typedef __int32(__stdcall* fun_ptr_type_1)();
			int ret = ((fun_ptr_type_1)fun->function)();
			if (ret != 0)
			{
				err_msg = "插件`"+ plus->name+"`拒绝启动";
				plus->is_enable = false;
				return false;
			}
		}
			
	}

	auto fun = plus->get_event_fun(1003); /* 插件启用 */
	if (fun)
	{
		/* shared_lock<shared_mutex> lock(plus->mx); */
		typedef __int32(__stdcall* fun_ptr_type_1)();
		int ret = ((fun_ptr_type_1)fun->function)();
		if (ret != 0)
		{
			err_msg = "插件`" + plus->name + "`拒绝启用";
			plus->is_enable = false;
			return false;
		}
	}
	return true;
}

bool MiraiPlus::disable_plus(int ac, std::string & err_msg) noexcept
{
	err_msg.clear();
	auto plus = get_plus(ac);
	if (!plus)
	{
		err_msg = "ac不存在";
		return false;
	}
	if (!plus->is_enable) /* 插件没有启用 */
	{
		return true;
	}
	else 
	{
		plus->is_enable = false;
	}
	auto fun = plus->get_event_fun(1004); /* 插件禁用 */
	if (fun)
	{
		/* shared_lock<shared_mutex> lock(plus->mx); */
		typedef __int32(__stdcall* fun_ptr_type_1)();
		int ret = ((fun_ptr_type_1)fun->function)();
		if (ret != 0)
		{
			plus->is_enable = true;
			err_msg = "插件`" + plus->name + "`拒绝被禁用";
			return false;
		}
	}
	return true;
}

bool MiraiPlus::is_enable(int ac) noexcept
{
	auto plus = get_plus(ac);
	if (!plus)
	{
		return false;
	}
	return plus->is_enable;
}

bool MiraiPlus::del_plus(int ac) noexcept
{
	auto plus = get_plus(ac);
	if (!plus)
	{
		/* ac不存在，说明已经被卸载 */
		return true;
	}
	if (!plus->is_first_enable) /* 初始化的插件才能调用卸载函数 */
	{
		auto fun =plus->get_event_fun(1002); /* 框架退出事件 */
		if (fun)
		{
			/* shared_lock<shared_mutex> lock(plus->mx); */
			typedef __int32(__stdcall* fun_ptr_type_1)();
			int ret = ((fun_ptr_type_1)fun->function)();
			if (ret != 0)
			{
				plus->is_enable = true;
				//err_msg = "插件`" + plus->name + "`拒绝被卸载";
				return false;
			}
		}
	}
	return true;
}

std::shared_ptr<MiraiPlus::PlusDef> MiraiPlus::get_plus(int ac) noexcept
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

std::map<int, std::shared_ptr<MiraiPlus::PlusDef>> MiraiPlus::get_all_plus() noexcept
{
	shared_lock<shared_mutex> lock(mx_plus_map);
	return plus_map;
}

std::vector<std::pair<int,std::weak_ptr<MiraiPlus::PlusDef>>> MiraiPlus::get_all_ac() noexcept
{
	shared_lock<shared_mutex> lock(mx_plus_map);
	std::vector<std::pair<int, std::weak_ptr<MiraiPlus::PlusDef>>> ret_vec;
	for (auto& it : plus_map)
	{
		ret_vec.push_back({ it.first,it.second });
	}
	return ret_vec;
}

MiraiPlus* MiraiPlus::get_instance() noexcept
{
	static MiraiPlus mirai_plus;
	return &mirai_plus;
}

MiraiPlus::PlusDef::~PlusDef()
{

}

const std::shared_ptr<const MiraiPlus::PlusDef::Event> MiraiPlus::PlusDef::get_event_fun(int type) noexcept
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

std::vector<std::shared_ptr<const MiraiPlus::PlusDef::Menu>> MiraiPlus::PlusDef::get_menu_vec() noexcept
{
	return menu_vec;
}

std::string MiraiPlus::PlusDef::get_name() noexcept
{
	return this->name;
}

std::string MiraiPlus::PlusDef::get_filename() noexcept
{
	return this->filename;
}

