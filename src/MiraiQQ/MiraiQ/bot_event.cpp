#include "bot_event.h"
#include "bot.h"
#include "plus.h"
#include "binpack.h"
#include "base64.h"
#include "msg_id_convert.h"


#include <boost/locale/encoding.hpp>


#define to_gbk(U8) boost::locale::conv::between((U8), "GBK", "UTF-8")
#define to_u8(GBK) boost::locale::conv::between((GBK), "UTF-8", "GBK").c_str()

std::map<std::string,std::map<std::string,__int32(*)(const Json::Value &,boost::shared_ptr<Plus>)> > g_message_map;

#define EVENT_IGNORE 0
#define EVENT_BLOCK 1

#define TEMP_EVENT_FUN(x) static __int32 temp_call_##x##(const std::map<__int32,Plus::PlusDef>::iterator & iter,const Json::Value & root)
#define EVENT_FUN_ID(x) Plus::cq_##x##_id
#define EVENT_FUN_TYPE(x) Plus::cq_##x##_funtype
#define GET_FUNPTR(FUNTYPE) \
	BOOST_AUTO(fun_ptr,((EVENT_FUN_TYPE(FUNTYPE))Plus::get_plus_function(iter->second,EVENT_FUN_ID(FUNTYPE))));\
	if(!fun_ptr){return EVENT_BLOCK;}




TEMP_EVENT_FUN(event_private_message)
{
	GET_FUNPTR(event_private_message)
	std::string msg = to_gbk(root["raw_message"].asString());
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "friend")
	{
		sub_type = 11;
	}else if(subtype == "group")
	{
		sub_type = 2;
	}else
	{
		sub_type = 1;
	}
	MsgIdConvert * msgid_convert = MsgIdConvert::getInstance();
	assert(msgid_convert);
	__int32 ret = fun_ptr(sub_type,msgid_convert->to_cq(root["message_id"].asInt()),root["user_id"].asInt64(),msg.c_str(),root["font"].asInt());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK; //
}

TEMP_EVENT_FUN(event_group_message)
{
	GET_FUNPTR(event_group_message)
	std::string from_anonymous_base64;

	Json::Value anonymous = root["anonymous"];
	if(anonymous.isObject())
	{
		__int64 id = anonymous["id"].asInt64();
		std::string name = to_gbk(anonymous["name"].asString());
		std::string flag = anonymous["flag"].asString();
		BinPack bin_pack;
		bin_pack.int64_push(id);
		bin_pack.string_push(name);
		bin_pack.token_push(std::vector<char>(flag.begin(),flag.end()));
		from_anonymous_base64 = base64_encode((const unsigned char *)(&(bin_pack.content[0])),bin_pack.content.size());

	}

	std::string msg = to_gbk(root["raw_message"].asString());

	MsgIdConvert * msgid_convert = MsgIdConvert::getInstance();
	assert(msgid_convert);
	__int32 ret = fun_ptr(1,msgid_convert->to_cq(root["message_id"].asInt()),root["group_id"].asInt64(),root["user_id"].asInt64(),from_anonymous_base64.c_str(),msg.c_str(),root["font"].asInt());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

/* Ä¿Ç°Ã»ÓÐdiscussÁË */
TEMP_EVENT_FUN(event_discuss_message)
{
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_upload)
{
	GET_FUNPTR(event_group_upload)

	std::string file_base64;

	Json::Value file = root["file"];

	if(file.isObject())
	{
		std::string id = file["id"].asString();
		std::string name = to_gbk(file["name"].asString());
		__int64 size = file["size"].asInt64();
		__int64 busid = file["busid"].asInt64();
		BinPack bin_pack;
		bin_pack.string_push(id);
		bin_pack.string_push(name);
		bin_pack.int64_push(size);
		bin_pack.int64_push(busid);
		file_base64 = base64_encode((const unsigned char *)(&(bin_pack.content[0])),bin_pack.content.size());

	}
	__int32 ret = fun_ptr(1, root["time"].asInt(),root["group_id"].asInt64(),root["user_id"].asInt64(),file_base64.c_str());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_admin)
{
	GET_FUNPTR(event_group_admin)
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "set")
	{
		sub_type = 2;
	}else
	{
		sub_type = 1;
	}
	__int32 ret = fun_ptr(sub_type,root["time"].asInt(),root["group_id"].asInt64(),root["user_id"].asInt64());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_member_decrease)
{
	GET_FUNPTR(event_group_member_decrease)
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "leave")
	{
		sub_type = 1;
	}else if(subtype == "kick")
	{
		sub_type = 2;
	}else
	{
		sub_type = 3; //kick self
	}
	__int32 ret = fun_ptr(sub_type,root["time"].asInt(),root["group_id"].asInt64(),root["user_id"].asInt64(),root["operator_id"].asInt64());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_member_increase)
{
	GET_FUNPTR(event_group_member_increase)
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "approve")
	{
		sub_type = 1;
	}else
	{
		sub_type = 2; //invite
	}
	__int32 ret = fun_ptr(sub_type,root["time"].asInt(),root["group_id"].asInt64(),root["operator_id"].asInt64(),root["user_id"].asInt64());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_ban)
{
	GET_FUNPTR(event_group_ban)
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "lift_ban")
	{
		sub_type = 1;
	}else
	{
		sub_type = 2; //ban
	}
	__int32 ret = fun_ptr(sub_type,root["time"].asInt(),root["group_id"].asInt64(),root["user_id"].asInt64(),root["operator_id"].asInt64(),root["duration"].asInt64());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_friend_add)
{
	GET_FUNPTR(event_friend_add)
	__int32 ret = fun_ptr(1,root["time"].asInt(),root["user_id"].asInt64());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_recall)
{
	GET_FUNPTR(event_group_recall)
	MsgIdConvert * msgid_convert = MsgIdConvert::getInstance();
	assert(msgid_convert);
	__int32 ret = fun_ptr(root["group_id"].asInt64(),root["user_id"].asInt64(),root["operator_id"].asInt64(),msgid_convert->get_cq(root["message_id"].asInt()));
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_friend_recall)
{
	GET_FUNPTR(event_friend_recall)
	MsgIdConvert * msgid_convert = MsgIdConvert::getInstance();
	assert(msgid_convert);
	__int32 ret = fun_ptr(root["user_id"].asInt64(),msgid_convert->get_cq(root["message_id"].asInt()));
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_notify)
{
	if(root["sub_type"] == "poke")
	{
		GET_FUNPTR(event_notify_pock)
		__int32 ret = fun_ptr(root["group_id"].asInt64(),root["user_id"].asInt64(),root["target_id"].asInt64());
		if(ret == 0)
		{
			return EVENT_IGNORE;
		}
		return EVENT_BLOCK;
	}else if(root["sub_type"] == "lucky_king")
	{
		GET_FUNPTR(event_notify_lucky_king)
		__int32 ret = fun_ptr(root["group_id"].asInt64(),root["user_id"].asInt64(),root["target_id"].asInt64());
		if(ret == 0)
		{
			return EVENT_IGNORE;
		}
		return EVENT_BLOCK;
	}else if(root["sub_type"] == "honor")
	{
		
		GET_FUNPTR(event_notify_honor)
			__int32 ret = fun_ptr(root["group_id"].asInt64(),root["honor_type"].asCString(),root["user_id"].asInt64());
		if(ret == 0)
		{
			return EVENT_IGNORE;
		}
		return EVENT_BLOCK;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_friend_request)
{
	GET_FUNPTR(event_friend_request)
	std::string msg = boost::locale::conv::between(root["comment"].asString(), "GBK", "UTF-8");
	std::string response_flag = root["flag"].asString();
	__int32 ret = fun_ptr(1,root["time"].asInt(),root["user_id"].asInt64(),msg.c_str(),response_flag.c_str());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_request)
{
	GET_FUNPTR(event_group_request)
	std::string msg = boost::locale::conv::between(root["comment"].asString(), "GBK", "UTF-8");
	std::string response_flag = root["flag"].asString();
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "add")
	{
		sub_type = 1;
	}else
	{
		sub_type = 2; //invite
	}
	__int32 ret = fun_ptr(sub_type,root["time"].asInt(),root["group_id"].asInt64(),root["user_id"].asInt64(),msg.c_str(),response_flag.c_str());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_all)
{
	GET_FUNPTR(event_all)
	std::string outstr = Json::FastWriter().write(root);
	__int32 ret = fun_ptr(to_gbk(outstr).c_str());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}





#define CALL_FUN_EVENT(X,M1,M2)  __int32 call_##X (const Json::Value & root,boost::shared_ptr<Plus> plus) \
{ \
	std::map<__int32,Plus::PlusDef> plus_map = plus->get_plus_map();\
	std::map<__int32,Plus::PlusDef>::iterator iter;\
	for(iter = plus_map.begin();iter != plus_map.end();++iter)\
	{\
		if(iter->second.is_enable == false)\
		{\
			continue;\
		}\
		__int32 ret;try{ret = temp_call_##X(iter,root);}\
		catch(const std::exception & e){\
		BOOST_LOG_TRIVIAL(info) <<"crashed on call fun temp_call_" << #X << ":" << e.what();}\
	}\
	return 0;\
}


//CALL_FUN_EVENT(event_enable,_,_)
//CALL_FUN_EVENT(event_disable,_,_)
//CALL_FUN_EVENT(event_coolq_start,_,_)
//CALL_FUN_EVENT(event_coolq_exit,_,_)
CALL_FUN_EVENT(event_private_message,_,_)
CALL_FUN_EVENT(event_group_message,message,group)
CALL_FUN_EVENT(event_discuss_message,_,_)
CALL_FUN_EVENT(event_group_upload,_,_)
CALL_FUN_EVENT(event_group_admin,_,_)
CALL_FUN_EVENT(event_group_member_decrease,_,_)
CALL_FUN_EVENT(event_group_member_increase,_,_)
CALL_FUN_EVENT(event_group_ban,_,_)
CALL_FUN_EVENT(event_friend_add,_,_)
CALL_FUN_EVENT(event_group_recall,_,_)
CALL_FUN_EVENT(event_friend_recall,_,_)
CALL_FUN_EVENT(event_notify,_,_)
CALL_FUN_EVENT(event_friend_request,_,_)
CALL_FUN_EVENT(event_group_request,_,_)
CALL_FUN_EVENT(event_all,_,_)

#define CALL_FUNNAME(X)  call_##X

void init_event_map()
{
	//g_message_map["_" ]["_"] = CALL_FUNNAME(event_enable);
	//g_message_map["_"]["_"] = CALL_FUNNAME(event_disable);
	//g_message_map["_"]["_"] = CALL_FUNNAME(event_coolq_start);
	//g_message_map["_"]["_"] = CALL_FUNNAME(event_coolq_exit);
	g_message_map["message"]["private"] = CALL_FUNNAME(event_private_message);
	g_message_map["message"]["group"] = CALL_FUNNAME(event_group_message);
	g_message_map["message"]["_"] = CALL_FUNNAME(event_discuss_message);
	g_message_map["notice"]["group_upload"] = CALL_FUNNAME(event_group_upload);
	g_message_map["notice"]["group_admin"] = CALL_FUNNAME(event_group_admin);
	g_message_map["notice"]["group_decrease"] = CALL_FUNNAME(event_group_member_decrease);
	g_message_map["notice"]["group_increase"] = CALL_FUNNAME(event_group_member_increase);
	g_message_map["notice"]["group_ban"] = CALL_FUNNAME(event_group_ban);
	g_message_map["notice"]["friend_add"] = CALL_FUNNAME(event_friend_add);
	g_message_map["notice"]["group_recall"] = CALL_FUNNAME(event_group_recall);
	g_message_map["notice"]["friend_recall"] = CALL_FUNNAME(event_friend_recall);
	g_message_map["notice"]["notify"] = CALL_FUNNAME(event_notify);
	g_message_map["request"]["friend"] = CALL_FUNNAME(event_friend_request);
	g_message_map["request"]["group"] = CALL_FUNNAME(event_group_request);
	g_message_map["all_event"][""] = CALL_FUNNAME(event_all);

};



