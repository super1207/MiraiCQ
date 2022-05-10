#include "settingDlg.h"

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Tabs.H>
#include <Fl/Fl_Text_Display.H>
#include <Fl/Fl_Choice.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Multiline_Input.H>

#include <spdlog/fmt/fmt.h>

#include "../tool/StrTool.h"
#include "../log/MiraiLog.h"
#include "../tool//PathTool.h"
#include "../center/center.h"

#include <lua.hpp>




SettingDlg::SettingDlg()
{
	const char* str1 = autoDel(new std::string(StrTool::to_utf8("详细设置(ESC退出)")))->c_str();
	win = autoDel(new Fl_Window(800, 500, str1));
	win->color(fl_rgb_color(0, 255, 255));
	win->set_modal();
	std::string exe_dir = PathTool::get_exe_dir();
	PathTool::create_dir(exe_dir + "\\config");
	std::string json_file = exe_dir + "\\config\\debug_tip.json";
	if (PathTool::is_file_exist(json_file))
	{
		std::string file_data = PathTool::read_biniary_file(json_file);
		Json::Value root;
		Json::Reader reader;
		if (reader.parse(file_data, root))
		{
			if (root.isArray()) {
				for (auto it : root) {
					model_tip_list.push_back(std::pair<std::string, std::string>{ it["key"].asString(), it["val"].asString() });
				}
			}
		}
	}
	//model_tip_list.push_back(std::pair<std::string, std::string>{ StrTool::to_utf8("取登录号信息"), "{\n\t\"action\":\"get_login_info\",\n\t\"params\":{}\n}" });
	//model_tip_list.push_back(std::pair<std::string, std::string>{ StrTool::to_utf8("456"), StrTool::to_utf8("这是456") });
}


SettingDlg::~SettingDlg()
{
	for (auto& it : this->delete_list) {
		it();
	}
}

void SettingDlg::show()
{
	/*std::string ex_btn_str = StrTool::to_utf8("保存");
	Fl_Button ex_btn(10, 10, 70, 25, ex_btn_str.c_str());
	ex_btn.color(fl_rgb_color(255, 255, 255));
	ex_btn.callback(SettingDlg::ex_btn_cb_t, this);*/

	// define size---------------------------
	int margin = 10;
	int tabs_x = 0 + margin;
	int tabs_y = 10;
	int tabs_w = win->w() - 2 * tabs_x;
	int tabs_h = win->h() - tabs_y - margin;
	int tabs_hh = 25; // header高度
	int group_x = tabs_x;
	int group_y = tabs_y + tabs_hh;
	int group_w = tabs_w;
	int group_h = tabs_h - tabs_hh;;

	Fl_Tabs *  tabs = autoDel(new Fl_Tabs(tabs_x, tabs_y, tabs_w, tabs_h));
	
	const char* group1_str = autoDel(new std::string(StrTool::to_utf8("  日志  ")))->c_str();
	Fl_Group* log_group = autoDel(new Fl_Group(margin, tabs_y + tabs_hh, tabs_w, tabs_h - tabs_hh, group1_str));
	create_log_group(group_x, group_y, group_w, group_h);
	log_group->end();


	const char* group2_str = autoDel(new std::string(StrTool::to_utf8("  调试  ")))->c_str();
	Fl_Group* debug_group2 = autoDel(new Fl_Group(margin, tabs_y + tabs_hh, tabs_w, tabs_h - tabs_hh, group2_str));
	create_debug_group(group_x, group_y, group_w, group_h);
	debug_group2->end();


	tabs->end();

	win->end();
	win->show();
	while (win->shown()) Fl::wait();
	
}

class AutoFlUnlock {
public:
	AutoFlUnlock() {
		Fl::lock();
	}
	~AutoFlUnlock() {
		Fl::unlock();
	}
};


const char* SettingDlg::make_u8_str(const std::string& str)
{
	return autoDel(new std::string(StrTool::to_utf8(str)))->c_str();
}

void SettingDlg::create_log_group(int group_x, int group_y, int group_w, int group_h)
{
	Fl_Text_Buffer* log_buffer = autoDel(new Fl_Text_Buffer);
	Fl_Text_Display* edit_des = autoDel(new Fl_Text_Display(group_x, group_y, group_w, group_h));
	edit_des->buffer(log_buffer);
	std::string log_flag = StrTool::gen_uuid();
	MiraiLog::get_instance()->add_front_sinks(log_flag, [this,edit_des, log_buffer](const MiraiLog::Level& lv, const std::string& category, const std::string& dat, void* user_ptr) -> std::pair<std::string, std::string> {
		AutoFlUnlock fl_lock;
		if (log_buffer->length() > 100000) {
			char* text = log_buffer->text_range(0, 100000);
			log_buffer->remove(0, log_buffer->length());
			log_buffer->append(text);
			free(text);
		}
		std::string logStr = StrTool::to_utf8(fmt::format("[{}]:{}\n", category, dat));
		log_buffer->append(logStr.c_str());
		return { category ,dat };
	}, 0);
	this->delete_list.push_front([log_flag]() {
		MiraiLog::get_instance()->del_front_sinks(log_flag);
	});
	
}

void SettingDlg::com_tip_cb(Fl_Widget* o)
{
	Fl_Choice* combox = (Fl_Choice*)o;
	int index = combox->value();
	this->edit_send->value(model_tip_list.at(index).second.c_str());
}

void SettingDlg::com_tip_cb_t(Fl_Widget* o, void* p)
{
	((SettingDlg*)p)->com_tip_cb(o);
}

void SettingDlg::send_btn_cb()
{
	std::string to_send_str = this->edit_send->value();
	lua_State* L = luaL_newstate();
	if (L) {
		lua_close(L);
	}
	this->edit_debug->value(fmt::format("{}:\n{}\n", StrTool::to_utf8("调试发送"), to_send_str).c_str());
	std::string ret;
	try
	{
		ret = Center::get_instance()->CQ_callApi(1, this->edit_send->value());
	}
	catch (const std::exception & e) {
		ret = e.what();
		this->edit_debug->value(fmt::format("{}{}:\n{}\n", this->edit_debug->value(), StrTool::to_utf8("调试发送失败"), ret).c_str());
		return;
	}
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(ret, root))
	{
		ret = StrTool::to_utf8("Json解析失败:") + ret;
	}
	else {
		ret = root.toStyledString();
	}
	this->edit_debug->value(fmt::format("{}{}:\n{}\n", this->edit_debug->value(), StrTool::to_utf8("调试接收"), ret).c_str());
}

void SettingDlg::send_btn_cb_t(Fl_Widget* o, void* p)
{
	((SettingDlg*)p)->send_btn_cb();
}

void SettingDlg::create_debug_group(int group_x, int group_y, int group_w, int group_h)
{
	const char* comlable_str = make_u8_str("选择模板");
	Fl_Box* comlable = autoDel(new Fl_Box(group_x + 10, group_y + 10, 100 , 25, comlable_str));
	Fl_Choice * combox = autoDel(new Fl_Choice(group_x+ 10 + 100 +10, group_y + 10, group_w - 10 - 120 ,25));
	for (size_t i = 0; i < model_tip_list.size(); ++i)
	{
		combox->add(model_tip_list.at(i).first.c_str());
	}
	Fl_Multiline_Input* edit_send = autoDel(new Fl_Multiline_Input(group_x + 10, group_y + 45, (group_w - 20) / 2, group_h - 80));
	Fl_Multiline_Input* edit_debug = autoDel(new Fl_Multiline_Input(group_x + 10 + (group_w - 20) / 2, group_y + 45, (group_w - 20) / 2, group_h - 80));
	edit_debug->value(StrTool::to_utf8("debug info:\n").c_str());
	edit_debug->readonly(1);
	this->edit_debug = edit_debug;
	this->edit_send = edit_send;
	combox->callback(com_tip_cb_t, this);
	Fl_Button * send_btn = autoDel(new Fl_Button(group_x + 10, group_y + group_h - 35, group_w - 20, 25, make_u8_str("发送")));
	send_btn->callback(SettingDlg::send_btn_cb_t, this);
}
