#include "MiraiHttpImpl.h"

#include "../../log/MiraiLog.h"
#include "MiraiHttpEventDeal.h"
#include "MiraiHttpApiDeal.h"
#include "../../tool/StrTool.h"
#include "../../tool/TimeTool.h"
#include "../../config/config.h"
#include <httplib/httplib.h>

using namespace std;

MiraiHttpNetImpl::MiraiHttpNetImpl()
{
	MiraiLog::get_instance()->add_debug_log("MiraiHttp", "MiraiHttpNetImpl");
}

MiraiHttpNetImpl::~MiraiHttpNetImpl()
{
	MiraiLog::get_instance()->add_debug_log("MiraiHttp", "~MiraiHttpNetImpl");
	try
	{
		client.get_con_from_hdl(hdl)->close(websocketpp::close::status::value(), "byebye");
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_debug_log("~MiraiHttpNetImpl", e.what());
	}
	try
	{
		thrd.join();
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_debug_log("~MiraiHttpNetImpl", e.what());
	}
	
}

bool MiraiHttpNetImpl::connect()
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
				MiraiLog::get_instance()->add_debug_log("MiraiHttpClose", e.what());
			}
			return false;
		}
	}
	return true;
}

bool MiraiHttpNetImpl::connect_()
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
			Json::Value root;
			Json::Reader reader;
			if (!reader.parse(event_str, root))
			{
				/* Json解析失败 */
				MiraiLog::get_instance()->add_debug_log("收到不规范的Json", event_str);
				return;
			}
			MiraiLog::get_instance()->add_debug_log("OnMessage", StrTool::to_ansi(root.toStyledString()));
			MiraiNet::NetStruct event = MiraiHttpEventDeal::deal_event(root,mx_msg_id_vec,msg_id_vec, curr_msg_id);
			if (!event)
			{
				string type;
				try
				{
					 type = root["data"]["type"].asString();
				}
				catch (...)
				{

				}
				MiraiLog::get_instance()->add_debug_log("事件解析失败", "未知事件:" + type);
				return;
			}
			add_event(event);
		}
		catch (const std::exception & e)
		{
			MiraiLog::get_instance()->add_debug_log("MiraiHttp", string("事件解析失败") + e.what());
		}
	});
	client.set_open_handler([&](websocketpp::connection_hdl hdl) ->void
	{
		MiraiLog::get_instance()->add_info_log("MiraiHttp", "MiraiHttp连接成功");
		is_run = true;
	});

	client.set_fail_handler([&](websocketpp::connection_hdl hdl) ->void
	{
		MiraiLog::get_instance()->add_debug_log("MiraiHttp", "OnFail");
		is_run = false;
	});
	client.set_close_handler([&](websocketpp::connection_hdl hdl) ->void
	{
		MiraiLog::get_instance()->add_debug_log("MiraiHttp", "Onclose");
		is_run = false;
	});

	websocketpp::lib::error_code ec;
	std::string ws_url = get_config("ws_url") + "/all";
	std::string verifyKey = get_config("verifyKey");
	if (verifyKey != "")
	{
		ws_url += "?verifyKey=" + verifyKey;
	}
	Client::connection_ptr con = client.get_connection(ws_url, ec);
	if (ec)
	{
		//errmsg = ec.message();
		return false;
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
			MiraiLog::get_instance()->add_debug_log("MiraiHttpRun", e.what());
		}
		is_run = false;
		MiraiLog::get_instance()->add_info_log("MiraiHttp", "MiraiHttp连接断开");
		
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

bool MiraiHttpNetImpl::is_connect() 
{
	return is_run;
}

MiraiNet::NetStruct MiraiHttpNetImpl::call_fun(NetStruct senddat, int timeout,bool in_new_net) 
{
	using namespace httplib;
	if (!senddat)
	{
		//err_msg = "param error";
		return MiraiNet::NetStruct();
	}
	MiraiNet::NetStruct api_json;
	try
	{
		/* 此处将cq的api格式转化为mha的格式 */
		api_json = MiraiHttpApiDeal::deal_api(*senddat, mx_msg_id_vec, msg_id_vec, curr_msg_id);
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
	httplib::Client cli(get_config("http_url"));
	auto res = cli.Post(("/" + (*api_json)["action"].asString()).c_str(), Json::FastWriter().write(*api_json), "application/json;charset=UTF-8");
	if (res && res->status == 200)
	{
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(res->body, root))
		{
			/* Json解析失败 */
			MiraiLog::get_instance()->add_debug_log("收到不规范的api返回", res->body);
			return nullptr;
		}
		auto ret_json = MiraiHttpApiDeal::deal_recv(root, mx_msg_id_vec, msg_id_vec, curr_msg_id);
		return ret_json;
		
	}
	return nullptr;
}

