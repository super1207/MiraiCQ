#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include "config/config.h"
#include "log/MiraiLog.h"
#include "tool/TimeTool.h"
#include "tool/InputTool.h"
#include "tool/StrTool.h"
#include "center/center.h"
#include "tool/PathTool.h"
#include <iostream>
#include <string>

#pragma comment(lib,"USer32.lib")
#pragma comment(lib,"gdi32.lib")

std::atomic_bool printLog = true;
static thread_local std::string retStr;

std::atomic_int last_ac = 1000, menu_pos;

//过程函数的声明
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
//入口函数 
int WINAPI mWinMain()
{
	auto uuid = StrTool::gen_uuid();
	//设计窗体
	WNDCLASSEX wndClass = { 0 };
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndClass.lpszClassName = uuid.c_str();
	wndClass.lpszMenuName = 0;
	wndClass.lpfnWndProc = WndProc;

	//注册窗体
	if (!RegisterClassEx(&wndClass))
	{
		MessageBox(NULL, "RegisterClassEx Error", "ERROR", MB_OK);
		return -1;
	}

	//创建窗体
	HWND hWnd = CreateWindow(uuid.c_str(),
		"窗口循环",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		300,
		300,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL);

	//显示与更新
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	//读取窗体消息队列
	MSG msg = { 0 };//描述消息队列的结构体
	while (msg.message != WM_QUIT)//判断是否退出程序，如果没有退出，就一直循环读取消息
	{
		if (PeekMessage(&msg,//存消息信息的结构体
			0,//窗体的句柄，0表示获取所有消息
			0,//消息最小值，为0即可
			0,//消息最大值，为0即可
			PM_REMOVE)//读取后，如果存在，发送消息，并移除该条信息
			)//读取消息
		{
			TranslateMessage(&msg);//将虚拟键消息转换为字符消息
			DispatchMessage(&msg);//把这个消息发给窗口程序
		}
	}
	UnregisterClass(uuid.c_str(), GetModuleHandle(NULL));
	return 0;
}

//窗口过程函数处理
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)//根据消息ID判断需要处理的消息
	{
	case WM_CREATE:
	{
		//调用插件菜单函数
		auto center = Center::get_instance();
		center->call_menu_fun_by_ac(last_ac, menu_pos);
		break;
	}
	case WM_PAINT://重绘窗体
		ValidateRect(hWnd, NULL);
		break;
	case WM_DESTROY://窗口的关闭消息
		PostQuitMessage(0);
		break;
	default://如果没有需要处理的消息，就调用默认的过程函数
		return DefWindowProc(hWnd, message, wparam, lparam);
	}
	return 0;
}
int InitBot()  
{
	
	static auto  net = MiraiNet::get_instance(Config::get_instance()->get_adapter());
	if (!net)
	{
		MiraiLog::get_instance()->add_debug_log("Net", "网络模块实例化失败");
		return -1;
	}
	net->set_config("ws_url", Config::get_instance()->get_ws_url());
	net->set_config("verifyKey", Config::get_instance()->get_verifyKey());
	net->set_config("http_url", Config::get_instance()->get_http_url());
	net->set_config("access_token", Config::get_instance()->get_access_token());
	/* net->set_config("adapter", Config::get_instance()->get_adapter()); */
	if (!net->connect())
	{
		MiraiLog::get_instance()->add_debug_log("Net", "网络连接错误");
		return -1;
	}
	Center::get_instance()->set_net(net);
	
	std::thread([&]() {
		while (1)
		{
			TimeTool::sleep(5000);
			if (!net->is_connect())
			{
				printLog = true;
				MiraiLog::get_instance()->add_debug_log("Net", "连接断开,正在重连...");
				auto new_net = MiraiNet::get_instance(net->get_config("net_type"));
				if (!new_net)
				{
					MiraiLog::get_instance()->add_debug_log("Net", "创建新连接失败...");
					continue;
				}
				new_net->set_all_config(net->get_all_config());
				if (!new_net->connect())
				{
					MiraiLog::get_instance()->add_debug_log("Net", "重新连接失败，5秒后再试");
					continue;
				}
				net = new_net;
				Center::get_instance()->set_net(net);
				MiraiLog::get_instance()->add_debug_log("Net", "重新连接成功");
			}
		}

	}).detach();
	if (Center::get_instance()->load_all_plus() < 0)
	{
		MiraiLog::get_instance()->add_debug_log("Center", "插件加载失败");
		return -1;
	}
	if (Center::get_instance()->enable_all_plus() < 0)
	{
		MiraiLog::get_instance()->add_debug_log("Center", "插件启动失败");
		return -1;
	}

	if (!Center::get_instance()->run())
	{
		MiraiLog::get_instance()->add_debug_log("Center", "Center运行失败");
		return -1;
	}
	return 0;
}

const char* GetAllAC(const char* gap)
{
	if (!gap)
	{
		return "";
	}
	auto center = Center::get_instance();
	auto ac_vec = center->get_ac_vec();
	if (ac_vec.size() == 0)
	{
		return "";
	}
	retStr.clear();
	for (size_t i = 0; i < ac_vec.size(); ++i)
	{
		if (i == 0)
		{
			retStr.append(std::to_string(ac_vec[i]));
		}
		else
		{
			retStr.append(gap + std::to_string(ac_vec[i]));
		}
	}
	return retStr.c_str();
}

const char* GetName(int ac)
{
	auto center = Center::get_instance();
	std::string err_msg;
	auto info = center->get_plus_by_ac(ac);
	if (!info)
	{
		return "";
	}
	retStr = info->name;
	return retStr.c_str();
}

const char* GetMenuName(int ac, int pos)
{
	auto center = Center::get_instance();
	auto name = center->get_menu_name_by_ac(ac, pos);
	retStr = name;
	return retStr.c_str();
}

void CallMenuFun(int ac, int pos)
{
	auto center = Center::get_instance();
	center->call_menu_fun_by_ac(ac, pos);
}

static std::vector<std::string> tokenize(const std::string& s, char c) {
	std::string::const_iterator end = s.end();
	std::string::const_iterator start = end;

	std::vector<std::string> v;
	for (std::string::const_iterator it = s.begin(); it != end; ++it) {
		if (*it != c) {
			if (start == end)
				start = it;
			continue;
		}
		if (start != end) {
			v.push_back(std::string(start, it));
			start = end;
		}
	}
	if (start != end)
		v.push_back(std::string(start, end));
	return v;
}

static void PrintUnkowCmd()
{
	printf("未知的命令\n");
	printf("使用`ls`来查看所有插件\n使用`log`来开关log\n输入`q`可以安全退出程序\n>>>");
}

int main()
{	
	MiraiLog::get_instance()->add_backend_sinks([&](const MiraiLog::Level& lv, const std::string& category, const std::string& dat, void* user_ptr) {
		if (!printLog)
		{
			return;
		}
		static std::mutex mx;
		std::lock_guard<std::mutex> lock(mx);
		switch (lv)
		{
		case MiraiLog::Level::DEBUG:
			printf("[DEBUG][%s]:%s\n", category.c_str(), dat.c_str());
			break;
		case MiraiLog::Level::INFO:
			printf("[INFO][%s]:%s\n", category.c_str(), dat.c_str());
			break;
		case MiraiLog::Level::WARNING:
			printf("[WARNING][%s]:%s\n", category.c_str(), dat.c_str());
			break;
		case MiraiLog::Level::FATAL:
			printf("[FATAL][%s]:%s\n", category.c_str(), dat.c_str());
			break;
		default:
			break;
		}
	}, NULL);
	if (InitBot() != 0)
	{
		printf("启动失败\n1:检查config目录下的配置文件\n2:检查app目录中有无插件\n3:检查CQP.dll是否存在\n");
		while (true)
		{
			TimeTool::sleep(0);
		}
	}
	std::string name = "";
	/* 这里延时用于等待打印lifecycle，保证视觉良好 */
	TimeTool::sleep(1000);
	printf("使用`ls`来查看所有插件\n使用`log`来开关log\n输入`q`可以安全退出程序\n>>>");
	/* printLog = !printLog; */
	while (true)
	{
		std::vector<std::string> LineVec = InputTool::get_line();
		if (LineVec.size() == 1 && LineVec[0] == "ls")
		{
			last_ac = 1000;
			std::string ac_str = GetAllAC("\n");
			auto ac_vec = tokenize(ac_str, '\n');
			printf("使用`cd <数字>`来查看一个插件的菜单:\n\n");
			for (auto i : ac_vec)
			{
				std::string name = GetName(atoi(i.c_str()));
				printf("%s	%s\n", i.c_str(), name.c_str());
			}
			printf("\n>>>");
		}
		else if (LineVec.size() == 2 && LineVec[0] == "cd")
		{
			int ac;
			try
			{
				ac = std::stoi(LineVec[1]);
			}
			catch (const std::exception&)
			{
				PrintUnkowCmd();
			}
			name = GetName(ac);
			last_ac = ac;
			printf("使用`call <数字>`来调用插件[%s]的一个菜单,对于部分插件(如铃心)，使用callx:\n\n", name.c_str());
			for (int i = 0;; ++i)
			{
				std::string name = GetMenuName(ac, i);
				if (name != "")
				{
					printf("%d	%s\n", i, name.c_str());
				}
				else
				{
					break;
				}
			}
			printf("\n>>>");
		}
		else if (LineVec.size() == 2 && LineVec[0] == "call")
		{
			int n;
			try
			{
				n = std::stoi(LineVec[1]);
			}
			catch (const std::exception&)
			{
				PrintUnkowCmd();
			}
			menu_pos = n;
			std::thread([]() {
				mWinMain();
			}).detach();
			
			
			//CallMenuFun(last_ac, n);
			printf("\n>>>");
		}
		else if (LineVec.size() == 2 && LineVec[0] == "callx")
		{
			int n;
			try
			{
				n = std::stoi(LineVec[1]);
			}
			catch (const std::exception&)
			{
				PrintUnkowCmd();
			}
			menu_pos = n;
			CallMenuFun(last_ac, n);
			printf("\n>>>");
		}
		else if (LineVec.size() == 1 && LineVec[0] == "log")
		{
			printLog = !printLog;
			if (printLog)
				printf("log已经打开,再次输入`log`来关闭log\n>>>");
			else
				printf("log已经关闭,再次输入`log`来开启log\n>>>");
		}
		else if (LineVec.size() == 1 && LineVec[0] == "q")
		{
			printLog = true;
			auto center = Center::get_instance();
			center->del_all_plus();
			HANDLE hself = GetCurrentProcess();
			TerminateProcess(hself, 0);
		}
		else if (LineVec.size() != 0)
		{
			PrintUnkowCmd();
		}
	}

	return 0;
}