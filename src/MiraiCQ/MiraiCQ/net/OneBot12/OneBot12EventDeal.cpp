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
		}else if (msg_type == "mention") {
			obj["type"] = "at";
			obj["data"]["qq"] = obj["data"]["user_id"].asString();
		}
		else if (msg_type == "mention_all") {
			obj["type"] = "at";
			obj["data"]["qq"] = "all";
		}
	}
}

static void deal_message(Json::Value& root)
{
	root["post_type"] = "message";
	root["message_type"] = StrTool::get_str_from_json(root, "detail_type", "");
	std::string sub_type = StrTool::get_str_from_json(root, "sub_type", "normal");
	if (sub_type == "") {
		sub_type = "normal";
	}
	root["sender"]["user_id"] = root["user_id"];
	root["sender"]["nickname"] = StrTool::get_str_from_json(root, "user_name", "");
	root["sub_type"] = sub_type;
	root["raw_message"] = StrTool::get_str_from_json(root, "alt_message", "");
	deal_message_arr(root["message"]);
}

static void deal_notice(Json::Value& root)
{
	root["post_type"] = "notice";
	std::string notice_type = StrTool::get_str_from_json(root, "detail_type", "");
	if (notice_type == "group_member_increase") {
		root["notice_type"] = "group_increase";
	}
	else if(notice_type == "group_member_decrease") {
		root["notice_type"] = "group_decrease";
	}
	else if (notice_type == "group_message_delete") {
		root["notice_type"] = "group_recall";
	}
	else if (notice_type == "friend_increase") {
		root["notice_type"] = "friend_add";
	}
	else if (notice_type == "private_message_delete") {
		root["notice_type"] = "friend_recall";
	}
	else {
		root["notice_type"] = notice_type;
	}
	std::string sub_type = StrTool::get_str_from_json(root, "sub_type", "");
	if (sub_type == "join") {
		root["sub_type"] = "	approve";
	}
	else if (sub_type == "invite") {
		root["sub_type"] = "invite";
	}
	else if (sub_type == "leave") {
		root["sub_type"] = "leave";
	}
	else if (sub_type == "kick") {
		root["sub_type"] = "kick";
	}
	else {
		root["sub_type"] = sub_type;
	}
}

MiraiNet::NetStruct OneBot12EventDeal::deal_event(const Json::Value & root_)
{
	auto root = MiraiNet::NetStruct(new Json::Value(root_));
	std::string self_id = StrTool::get_str_from_json((*root), "self_id", "");
	if (self_id != "") {
		(*root)["self_id"] = std::stoll(self_id);
	}
	std::string group_id = StrTool::get_str_from_json((*root), "group_id", "");
	if (group_id != "") {
		(*root)["group_id"] = std::stoll(group_id);
	}
	std::string user_id = StrTool::get_str_from_json((*root), "user_id", "");
	if (group_id != "") {
		(*root)["user_id"] = std::stoll(user_id);
	}
	std::string operator_id = StrTool::get_str_from_json((*root), "operator_id", "");
	if (operator_id != "") {
		(*root)["operator_id"] = std::stoll(operator_id);
	}
	double tm = root->get("time", Json::Value()).asDouble();
	(*root)["time"] = (int64_t)(tm + 0.5);
	if (operator_id != "") {
		(*root)["operator_id"] = std::stoll(operator_id);
	}
	std::string post_type = StrTool::get_str_from_json(root_, "type", "");
	if (post_type == "message")
	{
		deal_message(*root);
	}
	else if (post_type == "notice") {
		deal_notice(*root);
	}
	return root;
}
