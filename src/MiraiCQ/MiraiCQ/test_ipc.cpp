#include <thread>
#include <list>
#include <mutex>
#include <map>

#include "./log/MiraiLog.h"
#include "./tool/TimeTool.h"
#include "./tool/StrTool.h"
#include "./net/MiraiNet.h"
#include "./config/config.h"
#include "./center/center.h"
#include <iostream>

#include "mainprocess/mainprocess.h"
#include "plusprocess/plusprocess.h"
#include "ipcprocess/ipcprocess.h"
#include <assert.h>
#include <websocketpp/base64/base64.hpp>


bool g_is_plus = false;
bool g_is_alone = false;
std::string g_plus_name;
std::string g_main_flag;



int main(int argc,char * argv[])
{
	//≈–∂œ «∑Òcenter
	//0:
	//1:main_uuid
	//2:plus_uuid
	//3:plus name base64
	if (argc < 4) {
		g_is_plus = false;
		g_is_alone = false;
		mainprocess();
	}
	else if (argc == 4) {
		g_is_plus = true;
		g_is_alone = false;
		g_main_flag = argv[1];
		std::string plus_flag = argv[2];
		g_plus_name = websocketpp::base64_decode(argv[3]);
		std::string plus_file = g_plus_name;
		plusprocess(g_main_flag, plus_flag, plus_file);
	}
	else {
		g_is_plus = true;
		g_is_alone = true;
		g_main_flag = argv[2];
		std::string plus_flag = argv[3];
		g_plus_name = websocketpp::base64_decode(argv[4]);
		std::string plus_file = g_plus_name;
		ipcprocess(g_main_flag, plus_flag, plus_file);
	}
	while (true) {
		TimeTool::sleep(1000);
	}
}


