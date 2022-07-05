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
	std::string action = root["action"].asString();
	if (action == "send_group_msg") {
		(*ret_json)["action"] = "send_message";
		(*ret_json)["params"]["detail_type"] = "group";
		(*ret_json)["params"]["group_id"] = std::to_string(root["params"]["group_id"].asInt64());
		if ((*ret_json)["params"]["message"].isString()) {
			(*ret_json)["params"]["message"] = StrTool::cq_str_to_jsonarr((*ret_json)["params"]["message"].asString());
		}
	}else if (action == "send_private_msg") {
		(*ret_json)["action"] = "send_message";
		(*ret_json)["params"]["detail_type"] = "private";
		(*ret_json)["params"]["user_id"] = std::to_string(root["params"]["user_id"].asInt64());
		if ((*ret_json)["params"]["message"].isString()) {
			(*ret_json)["params"]["message"] = StrTool::cq_str_to_jsonarr((*ret_json)["params"]["message"].asString());
		}
	}
	else if (action == "send_msg") {
		(*ret_json)["action"] = "send_message";
		(*ret_json)["params"]["detail_type"] = root["params"]["message_type"].asString();
		int64_t user_id = StrTool::get_int64_from_json(root["params"], "user_id", 0);
		int64_t group_id = StrTool::get_int64_from_json(root["params"], "group_id", 0);
		if (user_id != 0) {
			(*ret_json)["params"]["user_id"] = std::to_string(user_id);
		}
		if (group_id != 0) {
			(*ret_json)["params"]["group_id"] = std::to_string(group_id);
		}
		if ((*ret_json)["params"]["message"].isString()) {
			(*ret_json)["params"]["message"] = StrTool::cq_str_to_jsonarr((*ret_json)["params"]["message"].asString());
		}
	}
	else if (action == "delete_msg") {
		(*ret_json)["action"] = "delete_message";
	}
	else if (action == "get_msg") {
		// onebot12没有对应的api
	}
	else if (action == "get_forward_msg") {
		// onebot12没有对应的api
	}
	else if (action == "send_like") {
		// onebot12没有对应的api
	}
	else if (action == "set_group_kick") {
		// onebot12没有对应的api
	}
	else if (action == "set_group_ban") {
		// onebot12没有对应的api
	}
	else if (action == "set_group_anonymous_ban") {
		// onebot12没有对应的api
	}
	else if (action == "set_group_whole_ban") {
		// onebot12没有对应的api
	}
	else if (action == "set_group_admin") {
		// onebot12没有对应的api
	}
	else if (action == "set_group_anonymous") {
		// onebot12没有对应的api
	}
	else if (action == "set_group_card") {
		// onebot12没有对应的api
	}
	else if (action == "set_group_name") {
		(*ret_json)["params"]["group_id"] = std::to_string(root["params"]["group_id"].asInt64());
	}
	else if (action == "set_group_leave") {
		(*ret_json)["action"] = "leave_group";
		(*ret_json)["params"]["group_id"] = std::to_string(root["params"]["group_id"].asInt64());
	}
	else if (action == "set_group_special_title") {
		// onebot12没有对应的api
	}
	else if (action == "set_friend_add_request") {
		// onebot12没有对应的api
	}
	else if (action == "set_group_add_request") {
		// onebot12没有对应的api
	}
	else if (action == "get_login_info") {
		(*ret_json)["action"] = "get_self_info";
	}
	else if (action == "get_stranger_info") {
		(*ret_json)["action"] = "get_user_info";
		(*ret_json)["params"]["user_id"] = std::to_string(root["params"]["user_id"].asInt64());
	}
	else if (action == "get_group_list") {
		// do nothing
	}
	else if (action == "get_friend_list") {
		// do nothing
	}
	else if (action == "get_group_member_info") {
		(*ret_json)["params"]["user_id"] = std::to_string(root["params"]["user_id"].asInt64());
		(*ret_json)["params"]["group_id"] = std::to_string(root["params"]["group_id"].asInt64());
	}
	else if (action == "get_group_member_list") {
		(*ret_json)["params"]["group_id"] = std::to_string(root["params"]["group_id"].asInt64());
	}
	else if (action == "get_group_honor_info") {
		// onebot12没有对应的api
	}
	else if (action == "get_cookies") {
		// onebot12没有对应的api
	}
	else if (action == "get_csrf_token") {
		// onebot12没有对应的api
	}
	else if (action == "get_credentials") {
		// onebot12没有对应的api
	}
	else if (action == "get_record") {
		// todo
	}
	else if (action == "get_image") {
		// todo
	}
	else if (action == "can_send_image") {
		ret["status"] = "ok";
		ret["retcode"] = 0;
		ret["message"] = "";
		ret["data"]["yes"] = true;
	}
	else if (action == "can_send_record") {
		ret["status"] = "ok";
		ret["retcode"] = 0;
		ret["message"] = "";
		ret["data"]["yes"] = true;
	}
	else if (action == "get_status") {
		// do nothing
	}
	else if (action == "get_version_info") {
		ret["status"] = "ok";
		ret["retcode"] = 0;
		ret["message"] = "";
		ret["data"]["app_name"] = "MiraiCQ";
		ret["data"]["app_version"] = "2.3.9";
		ret["data"]["protocol_version"] = "v11";
	}
	else if (action == "set_restart") {
		// onebot12没有对应的api
	}
	else if (action == "clean_cache") {
		// onebot12没有对应的api
	}
	else {
		(*ret_json)["action"] = "";
	}
	return ret_json;
}

MiraiNet::NetStruct OneBot12ApiDeal::deal_recv(const Json::Value & root)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	if ((*ret_json)["data"].isArray()) {
		for (auto& it : (*ret_json)["data"]) {
			std::string user_id = StrTool::get_str_from_json(it, "user_id", "");
			if (user_id != "") {
				it["user_id"] = std::stoll(user_id);
			}
			std::string group_id = StrTool::get_str_from_json(it, "group_id", "");
			if (group_id != "") {
				it["group_id"] = std::stoll(group_id);
			}
		}
	}
	else {
		std::string user_id = StrTool::get_str_from_json((*ret_json)["data"], "user_id", "");
		if (user_id != "") {
			(*ret_json)["data"]["user_id"] = std::stoll(user_id);
		}
		std::string group_id = StrTool::get_str_from_json((*ret_json)["data"], "group_id", "");
		if (group_id != "") {
			(*ret_json)["data"]["group_id"] = std::stoll(group_id);
		}
	}
	return ret_json;
}
