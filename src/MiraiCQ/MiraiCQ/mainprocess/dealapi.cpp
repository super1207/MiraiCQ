#include <cassert>

#include "../plus/MiraiPlus.h"
#include "../log/MiraiLog.h"
#include "../tool/StrTool.h"
#include "../center/center.h"
#include "../tool/IPCTool.h"
#include "../tool/ThreadTool.h"

static int get_auth_code_by_uuid(const std::string& uuid)
{
	auto plus = MiraiPlus::get_instance()->get_all_plus();
	for (auto i : plus) {
		if (uuid == i.second->get_uuid()) {
			return i.first;
		}
	}
	MiraiLog::get_instance()->add_fatal_log("GETAUTHCODEERR", uuid);
	exit(-1);
}

static std::string get_fun_name_by_type(const std::string& uuid, int tp)
{
	auto all_plus = MiraiPlus::get_instance()->get_all_plus();
	for (auto plus : all_plus) {
		if (uuid != plus.second->get_uuid()) {
			continue;
		}
		for (auto fun : plus.second->event_vec) {
			if (fun->type != tp) {
				continue;
			}
			std::string ret = fun->fun_name;
			if (ret == "?") {
				MiraiLog::get_instance()->add_fatal_log("mainprocess", "`?` can not be a plus's event name");
				exit(-1);
			}
			return ret;
		}
	}
	return "?";
}

static void deal_api_thread_(const std::string& sender, const std::string& flag, const std::string& msg)
{
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(msg, root))
	{
		/* Json解析失败 */
		MiraiLog::get_instance()->add_debug_log("TEST_IPC", "收到不规范的Json" + std::string(msg));
		IPC_ApiReply(sender.c_str(), flag.c_str(), NULL);
		return;
	}
	std::string action = StrTool::get_str_from_json(root, "action", "");
	Json::Value params = root.get("params", Json::Value());
	if (params.isObject())
	{
		params["auth_code"] = get_auth_code_by_uuid(flag);
	}
	if (action == "CQ_sendPrivateMsg") {
		auto ret = Center::get_instance()->CQ_sendPrivateMsg(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_str_from_json(params, "msg", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_sendGroupMsg") {
		auto ret = Center::get_instance()->CQ_sendGroupMsg(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_str_from_json(params, "msg", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_sendDiscussMsg") {
		auto ret = Center::get_instance()->CQ_sendDiscussMsg(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "discuss_id", 0)
			, StrTool::get_str_from_json(params, "msg", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_deleteMsg") {
		auto ret = Center::get_instance()->CQ_deleteMsg(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "msg_id", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_sendLike") {
		auto ret = Center::get_instance()->CQ_sendLike(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_sendLikeV2") {
		auto ret = Center::get_instance()->CQ_sendLikeV2(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int_from_json(params, "times", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupKick") {
		auto ret = Center::get_instance()->CQ_setGroupKick(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int_from_json(params, "reject_add_request", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupBan") {
		auto ret = Center::get_instance()->CQ_setGroupBan(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int64_from_json(params, "duration", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupAnonymousBan") {
		auto ret = Center::get_instance()->CQ_setGroupAnonymousBan(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_str_from_json(params, "anonymous", "").c_str()
			, StrTool::get_int64_from_json(params, "duration", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupWholeBan") {
		auto ret = Center::get_instance()->CQ_setGroupWholeBan(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int_from_json(params, "enable", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupAdmin") {
		auto ret = Center::get_instance()->CQ_setGroupAdmin(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int_from_json(params, "set", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupAnonymous") {
		auto ret = Center::get_instance()->CQ_setGroupAnonymous(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int_from_json(params, "enable", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupCard") {
		auto ret = Center::get_instance()->CQ_setGroupCard(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_str_from_json(params, "new_card", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupLeave") {
		auto ret = Center::get_instance()->CQ_setGroupLeave(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int_from_json(params, "is_dismiss", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupSpecialTitle") {
		auto ret = Center::get_instance()->CQ_setGroupSpecialTitle(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_str_from_json(params, "new_special_title", "").c_str()
			, StrTool::get_int64_from_json(params, "duration", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setDiscussLeave") {
		auto ret = Center::get_instance()->CQ_setDiscussLeave(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "discuss_id", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setFriendAddRequest") {
		auto ret = Center::get_instance()->CQ_setFriendAddRequest(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "response_flag", "").c_str()
			, StrTool::get_int_from_json(params, "response_operation", 0)
			, StrTool::get_str_from_json(params, "remark", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupAddRequest") {
		auto ret = Center::get_instance()->CQ_setGroupAddRequest(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "response_flag", "").c_str()
			, StrTool::get_int_from_json(params, "request_type", 0)
			, StrTool::get_int_from_json(params, "response_operation", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setGroupAddRequestV2") {
		auto ret = Center::get_instance()->CQ_setGroupAddRequestV2(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "response_flag", "").c_str()
			, StrTool::get_int_from_json(params, "request_type", 0)
			, StrTool::get_int_from_json(params, "response_operation", 0)
			, StrTool::get_str_from_json(params, "reason", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getLoginQQ") {
		auto ret = Center::get_instance()->CQ_getLoginQQ(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getLoginNick") {
		auto ret = Center::get_instance()->CQ_getLoginNick(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getStrangerInfo") {
		auto ret = Center::get_instance()->CQ_getStrangerInfo(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int_from_json(params, "no_cache", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getFriendList") {
		auto ret = Center::get_instance()->CQ_getFriendList(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int_from_json(params, "reserved", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getGroupList") {
		auto ret = Center::get_instance()->CQ_getGroupList(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getGroupInfo") {
		auto ret = Center::get_instance()->CQ_getGroupInfo(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int_from_json(params, "no_cache", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getGroupMemberList") {
		auto ret = Center::get_instance()->CQ_getGroupMemberList(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getGroupMemberInfoV2") {
		auto ret = Center::get_instance()->CQ_getGroupMemberInfoV2(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int64_from_json(params, "group_id", 0)
			, StrTool::get_int64_from_json(params, "qq", 0)
			, StrTool::get_int_from_json(params, "no_cache", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getCookies") {
		auto ret = Center::get_instance()->CQ_getCookies(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getCookiesV2") {
		auto ret = Center::get_instance()->CQ_getCookiesV2(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "domain", "").c_str()
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getCsrfToken") {
		auto ret = Center::get_instance()->CQ_getCsrfToken(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getAppDirectory") {
		auto ret = Center::get_instance()->CQ_getAppDirectory(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getRecord") {
		auto ret = Center::get_instance()->CQ_getRecord(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "file", "").c_str()
			, StrTool::get_str_from_json(params, "out_format", "").c_str()
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getRecordV2") {
		auto ret = Center::get_instance()->CQ_getRecordV2(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "file", "").c_str()
			, StrTool::get_str_from_json(params, "out_format", "").c_str()
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_getImage") {
		auto ret = Center::get_instance()->CQ_getImage(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "file", "").c_str()
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_canSendImage") {
		auto ret = Center::get_instance()->CQ_canSendImage(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_canSendRecord") {
		auto ret = Center::get_instance()->CQ_canSendRecord(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_addLog") {
		auto ret = Center::get_instance()->CQ_addLog(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_int_from_json(params, "log_level", 0)
			, StrTool::get_str_from_json(params, "category", "").c_str()
			, StrTool::get_str_from_json(params, "log_msg", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setFatal") {
		auto ret = Center::get_instance()->CQ_setFatal(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "error_info", "").c_str()
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_setRestart") {
		auto ret = Center::get_instance()->CQ_setRestart(
			StrTool::get_int_from_json(params, "auth_code", 0)
		);
		std::string ret_str = std::to_string(ret);
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "CQ_callApi") {
		auto ret = Center::get_instance()->CQ_callApi(
			StrTool::get_int_from_json(params, "auth_code", 0)
			, StrTool::get_str_from_json(params, "msg", "").c_str()
		);
		std::string ret_str = ret;
		IPC_ApiReply(sender.c_str(), flag.c_str(), ret_str.c_str());
	}
	else if (action == "get_fun_name")
	{
		int funtype = StrTool::get_int_from_json(root, "params", 0);
		std::string funname = get_fun_name_by_type(flag, funtype);
		IPC_ApiReply(sender.c_str(), flag.c_str(), funname.c_str());
	}
	else if (action == "heartbeat")
	{
		IPC_ApiReply(sender.c_str(), flag.c_str(), "OK");
	}
	else
	{
		IPC_ApiReply(sender.c_str(), flag.c_str(), "");
		// 无法处理的函数
	}
}

static void deal_api_thread(const std::string& sender, const std::string& flag, const std::string& msg)
{
	try {
		deal_api_thread_(sender, flag, msg);
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_warning_log("deal_api_thread", e.what());
	}

}

void do_api_call(const char* sender, const char* flag, const char* msg)
{
	assert(msg);
	assert(sender);
	assert(flag);
	std::string msg_str = msg;
	std::string flag_str = flag;
	std::string sender_str = sender;
	ThreadTool::get_instance()->submit([msg_str, flag_str, sender_str]() {
		deal_api_thread(sender_str, flag_str, msg_str);
		});
}