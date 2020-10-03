// CQP.dll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CQP.dll.h"

//#include "pch.h"
//#include "stdio.h"

#define FUNC(ReturnType, FuncName, ...) extern "C"  ReturnType __stdcall CQ_##FuncName(__VA_ARGS__)
#define cq_bool_t __int32

#define STR1(R)  #R
#define STR2(R)  STR1(R)


#define FFUN1(ReturnType, FuncName, ...) FUNC(ReturnType, FuncName, __VA_ARGS__) \
{\
	typedef ReturnType( __stdcall  * fun_ptr_type)(__VA_ARGS__);\
	fun_ptr_type fun_ptr = (fun_ptr_type)GetProcAddress(GetModuleHandle(NULL),"CQ_"STR2(FuncName) "_T");\
	if(!fun_ptr){ReturnType ret = NULL;return ret;}

#define FFUN2(...) return fun_ptr(__VA_ARGS__);}
	


typedef __int32(*getfunType)(const char*);
getfunType getfun = 0;


// Message
FFUN1(__int32, sendPrivateMsg, __int32 auth_code, __int64 qq, const char *msg) FFUN2(   auth_code,  qq, msg)
FFUN1(__int32, sendGroupMsg, __int32 auth_code, __int64 group_id, const char *msg) FFUN2(   auth_code,  group_id, msg)
FFUN1(__int32, sendDiscussMsg, __int32 auth_code, __int64 discuss_id, const char *msg) FFUN2(   auth_code,  discuss_id, msg)
FFUN1(__int32, deleteMsg, __int32 auth_code, __int64 msg_id) FFUN2(   auth_code,  msg_id)

// Friend Operation
FFUN1(__int32, sendLike, __int32 auth_code, __int64 qq) FFUN2(   auth_code,  qq)
FFUN1(__int32, sendLikeV2, __int32 auth_code, __int64 qq, __int32 times) FFUN2(   auth_code,  qq,  times)

// Group Operation
FFUN1(__int32, setGroupKick, __int32 auth_code, __int64 group_id, __int64 qq, cq_bool_t reject_add_request) FFUN2(   auth_code,  group_id,  qq,  reject_add_request)
FFUN1(__int32, setGroupBan, __int32 auth_code, __int64 group_id, __int64 qq, __int64 duration) FFUN2(   auth_code,  group_id,  qq,  duration)
FFUN1(__int32, setGroupAnonymousBan, __int32 auth_code, __int64 group_id, const char *anonymous, __int64 duration) FFUN2(   auth_code,  group_id, anonymous,  duration)
FFUN1(__int32, setGroupWholeBan, __int32 auth_code, __int64 group_id, cq_bool_t enable) FFUN2(   auth_code,  group_id,  enable)
FFUN1(__int32, setGroupAdmin, __int32 auth_code, __int64 group_id, __int64 qq, cq_bool_t set) FFUN2(   auth_code,  group_id,  qq,  set)
FFUN1(__int32, setGroupAnonymous, __int32 auth_code, __int64 group_id, cq_bool_t enable) FFUN2(   auth_code,  group_id,  enable)
FFUN1(__int32, setGroupCard, __int32 auth_code, __int64 group_id, __int64 qq, const char *new_card) FFUN2(   auth_code,  group_id,  qq, new_card)
FFUN1(__int32, setGroupLeave, __int32 auth_code, __int64 group_id, cq_bool_t is_dismiss) FFUN2(   auth_code,  group_id,  is_dismiss)
FFUN1(__int32, setGroupSpecialTitle, __int32 auth_code, __int64 group_id, __int64 qq, const char *new_special_title,__int64 duration) FFUN2(   auth_code,  group_id,  qq, new_special_title,duration)
FFUN1(__int32, setDiscussLeave, __int32 auth_code, __int64 discuss_id) FFUN2(   auth_code,  discuss_id)

// Request
FFUN1(__int32, setFriendAddRequest, __int32 auth_code, const char *response_flag, __int32 response_operation,const char *remark) FFUN2(   auth_code, response_flag,  response_operation,remark)
FFUN1(__int32, setGroupAddRequest, __int32 auth_code, const char *response_flag, __int32 request_type,__int32 response_operation) FFUN2(   auth_code, response_flag,  request_type,response_operation)
FFUN1(__int32, setGroupAddRequestV2, __int32 auth_code, const char *response_flag, __int32 request_type,__int32 response_operation, const char *reason) FFUN2(   auth_code, response_flag,  request_type,response_operation, reason)

// QQ Information
FFUN1(__int64, getLoginQQ, __int32 auth_code) FFUN2(   auth_code)
FFUN1(const char *, getLoginNick, __int32 auth_code) FFUN2(   auth_code)
FFUN1(const char *, getStrangerInfo, __int32 auth_code, __int64 qq, cq_bool_t no_cache) FFUN2(   auth_code,  qq,  no_cache)
FFUN1(const char *, getFriendList, __int32 auth_code, cq_bool_t reserved) FFUN2(   auth_code,  reserved)
FFUN1(const char *, getGroupList, __int32 auth_code) FFUN2(   auth_code)
FFUN1(const char *, getGroupInfo, __int32 auth_code, __int64 group_id, cq_bool_t no_cache) FFUN2(   auth_code,  group_id,  no_cache)
FFUN1(const char *, getGroupMemberList, __int32 auth_code, __int64 group_id) FFUN2(   auth_code,  group_id)
FFUN1(const char *, getGroupMemberInfoV2, __int32 auth_code, __int64 group_id, __int64 qq, cq_bool_t no_cache) FFUN2(   auth_code,  group_id,  qq,  no_cache)

// CoolQ
FFUN1(const char *, getCookies, __int32 auth_code) FFUN2(   auth_code)
FFUN1(const char *, getCookiesV2, __int32 auth_code, const char *domain) FFUN2(   auth_code, domain)
FFUN1(__int32, getCsrfToken, __int32 auth_code) FFUN2(   auth_code)
FFUN1(const char *, getAppDirectory, __int32 auth_code) FFUN2(   auth_code)
FFUN1(const char *, getRecord, __int32 auth_code, const char *file, const char *out_format) FFUN2(   auth_code, file, out_format)
FFUN1(const char *, getRecordV2, __int32 auth_code, const char *file, const char *out_format) FFUN2(   auth_code, file, out_format)
FFUN1(const char *, getImage, __int32 auth_code, const char *file) FFUN2(   auth_code, file)
FFUN1(__int32, canSendImage, __int32 auth_code) FFUN2(   auth_code)
FFUN1(__int32, canSendRecord, __int32 auth_code) FFUN2(   auth_code)
FFUN1(__int32, addLog, __int32 auth_code, __int32 log_level, const char *category, const char *log_msg) FFUN2(   auth_code,  log_level, category, log_msg)
FFUN1(__int32, setFatal, __int32 auth_code, const char *error_info) FFUN2(   auth_code, error_info)
FFUN1(__int32, setRestart, __int32 auth_code) FFUN2(   auth_code) // currently ineffective
