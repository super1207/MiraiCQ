#include "OneBotImpl.h"

#include "../../log/MiraiLog.h"
#include "OneBotEventDeal.h"
#include "OneBotApiDeal.h"
#include "../../tool/StrTool.h"
#include "../../tool/TimeTool.h"

using namespace std;

OneBotNetImpl::OneBotNetImpl()
{
	MiraiLog::get_instance()->add_debug_log("Onebot", "OneBotNetImpl");
}

OneBotNetImpl::~OneBotNetImpl()
{
	MiraiLog::get_instance()->add_debug_log("Onebot", "~OneBotNetImpl");
	try
	{
		client.get_con_from_hdl(hdl)->close(websocketpp::close::status::value(), "byebye");
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_debug_log("~OneBotNetImpl", e.what());
	}
	try
	{
		thrd.join();
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_debug_log("~OneBotNetImpl", e.what());
	}
	
}

bool OneBotNetImpl::connect()
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
				MiraiLog::get_instance()->add_debug_log("OneBotClose", e.what());
			}
			return false;
		}
	}
	return true;
}

bool OneBotNetImpl::connect_()
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
			Json::Value post_type_json = root.get("post_type", "");
			if (post_type_json.asString() != "") /* 收到事件 */
			{
				
				MiraiNet::NetStruct event = OneBotEventDeal::deal_event(root,mx_msg_id_vec,msg_id_vec, curr_msg_id);
				if (!event)
				{
					MiraiLog::get_instance()->add_debug_log("事件解析失败", event_str);
					return;
				}
				add_event(event);
			}
			else /* 收到API返回 */
			{
				MiraiNet::NetStruct recv = OneBotApiDeal::deal_recv(root, mx_msg_id_vec, msg_id_vec, curr_msg_id);
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
				lock_guard<mutex> lock(mx_call_map);
				auto iter = call_map.find(echo);
				if (iter != call_map.end())
				{
					iter->second.recv_json = recv;
					iter->second.isdeal = true;
				}
				
			}
		}
		catch (const std::exception&)
		{
			MiraiLog::get_instance()->add_debug_log("OneBot", "OnMessage");
		}
	});
	client.set_open_handler([&](websocketpp::connection_hdl hdl) ->void
	{
		MiraiLog::get_instance()->add_info_log("OneBot", "OneBot连接成功");
		is_run = true;
	});

	client.set_fail_handler([&](websocketpp::connection_hdl hdl) ->void
	{
		MiraiLog::get_instance()->add_debug_log("OneBot", "OnFail");
		is_run = false;
	});
	client.set_close_handler([&](websocketpp::connection_hdl hdl) ->void
	{
		MiraiLog::get_instance()->add_debug_log("OneBot", "Onclose");
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
			MiraiLog::get_instance()->add_debug_log("OneBotRun", e.what());
		}
		is_run = false;
		MiraiLog::get_instance()->add_info_log("OneBot", "OneBot连接断开");
		
	});

	/* 此处进行一些等待，确保连接成功 */
	for (int i = 0; i < 10; ++i)
	{
		TimeTool::sleep(500);
		if (is_run)
		{
			return true;
		}
	}
	return false;
}

bool OneBotNetImpl::is_connect() 
{
	return is_run;
}

MiraiNet::NetStruct OneBotNetImpl::call_fun(NetStruct senddat, int timeout) 
{
	if (!senddat)
	{
		//err_msg = "param error";
		return MiraiNet::NetStruct();
	}
	MiraiNet::NetStruct api_json;
	try
	{
		api_json = OneBotApiDeal::deal_api(*senddat, mx_msg_id_vec, msg_id_vec, curr_msg_id);
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

	{
		lock_guard<mutex> lock(mx_call_map);
		call_map[echo] = call_struct;
		/* MiraiLog::get_instance()->add_debug_log("map size", std::to_string(call_map.size())); */
	}

	try
	{
		if (!is_run)
		{
			//err_msg = "net error";
			lock_guard<mutex> lock(mx_call_map);
			call_map.erase(echo);
			return MiraiNet::NetStruct();
		}
		auto send_json = Json::FastWriter().write(*api_json);
		client.send(hdl, send_json, websocketpp::frame::opcode::text);
	}
	catch (const std::exception& e)
	{

		MiraiLog::get_instance()->add_debug_log("发送消息失败", e.what());
		//err_msg = "net error";
		lock_guard<mutex> lock(mx_call_map);
		call_map.erase(echo);
		return MiraiNet::NetStruct();
	}

	
	if (timeout <= 0)
	{
		lock_guard<mutex> lock(mx_call_map);
		call_map.erase(echo);
		//err_msg = "timeout error";
		return MiraiNet::NetStruct();
	}
	MiraiNet::NetStruct ret_json;
	while (true)
	{
		TimeTool::sleep(0);
		lock_guard<mutex> lock(mx_call_map);
		auto& it = call_map.at(echo);
		if (it.isdeal)
		{
			ret_json = it.recv_json;
			call_map.erase(echo);
			break;
		}
		if (TimeTool::get_tick_count() - start_time > timeout)
		{
			call_map.erase(echo);
			break;
		}
	}
	if (!ret_json)
	{
		//err_msg = "timeout error";
		return MiraiNet::NetStruct();
	}
	return ret_json;
}

