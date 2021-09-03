#pragma once

#include <string>

class Config
{
public:
	/* ªÒ»°≈‰÷√ µ¿˝ */
	static Config * get_instance() ;
	std::string get_ws_url() ;
	std::string get_http_url();
	std::string get_verifyKey();
	std::string get_adapter();
	std::string get_access_token() ;
private:
	Config();
	~Config();
	std::string ws_url = "ws://localhost:6700";
	std::string access_token = "";
	std::string verifyKey = "";
	std::string http_url = "http://localhost:8080";
	std::string adapter = "onebotv11";
};

