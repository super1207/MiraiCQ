#include <cstdint>
#include <memory>
#include <functional>

#include "center.h"
#include "../log/MiraiLog.h"


static thread_local std::string ret_str;

static int call_int_api(std::function<int(Center *)> fun_ptr) 
{
	auto center_ptr = Center::get_instance();
	try
	{
		return fun_ptr(center_ptr);
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_debug_log("CQP", e.what());
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
		MiraiLog::get_instance()->add_debug_log("CQP", e.what());
		return -1;
	}
}

static const char * call_str_api(std::function<std::string(Center *)> fun_ptr) 
{
	auto center_ptr = Center::get_instance();
	try
	{
		ret_str =  fun_ptr(center_ptr);
		return ret_str.c_str();
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_debug_log("CQP", e.what());
		return "";
	}
}


extern "C"
{
	int __stdcall CQ_sendPrivateMsg_T(int auth_code, int64_t qq, const char* msg)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_sendPrivateMsg(auth_code, qq, msg);
		});
	}

	int __stdcall CQ_sendGroupMsg_T(int auth_code, int64_t group_id, const char* msg)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_sendGroupMsg(auth_code, group_id, msg);
		});
	}

	int __stdcall CQ_sendDiscussMsg_T(int auth_code, int64_t discuss_id, const char* msg)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			/* 已经没有讨论组消息啦 */
			return -1;
		});
	}

	int __stdcall CQ_deleteMsg_T(int auth_code, int64_t msg_id)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_deleteMsg(auth_code, msg_id);
		});
	}

	int __stdcall CQ_sendLike_T(int auth_code, int64_t qq)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			/* 大部分开源OneBot实现都没有实现此接口 */
			return center_ptr->CQ_sendLike(auth_code, qq ,1);
		});
	}

	int __stdcall CQ_sendLikeV2_T(int auth_code, int64_t qq,int times)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			/* 大部分开源OneBot实现都没有实现此接口 */
			return center_ptr->CQ_sendLike(auth_code, qq, times);
		});
	}

	int __stdcall CQ_setGroupKick_T(int auth_code, int64_t group_id, int64_t qq, int reject_add_request)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setGroupKick(auth_code, group_id, qq, reject_add_request);
		});
	}

	int __stdcall CQ_setGroupBan_T(int auth_code, int64_t group_id, int64_t qq, int64_t duration)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setGroupBan(auth_code, group_id, qq, duration);
		});
	}

	int __stdcall CQ_setGroupAnonymousBan_T(int auth_code, int64_t group_id, const char* anonymous, int64_t duration)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setGroupAnonymousBan(auth_code, group_id, anonymous, duration);
		});
	}

	int __stdcall CQ_setGroupWholeBan_T(int auth_code, int64_t group_id, int enable)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setGroupWholeBan(auth_code, group_id, enable);
		});
	}

	int __stdcall CQ_setGroupAdmin_T(int auth_code, __int64 group_id, __int64 qq, int set)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setGroupAdmin(auth_code, group_id, qq, set);
		});
	}

	int __stdcall CQ_setGroupAnonymous_T(int auth_code, __int64 group_id, int enable)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setGroupAnonymous(auth_code, group_id, enable);
		});
	}

	int __stdcall CQ_setGroupCard_T(int auth_code, __int64 group_id, __int64 qq, const char* new_card)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setGroupCard(auth_code, group_id, qq, new_card);
		});
	}

	int __stdcall CQ_setGroupLeave_T(int auth_code, __int64 group_id, int is_dismiss)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setGroupLeave(auth_code, group_id, is_dismiss);
		});
	}

	int __stdcall CQ_setGroupSpecialTitle_T(int auth_code, __int64 group_id, __int64 qq, const char* new_special_title, __int64 duration)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setGroupSpecialTitle(auth_code, group_id,qq, new_special_title, duration);
		});
	}

	int __stdcall CQ_setDiscussLeave_T(int auth_code, __int64 discuss_id)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			/* 已经没有讨论组消息啦 */
			return -1;
		});
	}

	int __stdcall CQ_setFriendAddRequest_T(__int32 auth_code, const char* response_flag, __int32 response_operation, const char* remark)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setFriendAddRequest(auth_code, response_flag, response_operation, remark);
		});
	}

	int __stdcall CQ_setGroupAddRequest_T(__int32 auth_code,const char* response_flag, int request_type, int response_operation)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setGroupAddRequest(auth_code, response_flag, request_type, response_operation,"");
		});
	}

	int __stdcall CQ_setGroupAddRequestV2_T(int auth_code, const char* response_flag, __int32 request_type, __int32 response_operation, const char* reason)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setGroupAddRequest(auth_code, response_flag, request_type, response_operation, reason);
		});
	}

	int64_t __stdcall CQ_getLoginQQ_T(int auth_code)
	{
		return call_int64_api([&](Center *  center_ptr)->int64_t {
			return center_ptr->CQ_getLoginQQ(auth_code);
		});
	}

	const char * __stdcall CQ_getLoginNick_T(int auth_code)
	{
		return call_str_api([&](Center *  center_ptr)->std::string {
			return center_ptr->CQ_getLoginNick(auth_code);
		});
	}

	const char* __stdcall CQ_getStrangerInfo_T(int auth_code, __int64 qq, int no_cache)
	{
		return call_str_api([&](Center *  center_ptr)->std::string {
			return center_ptr->CQ_getStrangerInfo(auth_code,qq, no_cache);
		});
	}

	const char* __stdcall CQ_getFriendList_T(int auth_code, int reserved)
	{
		return call_str_api([&](Center *  center_ptr)->std::string {
			return center_ptr->CQ_getFriendList(auth_code, reserved);
		});
	}

	const char* __stdcall CQ_getGroupList_T(int auth_code)
	{
		return call_str_api([&](Center *  center_ptr)->std::string {
			return center_ptr->CQ_getGroupList(auth_code);
		});
	}

	const char* __stdcall CQ_getGroupInfo_T(int auth_code, int64_t group_id, int no_cache)
	{
		return call_str_api([&](Center *  center_ptr)->std::string {
			return center_ptr->CQ_getGroupInfo(auth_code, group_id, no_cache);
		});
	}

	const char* __stdcall CQ_getGroupMemberList_T(int auth_code, int64_t group_id)
	{
		return call_str_api([&](Center *  center_ptr)->std::string {
			return center_ptr->CQ_getGroupMemberList(auth_code, group_id);
		});
	}

	const char* __stdcall CQ_getGroupMemberInfoV2_T(int auth_code, int64_t group_id, int64_t qq, int no_cache)
	{
		return call_str_api([&](Center *  center_ptr)->std::string {
			return center_ptr->CQ_getGroupMemberInfoV2(auth_code, group_id,qq,no_cache);
		});
	}

	const char* __stdcall CQ_getCookies_T(int auth_code)
	{
		/* 大部分开源OneBot实现都没有实现此接口 */
		return call_str_api([&](Center *  center_ptr)->std::string {
			return center_ptr->CQ_getCookies(auth_code,"");
		});
	}

	const char* __stdcall CQ_getCookiesV2_T(int auth_code, const char* domain)
	{
		/* 大部分开源OneBot实现都没有实现此接口 */
		return call_str_api([&](Center *  center_ptr)->std::string {
			return center_ptr->CQ_getCookies(auth_code, domain);
		});
	}

	int __stdcall CQ_getCsrfToken_T(int auth_code)
	{
		/* 大部分开源OneBot实现都没有实现此接口 */
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_getCsrfToken(auth_code);
		});
	}

	const char* __stdcall CQ_getAppDirectory_T(int auth_code)
	{
		return call_str_api([&](Center *  center_ptr)->std::string {
			return center_ptr->CQ_getAppDirectory(auth_code);
		});
	}

	const char* __stdcall CQ_getRecord_T(int auth_code, const char* file, const char* out_format)
	{
		/* 此接口暂时未实现 */
		return call_str_api([&](Center *  center_ptr)->std::string {
			/* 返回相对路径 */
			return center_ptr->CQ_getRecord(auth_code, file, out_format,false);
		});
	}

	const char* __stdcall CQ_getRecordV2_T(int auth_code, const char* file, const char* out_format)
	{
		/* 此接口暂时未实现 */
		return call_str_api([&](Center *  center_ptr)->std::string {
			/* 返回绝对路径 */
			return center_ptr->CQ_getRecord(auth_code, file, out_format,true);
		});
	}

	const char* __stdcall CQ_getImage_T(int auth_code, const char* file)
	{
		return call_str_api([&](Center *  center_ptr)->std::string {
			return center_ptr->CQ_getImage(auth_code, file);
		});
	}

	int __stdcall CQ_canSendImage_T(int auth_code)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_canSendImage(auth_code);
		});
	}

	int __stdcall CQ_canSendRecord_T(int auth_code)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_canSendRecord(auth_code);
		});
	}

	int __stdcall CQ_addLog_T(int auth_code, int log_level, const char* category, const char* log_msg)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_addLog(auth_code, log_level, category, log_msg);
		});
	}

	int __stdcall CQ_setFatal_T(__int32 auth_code, const char* error_info)
	{
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setFatal(auth_code, error_info);
		});
	}

	int __stdcall CQ_setRestart_T(int auth_code)
	{
		/* 此接口暂时未实现 */
		return call_int_api([&](Center *  center_ptr)->int {
			return center_ptr->CQ_setRestart(auth_code);
		});
	}


}