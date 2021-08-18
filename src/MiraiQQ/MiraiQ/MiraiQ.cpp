#pragma warning(disable:4819)

#include <boost/thread/recursive_mutex.hpp>
#include <boost/typeof/typeof.hpp> 
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/shared_ptr.hpp>

#include "MIraiQ.h"
#include "bot.h"
#include "plus.h"
#include "bot_event.h"

#include <map>



#define EVENT_FUN_TYPE(x) Plus::cq_##x##_funtype
#define EVENT_FUN_ID(x) Plus::cq_##x##_id

#define GET_FUNPTR(FUNTYPE,PTROUT,BOJIN) \
	BOOST_AUTO(PTROUT,((EVENT_FUN_TYPE(FUNTYPE))Plus::get_plus_function(BOJIN,EVENT_FUN_ID(FUNTYPE))));


#define LOCK_AREA boost::recursive_mutex::scoped_lock lock(gs_mutex); 
#define LOCK_DATA boost::recursive_mutex::scoped_lock lock_data(gs_mutex_data); 

static boost::shared_ptr<Bot> gs_bot; 
static boost::shared_ptr<Plus> gs_plus;
static __int32 gs_is_bot_connect;
static boost::recursive_mutex gs_mutex;
static boost::recursive_mutex gs_mutex_data;
static std::string gs_url;
static std::string gs_access_token;


static boost::asio::io_service gs_io_service;
static boost::thread_group gs_threadpool;
static boost::asio::io_service::work gs_work(gs_io_service);

MiraiQ::MiraiQ()
{
	init_event_map();
	gs_is_bot_connect = MIRAIQ_BOT_NOT_CONNECT;
	gs_url = "";
	gs_plus = boost::shared_ptr<Plus>(new Plus());
	gs_bot = boost::shared_ptr<Bot>(Bot::getInstance());
	assert(gs_plus);
	for(size_t i = 0;i < 30;++i)
	{
		gs_threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &gs_io_service));
	}
}

//MIRAIQ_BOT_IS_CONNECT
//MIRAIQ_BOT_NOT_CONNECT
__int32 MiraiQ::is_bot_connect()
{
	LOCK_DATA
	if(gs_bot == NULL)
	{
		gs_is_bot_connect = MIRAIQ_BOT_NOT_CONNECT;
		return gs_is_bot_connect;
	}

	if(gs_bot->is_connect() == false)
	{
		gs_is_bot_connect = MIRAIQ_BOT_NOT_CONNECT;
		return gs_is_bot_connect;
	}
	gs_is_bot_connect = MIRAIQ_BOT_IS_CONNECT;
	return gs_is_bot_connect;
}

//""
//url
std::string MiraiQ::get_bot_url()
{
	LOCK_DATA
	return gs_url;
}

//MIRAI_OK
//MIRAIQ_BOT_NOT_CONNECT
//MIRAIQ_BOT_IS_CONNECT
__int32 MiraiQ::bot_connect(const std::string & url_set,const std::string & access_token_set)
{
	LOCK_AREA
	{
		if(is_bot_connect() == MIRAIQ_BOT_IS_CONNECT)
		{
			return MIRAIQ_BOT_IS_CONNECT;
		}
	}
	std::string url;
	std::string access_token;
	{
		LOCK_DATA
		if(url_set == "")
		{
			url = gs_url;
		}else
		{
			url = url_set;
			gs_url = url;
		}
		if(access_token_set == "")
		{
			access_token = gs_access_token;
		}else
		{
			access_token = access_token_set;
			gs_access_token = access_token;

		}
	}
	boost::shared_ptr<Bot> bot(Bot::getInstance(url,access_token));
	if(!bot)
	{
		return MIRAIQ_BOT_NOT_CONNECT;
	}
	if(!bot->connect())
	{
		return MIRAIQ_BOT_NOT_CONNECT;
	}
	LOCK_DATA
	gs_bot = bot;
	gs_is_bot_connect = MIRAIQ_BOT_IS_CONNECT;
	return MIRAI_OK;
}


__int32 MiraiQ::bot_disconnect()
{
	LOCK_AREA
	boost::shared_ptr<Bot> bot;
	{
		LOCK_DATA
		bot = gs_bot;
	}
	if(bot)
	{
		bot->disconnect();
	}
	LOCK_DATA
	gs_bot = boost::shared_ptr<Bot>(Bot::getInstance(gs_url));
	gs_is_bot_connect = MIRAIQ_BOT_NOT_CONNECT;
	return 0;//TODO...
}

//MIRAIQ_LOAD_PLUS_ERR
//MIRAI_OK
__int32 MiraiQ::load_plus(const boost::filesystem::path & path)
{
	boost::shared_ptr<Plus> plus;
	{
		LOCK_DATA
		plus = gs_plus;
	}
	__int32 ret = plus->add_plus(path);
	if(ret <= 0)
	{
		return MIRAIQ_LOAD_PLUS_ERR;
	}
	return MIRAI_OK;
}	

std::vector<__int32> MiraiQ::get_plus_id_list()
{
	boost::shared_ptr<Plus> plus;
	{
		LOCK_DATA
		plus = gs_plus;
	}
	std::map<__int32,Plus::PlusDef> mp = plus->get_plus_map();
	std::map<__int32,Plus::PlusDef>::iterator iter;
	std::vector<__int32> ret_vec;
	for(iter = mp.begin();iter!=mp.end();++iter)
	{
		ret_vec.push_back(iter->first);
	}
	return ret_vec;
}

//MIRAIQ_PLUS_IS_ENABLE
//MIRAI_PLUS_NOT_EXIST
//MIRAI_OK
//MIRAIQ_PLUS_IS_NOT_ENABLE
__int32 MiraiQ::enable_plus(__int32 plus_id)
{
	boost::shared_ptr<Plus> plus;
	{
		LOCK_DATA
			plus = gs_plus;
	}
	__int32 status = is_plus_enable(plus_id);
	if(status == MIRAIQ_PLUS_IS_ENABLE)
	{
		return MIRAIQ_PLUS_IS_ENABLE;
	}
	if(status == MIRAI_PLUS_NOT_EXIST)
	{
		return MIRAI_PLUS_NOT_EXIST;
	}
	if(!plus->enable_plus(plus_id))
	{
		return MIRAIQ_PLUS_IS_NOT_ENABLE;
	}
	return MIRAI_OK;
}

//MIRAI_OK
//MIRAI_FAILD
__int32 MiraiQ::disable_plus(__int32 plus_id)
{
	boost::shared_ptr<Plus> plus;
	{
		LOCK_DATA
			plus = gs_plus;
	}
	if(plus->disable_plus(plus_id))
	{
		return MIRAI_OK;
	}
	return MIRAI_FAILD;
}


//MIRAIQ_PLUS_IS_ENABLE
//MIRAIQ_PLUS_IS_NOT_ENABLE
//MIRAI_PLUS_NOT_EXIST
__int32 MiraiQ::is_plus_enable(__int32 plus_id)
{
	boost::shared_ptr<Plus> plus;
	{
		LOCK_DATA
			plus = gs_plus;
	}
	std::pair<bool,Plus::PlusDef> def = plus->get_plusdef(plus_id);
	if(def.first == false)
	{
		return MIRAI_PLUS_NOT_EXIST;
	}
	if(def.second.is_enable)
	{
		return MIRAIQ_PLUS_IS_ENABLE;
	}
	return MIRAIQ_PLUS_IS_NOT_ENABLE;
}

void MiraiQ::call_cq_start_fun()
{
	boost::shared_ptr<Plus> plus;
	{
		LOCK_DATA
		plus = gs_plus;
	}

	std::map<__int32,Plus::PlusDef>::iterator iter;
	for (iter=plus->plus_map.begin(); iter!=plus->plus_map.end(); iter++)
	{
		GET_FUNPTR(event_coolq_start,fun_ptr,iter->second)
		if(fun_ptr)
		{
			BOOST_LOG_TRIVIAL(debug) << "call plus's fun:event_coolq_start: " <<fun_ptr;
			fun_ptr();
		}
	}
}

void MiraiQ::call_cq_stop_fun()
{
	boost::shared_ptr<Plus> plus;
	{
		LOCK_DATA
			plus = gs_plus;
	}

	std::map<__int32,Plus::PlusDef>::iterator iter;
	for (iter=plus->plus_map.begin(); iter!=plus->plus_map.end(); iter++)
	{
		GET_FUNPTR(event_coolq_exit,fun_ptr,iter->second)
			if(fun_ptr)
			{
				BOOST_LOG_TRIVIAL(debug) << "call plus's fun:event_coolq_stop: " <<fun_ptr;
				fun_ptr();
				//gs_io_service.post(fun_ptr);
			}
	}
	//boost::this_thread::sleep_for(boost::chrono::seconds(5));

}

__int32 MiraiQ::deal_a_message()
{
	boost::shared_ptr<Bot> bot;
	boost::shared_ptr<Plus> plus;
	{
		LOCK_DATA
		bot = gs_bot;
		plus = gs_plus;
	}
	if(is_bot_connect() != MIRAIQ_BOT_IS_CONNECT)
	{
		return -1;
	}
	Json::Value root = bot->get_message();
	if(root.isNull())
	{
		return 0;
	}
	//gs_io_service.post(boost::bind(g_message_map["all_event"][""], root,plus));
	std::string post_type,type2;
	try
	{
		post_type = root["post_type"].asString();
		type2 = root[post_type+"_type"].asString();
	}catch(const std::exception & e)
	{
		BOOST_LOG_TRIVIAL(info) << e.what();
		return 1;
	}
	if(g_message_map[post_type][type2] != 0)
	{
		gs_io_service.post(boost::bind(g_message_map[post_type][type2], root,plus));
		//g_message_map[post_type][type2](root,gs_plus);
	}
	return 1;
}

boost::shared_ptr<Bot> MiraiQ::get_bot_ptr()
{
	LOCK_DATA
	return gs_bot;
}

boost::shared_ptr<Plus> MiraiQ::get_plus_ptr()
{
	LOCK_DATA
	return gs_plus;
}

MiraiQ * MiraiQ::getInstance()
{
	static MiraiQ sm;
	return &sm;
}


