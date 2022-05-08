#pragma once

#include <list>
#include <functional>

#include <FL/Fl_Window.H>

class SettingDlg
{
public:
	SettingDlg();
	~SettingDlg();
	void show();

private:
	void ex_btn_cb()
	{
		win->hide();
	}
	static void ex_btn_cb_t(Fl_Widget* o, void* p)
	{
		((SettingDlg*)p)->ex_btn_cb();
	}
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
	void create_log_group(int group_x,int group_y,int group_w,int group_h);
	void create_debug_group(int group_x, int group_y, int group_w, int group_h);
	Fl_Window * win = 0;
	std::list<std::function<void()>> delete_list;
};

