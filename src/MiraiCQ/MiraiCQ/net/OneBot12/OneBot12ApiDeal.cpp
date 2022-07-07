#include "OneBot12ApiDeal.h"
#include "../../tool/StrTool.h"
#include "../../log/MiraiLog.h"

OneBot12ApiDeal::OneBot12ApiDeal()
{
}

OneBot12ApiDeal::~OneBot12ApiDeal()
{
}

MiraiNet::NetStruct OneBot12ApiDeal::deal_api(const Json::Value& root, Json::Value& ret)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	return ret_json;
}

MiraiNet::NetStruct OneBot12ApiDeal::deal_recv(const Json::Value & root)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	return ret_json;
}
