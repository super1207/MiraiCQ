#pragma once

#include "../MiraiNet.h"

#include <list>
#include <mutex>

class OneBotEventDeal
{
public:
	OneBotEventDeal();
	~OneBotEventDeal();
	static MiraiNet::NetStruct deal_event(const Json::Value& root);

private:

};

