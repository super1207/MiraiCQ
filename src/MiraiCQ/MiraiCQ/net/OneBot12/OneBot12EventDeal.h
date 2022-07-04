#pragma once

#include "../MiraiNet.h"

#include <list>
#include <mutex>

class OneBot12EventDeal
{
public:
	OneBot12EventDeal();
	~OneBot12EventDeal();
	static MiraiNet::NetStruct deal_event(const Json::Value& root);

private:

};

