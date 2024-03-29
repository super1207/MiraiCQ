#include "center.h"
#include <Windows.h>
#include "../log/MiraiLog.h"
#include "../tool/PathTool.h"
#include "../tool/StrTool.h"
#include "../tool/TimeTool.h"
#include "../config/config.h"
#include "../tool/ThreadTool.h"
#include "../tool/IPCTool.h"
#include <cassert>
#include <algorithm>

using namespace std;

Center::Center()
{
}

Center* Center::get_instance() 
{
	static Center center;
	return &center;
}

Center::~Center()
{

}

void Center::set_net(std::weak_ptr<MiraiNet> net) 
{
	unique_lock<shared_mutex> lk(mx_net);
	this->net = net;
}

int Center::load_all_plus() 
{
	auto plus = MiraiPlus::get_instance();
	assert(plus);
	std::string exe_dir = PathTool::get_exe_dir();
	std::string plus_dir = exe_dir + "app\\";
	PathTool::create_dir(plus_dir);
	PathTool::create_dir(exe_dir + "pyapp\\");
	/* 创建图片目录 */
	PathTool::create_dir(exe_dir + "data\\");
	PathTool::create_dir(exe_dir + "data\\image");
	std::vector<std::string> path_file = PathTool::get_path_file(plus_dir);
	int success_num = 0;
	for (const auto& file_str : path_file)
	{
		if (!StrTool::end_with(StrTool::tolower(file_str), ".dll"))
		{
			continue;
		}
		auto file_name = StrTool::remove_suffix(file_str);
		MiraiLog::get_instance()->add_debug_log("Center", "开始加载插件`" + file_name + "`");
		std::string err;
		if (plus->load_plus(file_name,"dll", err))
		{
			++success_num;
			MiraiLog::get_instance()->add_info_log("Center", "插件`" + file_name + "`加载成功");
		}
		else
		{
			MiraiLog::get_instance()->add_fatal_log("Center", "插件`" + file_name + "`加载失败："+ err);
			exit(-1);
		}

	}
	 
	// 加载python插件
	{
		std::string python_path = PathTool::get_exe_dir() + "\\bin\\Python38-32\\python.exe";
		bool is_python_env = PathTool::is_file_exist(python_path);
		auto path_dirs = PathTool::get_path_dir(PathTool::get_exe_dir() + "pyapp");
		if (path_dirs.size() != 0 && is_python_env == false) {
			MiraiLog::get_instance()->add_warning_log("Center", "`" + python_path + "`不存在,pyapp下的python插件无法加载!");
		}
		else {
			for (auto& file_name : path_dirs) {
				std::string err;
				if (plus->load_plus(file_name, "py", err))
				{
					++success_num;
					MiraiLog::get_instance()->add_info_log("Center", "插件`" + file_name + "`加载成功");
				}
			}
		}
	}
	return success_num;
}

int Center::enable_all_plus() 
{
	auto plus = MiraiPlus::get_instance();
	assert(plus);
	auto plus_map = plus->get_all_plus();
	int success_num = 0;
	for (auto& p : plus_map)
	{
		std::string err;
		if (plus->enable_plus(p.first, err))
		{
			++success_num;
			MiraiLog::get_instance()->add_info_log("Center", "插件`" + p.second->get_name() + "`启动成功");
		}
		else
		{
			MiraiLog::get_instance()->add_warning_log("Center", "插件`" + p.second->get_name() + "`启动失败：" + err);
		}
	}
	return success_num;
}

static void IPC_SendEvent_T(const char* msg)
{
	auto plus = MiraiPlus::get_instance()->get_all_plus();
	for (auto p : plus) {
		IPC_SendEvent(p.second->get_uuid().c_str(), msg);
	}
}

int Center::del_all_plus() 
{
	//先禁用插件，防止插件再收到事件
	auto plus = MiraiPlus::get_instance()->get_all_plus();
	for (auto p : plus) {
		MiraiPlus::get_instance()->disable_plus(p.first);
	}
	return 0;
}

bool Center::run() 
{
	auto plus = MiraiPlus::get_instance();
	assert(plus);
	{
		shared_lock<shared_mutex> lk(mx_net);
		if (!net.lock())
		{
			return false;
		}
	}
	std::thread([this]() {
		auto pool = ThreadTool::get_instance();
		MiraiLog::get_instance()->add_info_log("Center", "Center已经开始运行");
		while (true)
		{
			/* 在这里处理所有Net事件即可 */
			shared_ptr<MiraiNet> net;
			{
				shared_lock<shared_mutex> lk(mx_net);
				net = this->net.lock();
				if (!net)
				{
					TimeTool::sleep(0);
					continue;
				}
			}
			
			auto event_vec = net->get_event(5000);
			for (auto evt : event_vec)
			{
				if (!evt)
				{
					continue;
				}
				/* 将事件放入线程池 */
				pool->submit([this,evt]() {
					try
					{
						std::map<std::string, Json::Value> cqEvtMap = this->deal_event(evt);
						if (cqEvtMap.at("1207_event").isNull() == false) {
							auto plus = MiraiPlus::get_instance()->get_all_plus();
							for (auto p : plus) {
								if (p.second->is_enable())
								{
									IPC_SendEvent(p.second->get_uuid().c_str(), Json::FastWriter().write(cqEvtMap.at("1207_event")).c_str());
								}
							}
						}

						if (cqEvtMap.at("ex_event").isNull() == false) {
							auto plus = MiraiPlus::get_instance()->get_all_plus();
							for (auto p : plus) {
								if (p.second->is_enable() && p.second->is_recive_ex_event())
								{
									IPC_SendEvent(p.second->get_uuid().c_str(), Json::FastWriter().write(cqEvtMap.at("ex_event")).c_str());
								}
							}
						}

						if (cqEvtMap.at("poke_event").isNull() == false) {
							auto plus = MiraiPlus::get_instance()->get_all_plus();
							for (auto p : plus) {
								if (p.second->is_enable() && p.second->is_recive_poke_event())
								{
									IPC_SendEvent(p.second->get_uuid().c_str(), Json::FastWriter().write(cqEvtMap.at("poke_event")).c_str());
								}
							}
						}

						if (cqEvtMap.at("cq_event").isNull() == false) {
							auto plus = MiraiPlus::get_instance()->get_all_plus();
							for (auto p : plus) {
								if (p.second->is_enable())
								{
									IPC_SendEvent(p.second->get_uuid().c_str(), Json::FastWriter().write(cqEvtMap.at("cq_event")).c_str());
								}
							}
						}
					}
					catch (const std::exception& e)
					{
						MiraiLog::get_instance()->add_debug_log("Center", string("在事件处理中发生未知异常") + e.what());
					}
					
				});
			}
		}
	}).detach();
	return true;
}

std::vector<int> Center::get_ac_vec() 
{
	auto plus = MiraiPlus::get_instance();
	assert(plus);
	vector<int> ret_vec;
	auto plus_map = plus->get_all_plus();
	for (auto& it : plus_map)
	{
		ret_vec.push_back(it.first);
	}
	return ret_vec;
}

std::shared_ptr<Center::PlusInfo> Center::get_plus_by_ac(int ac) 
{
	auto plus = MiraiPlus::get_instance();
	assert(plus);
	auto pdf = plus->get_plus(ac);
	if (!pdf)
	{
		/* ac错误 */
		return std::shared_ptr<Center::PlusInfo>();
	}
	std::shared_ptr<Center::PlusInfo> info(new Center::PlusInfo);
	info->ac = pdf->ac;
	info->description = pdf->description;
	info->name = pdf->name;
	info->version = pdf->version;
	info->author = pdf->author;
	return info;
}

std::string Center::get_menu_name_by_ac(int ac, int pos) 
{
	auto plus = MiraiPlus::get_instance();
	assert(plus);
	auto pdf = plus->get_plus(ac);
	if (!pdf)
	{
		return "";
	}
	try
	{
		return pdf->menu_vec.at(pos)->name;
	}
	catch (const std::exception&)
	{
		return "";
	}
	
}

void Center::call_menu_fun_by_ac(int ac, int pos) 
{
	auto plus = MiraiPlus::get_instance();
	assert(plus);
	auto pdf = plus->get_plus(ac);
	if (!pdf)
	{
		return ;
	}
	Json::Value to_send;
	to_send["action"] = "call_menu";
	auto menu_vec = pdf->get_menu_vec();
	try {
		to_send["params"]["fun_name"] = menu_vec.at(pos)->fun_name;
		IPC_ApiSend(pdf->get_uuid().c_str(), Json::FastWriter().write(to_send).c_str(), 100);
	}
	catch (...) {

	}

}

