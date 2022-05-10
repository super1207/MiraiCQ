#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include <jsoncpp/json.h>

class ScriptRun
{
public:
	// 初始化脚本/重装脚本
	void init();

	// 用于过滤/修改来自Center的onebot数据
	bool onebot_event_filter(const char * dat);

	// 用于过滤/修改来自Plus的onebot数据
	bool onebot_api_filter(const std::string & filename, const char* dat);

	static ScriptRun* get_instance();
private:
	void init_t();
	std::shared_mutex mx_lua_sta;
	void* lua_sta = 0;
	ScriptRun() {};
};

