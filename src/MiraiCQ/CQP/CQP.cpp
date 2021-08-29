#include "pch.h"

#include <stdint.h>

#define FUNC(ReturnType, FuncName, ...) extern "C"  ReturnType __stdcall CQ_##FuncName(__VA_ARGS__)
#define cq_bool_t int

#define STR1(R)  #R
#define STR2(R)  STR1(R)

template <class T>
class TP10086
{
};

static const char* RETERR(const TP10086<const char*>& v)
{
	return "";
}
static int RETERR(const TP10086<int>& v)
{
	return -1;
}
static int64_t RETERR(const TP10086<int64_t>& v)
{
	return -1;
}

#define FFUN1(ReturnType, FuncName, ...) FUNC(ReturnType, FuncName, __VA_ARGS__) \
{\
	typedef ReturnType( __stdcall  * fun_ptr_type)(__VA_ARGS__);\
	fun_ptr_type fun_ptr = (fun_ptr_type)GetProcAddress(GetModuleHandle(NULL),"CQ_"STR2(FuncName) "_T");\
	if(!fun_ptr){ReturnType ret = RETERR(TP10086<ReturnType>());return ret;}

#define FFUN2(...) return fun_ptr(__VA_ARGS__);}

// Message
FFUN1(int, sendPrivateMsg, int auth_code, int64_t qq, const char* msg) FFUN2(auth_code, qq, msg)
FFUN1(int, sendGroupMsg, int auth_code, int64_t group_id, const char* msg) FFUN2(auth_code, group_id, msg)
FFUN1(int, sendDiscussMsg, int auth_code, int64_t discuss_id, const char* msg) FFUN2(auth_code, discuss_id, msg)
FFUN1(int, deleteMsg, int auth_code, int64_t msg_id) FFUN2(auth_code, msg_id)

// Friend Operation
FFUN1(int, sendLike, int auth_code, int64_t qq) FFUN2(auth_code, qq)
FFUN1(int, sendLikeV2, int auth_code, int64_t qq, int times) FFUN2(auth_code, qq, times)

// Group Operation
FFUN1(int, setGroupKick, int auth_code, int64_t group_id, int64_t qq, cq_bool_t reject_add_request) FFUN2(auth_code, group_id, qq, reject_add_request)
FFUN1(int, setGroupBan, int auth_code, int64_t group_id, int64_t qq, int64_t duration) FFUN2(auth_code, group_id, qq, duration)
FFUN1(int, setGroupAnonymousBan, int auth_code, int64_t group_id, const char* anonymous, int64_t duration) FFUN2(auth_code, group_id, anonymous, duration)
FFUN1(int, setGroupWholeBan, int auth_code, int64_t group_id, cq_bool_t enable) FFUN2(auth_code, group_id, enable)
FFUN1(int, setGroupAdmin, int auth_code, int64_t group_id, int64_t qq, cq_bool_t set) FFUN2(auth_code, group_id, qq, set)
FFUN1(int, setGroupAnonymous, int auth_code, int64_t group_id, cq_bool_t enable) FFUN2(auth_code, group_id, enable)
FFUN1(int, setGroupCard, int auth_code, int64_t group_id, int64_t qq, const char* new_card) FFUN2(auth_code, group_id, qq, new_card)
FFUN1(int, setGroupLeave, int auth_code, int64_t group_id, cq_bool_t is_dismiss) FFUN2(auth_code, group_id, is_dismiss)
FFUN1(int, setGroupSpecialTitle, int auth_code, int64_t group_id, int64_t qq, const char* new_special_title, int64_t duration) FFUN2(auth_code, group_id, qq, new_special_title, duration)
FFUN1(int, setDiscussLeave, int auth_code, int64_t discuss_id) FFUN2(auth_code, discuss_id)

// Request
FFUN1(int, setFriendAddRequest, int auth_code, const char* response_flag, int response_operation, const char* remark) FFUN2(auth_code, response_flag, response_operation, remark)
FFUN1(int, setGroupAddRequest, int auth_code, const char* response_flag, int request_type, int response_operation) FFUN2(auth_code, response_flag, request_type, response_operation)
FFUN1(int, setGroupAddRequestV2, int auth_code, const char* response_flag, int request_type, int response_operation, const char* reason) FFUN2(auth_code, response_flag, request_type, response_operation, reason)

// QQ Information
FFUN1(int64_t, getLoginQQ, int auth_code) FFUN2(auth_code)
FFUN1(const char*, getLoginNick, int auth_code) FFUN2(auth_code)
FFUN1(const char*, getStrangerInfo, int auth_code, int64_t qq, cq_bool_t no_cache) FFUN2(auth_code, qq, no_cache)
FFUN1(const char*, getFriendList, int auth_code, cq_bool_t reserved) FFUN2(auth_code, reserved)
FFUN1(const char*, getGroupList, int auth_code) FFUN2(auth_code)
FFUN1(const char*, getGroupInfo, int auth_code, int64_t group_id, cq_bool_t no_cache) FFUN2(auth_code, group_id, no_cache)
FFUN1(const char*, getGroupMemberList, int auth_code, int64_t group_id) FFUN2(auth_code, group_id)
FFUN1(const char*, getGroupMemberInfoV2, int auth_code, int64_t group_id, int64_t qq, cq_bool_t no_cache) FFUN2(auth_code, group_id, qq, no_cache)

// CoolQ
FFUN1(const char*, getCookies, int auth_code) FFUN2(auth_code)
FFUN1(const char*, getCookiesV2, int auth_code, const char* domain) FFUN2(auth_code, domain)
FFUN1(int, getCsrfToken, int auth_code) FFUN2(auth_code)
FFUN1(const char*, getAppDirectory, int auth_code) FFUN2(auth_code)
FFUN1(const char*, getRecord, int auth_code, const char* file, const char* out_format) FFUN2(auth_code, file, out_format)
FFUN1(const char*, getRecordV2, int auth_code, const char* file, const char* out_format) FFUN2(auth_code, file, out_format)
FFUN1(const char*, getImage, int auth_code, const char* file) FFUN2(auth_code, file)
FFUN1(int, canSendImage, int auth_code) FFUN2(auth_code)
FFUN1(int, canSendRecord, int auth_code) FFUN2(auth_code)
FFUN1(int, addLog, int auth_code, int log_level, const char* category, const char* log_msg) FFUN2(auth_code, log_level, category, log_msg)
FFUN1(int, setFatal, int auth_code, const char* error_info) FFUN2(auth_code, error_info)
FFUN1(int, setRestart, int auth_code) FFUN2(auth_code) // currently ineffective
