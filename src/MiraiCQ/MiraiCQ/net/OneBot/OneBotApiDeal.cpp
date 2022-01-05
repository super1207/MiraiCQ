#include "OneBotApiDeal.h"
#include "../../tool/StrTool.h"
#include "../../log/MiraiLog.h"
#include "../../tool/EmojiTool.h"

OneBotApiDeal::OneBotApiDeal()
{
}

OneBotApiDeal::~OneBotApiDeal()
{
}

MiraiNet::NetStruct OneBotApiDeal::deal_api(const Json::Value& root)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	return ret_json;
}

MiraiNet::NetStruct OneBotApiDeal::deal_recv(const Json::Value & root)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	return ret_json;
}
