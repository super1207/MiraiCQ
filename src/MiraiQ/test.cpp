#pragma warning(disable:4819)

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp> 

#include "MIraiQ.h"
#include "plus.h"

//得到执行的exe的名字和路径(windows)
static void get_program_dir(std::string &path_name, std::string &exe_name)
{
	char exe_path[MAX_PATH];
	if (GetModuleFileNameA(NULL, exe_path, MAX_PATH) == 0)
	{
		return ;
		//throw logic_error("GetModuleFileNameA错误");
	}
	std::string exe_path_string = exe_path;
	size_t pos = exe_path_string.find_last_of('\\', exe_path_string.length());
	path_name = exe_path_string.substr(0, pos);
	exe_name = exe_path_string.substr(pos + 1);
}

void keyfun()
{
	BOOST_LOG_TRIVIAL(info) << "欢迎使用MiraiQ";
	while(true)
	{
		try
		{
			
			char a[30];
			std::cin.getline(a,30);
			std::string cmd = a;
			if(cmd == "show")
			{
				std::map<__int32,Plus::PlusDef> mp = MiraiQ::get_plus_ptr()->get_plus_map();
				std::map<__int32,Plus::PlusDef>::iterator iter;
				for(iter = mp.begin();iter!=mp.end();++iter)
				{
					BOOST_LOG_TRIVIAL(info) << "AC:" << iter->first << "\n"
						<< iter->second.json_value;

				}
			}else
			{
				std::vector<std::string> itemlist;
				boost::split(itemlist, cmd, boost::is_any_of(" "));
				int ac = boost::lexical_cast<__int32>(itemlist.at(0));
				int num = boost::lexical_cast<__int32>(itemlist.at(1));
					void *  ptr = (
						MiraiQ::get_plus_ptr()->get_plusdef(ac).second.menu.at(num).function_ptr
					);
					((void(*)())ptr)();
			}
		}catch(const std::exception & e)
		{
			BOOST_LOG_TRIVIAL(info) << e.what();
		}
		
	}
}

int runBot()
{

		//boost::function0< void> f =  boost::bind(&keyfun);
		//new boost::thread(f);

		//boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::info);
		MiraiQ * mq = MiraiQ::getInstance();
		assert(mq);
		//BOOST_LOG_TRIVIAL(debug) << "set_bot_url";
		mq->set_bot_url("ws://localhost:6700");
		//BOOST_LOG_TRIVIAL(debug) << "bot_connect";
		mq->bot_connect();
	
		static int is_first;
		if(!is_first)
		{
			//BOOST_LOG_TRIVIAL(debug) << "get_program_dir";
			//加载所有插件
			std::string path_name, exe_name;
			get_program_dir(path_name, exe_name);
			std::vector<boost::filesystem::path>  dll_path_vec = MiraiQ::get_plus_ptr()->find_plus_file(boost::filesystem::path(path_name)/"app");
			for(size_t i = 0;i < dll_path_vec.size();++i)
			{
				BOOST_LOG_TRIVIAL(debug) << dll_path_vec[i];
				mq->load_plus(dll_path_vec[i]);
			}
	
			//BOOST_LOG_TRIVIAL(debug) << "call_cq_start_fun";
			//调用：启动
			mq->call_cq_start_fun();
	
			//BOOST_LOG_TRIVIAL(debug) << "get_plus_id_list";
			std::vector<__int32> ac_list = mq->get_plus_id_list();

			//调用：启用
			for (size_t i = 0; i < ac_list.size();++i)
			{
				mq->enable_plus(ac_list[i]);
			}
			is_first = 1;
		}
		while(true)
		{
			__int32 ret = mq->deal_a_message();
			if(ret == -1)
			{
				BOOST_LOG_TRIVIAL(info) << "已经断线，五秒后重新连接";
				boost::this_thread::sleep_for(boost::chrono::seconds(5));
				BOOST_LOG_TRIVIAL(info) << "正在重新连接......";
				if(mq->bot_connect() == MIRAI_OK)
				{
					BOOST_LOG_TRIVIAL(info) << "重新连接成功" ;
				}else
				{
					BOOST_LOG_TRIVIAL(info) << "重新连接失败" ;
				}
				
			}else if(ret == 0)  //no message be dealed
			{
				Sleep(1);
			}else if(ret == 1)  //dealed a message
			{
				continue;
			}
			
		}
	
	return 0;
}

int main()
{
	if(!LoadLibraryA("CQP.dll"))
	{
		MessageBoxA(NULL,"CQP.dll not found!","error",MB_OK);
		return -1;
	}
	try
	{
		runBot();
	}catch(const std::exception & e)
	{
		BOOST_LOG_TRIVIAL(info) << "crash:" <<e.what();
	}catch(...)
	{
		BOOST_LOG_TRIVIAL(info) << "crash:unkown:";
	}
	
}




