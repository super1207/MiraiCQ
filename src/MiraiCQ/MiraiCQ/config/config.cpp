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

Config* Config::get_instance() 
{
	static Config cfg;
	return &cfg;
}

std::string Config::get_ws_url() 
{
	std::string config_path = PathTool::get_exe_dir() + "config\\";
	PathTool::create_dir(config_path);
	std::string config_file = config_path + "config.ini";
	std::string url = StrTool::get_str_from_ini(config_file,"Setting","ws_url","");
	if (url == "")
	{
		WritePrivateProfileStringA("Setting", "ws_url", this->ws_url.c_str(), config_file.c_str());
		return this->ws_url;
	}
	return url;
}

bool Config::set_ws_url(const std::string & ws_url)
{
	std::string config_path = PathTool::get_exe_dir() + "config\\";
	PathTool::create_dir(config_path);
	std::string config_file = config_path + "config.ini";
	WritePrivateProfileStringA("Setting", "ws_url", ws_url.c_str(), config_file.c_str());
	return get_ws_url() == ws_url;
}

std::string Config::get_http_url()
{
	std::string config_path = PathTool::get_exe_dir() + "config\\";
	PathTool::create_dir(config_path);
	std::string config_file = config_path + "config.ini";
	std::string url = StrTool::get_str_from_ini(config_file, "Setting", "http_url", "");
	if (url == "")
	{
		WritePrivateProfileStringA("Setting", "http_url", this->http_url.c_str(), config_file.c_str());
		return this->http_url;
	}
	return url;
}

std::string Config::get_verifyKey()
{
	std::string config_path = PathTool::get_exe_dir() + "config\\";
	PathTool::create_dir(config_path);
	std::string config_file = config_path + "config.ini";
	std::string uuid = StrTool::gen_uuid();
	std::string verifyKey = StrTool::get_str_from_ini(config_file, "Setting", "verifyKey", uuid);
	if (verifyKey == uuid)
	{
		WritePrivateProfileStringA("Setting", "verifyKey", this->verifyKey.c_str(), config_file.c_str());
		return this->verifyKey;
	}
	return verifyKey;
}

std::string Config::get_adapter()
{
	std::string config_path = PathTool::get_exe_dir() + "config\\";
	PathTool::create_dir(config_path);
	std::string config_file = config_path + "config.ini";
	std::string adapter = StrTool::get_str_from_ini(config_file, "Adapter", "adapter", "");
	if (adapter == "")
	{
		WritePrivateProfileStringA("Adapter", "adapter", this->adapter.c_str(), config_file.c_str());
		return this->adapter;
	}
	return adapter;
}

std::string Config::get_access_token() 
{
	std::string config_path = PathTool::get_exe_dir() + "config\\";
	PathTool::create_dir(config_path);
	std::string config_file = config_path + "config.ini";
	std::string uuid = StrTool::gen_uuid();
	std::string access_token = StrTool::get_str_from_ini(config_file, "Setting", "access_token", uuid);
	if (access_token == uuid)
	{
		WritePrivateProfileStringA("Setting", "access_token", this->access_token.c_str(), config_file.c_str());
		return this->access_token;
	}
	return access_token;
}

bool Config::set_access_token(const std::string& access_token)
{
	std::string config_path = PathTool::get_exe_dir() + "config\\";
	PathTool::create_dir(config_path);
	std::string config_file = config_path + "config.ini";
	WritePrivateProfileStringA("Setting", "access_token", access_token.c_str(), config_file.c_str());
	return get_access_token() == access_token;
}

std::string Config::get_name()
{
	std::string config_path = PathTool::get_exe_dir() + "config\\";
	PathTool::create_dir(config_path);
	std::string config_file = config_path + "config.ini";
	std::string name = StrTool::get_str_from_ini(config_file, "Setting", "name", "");
	if (name == "")
	{
		WritePrivateProfileStringA("Setting", "name", this->name.c_str(), config_file.c_str());
		return this->name;
	}
	return name;
}
