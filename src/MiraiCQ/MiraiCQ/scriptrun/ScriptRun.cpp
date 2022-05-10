#include "ScriptRun.h"

#include "../log/MiraiLog.h"
#include "../tool/PathTool.h"

#include <lua.hpp>


static void loadcq_func(lua_State* L)
{
	// lua_register(L, "cq_sendmsg", lua_cq_sendmsg);
}

static bool lua_onebot_event_filter(lua_State* L, const char* dat)
{

	lua_getglobal(L, "onebot_event_filter");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		throw std::runtime_error("not found 'function onebot_event_filter(msg)->bool' in filter_script.lua");
	}
	lua_pushstring(L, dat);
	if (lua_pcall(L, 1, 1, 0) != 0)
	{
		std::string err = lua_tostring(L, -1);
		lua_pop(L, 1);
		throw std::runtime_error("call fun err:" + err);
	}

	if (!lua_isboolean(L, -1))
	{
		lua_pop(L, 1);
		throw std::runtime_error("call fun err:ret not boolean");
	}
	
	bool res = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return res;
}

static bool lua_onebot_api_filter(lua_State* L, const std::string& filename, const char* dat)
{

	lua_getglobal(L, "onebot_api_filter");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		throw std::runtime_error("not found 'function onebot_api_filter(msg)->bool' in filter_script.lua");
	}
	lua_pushstring(L, filename.c_str());
	lua_pushstring(L, dat);
	if (lua_pcall(L, 2, 1, 0) != 0)
	{
		std::string err = lua_tostring(L, -1);
		lua_pop(L, 1);
		throw std::runtime_error("call fun err:" + err);
	}

	if (!lua_isboolean(L, -1))
	{
		lua_pop(L, 1);
		throw std::runtime_error("call fun err:ret not boolean");
	}

	bool res = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return res;
}



void ScriptRun::init_t()
{
	if (lua_sta) {
		lua_close((lua_State*)lua_sta);
		lua_sta = 0;
	}
	lua_State* L = luaL_newstate();
	if (L == NULL) {
		throw std::runtime_error("luaL_newstate err");
		exit(-1);
	}
	lua_sta = L;
	std::string lua_file = PathTool::get_exe_dir() + "\\config\\filter_script.lua";
	if (!PathTool::is_file_exist(lua_file)) {
		//脚本文件不存在
		lua_close(L);
		lua_sta = 0;
		return;
	}
	//std::string file_string = PathTool::read_biniary_file(lua_file);
	luaL_openlibs(L);
	loadcq_func(L);
	if (luaL_dofile(L, lua_file.c_str()) != 0) {
		MiraiLog::get_instance()->add_warning_log("ScriptRun", std::string("filter_script.lua的语法有误:") + lua_tostring(L, -1));
		lua_close(L);
		lua_sta = 0;
		return;
	}
}

void ScriptRun::init() {
	std::unique_lock<std::shared_mutex> lk(mx_lua_sta);
	try {
		init_t();
	}
	catch (const std::exception& e) {
		MiraiLog::get_instance()->add_fatal_log("ScriptRun", e.what());
		exit(-1);
	}
	
}

bool ScriptRun::onebot_event_filter(const char* dat)
{
	std::unique_lock<std::shared_mutex> lk(mx_lua_sta);
	// 没有脚本，所以不过滤
	if (lua_sta == 0) {
		return true;
	}
	try {
		return lua_onebot_event_filter((lua_State *)lua_sta,dat);
	}
	catch (const std::exception& e) {
		MiraiLog::get_instance()->add_warning_log("ScriptRun", e.what());
		return true;
	}
}

bool ScriptRun::onebot_api_filter(const std::string& filename, const char* dat)
{
	std::unique_lock<std::shared_mutex> lk(mx_lua_sta);
	// 没有脚本，所以不过滤
	if (lua_sta == 0) {
		return true;
	}
	try {
		return lua_onebot_api_filter((lua_State*)lua_sta, filename, dat);
	}
	catch (const std::exception& e) {
		MiraiLog::get_instance()->add_warning_log("ScriptRun", e.what());
		return true;
	}
}

ScriptRun* ScriptRun::get_instance()
{
	static ScriptRun t;
	return &t;
}


