#include "bot.h"

#define cq_bool_t __int32
#define DEFAULT_TIMEOUT 5000

Json::Value Bot::sendPrivateMsg(__int64 qq, const char *msg)
{
	Json::Value outroot;
	outroot["action"] = "send_private_msg";
	outroot["params"]["user_id"] = qq;
	outroot["params"]["message"] = (msg?msg:"");
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::sendGroupMsg(__int64 group_id, const char *msg)
{
	Json::Value outroot;
	outroot["action"] = "send_group_msg";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["message"] = (msg?msg:"");
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::sendDiscussMsg(__int64 discuss_id, const char *msg)
{
	return Json::Value();
}

Json::Value Bot::deleteMsg(__int64 msg_id)
{
	Json::Value outroot;
	outroot["action"] = "delete_msg";
	outroot["params"]["message_id"] = msg_id;
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::sendLike(__int64 qq)
{
	return Json::Value();
}

Json::Value Bot::sendLikeV2(__int64 qq, __int32 times)
{
	return Json::Value();
}

Json::Value Bot::setGroupKick(__int64 group_id, __int64 qq, cq_bool_t reject_add_request)
{
	Json::Value outroot;
	outroot["action"] = "set_group_kick";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["user_id"] = qq;
	outroot["params"]["reject_add_request"] = (reject_add_request?true:false);
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::setGroupBan(__int64 group_id, __int64 qq, __int64 duration)
{
	Json::Value outroot;
	outroot["action"] = "set_group_ban";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["user_id"] = qq;
	outroot["params"]["duration"] = duration;
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::setGroupAnonymousBan(__int64 group_id, const char *anonymous, __int64 duration)
{
	Json::Value outroot;
	outroot["action"] = "set_group_anonymous_ban";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["anonymous"] = (anonymous?anonymous:"");
	outroot["params"]["duration"] = duration;
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::setGroupWholeBan(__int64 group_id, cq_bool_t enable)
{
	Json::Value outroot;
	outroot["action"] = "set_group_whole_ban";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["enable"] = (enable?true:false);
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::setGroupAdmin(__int64 group_id, __int64 qq, cq_bool_t set)
{
	Json::Value outroot;
	outroot["action"] = "set_group_admin";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["user_id"] = qq;
	outroot["params"]["enable"] = (set?true:false);
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::setGroupAnonymous(__int64 group_id, cq_bool_t enable)
{
	Json::Value outroot;
	outroot["action"] = "set_group_anonymous";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["enable"] = (enable?true:false);
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::setGroupCard(__int64 group_id, __int64 qq, const char *new_card)
{
	Json::Value outroot;
	outroot["action"] = "set_group_card";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["user_id"] = qq;
	outroot["params"]["card"] = (new_card?new_card:"");
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::setGroupLeave(__int64 group_id, cq_bool_t is_dismiss)
{
	Json::Value outroot;
	outroot["action"] = "set_group_leave";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["is_dismiss"] = (is_dismiss?true:false);
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::setGroupSpecialTitle(__int64 group_id, __int64 qq, const char *new_special_title,__int64 duration)
{
	Json::Value outroot;
	outroot["action"] = "set_group_special_title";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["user_id"] = qq;
	outroot["params"]["special_title"] = (new_special_title?new_special_title:"");
	outroot["params"]["duration"] = duration;
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::setDiscussLeave(__int64 discuss_id)
{
	return Json::Value();
}

Json::Value Bot::setFriendAddRequest(const char *response_flag, __int32 response_operation,const char *remark)
{
	Json::Value outroot;
	outroot["action"] = "set_friend_add_request";
	outroot["params"]["flag"] = (response_flag?response_flag:"");
	if(response_operation == 1)
	{
		outroot["params"]["approve"] = true;
	}else
	{
		outroot["params"]["approve"] = false;
	}
	outroot["params"]["remark"] = (remark?remark:"");
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::setGroupAddRequest(const char *response_flag, __int32 request_type,__int32 response_operation)
{
	Json::Value outroot;
	outroot["action"] = "set_group_add_request";
	outroot["params"]["flag"] = (response_flag?response_flag:"");
	if(request_type == 1)
	{
		outroot["params"]["sub_type"] = "add";
	}else
	{
		outroot["params"]["sub_type"] = "invite";
	}
	if(response_operation == 1)
	{
		outroot["params"]["approve"] = true;
	}else
	{
		outroot["params"]["approve"] = false;
	}
	outroot["params"]["reason"] = "";
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::setGroupAddRequestV2(const char *response_flag, __int32 request_type,__int32 response_operation, const char *reason)
{
	Json::Value outroot;
	outroot["action"] = "set_group_add_request";
	outroot["params"]["flag"] = (response_flag?response_flag:"");
	if(request_type == 1)
	{
		outroot["params"]["sub_type"] = "add";
	}else
	{
		outroot["params"]["sub_type"] = "invite";
	}
	if(response_operation == 1)
	{
		outroot["params"]["approve"] = true;
	}else
	{
		outroot["params"]["approve"] = false;
	}
	outroot["params"]["reason"] = (reason?reason:"");
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::getLoginQQ()
{
	Json::Value outroot;
	outroot["action"] = "get_login_info";
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::getLoginNick()
{
	Json::Value outroot;
	outroot["action"] = "get_login_info";
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::getStrangerInfo(__int64 qq, cq_bool_t no_cache)
{
	Json::Value outroot;
	outroot["action"] = "get_stranger_info";
	outroot["params"]["user_id"] = qq;
	outroot["params"]["no_cache"] = (no_cache?true:false);
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::getFriendList(cq_bool_t reserved)
{
	Json::Value outroot;
	outroot["action"] = "get_friend_list";
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::getGroupList()
{
	Json::Value outroot;
	outroot["action"] = "get_group_list";
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::getGroupInfo(__int64 group_id, cq_bool_t no_cache)
{
	Json::Value outroot;
	outroot["action"] = "get_group_info";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["no_cache"] = (no_cache?true:false);
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::getGroupMemberList(__int64 group_id)
{
	Json::Value outroot;
	outroot["action"] = "get_group_member_list";
	outroot["params"]["group_id"] = group_id;
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::getGroupMemberInfoV2(__int64 group_id, __int64 qq, cq_bool_t no_cache)
{
	Json::Value outroot;
	outroot["action"] = "get_group_member_info";
	outroot["params"]["group_id"] = group_id;
	outroot["params"]["user_id"] = qq;
	outroot["params"]["no_cache"] = (no_cache?true:false);
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::getCookies()
{
	return Json::Value();
}

Json::Value Bot::getCookiesV2(const char *domain)
{
	return Json::Value();
}

Json::Value Bot::getCsrfToken()
{
	return Json::Value();
}

Json::Value Bot::getAppDirectory()
{
	return Json::Value();
}

Json::Value Bot::getRecord(const char *file, const char *out_format)
{
	Json::Value outroot;
	outroot["action"] = "get_record";
	outroot["params"]["file"] = (file?file:"");
	outroot["params"]["out_format"] = (out_format?out_format:"");
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::getRecordV2(const char *file, const char *out_format)
{
	Json::Value outroot;
	outroot["action"] = "get_record";
	outroot["params"]["file"] = (file?file:"");
	outroot["params"]["out_format"] = (out_format?out_format:"");
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::getImage(const char *file)
{
	Json::Value outroot;
	outroot["action"] = "get_image";
	outroot["params"]["file"] = (file?file:"");
	return send(outroot,DEFAULT_TIMEOUT);
}

Json::Value Bot::canSendImage()
{
	Json::Value outroot;
	outroot["action"] = "can_send_image";
	return send(outroot,DEFAULT_TIMEOUT); 
}

Json::Value Bot::canSendRecord()
{
	Json::Value outroot;
	outroot["action"] = "can_send_record";
	return send(outroot,DEFAULT_TIMEOUT); 
}

Json::Value Bot::addLog(__int32 log_level, const char *category, const char *log_msg)
{
	return Json::Value();
}

Json::Value Bot::setFatal(const char *error_info)
{
	return Json::Value();
}

Json::Value Bot::setRestart()
{
	return Json::Value();
}
