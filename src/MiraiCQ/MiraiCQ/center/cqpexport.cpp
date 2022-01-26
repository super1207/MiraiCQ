#include <cstdint>
#include <memory>
#include <functional>

#include "center.h"
#include "../log/MiraiLog.h"
#include <spdlog/fmt/fmt.h>
#include "../../IPC/ipc.h"

extern std::string g_main_flag;
static thread_local std::string ret_str;

static int call_int_api(std::function<int(Center*)> fun_ptr)
{
	auto center_ptr = Center::get_instance();
	try
	{
		return fun_ptr(center_ptr);
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_warning_log("CQP", e.what());
		return -1;
	}
}

static int64_t call_int64_api(std::function<int64_t(Center*)> fun_ptr)
{
	auto center_ptr = Center::get_instance();
	try
	{
		return fun_ptr(center_ptr);
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_warning_log("CQP", e.what());
		return -1;
	}
}

static const char* call_str_api(std::function<std::string(Center*)> fun_ptr)
{
	auto center_ptr = Center::get_instance();
	try
	{
		ret_str = fun_ptr(center_ptr);
		return ret_str.c_str();
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_warning_log("CQP", e.what());
		return "";
	}
}


extern "C"
{
	int __stdcall CQ_sendPrivateMsg_T(int auth_code, int64_t qq, const char* msg) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_sendPrivateMsg called,auth_code:{},qq:{},msg:{}", auth_code, qq, (msg ? msg : "NULL")));
			Json::Value to_send;
			to_send["action"] = "CQ_sendPrivateMsg";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["qq"] = qq;
			to_send["params"]["msg"] = (msg ? msg : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_sendPrivateMsg ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_sendGroupMsg_T(int auth_code, int64_t group_id, const char* msg) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_sendGroupMsg called,auth_code:{},group_id:{},msg:{}", auth_code, group_id, (msg ? msg : "NULL")));
			Json::Value to_send;
			to_send["action"] = "CQ_sendGroupMsg";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["msg"] = (msg ? msg : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_sendGroupMsg ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_sendDiscussMsg_T(int auth_code, int64_t discuss_id, const char* msg) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_sendDiscussMsg called,auth_code:{},discuss_id:{},msg:{}", auth_code, discuss_id, (msg ? msg : "NULL")));
			Json::Value to_send;
			to_send["action"] = "CQ_sendDiscussMsg";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["discuss_id"] = discuss_id;
			to_send["params"]["msg"] = (msg ? msg : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_sendDiscussMsg ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_deleteMsg_T(int auth_code, int64_t msg_id) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_deleteMsg called,auth_code:{},msg_id:{}", auth_code, msg_id));
			Json::Value to_send;
			to_send["action"] = "CQ_deleteMsg";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["msg_id"] = msg_id;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_deleteMsg ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_sendLike_T(int auth_code, int64_t qq) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_sendLike called,auth_code:{},qq:{}", auth_code, qq));
			Json::Value to_send;
			to_send["action"] = "CQ_sendLike";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["qq"] = qq;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_sendLike ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_sendLikeV2_T(int auth_code, int64_t qq, int times) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_sendLikeV2 called,auth_code:{},qq:{},times:{}", auth_code, qq, times));
			Json::Value to_send;
			to_send["action"] = "CQ_sendLikeV2";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["qq"] = qq;
			to_send["params"]["times"] = times;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_sendLikeV2 ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setGroupKick_T(int auth_code, int64_t group_id, int64_t qq, int reject_add_request) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupKick called,auth_code:{},group_id:{},qq:{},reject_add_request:{}", auth_code, group_id, qq, reject_add_request));
			Json::Value to_send;
			to_send["action"] = "CQ_setGroupKick";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["qq"] = qq;
			to_send["params"]["reject_add_request"] = reject_add_request;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupKick ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setGroupBan_T(int auth_code, int64_t group_id, int64_t qq, int64_t duration) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupBan called,auth_code:{},group_id:{},qq:{},duration:{}", auth_code, group_id, qq, duration));
			Json::Value to_send;
			to_send["action"] = "CQ_setGroupBan";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["qq"] = qq;
			to_send["params"]["duration"] = duration;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupBan ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setGroupAnonymousBan_T(int auth_code, int64_t group_id, const char* anonymous, int64_t duration) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupAnonymousBan called,auth_code:{},group_id:{},anonymous:{},duration:{}", auth_code, group_id, (anonymous ? anonymous : "NULL"), duration));
			Json::Value to_send;
			to_send["action"] = "CQ_setGroupAnonymousBan";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["anonymous"] = (anonymous ? anonymous : "");
			to_send["params"]["duration"] = duration;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupAnonymousBan ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setGroupWholeBan_T(int auth_code, int64_t group_id, int enable) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupWholeBan called,auth_code:{},group_id:{},enable:{}", auth_code, group_id, enable));
			Json::Value to_send;
			to_send["action"] = "CQ_setGroupWholeBan";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["enable"] = enable;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupWholeBan ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setGroupAdmin_T(int auth_code, int64_t group_id, int64_t qq, int set) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupAdmin called,auth_code:{},group_id:{},qq:{},set:{}", auth_code, group_id, qq, set));
			Json::Value to_send;
			to_send["action"] = "CQ_setGroupAdmin";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["qq"] = qq;
			to_send["params"]["set"] = set;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupAdmin ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setGroupAnonymous_T(int auth_code, int64_t group_id, int enable) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupAnonymous called,auth_code:{},group_id:{},enable:{}", auth_code, group_id, enable));
			Json::Value to_send;
			to_send["action"] = "CQ_setGroupAnonymous";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["enable"] = enable;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupAnonymous ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setGroupCard_T(int auth_code, int64_t group_id, int64_t qq, const char* new_card) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupCard called,auth_code:{},group_id:{},qq:{},new_card:{}", auth_code, group_id, qq, (new_card ? new_card : "NULL")));
			Json::Value to_send;
			to_send["action"] = "CQ_setGroupCard";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["qq"] = qq;
			to_send["params"]["new_card"] = (new_card ? new_card : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupCard ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setGroupLeave_T(int auth_code, int64_t group_id, int is_dismiss) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupLeave called,auth_code:{},group_id:{},is_dismiss:{}", auth_code, group_id, is_dismiss));
			Json::Value to_send;
			to_send["action"] = "CQ_setGroupLeave";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["is_dismiss"] = is_dismiss;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupLeave ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setGroupSpecialTitle_T(int auth_code, int64_t group_id, int64_t qq, const char* new_special_title, int64_t duration) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupSpecialTitle called,auth_code:{},group_id:{},qq:{},new_special_title:{},duration:{}", auth_code, group_id, qq, (new_special_title ? new_special_title : "NULL"), duration));
			Json::Value to_send;
			to_send["action"] = "CQ_setGroupSpecialTitle";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["qq"] = qq;
			to_send["params"]["new_special_title"] = (new_special_title ? new_special_title : "");
			to_send["params"]["duration"] = duration;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupSpecialTitle ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setDiscussLeave_T(int auth_code, int64_t discuss_id) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setDiscussLeave called,auth_code:{},discuss_id:{}", auth_code, discuss_id));
			Json::Value to_send;
			to_send["action"] = "CQ_setDiscussLeave";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["discuss_id"] = discuss_id;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setDiscussLeave ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setFriendAddRequest_T(int auth_code, const char* response_flag, int response_operation, const char* remark) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setFriendAddRequest called,auth_code:{},response_flag:{},response_operation:{},remark:{}", auth_code, (response_flag ? response_flag : "NULL"), response_operation, (remark ? remark : "NULL")));
			Json::Value to_send;
			to_send["action"] = "CQ_setFriendAddRequest";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["response_flag"] = (response_flag ? response_flag : "");
			to_send["params"]["response_operation"] = response_operation;
			to_send["params"]["remark"] = (remark ? remark : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setFriendAddRequest ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setGroupAddRequest_T(int auth_code, const char* response_flag, int request_type, int response_operation) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupAddRequest called,auth_code:{},response_flag:{},request_type:{},response_operation:{}", auth_code, (response_flag ? response_flag : "NULL"), request_type, response_operation));
			Json::Value to_send;
			to_send["action"] = "CQ_setGroupAddRequest";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["response_flag"] = (response_flag ? response_flag : "");
			to_send["params"]["request_type"] = request_type;
			to_send["params"]["response_operation"] = response_operation;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupAddRequest ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_setGroupAddRequestV2_T(int auth_code, const char* response_flag, int request_type, int response_operation, const char* reason) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupAddRequestV2 called,auth_code:{},response_flag:{},request_type:{},response_operation:{},reason:{}", auth_code, (response_flag ? response_flag : "NULL"), request_type, response_operation, (reason ? reason : "NULL")));
			Json::Value to_send;
			to_send["action"] = "CQ_setGroupAddRequestV2";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["response_flag"] = (response_flag ? response_flag : "");
			to_send["params"]["request_type"] = request_type;
			to_send["params"]["response_operation"] = response_operation;
			to_send["params"]["reason"] = (reason ? reason : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setGroupAddRequestV2 ret:{}", ret));
			return ret;
			});
	}
	int64_t __stdcall CQ_getLoginQQ_T(int auth_code) {
		return call_int64_api([&](Center* center_ptr)->int64_t {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getLoginQQ called,auth_code:{}", auth_code));
			Json::Value to_send;
			to_send["action"] = "CQ_getLoginQQ";
			to_send["params"]["auth_code"] = auth_code;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int64_t ret = std::stoll((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getLoginQQ ret:{}", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getLoginNick_T(int auth_code) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getLoginNick called,auth_code:{}", auth_code));
			Json::Value to_send;
			to_send["action"] = "CQ_getLoginNick";
			to_send["params"]["auth_code"] = auth_code;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getLoginNick ret:\"{}\"", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getStrangerInfo_T(int auth_code, int64_t qq, int no_cache) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getStrangerInfo called,auth_code:{},qq:{},no_cache:{}", auth_code, qq, no_cache));
			Json::Value to_send;
			to_send["action"] = "CQ_getStrangerInfo";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["qq"] = qq;
			to_send["params"]["no_cache"] = no_cache;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getStrangerInfo ret:\"{}\"", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getFriendList_T(int auth_code, int reserved) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getFriendList called,auth_code:{},reserved:{}", auth_code, reserved));
			Json::Value to_send;
			to_send["action"] = "CQ_getFriendList";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["reserved"] = reserved;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getFriendList ret:\"{}\"", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getGroupList_T(int auth_code) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getGroupList called,auth_code:{}", auth_code));
			Json::Value to_send;
			to_send["action"] = "CQ_getGroupList";
			to_send["params"]["auth_code"] = auth_code;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getGroupList ret:\"{}\"", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getGroupInfo_T(int auth_code, int64_t group_id, int no_cache) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getGroupInfo called,auth_code:{},group_id:{},no_cache:{}", auth_code, group_id, no_cache));
			Json::Value to_send;
			to_send["action"] = "CQ_getGroupInfo";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["no_cache"] = no_cache;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getGroupInfo ret:\"{}\"", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getGroupMemberList_T(int auth_code, int64_t group_id) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getGroupMemberList called,auth_code:{},group_id:{}", auth_code, group_id));
			Json::Value to_send;
			to_send["action"] = "CQ_getGroupMemberList";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getGroupMemberList ret:\"{}\"", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getGroupMemberInfoV2_T(int auth_code, int64_t group_id, int64_t qq, int no_cache) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getGroupMemberInfoV2 called,auth_code:{},group_id:{},qq:{},no_cache:{}", auth_code, group_id, qq, no_cache));
			Json::Value to_send;
			to_send["action"] = "CQ_getGroupMemberInfoV2";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["group_id"] = group_id;
			to_send["params"]["qq"] = qq;
			to_send["params"]["no_cache"] = no_cache;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getGroupMemberInfoV2 ret:\"{}\"", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getCookies_T(int auth_code) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getCookies called,auth_code:{}", auth_code));
			Json::Value to_send;
			to_send["action"] = "CQ_getCookies";
			to_send["params"]["auth_code"] = auth_code;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getCookies ret:\"{}\"", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getCookiesV2_T(int auth_code, const char* domain) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getCookiesV2 called,auth_code:{},domain:{}", auth_code, (domain ? domain : "NULL")));
			Json::Value to_send;
			to_send["action"] = "CQ_getCookiesV2";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["domain"] = (domain ? domain : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getCookiesV2 ret:\"{}\"", ret));
			return ret;
			});
	}
	int __stdcall CQ_getCsrfToken_T(int auth_code) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getCsrfToken called,auth_code:{}", auth_code));
			Json::Value to_send;
			to_send["action"] = "CQ_getCsrfToken";
			to_send["params"]["auth_code"] = auth_code;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getCsrfToken ret:{}", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getAppDirectory_T(int auth_code) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getAppDirectory called,auth_code:{}", auth_code));
			Json::Value to_send;
			to_send["action"] = "CQ_getAppDirectory";
			to_send["params"]["auth_code"] = auth_code;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getAppDirectory ret:\"{}\"", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getRecord_T(int auth_code, const char* file, const char* out_format) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getRecord called,auth_code:{},file:{},out_format:{}", auth_code, (file ? file : "NULL"), (out_format ? out_format : "NULL")));
			Json::Value to_send;
			to_send["action"] = "CQ_getRecord";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["file"] = (file ? file : "");
			to_send["params"]["out_format"] = (out_format ? out_format : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getRecord ret:\"{}\"", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getRecordV2_T(int auth_code, const char* file, const char* out_format) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getRecordV2 called,auth_code:{},file:{},out_format:{}", auth_code, (file ? file : "NULL"), (out_format ? out_format : "NULL")));
			Json::Value to_send;
			to_send["action"] = "CQ_getRecordV2";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["file"] = (file ? file : "");
			to_send["params"]["out_format"] = (out_format ? out_format : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getRecordV2 ret:\"{}\"", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_getImage_T(int auth_code, const char* file) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getImage called,auth_code:{},file:{}", auth_code, (file ? file : "NULL")));
			Json::Value to_send;
			to_send["action"] = "CQ_getImage";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["file"] = (file ? file : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_getImage ret:\"{}\"", ret));
			return ret;
			});
	}
	int __stdcall CQ_canSendImage_T(int auth_code) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_canSendImage called,auth_code:{}", auth_code));
			Json::Value to_send;
			to_send["action"] = "CQ_canSendImage";
			to_send["params"]["auth_code"] = auth_code;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_canSendImage ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_canSendRecord_T(int auth_code) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_canSendRecord called,auth_code:{}", auth_code));
			Json::Value to_send;
			to_send["action"] = "CQ_canSendRecord";
			to_send["params"]["auth_code"] = auth_code;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_canSendRecord ret:{}", ret));
			return ret;
			});
	}
	int __stdcall CQ_addLog_T(int auth_code, int log_level, const char* category, const char* log_msg) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_addLog called"));
			Json::Value to_send;
			to_send["action"] = "CQ_addLog";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["log_level"] = log_level;
			to_send["params"]["category"] = (category ? category : "");
			to_send["params"]["log_msg"] = (log_msg ? log_msg : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			return ret;
			});
	}
	int __stdcall CQ_setFatal_T(int auth_code, const char* error_info) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setFatal called"));
			Json::Value to_send;
			to_send["action"] = "CQ_setFatal";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["error_info"] = (error_info ? error_info : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			return ret;
			});
	}
	int __stdcall CQ_setRestart_T(int auth_code) {
		return call_int_api([&](Center* center_ptr)->int {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setRestart called,auth_code:{}", auth_code));
			Json::Value to_send;
			to_send["action"] = "CQ_setRestart";
			to_send["params"]["auth_code"] = auth_code;
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			int ret = std::stoi((ret_str != "" ? ret_str : "-1"));
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_setRestart ret:{}", ret));
			return ret;
			});
	}
	const char* __stdcall CQ_callApi_T(int auth_code, const char* msg) {
		return call_str_api([&](Center* center_ptr)->std::string {
			MiraiLog::get_instance()->add_debug_log("CQP", fmt::format("CQ_callApi called"));
			Json::Value to_send;
			to_send["action"] = "CQ_callApi";
			to_send["params"]["auth_code"] = auth_code;
			to_send["params"]["msg"] = (msg ? msg : "");
			std::string ret_str = IPC_ApiSend(g_main_flag.c_str(), Json::FastWriter().write(to_send).c_str(), 15000);
			std::string ret = ret_str;
			return ret;
			});
	}


}