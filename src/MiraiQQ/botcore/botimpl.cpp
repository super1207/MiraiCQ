#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
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
		boost::mutex::scoped_lock lock(mx);
		return isconnect;
	}
	bool connect()
	{
		{
			boost::mutex::scoped_lock lock(mx);
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
			con = c.get_connection(ws_url, ec);
			if (ec) 
			{
				BOOST_LOG_TRIVIAL(debug) << "could not create connection because: " << ec.message();
				return false;
			}
			c.connect(con);
			
			boost::function0< void> f =  boost::bind(&BotImpl::run,this);
			thrd = new boost::thread(f);

			while(true)
			{
				boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
				bool is_con;
				bool is_fai;
				{
					boost::mutex::scoped_lock lock(mx);
					is_con = isconnect;
					is_fai = isfailed;
				}
				if(is_con)
				{
					break;
				}
				if(is_fai)
				{
					return false;
				}
				
			}
			//isconnect = true;
			return true;
		}
		catch (websocketpp::exception const & e)
		{
			BOOST_LOG_TRIVIAL(debug) << e.what();
			return false;
		}
		return true;
	}
	bool disconnect()
	{
		
		try
		{
			{
				boost::mutex::scoped_lock lock(mx);
				if(!isconnect)
				{
					return true;
				}
				websocketpp::lib::error_code ec;
				c.stop();
			}
			
			//c.get_local_endpoint(ec).socket_con_ptr->shutdown();
			if(thrd != 0)
			{
				thrd->join();
				delete thrd;
				thrd = 0;
			}
			{
				boost::mutex::scoped_lock lock(mx);
				isconnect = false;
				isfailed = false;
				cb->clear();
			}
			

		}
		catch (websocketpp::exception const & e)
		{
			BOOST_LOG_TRIVIAL(debug) << e.what();
			return false;
		};
	}
	Json::Value get_message()
	{
		boost::mutex::scoped_lock lock(mx);
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
	}
	Json::Value send(Json::Value & outroot,__int32 milliseconds = 1000)
	{
		
		//for get reply
		std::string s_uuid = boost::uuids::to_string(boost::uuids::random_generator()());
		outroot["echo"] = s_uuid;

		//send
		websocketpp::lib::error_code ec;

		{
			std::string outstr = Json::FastWriter().write(outroot);
			BOOST_LOG_TRIVIAL(debug) << outstr;
			boost::mutex::scoped_lock lock(mx);
			c.send(con->get_handle(),outstr, websocketpp::frame::opcode::value::text, ec);
		}
		
		if (ec) 
		{
			BOOST_LOG_TRIVIAL(debug) << "Send failed because: " << ec.message() ;
			return Json::Value();
		}
		
		//recieve
		boost::asio::deadline_timer t(c.get_io_service(), boost::posix_time::milliseconds(1));  
		for(int i = 0; ; ++i)
		{
			t.wait();
			{
				boost::mutex::scoped_lock lock(mx);
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
	Json::Value send_group_msg(unsigned __int64 group_id,const std::string & message,bool auto_escape = false,__int32 timeout_mills = 5000)
	{
		Json::Value outroot;
		outroot["action"] = "send_group_msg";
		outroot["params"]["group_id"] = group_id;
		outroot["params"]["message"] = message;
		outroot["params"]["auto_escape"] = auto_escape;
		return send(outroot,timeout_mills);
	}
	Json::Value send_private_msg(unsigned __int64 user_id,const std::string & message,bool auto_escape = false,__int32 timeout_mills = 5000)
	{
		Json::Value outroot;
		outroot["action"] = "send_private_msg";
		outroot["params"]["user_id"] = user_id;
		outroot["params"]["message"] = message;
		outroot["params"]["auto_escape"] = auto_escape;
		return send(outroot,timeout_mills);
	}
	Json::Value delete_msg(__int32 message_id,__int32 timeout_mills = 5000)
	{
		Json::Value outroot;
		outroot["action"] = "delete_msg";
		outroot["params"]["message_id"] = message_id;
		return send(outroot,timeout_mills);
	}
	//public:
private:
	// This message handler will be invoked once for each incoming message. It
	// prints the message and then sends a copy of the message back to the server.
	static void on_message(myclient* c, websocketpp::connection_hdl hdl, message_ptr msg) 
	{
		//utf8 -> gbk
		std::string gbkmsg = boost::locale::conv::between(msg->get_payload(), "GBK", "UTF-8");
		BOOST_LOG_TRIVIAL(debug) << "message: " << gbkmsg << std::endl;

		Json::Value root;
		Json::Reader reader;
		if(!reader.parse(gbkmsg, root))
		{
			return ;
		}
		{
			boost::mutex::scoped_lock lock(c->bot->mx);
			if(root["post_type"].asString() != "")  //event
			{
				c->bot->cb->push_back(root);
			}else //api reply
			{
				c->bot->ecb->push_back(root);
			}
			
		}
	}
	static void on_open(myclient* c, websocketpp::connection_hdl hdl) 
	{
		boost::mutex::scoped_lock lock(c->bot->mx);
		c->bot->isconnect = true;
		BOOST_LOG_TRIVIAL(debug) << "websocket open!";
	}
	static void on_failed(myclient* c, websocketpp::connection_hdl hdl) 
	{
		boost::mutex::scoped_lock lock(c->bot->mx);
		c->bot->isfailed = true;
		BOOST_LOG_TRIVIAL(debug) << "websocket failed!";
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
		//MessageBoxA(NULL,"ok","ok",MB_OK);
		boost::mutex::scoped_lock lock(mx);
		isconnect = false;
	}
	client::connection_ptr  con;
	boost::mutex mx;
	std::string ws_url;
	myclient c;
	bool isconnect;
	bool isfailed;
	boost::thread * thrd;
	boost::circular_buffer<Json::Value> * cb;
	boost::circular_buffer<Json::Value> * ecb;

};


boost::shared_ptr<Bot> Bot::getInstance(const std::string & ws_url,size_t buffer_size)
{
	return boost::make_shared<BotImpl>(ws_url,buffer_size);
}





