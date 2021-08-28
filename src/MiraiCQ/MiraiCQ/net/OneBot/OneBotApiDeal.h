#pragma once

#include "../MiraiNet.h"

#include <list>
#include <mutex>

class OneBotApiDeal
{
public:
	OneBotApiDeal();
	~OneBotApiDeal();
	static MiraiNet::NetStruct deal_api(const Json::Value& root, std::mutex& mx_msg_id_vec, std::list<std::pair<int, int>>& msg_id_vec, int& curr_msg_id);
	static MiraiNet::NetStruct deal_recv(const Json::Value& root, std::mutex& mx_msg_id_vec, std::list<std::pair<int, int>>& msg_id_vec, int& curr_msg_id);

private:

};

