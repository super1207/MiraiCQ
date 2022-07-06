#pragma once 

#include "../MiraiNet.h"
#include "../../tool/ThreadSafeMap.h"
#include "../../tool/BlockQueue.h"


#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <thread>
#include <atomic>
#include <map>

class OneBot12NetImpl :public MiraiNet
{
public:
	OneBot12NetImpl();
	~OneBot12NetImpl()override;
	virtual bool connect() override;
	virtual bool is_connect() override;
	virtual NetStruct call_fun(NetStruct senddat, int timeout,bool in_new_net = false) override;
private:
	struct CallStuct
	{
		NetStruct recv_json;
		bool isdeal = false;
	};
	NetStruct call_fun_native(NetStruct senddat, int timeout, bool in_new_net = false);
	void deal_send_message(Json::Value& msg_json);
	bool connect_();
	using MessagePtr = websocketpp::config::asio_client::message_type::ptr;
	using Client = websocketpp::client<websocketpp::config::asio_client>;
	Client client;
	std::thread thrd;
	websocketpp::connection_hdl  hdl;
	std::atomic_bool is_run = false;

	ThreadSafeMap<std::shared_ptr<BlockQueue<NetStruct>>> call_map;

	std::mutex mx_send;

};
