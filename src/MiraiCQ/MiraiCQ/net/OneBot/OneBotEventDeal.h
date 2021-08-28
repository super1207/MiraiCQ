#pragma once

#include "../MiraiNet.h"

#include <list>
#include <mutex>

class OneBotEventDeal
{
public:
	OneBotEventDeal();
	~OneBotEventDeal();
	static MiraiNet::NetStruct deal_event(const Json::Value& root, std::mutex& mx_msg_id_vec, std::list<std::pair<int, int>>& msg_id_vec,int & curr_msg_id);

private:

};

