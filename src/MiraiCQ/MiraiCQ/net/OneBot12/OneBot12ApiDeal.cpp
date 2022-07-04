#include "OneBot12ApiDeal.h"
#include "../../tool/StrTool.h"
#include "../../log/MiraiLog.h"

OneBot12ApiDeal::OneBot12ApiDeal()
{
}

OneBot12ApiDeal::~OneBot12ApiDeal()
{
}

MiraiNet::NetStruct OneBot12ApiDeal::deal_api(const Json::Value& root)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	std::string action = root["action"].asString();
	if (action == "send_group_msg") {
		(*ret_json)["action"] = "send_message";
		(*ret_json)["params"]["detail_type"] = "group";
		(*ret_json)["params"]["group_id"] = std::to_string(root["params"]["group_id"].asInt64());
	}
	else {
		(*ret_json)["action"] = "";
	}
	return ret_json;
}

MiraiNet::NetStruct OneBot12ApiDeal::deal_recv(const Json::Value & root)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	return ret_json;
}
