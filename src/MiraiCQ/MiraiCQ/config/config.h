#pragma once

#include <string>

class Config
{
public:
	/* ªÒ»°≈‰÷√ µ¿˝ */
	static Config * get_instance() ;
	std::string get_ws_url();
	bool set_ws_url(const std::string& ws_url);
	std::string get_http_url();
	std::string get_verifyKey();
	std::string get_adapter();
	std::string get_access_token() ;
	bool set_access_token(const std::string& access_token);
	std::string get_name();
	std::string get_is_support_multi_ws();
private:
	Config();
	~Config();
	std::string ws_url = "ws://localhost:6700";
	std::string access_token = "";
	std::string verifyKey = "";
	std::string http_url = "http://localhost:8080";
	std::string adapter = "onebotv11";
	std::string name = "";
	std::string is_support_multi_ws = "true";
};

