#include "OneBot12EventDeal.h"
#include "../../log/MiraiLog.h"
#include "../../tool/StrTool.h"

#include <list>
#include <string>

OneBot12EventDeal::OneBot12EventDeal()
{
}

OneBot12EventDeal::~OneBot12EventDeal()
{
}

static void deal_message_arr(Json::Value& msgarr)
{
	for (Json::Value& obj : msgarr)
	{
		std::string msg_type = obj["type"].asString();
		if (msg_type == "image") {
			obj["data"]["file"] = obj["data"]["file_id"].asString();
		}
	}
}

static void deal_message(const std::string & post_type,Json::Value& root)
{
	root["post_type"] = post_type;
	root["message_type"] = StrTool::get_str_from_json(root, "detail_type", "");
	std::string sub_type = StrTool::get_str_from_json(root, "detail_type", "normal");
	if (sub_type == "") {
		sub_type = "normal";
	}
	root["sub_type"] = sub_type;
	root["raw_message"] = StrTool::get_str_from_json(root, "alt_message", "");
	deal_message_arr(root["message"]);
}

MiraiNet::NetStruct OneBot12EventDeal::deal_event(const Json::Value & root_)
{
	auto root = MiraiNet::NetStruct(new Json::Value(root_));
	std::string post_type = StrTool::get_str_from_json(root_, "type", "");
	if (post_type == "message")
	{
		deal_message(post_type ,*root);
	}
	return root;
}
