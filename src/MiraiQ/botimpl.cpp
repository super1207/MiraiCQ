#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/locale/encoding.hpp>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/log/trivial.hpp>

#include <iostream>
#include <vector>

#include <jsoncpp/json.h>

#include <boost/circular_buffer.hpp>

#include "bot.h"


typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;


class BotImpl;
class myclient:public client
{
public:
	BotImpl * bot;

};

class BotImpl:public Bot
{
public:
	BotImpl(const std::string & ws_url = "ws://localhost:6700",size_t buffer_size = 1024)
	{
		this->ws_url = ws_url;
		this->isconnect = false;
		this->thrd = 0;
		this->isfailed = false;
		c.bot = this;
		cb = new boost::circular_buffer<Json::Value>(buffer_size);
		assert(cb);
		ecb = new boost::circular_buffer<Json::Value>(buffer_size);
		assert(ecb);
	}
	bool is_connect()
	{
		boost::recursive_mutex::scoped_lock lock(mx);
		return isconnect;
	}
	bool connect()
	{
		{
			boost::recursive_mutex::scoped_lock lock(mx);
			if(isconnect == true)
			{
				return false;
			}
		}
		
		if(thrd != 0)
		{
			delete thrd;
			thrd = 0;
		}
		try {

			c.set_access_channels(websocketpp::log::alevel::none);
			c.clear_access_channels(websocketpp::log::alevel::frame_payload);
			c.init_asio();
			c.set_message_handler(bind(&on_message,&c,::_1,::_2));
			c.set_open_handler(bind(&on_open,&c,::_1));
			c.set_fail_handler(bind(&on_failed,&c,::_1));

			
			websocketpp::lib::error_code ec;
			client::connection_ptr con = c.get_connection(ws_url, ec);
			hdl = con->get_handle();
			if (ec) 
			{
				BOOST_LOG_TRIVIAL(info) << "could not create connection because: " << ec.message();
				return false;
			}
			c.connect(con);
			
			boost::function0< void> f =  boost::bind(&BotImpl::run,this);
			thrd = new boost::thread(f);

			int timess = 0;
			while(true)
			{
				timess ++;
				if(timess == 5000)
				{
					return false;
				}
				boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
				bool is_con;
				bool is_fai;
				{
					boost::recursive_mutex::scoped_lock lock(mx);
					is_con = isconnect;
					is_fai = isfailed;
				}
				if(isconnect)
				{
					break;
				}
				
			}
			//isconnect = true;
			return true;
		}
		catch (websocketpp::exception const & e)
		{
			BOOST_LOG_TRIVIAL(info) << "could not create connection because: " << e.what();
			return false;
		}
		return true;
	}
	bool disconnect()
	{
		try
		{
			{
				boost::recursive_mutex::scoped_lock lock(mx);
				if(isconnect == false)
				{
					return true;
				}
				try {
					c.close( hdl, websocketpp::close::status::normal, "" );
				}
				catch (websocketpp::exception const & e)
				{
					//已经关闭，无需log
					//std::cout << e.what() << std::endl;
				}

				//c.stop_perpetual();
				//c.stop();
				
			}	
			mx.lock();
			if(thrd != 0)
			{
				mx.unlock();
				thrd->join();  //必须确保此处属于unlock状态
				mx.lock();
				delete thrd;
				thrd = 0;
			}
			mx.unlock();

			mx.lock();
			isconnect = false;
			isfailed = false;
			 cb->clear();
			 ecb->clear();
			mx.unlock();
			return true;

		}
		catch (websocketpp::exception const & e)
		{
			BOOST_LOG_TRIVIAL(info) <<"disconnect failed:" << e.what();
			return false;
		};
	}
	Json::Value get_message()
	{
		boost::recursive_mutex::scoped_lock lock(mx);
		if(cb->size() == 0)
		{
			return Json::Value();
		}
		Json::Value ret_json = cb->front();
		cb->pop_front();
		return ret_json;
	}
	~BotImpl()
	{
		disconnect();
		delete cb;
		delete ecb;
		
	}
	Json::Value send(Json::Value & outroot,__int32 milliseconds = 1000)
	{
		
		{
			boost::recursive_mutex::scoped_lock lock(mx);
			if(!isconnect)
			{
				return Json::Value();
			}
		}
		//for get reply
		std::string s_uuid = boost::uuids::to_string(boost::uuids::random_generator()());
		outroot["echo"] = s_uuid;

		//send
		websocketpp::lib::error_code ec;

		{
			std::string outstr = Json::FastWriter().write(outroot);
			BOOST_LOG_TRIVIAL(debug) << outstr;
			boost::recursive_mutex::scoped_lock lock(mx);
			c.send(hdl,outstr,  websocketpp::frame::opcode::text , ec);
		}
		
		if (ec) 
		{
			BOOST_LOG_TRIVIAL(info) << "send failed because: " << ec.message() ;
			return Json::Value();
		}
		
		//recieve
		boost::asio::deadline_timer t(c.get_io_service(), boost::posix_time::milliseconds(1));  
		for(int i = 0; ; ++i)
		{
			t.wait();
			{
				boost::recursive_mutex::scoped_lock lock(mx);
				for(size_t i = 0;i < ecb->size();++i)
				{
					if(ecb->operator [](i)["echo"].asString() == s_uuid)
					{
						Json::Value ret_json = ecb->operator [](i);
						ecb->erase(ecb->begin()+i);
						return ret_json;
					}
				}
			}
			if((1 + i+1)*(i+1)/2. > milliseconds)
			{
				return Json::Value();
			}
			t.expires_from_now(boost::posix_time::milliseconds(i + 2));
		}


		return Json::Value();
	}
	//public:
private:
	// This message handler will be invoked once for each incoming message. It
	// prints the message and then sends a copy of the message back to the server.
	static void on_message(myclient* c, websocketpp::connection_hdl hdl, message_ptr msg) 
	{
		if(!msg)
		{
			return ;
		}
		//utf8 -> gbk
		std::string gbkmsg;
		try{
			gbkmsg = boost::locale::conv::between(msg->get_payload(), "GBK", "UTF-8");
		}catch(const std::exception & e){
			BOOST_LOG_TRIVIAL(info) <<"error in on_message's to_gbk:"<< e.what();
			return ;
		}
		BOOST_LOG_TRIVIAL(debug) << "message: " << gbkmsg << std::endl;

		Json::Value root;
		Json::Reader reader;
		if(!reader.parse(msg->get_payload(), root))
		{
			return ;
		}
		{
			boost::recursive_mutex::scoped_lock lock(c->bot->mx);
			try
			{
				if(root["post_type"].asString() != "")  //event
				{
					c->bot->cb->push_back(root);
				}else //api reply
				{
					c->bot->ecb->push_back(root);
				}
			}catch(const std::exception & e){
				BOOST_LOG_TRIVIAL(debug) << e.what();
				return ;
			}
		}
	}
	static void on_open(myclient* c, websocketpp::connection_hdl hdl) 
	{
		boost::recursive_mutex::scoped_lock lock(c->bot->mx);
		c->bot->isconnect = true;
		BOOST_LOG_TRIVIAL(info) << "websocket open!";
	}
	static void on_failed(myclient* c, websocketpp::connection_hdl hdl) 
	{
		boost::recursive_mutex::scoped_lock lock(c->bot->mx);
		c->bot->isfailed = true;
		c->bot->isfailed = false;
		BOOST_LOG_TRIVIAL(info) << "websocket failed!";
	}
	static void on_close(myclient* c, websocketpp::connection_hdl hdl) 
	{
		boost::recursive_mutex::scoped_lock lock(c->bot->mx);
		c->bot->isfailed = true;
		c->bot->isfailed = false;
		BOOST_LOG_TRIVIAL(info) << "websocket close!";
	}
	void run()//in new thread
	{
		try 
		{
			this->c.run();
		}
		catch (websocketpp::exception const & e) 
		{
			BOOST_LOG_TRIVIAL(debug) << e.what();
		}
		boost::recursive_mutex::scoped_lock lock(mx);
		isconnect = false;
		
	}
	websocketpp::connection_hdl  hdl;
	boost::recursive_mutex mx;
	std::string ws_url;
	myclient c;
	bool isconnect;
	bool isfailed;
	boost::thread * thrd;
	boost::circular_buffer<Json::Value> * cb;
	boost::circular_buffer<Json::Value> * ecb;

};


Bot * Bot::getInstance(const std::string & ws_url,size_t buffer_size)
{
	return new BotImpl(ws_url,buffer_size);
}





