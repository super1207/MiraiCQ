#include "settingDlg.h"

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Tabs.H>
#include <Fl/Fl_Text_Display.H>

#include <spdlog/fmt/fmt.h>

#include "../tool/StrTool.h"
#include "../log/MiraiLog.h"


SettingDlg::SettingDlg()
{
	const char* str1 = autoDel(new std::string(StrTool::to_utf8("详细设置(ESC退出)")))->c_str();
	win = autoDel(new Fl_Window(800, 500, str1));
	win->color(fl_rgb_color(0, 255, 255));
	win->set_modal();
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

void SettingDlg::create_debug_group(int group_x, int group_y, int group_w, int group_h)
{
}
