#include "center.h"

#include "../tool/StrTool.h"
#include "../log/MiraiLog.h"
#include <websocketpp/base64/base64.hpp>


static Json::Value send_private_msg(int64_t user_id,const std::string &  msg) {
	Json::Value to_send;
	to_send["event_type"] = "cq_event_private_message";
	to_send["data"]["sub_type"] = 11; //friend
	to_send["data"]["msg_id"] = 0;
	to_send["data"]["from_qq"] = user_id;
	to_send["data"]["msg"] = "[CQ:mcqevt,data=" + msg + "]";;
	to_send["data"]["font"] = 11;
	return to_send;
}

static Json::Value send_group_msg(int64_t group_id,int64_t user_id, const std::string& msg) {
	Json::Value to_send;
	to_send["event_type"] = "cq_event_group_message";
	to_send["data"]["sub_type"] = 1;
	to_send["data"]["msg_id"] = 0;
	to_send["data"]["from_group"] = group_id;
	to_send["data"]["from_qq"] = user_id;
	to_send["data"]["anonymous"] = "";
	to_send["data"]["msg"] = "[CQ:mcqevt,data=" + msg + "]";
	to_send["data"]["font"] = 11;
	return to_send;
}

Json::Value Center::deal_ex_event(Json::Value& evt)
{
	int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
	std::string toSend;
	Json::Value ret;
	if (group_id == 0 && user_id != 0) {
		//����
		toSend = websocketpp::base64_encode(Json::FastWriter().write(evt));
		return send_private_msg(user_id, toSend);
	}
	else if(group_id != 0 && user_id != 0) {
		// Ⱥ��
		toSend = websocketpp::base64_encode(Json::FastWriter().write(evt));
		return send_group_msg(group_id, user_id, toSend);
	}
	return Json::Value();
}