#pragma once

#include "../MiraiNet.h"

#include <list>
#include <mutex>

class OneBot12ApiDeal
{
public:
	OneBot12ApiDeal();
	~OneBot12ApiDeal();
	static MiraiNet::NetStruct deal_api(const Json::Value& root, Json::Value& ret);
	static MiraiNet::NetStruct deal_recv(const Json::Value& root);

private:

};

