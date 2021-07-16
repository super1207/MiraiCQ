#ifndef __BOT_H__LGQKADOYHORUBXTX
#define __BOT_H__LGQKADOYHORUBXTX

#include <string>

#include <boost/shared_ptr.hpp>

#include "jsoncpp/json.h"

#define cq_bool_t __int32


class Bot
{
public:
	virtual bool is_connect() = 0;
	virtual bool connect() = 0;
	virtual bool disconnect() = 0;
	virtual Json::Value get_message() = 0;
	virtual Json::Value send(Json::Value & outroot,__int32 milliseconds = 1000) = 0;



	// Message
	Json::Value sendPrivateMsg(  __int64 qq, const char *msg) ;
	Json::Value sendGroupMsg(  __int64 group_id, const char *msg) ;
	Json::Value sendDiscussMsg(  __int64 discuss_id, const char *msg) ;
	Json::Value deleteMsg(  __int64 msg_id) ;

	// Friend Operation
	Json::Value sendLike(  __int64 qq);
	Json::Value sendLikeV2(  __int64 qq, __int32 times);

	// Group Operation
	Json::Value setGroupKick(  __int64 group_id, __int64 qq, cq_bool_t reject_add_request);
	Json::Value setGroupBan(  __int64 group_id, __int64 qq, __int64 duration) ;
	Json::Value setGroupAnonymousBan(  __int64 group_id, const char *anonymous, __int64 duration);
	Json::Value setGroupWholeBan(  __int64 group_id, cq_bool_t enable);
	Json::Value setGroupAdmin(  __int64 group_id, __int64 qq, cq_bool_t set);
	Json::Value setGroupAnonymous(  __int64 group_id, cq_bool_t enable);
	Json::Value setGroupCard(  __int64 group_id, __int64 qq, const char *new_card) ;
	Json::Value setGroupLeave(  __int64 group_id, cq_bool_t is_dismiss) ;
	Json::Value setGroupSpecialTitle(  __int64 group_id, __int64 qq, const char *new_special_title,__int64 duration); 
	Json::Value setDiscussLeave(  __int64 discuss_id) ;

	// Request
	Json::Value setFriendAddRequest(  const char *response_flag, __int32 response_operation,const char *remark) ;
	Json::Value setGroupAddRequest(  const char *response_flag, __int32 request_type,__int32 response_operation) ;
	Json::Value setGroupAddRequestV2(  const char *response_flag, __int32 request_type,__int32 response_operation, const char *reason) ;

	// QQ Information
	Json::Value getLoginQQ( ) ;
	Json::Value getLoginNick( ) ;
	Json::Value getStrangerInfo(  __int64 qq, cq_bool_t no_cache) ;
	Json::Value getFriendList(  cq_bool_t reserved) ;
	Json::Value getGroupList( ) ;
	Json::Value getGroupInfo(  __int64 group_id, cq_bool_t no_cache) ;
	Json::Value getGroupMemberList(  __int64 group_id) ;
	Json::Value getGroupMemberInfoV2(  __int64 group_id, __int64 qq, cq_bool_t no_cache) ;

	// CoolQ
	Json::Value getCookies( ) ;
	Json::Value getCookiesV2(  const char *domain) ;
	Json::Value getCsrfToken( ) ;
	Json::Value getAppDirectory( ) ;
	Json::Value getRecord(  const char *file, const char *out_format) ;
	Json::Value getRecordV2(  const char *file, const char *out_format) ;
	Json::Value getImage(  const char *file) ;
	Json::Value canSendImage( ) ;
	Json::Value canSendRecord( ) ;
	Json::Value addLog(  __int32 log_level, const char *category, const char *log_msg) ;
	Json::Value setFatal(  const char *error_info) ;
	Json::Value setRestart( ) ; // currently ineffective
	


	static Bot * getInstance(const std::string & ws_url = "ws://localhost:6700",const std::string & access_token = "",size_t buffer_size = 1024);
	virtual ~Bot(){};
protected:
	
};

#undef cq_bool_t




#endif