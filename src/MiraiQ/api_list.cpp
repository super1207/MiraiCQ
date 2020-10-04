#include <string>
#include <vector>

#include <boost/log/trivial.hpp>
#include <boost/thread/thread.hpp>
#include <boost/locale/encoding.hpp>
#include <boost/shared_ptr.hpp>


#include "bot.h"
#include "plus.h"
#include "bot_event.h"
#include "autotime_str.h"
#include "binpack.h"
#include "base64.h"
#include "MIraiQ.h"
#include "msg_id_convert.h"


static autotime_str g_autotime_str;

#define to_gbk(U8) boost::locale::conv::between((U8), "GBK", "UTF-8")
#define to_u8(GBK) boost::locale::conv::between((GBK), "UTF-8", "GBK").c_str()



#define CHECK_AC(AC,X) std::pair<bool,Plus::PlusDef> X = MiraiQ::get_plus_ptr()->get_plusdef(AC);\
	if(X##.first == false) {return AC_NOT_EXIST;} \
	if(X##.second.is_enable == false){return APP_NOT_ENABLE;}

#define CHECK_PTRAC(AC,X) std::pair<bool,Plus::PlusDef> X =MiraiQ::get_plus_ptr()->get_plusdef(AC);\
	if(X##.first == false) {return NULL;} \
	if(X##.second.is_enable == false){return NULL;}


#define RET_STR(X) char * retstr__t = new char[X##.length() + 1];\
	memcpy_s(retstr__t,X##.length() + 1,X##.c_str(),X##.length() + 1);\
	g_autotime_str.insert(retstr__t);\
	return retstr__t;

#define CHECK_RET(RET,RETCODE) if(RET##.isNull())return NET_ERROR;  __int32 RETCODE = RET##["retcode"].asInt();if(RETCODE != 0){return NET_ERROR;}
#define CHECK_PTRRET(RET,RETCODE) if(RET##.isNull())return NULL; __int32 RETCODE = RET##["retcode"].asInt();if(RETCODE != 0){return NULL;}

#define cq_bool_t __int32
#define FFUN1(RETTYPE,FUNNAME,...) static RETTYPE  CQ_##FUNNAME##_(__VA_ARGS__)
//#define FUNC(RETTYPE,FUNNAME,...) RETTYPE __stdcall CQ_##FUNNAME##_T(__VA_ARGS__) \
//{\
//	return CQ_##FUNNAME##_(__VA_ARGS__);\
//}



template <class T>
class TP10086
{
};

static const char * RETERR(const TP10086<const char *> & v)
{
	return NULL;
}
static __int32 RETERR(const TP10086<__int32> & v)
{
	return -100;
}
static __int64 RETERR(const TP10086<__int64> & v)
{
	return -100;
}



#define FFUN2(ReturnType, FuncName, ...) ReturnType __stdcall CQ_##FuncName##_T(__VA_ARGS__) \
{\
	BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << " has being called"; \
	const TP10086<ReturnType> er;\
	typedef ReturnType(  * fun_ptr_type)(__VA_ARGS__);\
	fun_ptr_type fun_ptr = (fun_ptr_type)CQ_##FuncName##_;

#define FFUN3(...) try{ return fun_ptr(__VA_ARGS__);}catch(const std::exception & e)\
{BOOST_LOG_TRIVIAL(info) << "crashed in " << __FUNCTION__ << ":" <<e.what();return RETERR(er);}}


enum{
	AC_NOT_EXIST = -1,
	APP_NOT_ENABLE = -2,
	FUN_NOT_EXIST = -3,
	NET_ERROR = -4
};

// Message
FFUN1(__int32, sendPrivateMsg, __int32 auth_code, __int64 qq, const char *msg)
{
	CHECK_AC(auth_code,pdf)
		std::string  gbkmsg = to_u8(msg?msg:"");
	Json::Value ret_json =  MiraiQ::get_bot_ptr()->sendPrivateMsg(qq,gbkmsg.c_str());
	CHECK_RET(ret_json,retcode)
		return ret_json["data"]["message_id"].asInt();
}
FFUN1(__int32, sendGroupMsg, __int32 auth_code, __int64 group_id, const char *msg)
{
	CHECK_AC(auth_code,pdf)
		std::string  gbkmsg = to_u8((msg?msg:""));
	Json::Value ret_json =  MiraiQ::get_bot_ptr()->sendGroupMsg(group_id,gbkmsg.c_str());
	CHECK_RET(ret_json,retcode)
		return ret_json["data"]["message_id"].asInt();
}
FFUN1(__int32, sendDiscussMsg, __int32 auth_code, __int64 discuss_id, const char *msg)
{
	return FUN_NOT_EXIST;
}
FFUN1(__int32, deleteMsg, __int32 auth_code, __int64 msg_id) 
{
	CHECK_AC(auth_code,pdf)
		MsgIdConvert * msg_id_convert = MsgIdConvert::getInstance();
	assert(msg_id_convert);
	Json::Value ret_json =  MiraiQ::get_bot_ptr()->deleteMsg(msg_id_convert->to_web((__int32)msg_id));
	CHECK_RET(ret_json,retcode)
		return 0;
}

// Friend Operation
FFUN1(__int32, sendLike, __int32 auth_code, __int64 qq)
{
	return FUN_NOT_EXIST;
}
FFUN1(__int32, sendLikeV2, __int32 auth_code, __int64 qq, __int32 times) 
{
	return FUN_NOT_EXIST;
}

// Group Operation
FFUN1(__int32, setGroupKick, __int32 auth_code, __int64 group_id, __int64 qq, cq_bool_t reject_add_request)
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->setGroupKick(group_id,qq,reject_add_request);
	CHECK_RET(ret_json,retcode)
		return 0;
}
FFUN1(__int32, setGroupBan, __int32 auth_code, __int64 group_id, __int64 qq, __int64 duration)
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->setGroupBan(group_id,qq,duration);
	CHECK_RET(ret_json,retcode)
		return 0;
}
FFUN1(__int32, setGroupAnonymousBan, __int32 auth_code, __int64 group_id, const char *anonymous, __int64 duration)
{

	//TODO...
	//CHECK_AC(auth_code,pdf)
	//Json::Value ret_json = MiraiQ::get_bot_ptr()->setGroupAnonymousBan(group_id,(anonymous?anonymous:""),duration);
	//CHECK_RET(ret_json,retcode)
	return FUN_NOT_EXIST;
}
FFUN1(__int32, setGroupWholeBan, __int32 auth_code, __int64 group_id, cq_bool_t enable) 
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->setGroupWholeBan(group_id,enable);
	CHECK_RET(ret_json,retcode)
		return 0;
}
FFUN1(__int32, setGroupAdmin, __int32 auth_code, __int64 group_id, __int64 qq, cq_bool_t set)
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->setGroupAdmin(group_id,qq,set);
	CHECK_RET(ret_json,retcode)
		return 0;
}
FFUN1(__int32, setGroupAnonymous, __int32 auth_code, __int64 group_id, cq_bool_t enable) 
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->setGroupAnonymous(group_id,enable);
	CHECK_RET(ret_json,retcode)
		return 0;
}
FFUN1(__int32, setGroupCard, __int32 auth_code, __int64 group_id, __int64 qq, const char *new_card)
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->setGroupCard(group_id,qq,to_u8(new_card?new_card:""));
	CHECK_RET(ret_json,retcode)
		return 0;
}
FFUN1(__int32, setGroupLeave, __int32 auth_code, __int64 group_id, cq_bool_t is_dismiss) 
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->setGroupLeave(group_id,is_dismiss);
	CHECK_RET(ret_json,retcode)
		return 0;
}
FFUN1(__int32, setGroupSpecialTitle, __int32 auth_code, __int64 group_id, __int64 qq, const char *new_special_title,__int64 duration) 
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->setGroupSpecialTitle(group_id,qq,to_u8(new_special_title?new_special_title:""),duration);
	CHECK_RET(ret_json,retcode)
		return 0;
}
FFUN1(__int32, setDiscussLeave, __int32 auth_code, __int64 discuss_id)
{
	return FUN_NOT_EXIST;
}

// Request
FFUN1(__int32, setFriendAddRequest, __int32 auth_code, const char *response_flag, __int32 response_operation,const char *remark)
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->setFriendAddRequest(response_flag,response_operation,to_u8(remark?remark:""));
	CHECK_RET(ret_json,retcode)
		return 0;
}
FFUN1(__int32, setGroupAddRequest, __int32 auth_code, const char *response_flag, __int32 request_type,__int32 response_operation) 
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->setGroupAddRequest(response_flag,request_type,response_operation);
	CHECK_RET(ret_json,retcode)
		return 0;
}
FFUN1(__int32, setGroupAddRequestV2, __int32 auth_code, const char *response_flag, __int32 request_type,__int32 response_operation, const char *reason)
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->setGroupAddRequestV2(response_flag,request_type,response_operation,to_u8(reason?reason:""));
	CHECK_RET(ret_json,retcode)
		return 0;
}

// QQ Information
FFUN1(__int64, getLoginQQ, __int32 auth_code)
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->getLoginQQ();
	CHECK_RET(ret_json,retcode)
		return ret_json["data"]["user_id"].asInt64();
}
FFUN1(const char *, getLoginNick, __int32 auth_code)
{
	CHECK_PTRAC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->getLoginNick();
	CHECK_PTRRET(ret_json,retcode)
		std::string login_nick = to_gbk(ret_json["data"]["nickname"].asString());
	RET_STR(login_nick)
}
FFUN1(const char *, getStrangerInfo, __int32 auth_code, __int64 qq, cq_bool_t no_cache)
{
	CHECK_PTRAC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->getStrangerInfo(qq,no_cache);
	CHECK_PTRRET(ret_json,retcode)
		__int64 user_id = ret_json["data"]["user_id"].asInt64();
	std::string nickname = ret_json["data"]["nickname"].asString();
	__int32 sex;
	{
		std::string sex_str = ret_json["data"]["sex"].asString();
		if(sex_str == "male")
		{
			sex = 0;
		}else if(sex_str == "female")
		{
			sex = 1;
		}else
		{
			sex = 255;
		}
	}
	__int32 age = ret_json["data"]["age"].asInt();
	BinPack bin_pack;
	bin_pack.int64_push(user_id);
	bin_pack.string_push(to_gbk(nickname));
	bin_pack.int32_push(sex);
	bin_pack.int32_push(age);
	std::string ret_str = base64_encode((const unsigned char *)(&(bin_pack.content[0])),bin_pack.content.size());
	RET_STR(ret_str)
}
FFUN1(const char *, getFriendList, __int32 auth_code, cq_bool_t reserved)
{
	CHECK_PTRAC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->getFriendList(reserved);
	CHECK_PTRRET(ret_json,retcode)
		Json::Value json_arr = ret_json["data"];
	BinPack bin_pack;
	bin_pack.int32_push(json_arr.size());
	for(size_t i = 0; i < json_arr.size();++i)
	{
		BinPack bin_pack_child;
		bin_pack_child.int64_push(json_arr[i]["user_id"].asInt64());
		bin_pack_child.string_push(to_gbk(json_arr[i]["nickname"].asString()));
		bin_pack_child.string_push(to_gbk(json_arr[i]["remark"].asString()));
		bin_pack.token_push(bin_pack_child.content);	
	}
	std::string ret_str = base64_encode((const unsigned char *)(&(bin_pack.content[0])),bin_pack.content.size());
	RET_STR(ret_str)
}
FFUN1(const char *, getGroupList, __int32 auth_code)
{
	CHECK_PTRAC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->getGroupList();
	CHECK_PTRRET(ret_json,retcode)
		Json::Value json_arr = ret_json["data"];
	BinPack bin_pack;
	bin_pack.int32_push(json_arr.size());
	for(size_t i = 0; i < json_arr.size();++i)
	{
		BinPack bin_pack_child;
		bin_pack_child.int64_push(json_arr[i]["group_id"].asInt64());
		bin_pack_child.string_push(to_gbk(json_arr[i]["group_name"].asString()));
		//bin_pack_child.int32_push(json_arr[i]["member_count"].asInt());
		//bin_pack_child.int32_push(json_arr[i]["max_member_count"].asInt());
		bin_pack.token_push(bin_pack_child.content);	
	}
	std::string ret_str = base64_encode((const unsigned char *)(&(bin_pack.content[0])),bin_pack.content.size());
	RET_STR(ret_str)
}
FFUN1(const char *, getGroupInfo, __int32 auth_code, __int64 group_id, cq_bool_t no_cache) 
{
	CHECK_PTRAC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->getGroupInfo(group_id,no_cache);
	CHECK_PTRRET(ret_json,retcode)
		BinPack bin_pack;
	bin_pack.int64_push(ret_json["data"]["group_id"].asInt64());
	bin_pack.string_push(to_gbk(ret_json["data"]["group_name"].asString()));
	bin_pack.int32_push(ret_json["data"]["member_count"].asInt());
	bin_pack.int32_push(ret_json["data"]["max_member_count"].asInt());
	std::string ret_str = base64_encode((const unsigned char *)(&(bin_pack.content[0])),bin_pack.content.size());
	RET_STR(ret_str)
}
FFUN1(const char *, getGroupMemberList, __int32 auth_code, __int64 group_id) 
{
	CHECK_PTRAC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->getGroupMemberList(group_id);
	CHECK_PTRRET(ret_json,retcode)
		Json::Value json_arr = ret_json["data"];
	BinPack bin_pack;
	bin_pack.int32_push(json_arr.size());
	for(size_t i = 0; i < json_arr.size();++i)
	{
		BinPack bin_pack_child;

		bin_pack_child.int64_push(json_arr[i]["group_id"].asInt64());
		bin_pack_child.int64_push(json_arr[i]["user_id"].asInt64());

		bin_pack_child.string_push(to_gbk(json_arr[i]["nickname"].asString()));
		bin_pack_child.string_push(to_gbk(json_arr[i]["card"].asString()));


		{
			std::string sex = json_arr[i]["sex"].asString();
			if(sex == "male")
			{
				bin_pack_child.int32_push(0);
			}else if(sex == "female")
			{
				bin_pack_child.int32_push(1);
			}else
			{
				bin_pack_child.int32_push(255);
			}
		}

		bin_pack_child.int32_push(json_arr[i]["age"].asInt());

		bin_pack_child.string_push(to_gbk(json_arr[i]["area"].asString()));

		bin_pack_child.int32_push(json_arr[i]["join_time"].asInt());
		bin_pack_child.int32_push(json_arr[i]["last_sent_time"].asInt());

		bin_pack_child.string_push(to_gbk(json_arr[i]["level"].asString()));


		{
			std::string role = json_arr[i]["role"].asString();
			if(role == "member")
			{
				bin_pack_child.int32_push(1);
			}else if(role == "admin")
			{
				bin_pack_child.int32_push(2);
			}else
			{
				bin_pack_child.int32_push(3);
			}
		}


		bin_pack_child.bool_push(json_arr[i]["unfriendly"].asBool());

		bin_pack_child.string_push(to_gbk(json_arr[i]["title"].asString()));

		bin_pack_child.int32_push(json_arr[i]["title_expire_time"].asInt());
		bin_pack_child.bool_push(json_arr[i]["card_changeable"].asBool());

		bin_pack.token_push(bin_pack_child.content);	
	}

	std::string ret_str = base64_encode((const unsigned char *)(&(bin_pack.content[0])),bin_pack.content.size());
	RET_STR(ret_str)
}
FFUN1(const char *, getGroupMemberInfoV2, __int32 auth_code, __int64 group_id, __int64 qq, cq_bool_t no_cache) 
{
	CHECK_PTRAC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->getGroupMemberInfoV2(group_id,qq,no_cache);
	CHECK_PTRRET(ret_json,retcode)
		BinPack bin_pack;
	bin_pack.int64_push(ret_json["data"]["group_id"].asInt64());
	bin_pack.int64_push(ret_json["data"]["user_id"].asInt64());

	bin_pack.string_push(to_gbk(ret_json["data"]["nickname"].asString()));
	bin_pack.string_push(to_gbk(ret_json["data"]["card"].asString()));


	{
		std::string sex = ret_json["data"]["sex"].asString();
		if(sex == "male")
		{
			bin_pack.int32_push(0);
		}else if(sex == "female")
		{
			bin_pack.int32_push(1);
		}else
		{
			bin_pack.int32_push(255);
		}
	}

	bin_pack.int32_push(ret_json["data"]["age"].asInt());

	bin_pack.string_push(to_gbk(ret_json["data"]["area"].asString()));

	bin_pack.int32_push(ret_json["data"]["join_time"].asInt());
	bin_pack.int32_push(ret_json["data"]["last_sent_time"].asInt());

	bin_pack.string_push(to_gbk(ret_json["data"]["level"].asString()));


	{
		std::string role = ret_json["data"]["role"].asString();
		if(role == "member")
		{
			bin_pack.int32_push(1);
		}else if(role == "admin")
		{
			bin_pack.int32_push(2);
		}else
		{
			bin_pack.int32_push(3);
		}
	}


	bin_pack.bool_push(ret_json["data"]["unfriendly"].asBool());

	bin_pack.string_push(to_gbk(ret_json["data"]["title"].asString()));

	bin_pack.int32_push(ret_json["data"]["title_expire_time"].asInt());
	bin_pack.bool_push(ret_json["data"]["card_changeable"].asBool());
	std::string ret_str = base64_encode((const unsigned char *)(&(bin_pack.content[0])),bin_pack.content.size());
	RET_STR(ret_str)
}

// CoolQ
FFUN1(const char *, getCookies, __int32 auth_code)
{
	return NULL;
}
FFUN1(const char *, getCookiesV2, __int32 auth_code, const char *domain)
{
	return NULL;
}
FFUN1(__int32, getCsrfToken, __int32 auth_code)
{
	return FUN_NOT_EXIST;
}
FFUN1(const char *, getAppDirectory, __int32 auth_code)
{
	CHECK_PTRAC(auth_code,pdf)
		boost::filesystem::path plus_path = pdf.second.plus_path / pdf.second.appid;
	if (!( boost::filesystem::exists(plus_path) && 
		boost::filesystem::is_directory(plus_path)))
	{
		if(!boost::filesystem::create_directory(plus_path))
		{
			return NULL;
		}
	}
	std::string path_str = plus_path.string();
	if(path_str == "")
	{
		return NULL;
	}
	if(path_str[path_str.length() -1] != '\\')
	{
		path_str += "\\";
	}
	RET_STR(path_str)
}
FFUN1(const char *, getRecord, __int32 auth_code, const char *file, const char *out_format)
{
	CHECK_PTRAC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->getRecord(file,out_format);
	CHECK_PTRRET(ret_json,retcode)
		std::string ret_str = ret_json["data"]["file"].asString();
	RET_STR(ret_str)
}
FFUN1(const char *, getRecordV2, __int32 auth_code, const char *file, const char *out_format)
{
	CHECK_PTRAC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->getRecordV2(file,out_format);
	CHECK_PTRRET(ret_json,retcode)
		std::string ret_str = ret_json["data"]["file"].asString();
	RET_STR(ret_str)
}
FFUN1(const char *, getImage, __int32 auth_code, const char *file)
{
	CHECK_PTRAC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->getImage(file);
	CHECK_PTRRET(ret_json,retcode)
		std::string ret_str = ret_json["data"]["file"].asString();
	RET_STR(ret_str)
}
FFUN1(__int32, canSendImage, __int32 auth_code)
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->canSendImage();
	CHECK_RET(ret_json,retcode)
		return ret_json["data"]["yes"].asBool();
}
FFUN1(__int32, canSendRecord, __int32 auth_code)
{
	CHECK_AC(auth_code,pdf)
		Json::Value ret_json = MiraiQ::get_bot_ptr()->canSendRecord();
	CHECK_RET(ret_json,retcode)
		return ret_json["data"]["yes"].asBool();
}
FFUN1(__int32, addLog, __int32 auth_code, __int32 log_level, const char *category, const char *log_msg) 
{
	CHECK_AC(auth_code,pdf)
		BOOST_LOG_TRIVIAL(debug) << "[" <<pdf.second.name << "] " << category << " " << log_msg;
	return 0;
}
FFUN1(__int32, setFatal, __int32 auth_code, const char *error_info) 
{
	CHECK_AC(auth_code,pdf)
		BOOST_LOG_TRIVIAL(debug) << "[" <<pdf.second.name << "] " << error_info;
	return 0;
}
FFUN1(__int32, setRestart, __int32 auth_code) 
{
	//TODO...
	return FUN_NOT_EXIST;
}

extern "C"
{


	// Message
	FFUN2(__int32, sendPrivateMsg, __int32 auth_code, __int64 qq, const char *msg) FFUN3(   auth_code,  qq, msg)
		FFUN2(__int32, sendGroupMsg, __int32 auth_code, __int64 group_id, const char *msg) FFUN3(   auth_code,  group_id, msg)
		FFUN2(__int32, sendDiscussMsg, __int32 auth_code, __int64 discuss_id, const char *msg) FFUN3(   auth_code,  discuss_id, msg)
		FFUN2(__int32, deleteMsg, __int32 auth_code, __int64 msg_id) FFUN3(   auth_code,  msg_id)

		// Friend Operation
		FFUN2(__int32, sendLike, __int32 auth_code, __int64 qq) FFUN3(   auth_code,  qq)
		FFUN2(__int32, sendLikeV2, __int32 auth_code, __int64 qq, __int32 times) FFUN3(   auth_code,  qq,  times)

		// Group Operation
		FFUN2(__int32, setGroupKick, __int32 auth_code, __int64 group_id, __int64 qq, cq_bool_t reject_add_request) FFUN3(   auth_code,  group_id,  qq,  reject_add_request)
		FFUN2(__int32, setGroupBan, __int32 auth_code, __int64 group_id, __int64 qq, __int64 duration) FFUN3(   auth_code,  group_id,  qq,  duration)
		FFUN2(__int32, setGroupAnonymousBan, __int32 auth_code, __int64 group_id, const char *anonymous, __int64 duration) FFUN3(   auth_code,  group_id, anonymous,  duration)
		FFUN2(__int32, setGroupWholeBan, __int32 auth_code, __int64 group_id, cq_bool_t enable) FFUN3(   auth_code,  group_id,  enable)
		FFUN2(__int32, setGroupAdmin, __int32 auth_code, __int64 group_id, __int64 qq, cq_bool_t set) FFUN3(   auth_code,  group_id,  qq,  set)
		FFUN2(__int32, setGroupAnonymous, __int32 auth_code, __int64 group_id, cq_bool_t enable) FFUN3(   auth_code,  group_id,  enable)
		FFUN2(__int32, setGroupCard, __int32 auth_code, __int64 group_id, __int64 qq, const char *new_card) FFUN3(   auth_code,  group_id,  qq, new_card)
		FFUN2(__int32, setGroupLeave, __int32 auth_code, __int64 group_id, cq_bool_t is_dismiss) FFUN3(   auth_code,  group_id,  is_dismiss)
		FFUN2(__int32, setGroupSpecialTitle, __int32 auth_code, __int64 group_id, __int64 qq, const char *new_special_title,__int64 duration) FFUN3(   auth_code,  group_id,  qq, new_special_title,duration)
		FFUN2(__int32, setDiscussLeave, __int32 auth_code, __int64 discuss_id) FFUN3(   auth_code,  discuss_id)

		// Request
		FFUN2(__int32, setFriendAddRequest, __int32 auth_code, const char *response_flag, __int32 response_operation,const char *remark) FFUN3(   auth_code, response_flag,  response_operation,remark)
		FFUN2(__int32, setGroupAddRequest, __int32 auth_code, const char *response_flag, __int32 request_type,__int32 response_operation) FFUN3(   auth_code, response_flag,  request_type,response_operation)
		FFUN2(__int32, setGroupAddRequestV2, __int32 auth_code, const char *response_flag, __int32 request_type,__int32 response_operation, const char *reason) FFUN3(   auth_code, response_flag,  request_type,response_operation, reason)

		// QQ Information
		FFUN2(__int64, getLoginQQ, __int32 auth_code) FFUN3(   auth_code)
		FFUN2(const char *, getLoginNick, __int32 auth_code) FFUN3(   auth_code)
		FFUN2(const char *, getStrangerInfo, __int32 auth_code, __int64 qq, cq_bool_t no_cache) FFUN3(   auth_code,  qq,  no_cache)
		FFUN2(const char *, getFriendList, __int32 auth_code, cq_bool_t reserved) FFUN3(   auth_code,  reserved)
		FFUN2(const char *, getGroupList, __int32 auth_code) FFUN3(   auth_code)
		FFUN2(const char *, getGroupInfo, __int32 auth_code, __int64 group_id, cq_bool_t no_cache) FFUN3(   auth_code,  group_id,  no_cache)
		FFUN2(const char *, getGroupMemberList, __int32 auth_code, __int64 group_id) FFUN3(   auth_code,  group_id)
		FFUN2(const char *, getGroupMemberInfoV2, __int32 auth_code, __int64 group_id, __int64 qq, cq_bool_t no_cache) FFUN3(   auth_code,  group_id,  qq,  no_cache)

		// CoolQ
		FFUN2(const char *, getCookies, __int32 auth_code) FFUN3(   auth_code)
		FFUN2(const char *, getCookiesV2, __int32 auth_code, const char *domain) FFUN3(   auth_code, domain)
		FFUN2(__int32, getCsrfToken, __int32 auth_code) FFUN3(   auth_code)
		FFUN2(const char *, getAppDirectory, __int32 auth_code) FFUN3(   auth_code)
		FFUN2(const char *, getRecord, __int32 auth_code, const char *file, const char *out_format) FFUN3(   auth_code, file, out_format)
		FFUN2(const char *, getRecordV2, __int32 auth_code, const char *file, const char *out_format) FFUN3(   auth_code, file, out_format)
		FFUN2(const char *, getImage, __int32 auth_code, const char *file) FFUN3(   auth_code, file)
		FFUN2(__int32, canSendImage, __int32 auth_code) FFUN3(   auth_code)
		FFUN2(__int32, canSendRecord, __int32 auth_code) FFUN3(   auth_code)
		FFUN2(__int32, addLog, __int32 auth_code, __int32 log_level, const char *category, const char *log_msg) FFUN3(   auth_code,  log_level, category, log_msg)
		FFUN2(__int32, setFatal, __int32 auth_code, const char *error_info) FFUN3(   auth_code, error_info)
		FFUN2(__int32, setRestart, __int32 auth_code) FFUN3(   auth_code) // currently ineffective

};

