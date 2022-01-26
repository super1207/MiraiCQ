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

#include "../IPC/ipc.h"
#include "mainprocess/mainprocess.h"
#include "plusprocess/plusprocess.h"
#include <assert.h>
#include <websocketpp/base64/base64.hpp>


bool g_is_plus = false;
std::string g_plus_name;



int main(int argc,char * argv[])
{
	//≈–∂œ «∑Òcenter
	//0:
	//1:main_uuid
	//2:plus_uuid
	//3:plus name base64
	bool is_mainprocess = (argc != 4); 
	if (is_mainprocess)
	{
		g_is_plus = false;
		mainprocess();
	}
	else
	{
		g_is_plus = true;
		std::string main_flag = argv[1];
		std::string plus_flag = argv[2];
		g_plus_name = websocketpp::base64_decode(argv[3]);
		std::string plus_file = g_plus_name;
		plusprocess(main_flag,plus_flag, plus_file);
	}
	while (true) {
		TimeTool::sleep(1000);
	}
}


