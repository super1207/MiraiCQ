#pragma once

#include "../MiraiNet.h"

#include <list>
#include <mutex>

class OneBotApiDeal
{
public:
	OneBotApiDeal();
	~OneBotApiDeal();
	static MiraiNet::NetStruct deal_api(const Json::Value& root);
	static MiraiNet::NetStruct deal_recv(const Json::Value& root);

private:

};

