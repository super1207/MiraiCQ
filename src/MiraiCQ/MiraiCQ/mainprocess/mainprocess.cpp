#include "mainprocess.h"


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


#include "../log/MiraiLog.h"
#include "../center/center.h"
#include "../config/config.h"
#include "../tool/PathTool.h"
#include "../tool/StrTool.h"
#include "../tool/TimeTool.h"
#include "../tool/ThreadTool.h"
#include "../tool/IPCTool.h"

#include <StackWalker/BaseException.h>

#include <assert.h>




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
	button.callback(login_dlg_cb, &login_info);
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
		const char* arr[] = { str.c_str() };
		switch (context) {
		case CONTEXT_STARTPAGE:                   // before page is drawn..
			fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
			return;
		case CONTEXT_COL_HEADER:                  // Draw column headers
			sprintf_s(s, "%s", arr[COL]);                // "A", "B", "C", etc.
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
		const char* arr[] = { str.c_str() };
		switch (context) {
		case CONTEXT_STARTPAGE:                   // before page is drawn..
			fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
			return;
		case CONTEXT_COL_HEADER:                  // Draw column headers
			sprintf_s(s, "%s", arr[COL]);                // "A", "B", "C", etc.
			DrawHeader(s, X, Y, W, H);
			return;
		case CONTEXT_CELL:                        // Draw data in cells
			sprintf_s(s, "%s", StrTool::to_utf8(data.at(ROW).second->name).c_str());
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

static void plus_dlg()
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

static std::string get_fun_name_by_type(const std::string& uuid, int tp)
{
	auto all_plus = MiraiPlus::get_instance()->get_all_plus();
	for (auto plus : all_plus) {
		if (uuid != plus.second->uuid) {
			continue;
		}
		for (auto fun : plus.second->event_vec) {
			if (fun->type != tp) {
				continue;
			}
			return fun->fun_name;
		}
	}
	return "";
}

static int get_auth_code_by_uuid(const std::string& uuid)
{
	 auto plus = MiraiPlus::get_instance()->get_all_plus();
	 for (auto i : plus) {
		 if (uuid == i.second->uuid) {
			 return i.first;
		 }
	 }
	 MiraiLog::get_instance()->add_fatal_log("GETAUTHCODEERR", uuid);
	 exit(-1);
}

static void deal_api_thread_(const std::string& sender, const std::string& flag, const std::string& msg)
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
	if (params.isObject())
	{
		params["auth_code"] = get_auth_code_by_uuid(flag);
	}
	if (action == "CQ_sendPrivateMsg") {
		auto ret = Center::get_instance()->CQ_sendPrivateMsg(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_str_from_json(params, "msg", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_sendGroupMsg") {
		auto ret = Center::get_instance()->CQ_sendGroupMsg(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_str_from_json(params, "msg", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_sendDiscussMsg") {
		auto ret = Center::get_instance()->CQ_sendDiscussMsg(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "discuss_id", 0)
			, StrTool::get_str_from_json(params, "msg", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_deleteMsg") {
		auto ret = Center::get_instance()->CQ_deleteMsg(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "msg_id", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_sendLike") {
		auto ret = Center::get_instance()->CQ_sendLike(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_sendLikeV2") {
		auto ret = Center::get_instance()->CQ_sendLikeV2(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int_from_json(params, "times", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupKick") {
		auto ret = Center::get_instance()->CQ_setGroupKick(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int_from_json(params, "reject_add_request", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupBan") {
		auto ret = Center::get_instance()->CQ_setGroupBan(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int64_from_json(params, "duration", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupAnonymousBan") {
		auto ret = Center::get_instance()->CQ_setGroupAnonymousBan(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_str_from_json(params, "anonymous", "").c_str()
			, StrTool::get_int64_from_json(params, "duration", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupWholeBan") {
		auto ret = Center::get_instance()->CQ_setGroupWholeBan(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int_from_json(params, "enable", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupAdmin") {
		auto ret = Center::get_instance()->CQ_setGroupAdmin(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int_from_json(params, "set", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupAnonymous") {
		auto ret = Center::get_instance()->CQ_setGroupAnonymous(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int_from_json(params, "enable", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupCard") {
		auto ret = Center::get_instance()->CQ_setGroupCard(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_str_from_json(params, "new_card", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupLeave") {
		auto ret = Center::get_instance()->CQ_setGroupLeave(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int_from_json(params, "is_dismiss", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupSpecialTitle") {
		auto ret = Center::get_instance()->CQ_setGroupSpecialTitle(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_str_from_json(params, "new_special_title", "").c_str()
			, StrTool::get_int64_from_json(params, "duration", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setDiscussLeave") {
		auto ret = Center::get_instance()->CQ_setDiscussLeave(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "discuss_id", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setFriendAddRequest") {
		auto ret = Center::get_instance()->CQ_setFriendAddRequest(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "response_flag", "").c_str()
			, StrTool::get_int_from_json(params, "response_operation", 0)
			, StrTool::get_str_from_json(params, "remark", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupAddRequest") {
		auto ret = Center::get_instance()->CQ_setGroupAddRequest(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "response_flag", "").c_str()
			, StrTool::get_int_from_json(params, "request_type", 0)
			, StrTool::get_int_from_json(params, "response_operation", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupAddRequestV2") {
		auto ret = Center::get_instance()->CQ_setGroupAddRequestV2(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "response_flag", "").c_str()
			, StrTool::get_int_from_json(params, "request_type", 0)
			, StrTool::get_int_from_json(params, "response_operation", 0)
			, StrTool::get_str_from_json(params, "reason", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getLoginQQ") {
		auto ret = Center::get_instance()->CQ_getLoginQQ(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getLoginNick") {
		auto ret = Center::get_instance()->CQ_getLoginNick(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getStrangerInfo") {
		auto ret = Center::get_instance()->CQ_getStrangerInfo(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int_from_json(params, "no_cache", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getFriendList") {
		auto ret = Center::get_instance()->CQ_getFriendList(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int_from_json(params, "reserved", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getGroupList") {
		auto ret = Center::get_instance()->CQ_getGroupList(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getGroupInfo") {
		auto ret = Center::get_instance()->CQ_getGroupInfo(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int_from_json(params, "no_cache", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getGroupMemberList") {
		auto ret = Center::get_instance()->CQ_getGroupMemberList(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getGroupMemberInfoV2") {
		auto ret = Center::get_instance()->CQ_getGroupMemberInfoV2(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int_from_json(params, "no_cache", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getCookies") {
		auto ret = Center::get_instance()->CQ_getCookies(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getCookiesV2") {
		auto ret = Center::get_instance()->CQ_getCookiesV2(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "domain", "").c_str()
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getCsrfToken") {
		auto ret = Center::get_instance()->CQ_getCsrfToken(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getAppDirectory") {
		auto ret = Center::get_instance()->CQ_getAppDirectory(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getRecord") {
		auto ret = Center::get_instance()->CQ_getRecord(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "file", "").c_str()
			, StrTool::get_str_from_json(params, "out_format", "").c_str()
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getRecordV2") {
		auto ret = Center::get_instance()->CQ_getRecordV2(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "file", "").c_str()
			, StrTool::get_str_from_json(params, "out_format", "").c_str()
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getImage") {
		auto ret = Center::get_instance()->CQ_getImage(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "file", "").c_str()
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_canSendImage") {
		auto ret = Center::get_instance()->CQ_canSendImage(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_canSendRecord") {
		auto ret = Center::get_instance()->CQ_canSendRecord(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_addLog") {
		auto ret = Center::get_instance()->CQ_addLog(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int_from_json(params, "log_level", 0)
			, StrTool::get_str_from_json(params, "category", "").c_str()
			, StrTool::get_str_from_json(params, "log_msg", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setFatal") {
		auto ret = Center::get_instance()->CQ_setFatal(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "error_info", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setRestart") {
		auto ret = Center::get_instance()->CQ_setRestart(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_callApi") {
		auto ret = Center::get_instance()->CQ_callApi(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "msg", "").c_str()
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "get_fun_name")
	{
		int funtype = StrTool::get_int_from_json(root, "params", 0);
		std::string funname = get_fun_name_by_type(flag, funtype);
		IPC_ApiReply(sender.c_str(), flag.c_str(), funname.c_str());
	}
	else if (action == "heartbeat")
	{
		IPC_ApiReply(sender.c_str(), flag.c_str(), "OK");
	}
	else
	{
		IPC_ApiReply(sender.c_str(), flag.c_str(), "");
		// 无法处理的函数
	}
}

static void deal_api_thread(const std::string& sender, const std::string& flag, const std::string& msg)
{
	try {
		deal_api_thread_(sender, flag, msg);
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_warning_log("deal_api_thread", e.what());
	}

}

static void fun(const char* sender, const char* flag, const char* msg)
{
	assert(msg);
	assert(sender);
	assert(flag);
	std::string msg_str = msg;
	std::string flag_str = flag;
	std::string sender_str = sender;
	ThreadTool::get_instance()->submit([msg_str, flag_str, sender_str]() {
		deal_api_thread(sender_str, flag_str, msg_str);
	});

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

	// 初始化IPC服务
	if (IPC_Init("") != 0)
	{
		MiraiLog::get_instance()->add_fatal_log("TESTIPC", "IPC_Init 执行失败");
		exit(-1);
	}

	/* 登录逻辑 */
	while (true)
	{
		if (!PathTool::is_file_exist(PathTool::get_exe_dir() + "config\\config.ini"))
		{
			bool is_login = login_dlg();
			if (is_login == false)
			{
				return;
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
				return;
			}
			break;
		}
	}

	/* 接收api调用 */
	std::thread([&]() {
		while (true) {
			IPC_ApiRecv(fun);
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
					Json::Value to_send;
					to_send["action"] = "heartbeat";
					std::string ret = IPC_ApiSend(plus.second->uuid.c_str(), to_send.toStyledString().c_str(), 2000);
					if (ret == "") {
						MiraiLog::get_instance()->add_fatal_log("检测到插件异常退出", plus.second->get_filename());
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
		exit(0);
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
			Json::Value to_send;
			to_send["action"] = "heartbeat";
			std::string ret = IPC_ApiSend(plus.second->uuid.c_str(), to_send.toStyledString().c_str(), 2000);
			if (ret == "OK") {
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