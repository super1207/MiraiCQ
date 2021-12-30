#pragma warning(disable : 4996)
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Multiline_Input.H>




#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>

#include "config/config.h"
#include "log/MiraiLog.h"
#include "tool/TimeTool.h"
#include "tool/InputTool.h"
#include "tool/StrTool.h"
#include "center/center.h"
#include "tool/PathTool.h"

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

static void login_cb(Fl_Widget* o, void* p) {
	LOGIN_INFO* login_info = (LOGIN_INFO*)p;
	if (strcmp(login_info->_url_input->value(), "") == 0)
	{
		std::string str = StrTool::to_utf8("ws_url不能为空");
		fl_alert(str.c_str());
		return;
	}
	login_info->ws_url = login_info->_url_input->value();
	login_info->access_token = login_info->_token_input->value();

	static auto  net = MiraiNet::get_instance(Config::get_instance()->get_adapter());
	if (!net)
	{
		std::string str = StrTool::to_utf8("网络模块实例化失败");
		fl_alert(str.c_str());
		return;
	}
	net->set_config("ws_url", login_info->ws_url);
	net->set_config("access_token", login_info->access_token);
	if (!net->connect())
	{
		std::string str = StrTool::to_utf8("网络连接错误");
		fl_alert(str.c_str());
		net = MiraiNet::get_instance(Config::get_instance()->get_adapter());
		return;
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
		if (Center::get_instance()->load_all_plus() < 0)
		{
			MiraiLog::get_instance()->add_debug_log("Center", "插件加载失败");
			return;
		}
		if (Center::get_instance()->enable_all_plus() < 0)
		{
			MiraiLog::get_instance()->add_debug_log("Center", "插件启动失败");
			return;
		}

		if (!Center::get_instance()->run())
		{
			MiraiLog::get_instance()->add_debug_log("Center", "Center运行失败");
			return;
		}
		login_info->is_login = true;
		login_info->_win->hide();
}

static bool login()
{
	LOGIN_INFO login_info;
	Fl_Window win(300, 180, "MiraiCQ V2.0");
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
	button.callback(login_cb, &login_info);
	win.show();
	Fl::run();
	return login_info.is_login;
}



// Derive a class from Fl_Table
class MyTable2 : public Fl_Table {
	std::vector<std::string> data;
	int ac = -1;
	void DrawHeader(const char* s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
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
		fl_color(color()); fl_rect(X, Y, W, H);
		fl_pop_clip();
	}
	void draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0, int H = 0) {
		static char s[100];
		std::string str = StrTool::to_utf8("插件菜单");
		const char* arr[] = { str.c_str()};
		switch (context) {
		case CONTEXT_STARTPAGE:                   // before page is drawn..
			fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
			return;
		case CONTEXT_COL_HEADER:                  // Draw column headers
			sprintf(s, "%s", arr[COL]);                // "A", "B", "C", etc.
			DrawHeader(s, X, Y, W, H);
			return;
		case CONTEXT_CELL:                        // Draw data in cells
			sprintf(s, "%s", data.at(ROW).c_str());
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
		int evt = Fl::event();//FL_PUSH
		if (context == 0x10 && evt == FL_RELEASE)
		{
			auto center = Center::get_instance();
			center->call_menu_fun_by_ac(ac, R);
		}
	}
	static void event_callback(Fl_Widget*, void* v) {      // table's event callback (static)
		((MyTable2*)v)->event_callback2();
	}
	MyTable2(int X, int Y, int W, int H, const char* L = 0) : Fl_Table(X, Y, W, H, L) {
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
		rows(data.size());
	}
	~MyTable2() { }
};
// Derive a class from Fl_Table
class MyTable : public Fl_Table {

	std::vector<std::pair<int, std::shared_ptr<Center::PlusInfo>>> data;
	void DrawHeader(const char* s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
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
		fl_color(color()); fl_rect(X, Y, W, H);
		fl_pop_clip();
	}
	void draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0, int H = 0) {
		static char s[100];
		std::string str = StrTool::to_utf8("名称");
		const char* arr[] = { str.c_str()};
		switch (context) {
		case CONTEXT_STARTPAGE:                   // before page is drawn..
			fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
			return;
		case CONTEXT_COL_HEADER:                  // Draw column headers
			sprintf(s, "%s", arr[COL]);                // "A", "B", "C", etc.
			DrawHeader(s, X, Y, W, H);
			return;
		case CONTEXT_CELL:                        // Draw data in cells
			sprintf(s, "%s", StrTool::to_utf8(data.at(ROW).second->name).c_str());
			DrawData(s, X, Y, W, H);
			return;
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
		if (context == 0x10 && evt == FL_RELEASE)
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
	MyTable(int X, int Y, int W, int H, MyTable2* tb2,
		Fl_Box* box_name,
		Fl_Box* box_author,
		Fl_Box* box_version,
		Fl_Multiline_Input* edit_des,
		const char* L = 0) : Fl_Table(X, Y, W, H, L) {

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
		row_height_all(20);         // default height of rows
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
	}
	~MyTable() { }
};

void plus_dlg()
{
	std::string str1 = StrTool::to_utf8("MiraiCQ插件管理");
	Fl_Window win(500, 400, str1.c_str());
	win.size_range(500, 400, 500, 400);
	MyTable2 table2(200, 280, 300, 110);
	std::string str2 = StrTool::to_utf8("插件名：");
	std::string str3 = StrTool::to_utf8("作者：");
	std::string str4 = StrTool::to_utf8("版本：");
	Fl_Box box_name(200, 10, 300, 20, str2.c_str());
	Fl_Box box_author(200, 40, 300, 20, str3.c_str());
	Fl_Box box_version(200, 70, 300, 20, str4.c_str());
	Fl_Multiline_Input edit_des(200, 100, 300, 180);
	MyTable table(10, 10, 180, 380, &table2, &box_name, &box_author, &box_version, &edit_des);
	win.end();
	win.show();
	Fl::run();
}


int  main(int argc, char* argv[])
{
	/* 登录 */
	bool is_login = login();
	if (is_login == false)
	{
		return -1;
	}
	/* 插件菜单 */
	plus_dlg();
	/* 退出 */
	auto center = Center::get_instance();
	std::thread([]() {
		TimeTool::sleep(10000);
		HANDLE hself = GetCurrentProcess();
		TerminateProcess(hself, 0);
	}).detach();
	center->del_all_plus();
	HANDLE hself = GetCurrentProcess();
	TerminateProcess(hself, 0);
	return 0;
}
