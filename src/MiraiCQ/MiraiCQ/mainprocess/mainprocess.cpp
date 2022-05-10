#include "mainprocess.h"


#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_BMP_Image.H>


#include "../log/MiraiLog.h"
#include "../center/center.h"
#include "../config/config.h"
#include "../tool/PathTool.h"
#include "../tool/StrTool.h"
#include "../tool/TimeTool.h"
#include "../tool/ThreadTool.h"
#include "../tool/IPCTool.h"
#include "./dealapi.h"
#include "./settingDlg.h"
#include "../scriptrun/ScriptRun.h"

#include <StackWalker/BaseException.h>

#include <assert.h>
#include <memory>

#include "../resource.h"


static std::atomic_int gui_flush = 0;



struct LOGIN_INFO
{
	std::string ws_url;
	std::string access_token;
	//---
	Fl_Input* _url_input;
	Fl_Input* _token_input;
	Fl_Window* _win;
	bool is_login = false;
};


static bool login(LOGIN_INFO* login_info)
{
	static auto  net = MiraiNet::get_instance(Config::get_instance()->get_adapter());
	if (!net)
	{
		std::string str = StrTool::to_utf8("网络模块实例化失败");
		fl_alert(str.c_str());
		return false;
	}
	net->set_config("ws_url", login_info->ws_url);
	net->set_config("access_token", login_info->access_token);
	net->set_config("verifyKey", Config::get_instance()->get_verifyKey());
	net->set_config("http_url", Config::get_instance()->get_http_url());
	if (!net->connect())
	{
		std::string str = StrTool::to_utf8("网络连接错误");
		fl_alert(str.c_str());
		net = MiraiNet::get_instance(Config::get_instance()->get_adapter());
		return false;
	}
	Center::get_instance()->set_net(net);
	std::thread([&]() {
		while (1)
		{
			TimeTool::sleep(5000);
			if (!net->is_connect())
			{
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
		if (!Center::get_instance()->run())
		{
			MiraiLog::get_instance()->add_debug_log("Center", "Center运行失败");
			return false;
		}
		return true;
}

static void login_dlg_cb(Fl_Widget* o, void* p) {
	LOGIN_INFO* login_info = (LOGIN_INFO*)p;
	if (strcmp(login_info->_url_input->value(), "") == 0)
	{
		std::string str = StrTool::to_utf8("ws_url不能为空");
		fl_alert(str.c_str());
		return;
	}
	login_info->ws_url = login_info->_url_input->value();
	login_info->access_token = login_info->_token_input->value();
	if (login(login_info))
	{
		login_info->is_login = true;
		login_info->_win->hide();
		Config::get_instance()->set_ws_url(login_info->ws_url);
		Config::get_instance()->set_access_token(login_info->access_token);
	}
	else
	{
		login_info->is_login = false;
	}
}

static bool login_dlg()
{
	LOGIN_INFO login_info;
	Fl_Window win(300, 180, "MiraiCQ V2.3.7");
	win.begin();
	login_info.ws_url = Config::get_instance()->get_ws_url();
	login_info.access_token = Config::get_instance()->get_access_token();
	Fl_Input url_input(100, 30, 180, 30, "ws_url");
	url_input.value(login_info.ws_url.c_str());
	Fl_Secret_Input token_input(100, 80, 180, 30, "access_token");
	token_input.value(login_info.access_token.c_str());
	std::string login_str = StrTool::to_utf8("登录");
	Fl_Button button(30, 130, 240, 30, login_str.c_str());
	win.end();
	login_info._url_input = &url_input;
	login_info._token_input = &token_input;
	login_info._win = &win;
	button.callback(login_dlg_cb, &login_info);
	win.show();
	Fl::run();
	return login_info.is_login;
}



// 插件菜单
class MyTable2 : public Fl_Table {
	std::vector<std::string> data;
	int ac = -1;
	void DrawHeader(const char* s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, fl_rgb_color(255, 204, 229));
		fl_color(FL_BLACK);
		fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		fl_pop_clip();
	}
	void DrawData(const char* s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		// Draw cell bg
		fl_color(FL_WHITE); fl_rectf(X, Y, W, H);
		// Draw cell data
		fl_color(FL_GRAY0); fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		// Draw box border
		fl_color(fl_rgb_color(51, 0, 0)); fl_rect(X, Y, W, H);
		fl_pop_clip();
	}
	void draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0, int H = 0) {
		static char s[100];
		std::string str = StrTool::to_utf8("插件菜单");
		const char* arr[] = { str.c_str() };
		switch (context) {
		case CONTEXT_STARTPAGE:                   // before page is drawn..
			fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
			return;
		case CONTEXT_COL_HEADER:                  // Draw column headers
			sprintf_s(s, "%s", arr[COL]);
			DrawHeader(s, X, Y, W, H);
			return;
		case CONTEXT_CELL:                        // Draw data in cells
			sprintf_s(s, "%s", data.at(ROW).c_str());
			DrawData(s, X, Y, W, H);
			return;
		default:
			return;
		}
	}
public:
	void event_callback2() {
		int R = callback_row();
		int C = callback_col();
		TableContext context = callback_context();
		int evt = Fl::event();
		if (context == 0x10 && evt == FL_RELEASE) //点击事件
		{
			auto center = Center::get_instance();
			if (R == data.size() - 1) //点击的是插件启停按键
			{
				if (MiraiPlus::get_instance()->is_enable(ac)) {
					MiraiPlus::get_instance()->disable_plus(ac);
					++gui_flush; //刷新界面
				}
				else {
					std::string err;
					if (!MiraiPlus::get_instance()->enable_plus(ac,err)) {
						MiraiLog::get_instance()->add_fatal_log("MAINPROCESS", err);
						exit(-1);
					}
					++gui_flush; //刷新界面
				}
			}
			else { //点击的是插件菜单
				center->call_menu_fun_by_ac(ac, R);
			}
			
		}
	}
	static void event_callback(Fl_Widget*, void* v) {
		((MyTable2*)v)->event_callback2();
	}
	static void callback_timer2(void* d) {
		static int flushh = gui_flush;
		MyTable2* instance = static_cast<MyTable2*>(d);
		// gui_flush 与上次不一样，插件菜单切换(重新渲染)
		if (flushh != gui_flush)
		{
			instance->set_ac(instance->ac);
			flushh = gui_flush;
		}
		Fl::repeat_timeout(1.0, callback_timer2, d);
	}
	MyTable2(int X, int Y, int W, int H, const char* L = 0) : Fl_Table(X, Y, W, H, L) {
		this->color(fl_rgb_color(255, 255, 255));
		this->vscrollbar->color(fl_rgb_color(255, 255, 255));
		this->vscrollbar->selection_color(fl_rgb_color(0, 255, 255));
		this->hscrollbar->color(fl_rgb_color(255, 255, 255));
		this->hscrollbar->selection_color(fl_rgb_color(0, 255, 255));
		rows(data.size());             // how many rows
		row_height_all(20);         // default height of rows
		row_resize(0);              // disable row resizing
		// Cols
		cols(1);             // how many columns
		col_header(1);              // enable column headers (along top)
		col_width_all(295);          // default width of columns
		col_resize(1);              // enable column resizing
		end();                      // end the Fl_Table group
		callback(&event_callback, (void*)this);
		Fl::add_timeout(1.0, callback_timer2, (void*)this);
	}
	void set_ac(int ac)
	{
		this->ac = ac;
		data.clear();
		auto center = Center::get_instance();
		for (int i = 0;;++i)
		{
			auto name = center->get_menu_name_by_ac(ac, i);
			if (name == "")
			{
				break;
			}
			data.push_back(StrTool::to_utf8(name));
		}
		if (MiraiPlus::get_instance()->is_enable(ac)) {
			data.push_back(StrTool::to_utf8("停用插件"));
		}
		else {
			data.clear();
			data.push_back(StrTool::to_utf8("启用插件"));
		}
		
		rows(0);
		rows(data.size());
	}
	~MyTable2() { }
};
// 插件列表
class MyTable : public Fl_Table {

	std::vector<std::pair<int, std::shared_ptr<Center::PlusInfo>>> data;
	void DrawHeader(const char* s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, fl_rgb_color(255, 204, 229));
		fl_color(FL_BLACK);
		fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		fl_pop_clip();
	}
	void DrawData(const char* s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		// Draw cell bg
		fl_color(FL_WHITE); fl_rectf(X, Y, W, H);
		// Draw cell data
		fl_color(FL_GRAY0); fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		// Draw box border
		fl_color(fl_rgb_color(51, 0, 0)); fl_rect(X, Y, W, H);
		fl_pop_clip();
	}
	void draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0, int H = 0) {
		static char s[100];
		std::string str = StrTool::to_utf8("名称");
		const char* arr[] = { str.c_str() };
		switch (context) {
		case CONTEXT_STARTPAGE:                   // before page is drawn..
			fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
			return;
		case CONTEXT_COL_HEADER:                  // Draw column headers
			sprintf_s(s, "%s", arr[COL]); 
			DrawHeader(s, X, Y, W, H);
			return;
		case CONTEXT_CELL:                        // Draw data in cells
			{
				sprintf_s(s, "%s", StrTool::to_utf8(data.at(ROW).second->name).c_str());
				bool is_enable = MiraiPlus::get_instance()->is_enable(data.at(ROW).second->ac);
				if (!is_enable) {
					sprintf_s(s, "[X] %s", std::string(s).c_str());
				}
				DrawData(s, X, Y, W, H);
				return;
			}	
		default:
			return;
		}
	}
public:
	MyTable2* tb2;
	Fl_Box* box_name;
	Fl_Box* box_author;
	Fl_Box* box_version;
	Fl_Multiline_Input* edit_des;
	void event_callback2() {
		int R = callback_row();//获取行
		TableContext context = callback_context();
		int evt = Fl::event();//FL_PUSH
		if (context == 0x10 && evt == FL_RELEASE) //点击事件
		{
			box_name->copy_label(StrTool::to_utf8(data.at(R).second->name).c_str());
			box_version->copy_label(StrTool::to_utf8(data.at(R).second->version).c_str());
			box_author->copy_label(StrTool::to_utf8(data.at(R).second->author).c_str());
			edit_des->value(StrTool::to_utf8(data.at(R).second->description).c_str());
			tb2->set_ac(data.at(R).first);

		}
	}
	static void event_callback(Fl_Widget*, void* v) {      // table's event callback (static)
		((MyTable*)v)->event_callback2();
	}
	static void callback_timer(void* d) {
		static int flushh = gui_flush;
		MyTable* instance = static_cast<MyTable*>(d);
		// gui_flush 与上次不一样，插件菜单切换(重新渲染)
		if (flushh != gui_flush)
		{
			instance->rows(0);
			instance->rows(instance->data.size());
			flushh = gui_flush;
		}
		Fl::repeat_timeout(1.0, callback_timer, d);
	}
	MyTable(int X, int Y, int W, int H, MyTable2* tb2,
		Fl_Box* box_name,
		Fl_Box* box_author,
		Fl_Box* box_version,
		Fl_Multiline_Input* edit_des,
		const char* L = 0) : Fl_Table(X, Y, W, H, L) {
		this->color(fl_rgb_color(255, 255, 255));
		auto center = Center::get_instance();
		auto ac_vec = center->get_ac_vec();
		for (auto ac : ac_vec)
		{
			auto info = center->get_plus_by_ac(ac);
			if (!info)
			{
				continue;
			}
			data.push_back({ ac,info });
		}
		rows(data.size());             // how many rows
		row_height_all(25);         // default height of rows
		cols(1);             // how many columns
		col_header(1);              // enable column headers (along top)
		col_width_all(175);          // default width of columns
		col_resize(1);              // enable column resizing
		end();                      // end the Fl_Table group
		callback(&event_callback, (void*)this);
		this->tb2 = tb2;
		this->box_name = box_name;
		this->box_author = box_author;
		this->box_version = box_version;
		this->edit_des = edit_des;
		//开启定时器，用于定时刷新界面
		Fl::add_timeout(1.0, callback_timer, (void*)this);
	}
	~MyTable() { }
};

// 详细设置的回调函数
static void ex_btn_cb(Fl_Widget* o, void* p)
{
	std::unique_ptr<SettingDlg> dlg = std::make_unique<SettingDlg>();
	dlg->show();
}

static void plus_dlg()
{
	//fl_register_images();
	std::string str1 = StrTool::to_utf8("MiraiCQ插件管理");
	Fl_Double_Window win(508, 400, str1.c_str());
	win.color(fl_rgb_color(0, 255, 255));
	win.size_range(500, 400, 500, 400);
	// table2为插件菜单
	MyTable2 table2(200, 280, 300, 110);
	std::string str2 = StrTool::to_utf8("插件名：");
	std::string str3 = StrTool::to_utf8("作者：");
	std::string str4 = StrTool::to_utf8("版本：");
	Fl_Box box_name(200, 10, 300, 20, str2.c_str());
	Fl_Box box_author(200, 40, 300, 20, str3.c_str());
	Fl_Box box_version(200, 70, 300, 20, str4.c_str());
	Fl_Multiline_Input edit_des(200, 100, 300, 180);
	// 头
	Fl_Box luna_sama_box(50, 5, 100, 100); 
	Fl_BMP_Image tmp = Fl_BMP_Image((StrTool::to_utf8(PathTool::get_exe_dir()) + "\\config\\luna_sama.bmp").c_str());
	Fl_Image* pImg = tmp.copy(100, 100);
	luna_sama_box.image(pImg);
	// table为插件列表
	MyTable table(10, 115, 180, 245, &table2, &box_name, &box_author, &box_version, &edit_des);
	std::string ex_btn_str = StrTool::to_utf8("详细设置");
	Fl_Button ex_btn(10, 365, 180, 25,ex_btn_str.c_str());
	ex_btn.color(fl_rgb_color(255, 255, 255));
	ex_btn.callback(ex_btn_cb, 0);
	win.end();
	win.show();
	Fl::run();
}

static void hide_all_window()
{
	// 获得当前进程的ID
	DWORD cur_process_id = GetCurrentProcessId();
	// 获得桌面窗口句柄
	HWND desk_window = GetDesktopWindow();
	// 遍历以桌面为根的所有窗口
	HWND window = FindWindowExA(desk_window, NULL, NULL, NULL);
	while (window)
	{
		DWORD process_id = 0;
		// 获得窗口对应进程的进程号
		GetWindowThreadProcessId(window, &process_id);
		// 如果与当前进程号相同，则隐藏窗口
		if (process_id == cur_process_id)
		{
			ShowWindow(window, SW_HIDE);
		}
		// 获得下一个窗口
		HWND window_next = FindWindowEx(desk_window, window, NULL, NULL);
		CloseHandle(window);
		window = window_next;
	}
	CloseHandle(desk_window);
}

static bool is_rub_in_cmd()
{
	DWORD ids[2] = { 0 };
	return (GetConsoleProcessList(ids, 2) > 1);
}


static void release_dll()
{
	std::string cqp1_dir = PathTool::get_exe_dir();
	std::string cqp1_file = cqp1_dir + "CQP.dll";
	std::string cqp2_dir = PathTool::get_exe_dir() + "bin\\";
	std::string cqp2_file = cqp2_dir + "CQP.dll";
	if (PathTool::is_file_exist(cqp1_file)) {
		PathTool::del_file(cqp1_file);
	}
	if (PathTool::is_file_exist(cqp2_file)) {
		PathTool::del_file(cqp2_file);
	}
	HRSRC hRes = FindResourceA(NULL, MAKEINTRESOURCEA(IDR_DLL_BIN1), "DLL_BIN");
	if (hRes == NULL) {
		MiraiLog::get_instance()->add_fatal_log("RELEASE_CQP.dll", "FindResourceA err");
		exit(-1);
	}
	HGLOBAL hMem = LoadResource(NULL, hRes);
	DWORD dwSize = SizeofResource(NULL, hRes);
	PathTool::create_dir(cqp2_dir);
	HANDLE hFile = CreateFileA(cqp2_file.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		MiraiLog::get_instance()->add_fatal_log("RELEASE_CQP.dll", "CreateFileA err");
		exit(-1);
	}
	DWORD dwWrite = 0;
	BOOL bRet = WriteFile(hFile, hMem, dwSize, &dwWrite, NULL);
	if (bRet == FALSE) {
		MiraiLog::get_instance()->add_fatal_log("RELEASE_CQP.dll", "WriteFile err");
		exit(-1);
	}
	CloseHandle(hFile);
}

static void release_config(int idr, const std::string & filename)
{
	std::string bmp_dir = PathTool::get_exe_dir() + "config\\";
	std::string bmp_file = bmp_dir + filename;
	if (PathTool::is_file_exist(bmp_file)) {
		return;
	}
	HRSRC hRes = FindResourceA(NULL, MAKEINTRESOURCEA(idr), "DLL_BIN");
	if (hRes == NULL) {
		MiraiLog::get_instance()->add_fatal_log("RELEASE_" + filename, "FindResourceA err");
		exit(-1);
	}
	HGLOBAL hMem = LoadResource(NULL, hRes);
	DWORD dwSize = SizeofResource(NULL, hRes);
	PathTool::create_dir(bmp_dir);
	HANDLE hFile = CreateFileA(bmp_file.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		MiraiLog::get_instance()->add_fatal_log("RELEASE_"+ filename, "CreateFileA err");
		exit(-1);
	}
	DWORD dwWrite = 0;
	BOOL bRet = WriteFile(hFile, hMem, dwSize, &dwWrite, NULL);
	if (bRet == FALSE) {
		MiraiLog::get_instance()->add_fatal_log("RELEASE_" + filename, "WriteFile err");
		exit(-1);
	}
	CloseHandle(hFile);
}




static void do_login()
{
	/* 登录逻辑 */
	while (true)
	{
		if (!PathTool::is_file_exist(PathTool::get_exe_dir() + "config\\config.ini"))
		{
			bool is_login = login_dlg();
			if (is_login == false)
			{
				exit(-1);
			}
			break;
		}
		LOGIN_INFO login_info;
		login_info.ws_url = Config::get_instance()->get_ws_url();
		login_info.access_token = Config::get_instance()->get_access_token();
		if (login(&login_info))
		{
			break;
		}
		else
		{
			bool is_login = login_dlg();
			if (is_login == false)
			{
				exit(-1);
			}
			break;
		}
	}
}

void mainprocess()
{
	SET_DEFULTER_HANDLER();

	if (is_rub_in_cmd())
	{
		// 在cmd中运行
		// do nothing
	}
	else
	{
		// 不在cmd中运行，则需要隐藏窗口
		hide_all_window();
	}

	MiraiLog::get_instance()->add_info_log("VERSION", "V2.3.7");
	MiraiLog::get_instance()->add_info_log("CORE", "开源地址：https://github.com/super1207/MiraiCQ");

	// 释放CQP.dll
	release_dll();

	release_config(IDR_DLL_BIN2,"luna_sama.bmp");
	release_config(IDR_DLL_BIN3,"debug_tip.json");
	release_config(IDR_DLL_BIN4, "JSON.lua");

	ScriptRun::get_instance()->init();

	// 初始化IPC服务
	if (IPC_Init("") != 0)
	{
		MiraiLog::get_instance()->add_fatal_log("TESTIPC", "IPC_Init 执行失败");
		exit(-1);
	}

	//登录
	do_login();

	/* 接收api调用 */
	std::thread([&]() {
		while (true) {
			IPC_ApiRecv(do_api_call);
		}
	}).detach();

	/* 加载并启用所有插件 */
	Center::get_instance()->load_all_plus();
	Center::get_instance()->enable_all_plus();

	/* 插件守护，发现有插件崩溃则主进程boom */
	std::atomic_bool is_protect = false;
	std::atomic_bool can_protect = true;
	std::thread([&]() {
		while (can_protect) {
			{
				is_protect = true;
				auto plus_vec = MiraiPlus::get_instance()->get_all_plus();
				for (auto plus : plus_vec)
				{
					if (!plus.second->is_enable()) {
						continue;
					}
					bool ret = plus.second->is_process_exist();
					if (ret == false) {
						MiraiLog::get_instance()->add_fatal_log("检测到插件异常退出", plus.second->get_filename());
						Center::get_instance()->del_all_plus();
						exit(-1);
					}
				}
			}
			for (int i = 0;i < 100;++i)
			{
				if (!can_protect)
					break;
				TimeTool::sleep(50);
			}
			
		}
		is_protect = false;
	}).detach();

	while (!is_protect);

	/* 打开插件菜单 */
	plus_dlg();

	auto center = Center::get_instance();

	/* 结束守护线程 */
	can_protect = false;
	while (is_protect);

	/* 7秒后强行退出退出 */
	std::thread([]() {
		TimeTool::sleep(7000);
		MiraiLog::get_instance()->add_warning_log("EXIT", "有插件不愿意自己结束自己.jpg");
		exit(-1);
	}).detach();

	/* 发送插件卸载事件 */
	center->del_all_plus();

	/* 等待插件进程全部结束 */
	MiraiLog::get_instance()->add_info_log("EXIT", "正在安全卸载所有插件");
	while (true) {
		auto plus_vec = MiraiPlus::get_instance()->get_all_plus();
		bool is_all_close = true;
		for (auto plus : plus_vec)
		{
			bool ret = plus.second->is_process_exist();
			if (ret == true) {
				is_all_close = false;
			}
		}
		if (is_all_close) {
			break;
		}
	}

	MiraiLog::get_instance()->add_info_log("EXIT", "已经安全卸载所有插件");

	/* 退出 */
	exit(0);
}