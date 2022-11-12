#include "center.h"

#include "../tool/StrTool.h"
#include "../log/MiraiLog.h"

static Json::Value send_private_msg(int64_t user_id, int64_t target_id) {
	Json::Value to_send;
	to_send["event_type"] = "cq_event_private_message";
	to_send["data"]["sub_type"] = 11; //friend
	to_send["data"]["msg_id"] = 0;
	to_send["data"]["from_qq"] = user_id;
	to_send["data"]["msg"] = "[CQ:poke,qq=" + std::to_string(target_id) + "]";
	to_send["data"]["font"] = 11;
	return to_send;
}

static Json::Value send_group_msg(int64_t group_id, int64_t user_id, int64_t target_id) {
	Json::Value to_send;
	to_send["event_type"] = "cq_event_group_message";
	to_send["data"]["sub_type"] = 1;
	to_send["data"]["msg_id"] = 0;
	to_send["data"]["from_group"] = group_id;
	to_send["data"]["from_qq"] = user_id;
	to_send["data"]["anonymous"] = "";
	to_send["data"]["msg"] = "[CQ:poke,qq=" + std::to_string(target_id) + "]";
	to_send["data"]["font"] = 11;
	return to_send;
}

Json::Value Center::deal_poke_event(Json::Value& evt)
{
	std::string post_type = StrTool::get_str_from_json(evt, "post_type","");
	if (post_type == "notice") {
		std::string notice_type = StrTool::get_str_from_json(evt, "notice_type", "");
		if (notice_type == "notify") {
			std::string sub_type = StrTool::get_str_from_json(evt, "sub_type", "");
			if (sub_type == "poke") {
				int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);
				int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
				int64_t target_id = StrTool::get_int64_from_json(evt, "target_id", 0);
				if (group_id == 0 && user_id != 0) {
					//ºÃÓÑ
					return send_private_msg(user_id, target_id);
				}
				else if (group_id != 0 && user_id != 0) {
					// ÈºÄÚ
					return send_group_msg(group_id, user_id, target_id);
				}
			}
		}
	}
	return Json::Value();
}