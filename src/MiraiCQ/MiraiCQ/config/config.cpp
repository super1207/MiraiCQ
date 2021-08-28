#include "config.h"
#include "../tool/StrTool.h"
#include "../tool/PathTool.h"

#include <Windows.h>
Config::Config()
{
}

Config::~Config()
{
}

Config* Config::get_instance() noexcept
{
	static Config cfg;
	return &cfg;
}

std::string Config::get_ws_url() noexcept
{
	std::string config_path = PathTool::get_exe_dir() + "config\\";
	PathTool::create_dir(config_path);
	std::string config_file = config_path + "config.ini";
	std::string url = StrTool::get_str_from_ini(config_file,"OneBot","ws_url","");
	if (url == "")
	{
		WritePrivateProfileStringA("OneBot", "ws_url", this->ws_url.c_str(), config_file.c_str());
		return this->ws_url;
	}
	return url;
}

std::string Config::get_access_token() noexcept
{
	std::string config_path = PathTool::get_exe_dir() + "config\\";
	PathTool::create_dir(config_path);
	std::string config_file = config_path + "config.ini";
	std::string uuid = StrTool::gen_uuid();
	std::string token = StrTool::get_str_from_ini(config_file, "OneBot", "access_token", uuid);
	if (token == uuid)
	{
		WritePrivateProfileStringA("OneBot", "access_token", this->access_token.c_str(), config_file.c_str());
		return this->access_token;
	}
	return token;
}
