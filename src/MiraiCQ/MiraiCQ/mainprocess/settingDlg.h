#pragma once

#include <list>
#include <functional>
#include <vector>

#include <FL/Fl_Window.H>
#include <FL/Fl_Multiline_Input.H>

class SettingDlg
{
public:
	SettingDlg();
	~SettingDlg();
	void show();

private:
	void send_btn_cb();
	void com_tip_cb(Fl_Widget* o);
	static void send_btn_cb_t(Fl_Widget* o, void* p);
	static void com_tip_cb_t(Fl_Widget* o, void* p);
	template<typename T>
	T autoDel(T ptr)
	{
		if (ptr) {
			delete_list.push_front([ptr]() {
				delete ptr;
			});
		}
		return ptr;
	}
	const char* make_u8_str(const std::string& str);
	void create_log_group(int group_x,int group_y,int group_w,int group_h);
	void create_debug_group(int group_x, int group_y, int group_w, int group_h);
	void create_filter_group(int group_x, int group_y, int group_w, int group_h);
	Fl_Window * win = 0;
	Fl_Multiline_Input* edit_send = nullptr; 
	Fl_Multiline_Input* edit_debug = nullptr;
	std::list<std::function<void()>> delete_list;
	std::vector<std::pair<std::string,std::string>> model_tip_list;
};

