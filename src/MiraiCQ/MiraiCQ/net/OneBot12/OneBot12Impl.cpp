#include "OneBot12Impl.h"

#include "../../log/MiraiLog.h"
#include "OneBot12EventDeal.h"
#include "OneBot12ApiDeal.h"
#include "../../tool/StrTool.h"
#include "../../tool/TimeTool.h"
#include "../../tool/AutoDoSth.h"

using namespace std;

OneBot12NetImpl::OneBot12NetImpl()
{
	MiraiLog::get_instance()->add_debug_log("Onebot12", "OneBot12NetImpl");
}

OneBot12NetImpl::~OneBot12NetImpl()
{
	MiraiLog::get_instance()->add_debug_log("Onebot12", "~OneBot12NetImpl");
	try
	{
		client.get_con_from_hdl(hdl)->close(websocketpp::close::status::value(), "byebye");
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_debug_log("~OneBot12NetImpl", e.what());
	}
	try
	{
		thrd.join();
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_debug_log("~OneBot12NetImpl", e.what());
	}
	
}

bool OneBot12NetImpl::connect()
{
	if (is_run)
	{
		return true;
	}
	bool is_connect = false;
	try
	{
		is_connect = connect_();
	}
	catch (std::exception&)
	{
		return false;
	}
	if (!is_connect)
	{
		return false;
	}
	auto start_time = TimeTool::get_tick_count();
	while (!is_run)
	{
		TimeTool::sleep(0);
		if (TimeTool::get_tick_count() - start_time > 5000)
		{
			try
			{
				client.get_con_from_hdl(hdl)->close(websocketpp::close::status::value(), "byebye");
			}
			catch (const std::exception& e)
			{
				MiraiLog::get_instance()->add_debug_log("OneBot12Close", e.what());
			}
			return false;
		}
	}
	return true;
}

bool OneBot12NetImpl::connect_()
{
	//errmsg.clear();
	client.set_access_channels(websocketpp::log::alevel::none);
	client.clear_access_channels(websocketpp::log::alevel::frame_payload);
	client.init_asio();
	client.set_message_handler([&](websocketpp::connection_hdl hdl, MessagePtr msg) ->void
	{
		try
		{
			std::string event_str = msg->get_payload();
			/* MiraiLog::get_instance()->add_debug_log("OnMessage", event_str); */
			Json::Value root;
			Json::Reader reader;
			if (!reader.parse(event_str, root))
			{
				/* Json解析失败 */
				MiraiLog::get_instance()->add_debug_log("收到不规范的Json", event_str);
				return;
			}
			MiraiLog::get_instance()->add_debug_log("收到Json", event_str);
			Json::Value post_type_json = root.get("type", "");
			if (post_type_json.asString() != "") /* 收到事件 */
			{
				MiraiNet::NetStruct event = OneBot12EventDeal::deal_event(root);
				MiraiLog::get_instance()->add_debug_log(" 处理后的Json", (*event).toStyledString());
				if (!event)
				{
					MiraiLog::get_instance()->add_debug_log("事件解析失败", event_str);
					return;
				}
				add_event(event);
			}
			else /* 收到API返回 */
			{
				MiraiNet::NetStruct recv = OneBot12ApiDeal::deal_recv(root);
				if (!recv)
				{
					MiraiLog::get_instance()->add_debug_log("API返回解析失败", event_str);
					return;
				}
				Json::Value echo_json = recv->get("echo","");
				if (!echo_json.isString() || echo_json.asString() == "")
				{
					MiraiLog::get_instance()->add_debug_log("API返回不含echo", event_str);
					return;
				}
				std::string echo = echo_json.asString();
				try {
					auto bqueue = call_map.get(echo);
					bqueue->push(recv);
				}
				catch (const std::exception & e) {
					MiraiLog::get_instance()->add_debug_log("处理API返回失败", e.what());
				}	
			}
		}
		catch (const std::exception& e)
		{
			MiraiLog::get_instance()->add_debug_log("OneBot12", string("OnMessage:") + e.what());
		}
	});
	client.set_open_handler([&](websocketpp::connection_hdl hdl) ->void
	{
		MiraiLog::get_instance()->add_info_log("OneBot12", "OneBot12连接成功");
		is_run = true;
	});

	client.set_fail_handler([&](websocketpp::connection_hdl hdl) ->void
	{
		MiraiLog::get_instance()->add_debug_log("OneBot12", "OnFail");
		is_run = false;
	});
	client.set_close_handler([&](websocketpp::connection_hdl hdl) ->void
	{
		MiraiLog::get_instance()->add_debug_log("OneBot12", "Onclose");
		is_run = false;
	});

	websocketpp::lib::error_code ec;
	std::string ws_url = get_config("ws_url");
	Client::connection_ptr con = client.get_connection(ws_url, ec);
	if (ec)
	{
		//errmsg = ec.message();
		return false;
	}
	std::string access_token = get_config("access_token");
	if (access_token != "")
	{
		con->append_header("Authorization", "Bearer " + access_token);
	}
	hdl = con->get_handle();
	client.connect(con);
	thrd = thread([this]()
	{
		try
		{
			this->client.run();
		}
		catch (const std::exception& e)
		{
			MiraiLog::get_instance()->add_debug_log("OneBot12Run", e.what());
		}
		is_run = false;
		MiraiLog::get_instance()->add_info_log("OneBot12", "OneBot12连接断开");
		
	});

	/* 此处进行一些等待，确保连接成功 */
	for (int i = 0; i < 100; ++i)
	{
		TimeTool::sleep(50);
		if (is_run)
		{
			return true;
		}
	}
	return false;
}

bool OneBot12NetImpl::is_connect() 
{
	return is_run;
}

MiraiNet::NetStruct OneBot12NetImpl::call_fun(NetStruct senddat, int timeout,bool in_new_net)
{
	if (!senddat)
	{
		//err_msg = "param error";
		return MiraiNet::NetStruct();
	}
	MiraiNet::NetStruct api_json;
	try
	{
		api_json = OneBot12ApiDeal::deal_api(*senddat);
		MiraiLog::get_instance()->add_debug_log("发送的Json", "");

	}
	catch (std::exception& e)
	{
		MiraiLog::get_instance()->add_debug_log("jsonapi处理失败", e.what());
	}
	if (!api_json)
	{
		//err_msg = "param error";
		return MiraiNet::NetStruct();
	}
	CallStuct call_struct;
	std::string echo = StrTool::gen_uuid();
	if (echo == "")
	{
		//err_msg = "unknow error";
		return MiraiNet::NetStruct();
	}
	(*api_json)["echo"] = echo;
	auto start_time = TimeTool::get_tick_count();

	std::shared_ptr<BlockQueue<NetStruct>> bqueue = std::make_shared<BlockQueue<NetStruct>>();
	call_map.put(echo, bqueue);
	AutoDoSth doSth([this, echo]() {
		this->call_map.remove(echo);
	});

	try
	{
		if (!is_run)
		{
			return MiraiNet::NetStruct();
		}
		auto send_json = Json::FastWriter().write(*api_json);
		// MiraiLog::get_instance()->add_debug_log("OnebotImpl", "send_json_size:" + std::to_string(send_json.size()));
		// 超过50kb的信息建立新的连接来发送
		if ((send_json.size() > 1024 * 50) && (in_new_net == false))
		{
			auto new_net = MiraiNet::get_instance(this->get_config("net_type"));
			if (!new_net)
			{
				throw std::runtime_error("can't create new net");
			}
			new_net->set_all_config(this->get_all_config());
			if (!new_net->connect())
			{
				throw std::runtime_error("can't connect new net");
			}
			auto ret_json = new_net->call_fun(senddat, timeout, true);
			return ret_json;
		}
		else
		{
			lock_guard<mutex> lock(mx_send);
			client.send(hdl, send_json, websocketpp::frame::opcode::text);
		}	
	}
	catch (const std::exception& e)
	{

		MiraiLog::get_instance()->add_warning_log("发送消息失败", e.what());
		//err_msg = "net error";
		return MiraiNet::NetStruct();
	}

	/* 说明用户并不想获得api调用结果，所以，直接返回，不等待结果 */
	if (timeout <= 0)
	{
		//err_msg = "timeout error";
		return MiraiNet::NetStruct();
	}
	try {
		return bqueue->pop(timeout);
	}
	catch(...) {
		return MiraiNet::NetStruct();
	}
}

