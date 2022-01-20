#include "center.h"
#include <Windows.h>
#include "../log/MiraiLog.h"
#include "../tool/PathTool.h"
#include "../tool/StrTool.h"
#include "../tool/TimeTool.h"
#include "../config/config.h"
#include "../tool/ThreadTool.h"
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
	/*if (is_run)
	{
		can_run = false;
	}
	run_thread.join();
	MiraiLog::get_instance()->add_debug_log("Center","正在卸载所有插件");
	this->del_all_plus();
	HANDLE hself = GetCurrentProcess();
	TerminateProcess(hself, 0);*/
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
		if (plus->load_plus(file_name, err))
		{
			++success_num;
			MiraiLog::get_instance()->add_info_log("Center", "插件`" + file_name + "`加载成功");
		}
		else
		{
			MiraiLog::get_instance()->add_info_log("Center", "插件`" + file_name + "`加载失败："+ err);
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
			MiraiLog::get_instance()->add_info_log("Center", "插件`" + p.second->get_name() + "`启动失败：" + err);
		}
	}
	return success_num;
}

int Center::del_all_plus() 
{
	auto plus = MiraiPlus::get_instance();
	assert(plus);
	std::vector <std::pair<int,std::string>> ac_name_vec;
	/* 获得所有插件的ac */
	{
		auto plus_map = plus->get_all_plus();
		for (auto& it : plus_map)
		{
			ac_name_vec.push_back({it.first,it.second->get_name()});
		}
	}
	int success_num = 0;
	for (auto & ac_name : ac_name_vec)
	{
		if (plus->del_plus(ac_name.first))
		{
			++success_num;
			MiraiLog::get_instance()->add_info_log("Center", "插件`" + ac_name.second + "`卸载成功");
		}
		else
		{
			MiraiLog::get_instance()->add_info_log("Center", "插件`" + ac_name.second + "`卸载失败");
		}
	}
	return success_num;
}

bool Center::run() 
{
	/* 已经在运行，直接返回true */
	if (is_run)
	{
		return true;
	}
	auto plus = MiraiPlus::get_instance();
	assert(plus);
	{
		shared_lock<shared_mutex> lk(mx_net);
		if (!net.lock())
		{
			return false;
		}
	}
	can_run = true;
	run_thread = std::thread([this]() {
		auto pool = ThreadTool::get_instance();
		//pool->init();
		is_run = true;
		MiraiLog::get_instance()->add_info_log("Center", "Center已经开始运行");
		while (can_run)
		{
			/* 在这里处理所有Net事件即可 */
			shared_ptr<MiraiNet> net;
			{
				shared_lock<shared_mutex>(mx_net);
				net = this->net.lock();
				if (!net)
				{
					TimeTool::sleep(0);
					continue;
				}
			}
			
			auto event_vec = net->get_event();
			/* 如果没获取到事件，则睡眠一下,缓解cpu压力 */
			if (event_vec.size() == 0)
			{
				TimeTool::sleep(0);
			}
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
						this->deal_event(evt);
					}
					catch (const std::exception& e)
					{
						MiraiLog::get_instance()->add_debug_log("Center", string("在事件处理中发生未知异常") + e.what());
					}
					
				});
			}
		}
		is_run = false;
	});
	/* 等待线程启动 */
	//while (!is_run)
	//{
	//	TimeTool::sleep(0);
	//}
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
	info->is_enable = pdf->is_enable;
	info->is_load = !(pdf->is_first_enable);
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
	try
	{
		typedef void(__stdcall* fun_type)();
		((fun_type)pdf->menu_vec.at(pos)->function)();
	}
	catch (const std::exception&)
	{
		return;
	}
}

void Center::normal_cal_plus_fun(int fun_type, std::function<int(const void* fun_ptr, void * user_data)> fun_ptr,void * user_data)
{
	/* 开始调用插件的函数 */
	auto plus = MiraiPlus::get_instance();
	assert(plus);
	auto all_ac = plus->get_all_ac();
	/* 按函数优先级从小到大排序 */
	std::sort(all_ac.begin(), all_ac.end(), [&](decltype(all_ac[0]) & p1,decltype(all_ac[0]) & p2) {
		auto s1 = p1.second.lock();
		auto s2 = p1.second.lock();
		/* 插件不存在，不排序 */
		if (!s1 || !s2)
		{
			return false;
		}
		auto fun1 = s1->get_event_fun(fun_type);
		auto fun2 = s1->get_event_fun(fun_type);
		/* 事件不存在，不排序 */
		if (!fun1 || !fun2)
		{
			return false;
		}
		return fun1->priority < fun2->priority;
	});
	for (auto ac : all_ac)
	{
		/* 获得插件的共享指针 */
		auto plus_def = ac.second.lock();
		if (!plus_def || !plus_def->is_enable)
		{
			//若插件未启用或者获取共享指针失败，则不处理
			continue;
		}
		auto fun = plus_def->get_event_fun(fun_type);/* 获得事件函数 */
		if (!fun)
		{
			/* 获取函数失败，则不处理 */
			continue;
		}
		int ret = fun_ptr(fun->function, user_data);
		if (ret != 0)
		{
			/*  拦截事件，阻止事件继续传递 */
			break;
		}
	}
}
