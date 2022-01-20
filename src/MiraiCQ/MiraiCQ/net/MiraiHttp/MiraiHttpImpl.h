#pragma once 

#include "../MiraiNet.h"


#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <thread>
#include <atomic>
#include <map>

class MiraiHttpNetImpl :public MiraiNet
{
public:
	MiraiHttpNetImpl();
	~MiraiHttpNetImpl()override;
	virtual bool connect() override;
	virtual bool is_connect() override;
	virtual NetStruct call_fun(NetStruct senddat, int timeout,bool in_new_net = false) override;
private:
	struct CallStuct
	{
		NetStruct recv_json;
		bool isdeal = false;
	};
	virtual bool connect_();
	using MessagePtr = websocketpp::config::asio_client::message_type::ptr;
	using Client = websocketpp::client<websocketpp::config::asio_client>;
	Client client;
	std::thread thrd;
	websocketpp::connection_hdl  hdl;
	std::atomic_bool is_run = false;

	std::map<std::string,CallStuct> call_map;
	std::mutex mx_call_map;

	std::mutex mx_msg_id_vec;
	std::list<std::pair<int, int>> msg_id_vec;
	int curr_msg_id = 1;


};
