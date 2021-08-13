// PicHP.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"

#include <string>
#include <vector>

#include "../jsoncpp/json.h"

#include "PicHP.h"

static int sg_authCode = 0;

/* 接收authCode */
extern "C" int __stdcall Initialize(int authCode)
{
	sg_authCode = authCode;
	return 0;
}

/* 此函数用于释放下面SendWs返回的字符串，若传入的参数为NULL,则不做任何动作 */
typedef  void  (__stdcall * FreeType)(void *);
static FreeType Free = NULL;

/* 
   描述:
     此函数用于直接调用onebot端的api
   参数:
     retcode用于接收调用情况
       0，调用成功
	   -1，参数错误
	   -2，网络错误，或者超时
	   -3，框架内部错误，请查看日志
     msg为要发送的json字符串，请使用gbk编码
     timeout为超时，单位毫秒
   返回:
     返回为gbk编码的json字符串，如果retcode不为0,则返回NULL。
	 返回的字符串[必须]使用上面的Free释放

*/
typedef  char * ( __stdcall * SendWsType)(int ac,const char * msg,int * retcode,unsigned int timeout);
static SendWsType SendWs = NULL;

/*
	描述:
	  此函数用于获取插件目录地址
	返回：
	  返回为gbk编码的字符串末尾有'\'，如果调用失败，返NULL
	  返回的字符串[必须]使用上面的Free释放
*/
typedef  char * ( __stdcall * GetAppDirectoryType)(int ac);
static GetAppDirectoryType GetAppDirectory = NULL;

/*
	描述:
	  打印日志
    参数:
     log_level指明等级
       0，debug
	   1，info
     category为日志种类，应该为gbk编码
     log_msg内容，应该为GBK编码
	返回：
	  0，成功
	  -1，auth_code错误
	  -2，log_level错误
	  -3，内部错误
*/
typedef int ( __stdcall * AddLogType)(int auth_code, int log_level, const char *category, const char *log_msg);
static AddLogType AddLog = NULL;



void SBotCore::add_info_log(const std::string & category, const std::string &  log_msg)
{
	AddLog(sg_authCode,1,category.c_str(),log_msg.c_str());
}
void SBotCore::add_debug_log(const std::string & category, const std::string &  log_msg)
{
	AddLog(sg_authCode,0,category.c_str(),log_msg.c_str());
}
std::string SBotCore::get_app_dir()
{
	char * ret = GetAppDirectory(sg_authCode);
	std::string retstr;
	if(ret)
	{
		retstr = ret;
	}
	Free(ret);
	return ret;
}
std::string SBotCore::send_ws(const std::string & js_str,unsigned int timeout)
{
	int retcode = 0;
	char * ret = SendWs(sg_authCode,js_str.c_str(),&retcode,timeout);
	std::string retstr;
	if(ret)
	{
		retstr = ret;
	}
	Free(ret);
	return retstr;
}

/* 此事件用于获取函数，f为函数指针数组，n为数组大小(目前恒为2) */
extern "C" int __stdcall Initialize2(void ** f,int n)
{
	if(n >= 4)
	{
		Free = (FreeType)f[0];
		SendWs = (SendWsType)f[1];
		GetAppDirectory = (GetAppDirectoryType)f[2];
		AddLog = (AddLogType)f[3];
		
	}
	return 0;
	
}

static std::string& replace_all_distinct(std::string& str, const std::string& old_value, const std::string& new_value)
{
	for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) != std::string::npos)
			str.replace(pos, old_value.length(), new_value);
		else   break;
	}
	return   str;
}

/* 用于将json数组格式的消息转化为字符串格式消息,转化失败返回"" */
static std::string CvtJsonToStrMsg(const Json::Value& jsonArr)
{
	std::string retStr;
	Json::Value defstr = Json::Value("");
	if (!jsonArr.isArray())
	{
		return "";
	}
	try
	{
		for(size_t i = 0;i < jsonArr.size();++i)
		{
		/*for (const auto& node : jsonArr)
		{*/
			std::string type = jsonArr[i].get("type", Json::Value("")).asString();
			if (type == "")
			{
				return "";
			}
			if (type == "text")
			{
				std::string temStr = jsonArr[i].get("data", defstr).get("text", defstr).asString();
				replace_all_distinct(temStr, "&", "&amp;");
				replace_all_distinct(temStr, "[", "&#91;");
				replace_all_distinct(temStr, "]", "&#93;");
				replace_all_distinct(temStr, ",", "&#44;");
				retStr.append(temStr);
			}
			else
			{
				std::string cqStr = "[CQ:" + type;
				Json::Value datObj = jsonArr[i].get("data", defstr);
				if (!datObj.isObject())
				{
					return "";
				}
				Json::Value::Members member = datObj.getMemberNames();
				for (std::vector<std::string>::iterator iter = member.begin(); iter != member.end(); iter++)
				{
					cqStr.append("," + (*iter) + "=" + datObj[(*iter)].asString());
				}
				cqStr.append("]");
				retStr.append(cqStr);

			}
		}
		return retStr;

	}
	catch (const std::exception& e)
	{
		SBotCore::add_debug_log("CvtJsonToStrMsg", e.what());
		return "";
	}

}

static int deal_event_message_private(const Json::Value & root,const SbotEventMessageBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventMessagePrivate bot_event_base;
	bot_event_base.copy_init_pre(base);

	std::string sub_type = root.get("sub_type",defstr).asString();
	if(sub_type == "friend")
	{
		bot_event_base.sub_type = SbotEventMessagePrivate::SubType::FRIEND;
	}
	else if(sub_type == "group")
	{
		bot_event_base.sub_type = SbotEventMessagePrivate::SubType::GROUP;
	}
	else if(sub_type == "other")
	{
		bot_event_base.sub_type = SbotEventMessagePrivate::SubType::OTHER;
	}

	bot_event_base.message_id = root.get("message_id",defint).asInt();

	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	
	bot_event_base.raw_message = root.get("raw_message", defstr).asString();

	Json::Value msg = root.get("message", defstr);
	if (msg.isArray())
	{
		bot_event_base.message = CvtJsonToStrMsg(msg);
	}
	else
	{
		bot_event_base.message = msg.asString();
	}
	if (bot_event_base.message == "")
	{
		bot_event_base.message = bot_event_base.raw_message;
	}

	bot_event_base.font = root.get("font",defint).asInt();

	Json::Value js_sender = root.get("sender",Json::Value());
	bot_event_base.sender.user_id = js_sender.get("user_id",defint).asInt64();
	bot_event_base.sender.nickname = js_sender.get("nickname",defstr).asString();
	std::string sexstr = js_sender.get("sex",defstr).asString();
	if(sexstr == "male")
	{
		bot_event_base.sender.sex = SbotEventMessagePrivate::Sender::SexType::MALE;
	}
	else if(sexstr == "female")
	{
		bot_event_base.sender.sex = SbotEventMessagePrivate::Sender::SexType::FEMALE;
	}
	else if(sexstr == "unknown")
	{
		bot_event_base.sender.sex = SbotEventMessagePrivate::Sender::SexType::UNKNOWN;
	}
	bot_event_base.sender.age = js_sender.get("age",defint).asInt();
	
	user_event_message_private(root,bot_event_base);
	
	return 0;
}
static int deal_event_message_group(const Json::Value & root,const SbotEventMessageBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventMessageGroup bot_event_base;
	bot_event_base.copy_init_pre(base);

	std::string sub_type = root.get("sub_type",defstr).asString();
	if(sub_type == "normal")
	{
		bot_event_base.sub_type = SbotEventMessageGroup::SubType::NORMAL;
	}
	else if(sub_type == "anonymous")
	{
		bot_event_base.sub_type = SbotEventMessageGroup::SubType::ANONYMOUS;
	}
	else if(sub_type == "notice")
	{
		bot_event_base.sub_type = SbotEventMessageGroup::SubType::NOTICE;
	}

	bot_event_base.message_id = root.get("message_id",defint).asInt();

	bot_event_base.group_id = root.get("group_id",defint).asInt64();

	bot_event_base.user_id = root.get("user_id",defint).asInt64();

	Json::Value js_anonymous = root.get("anonymous",Json::Value());
	bot_event_base.anonymous.id = js_anonymous.get("id",defint).asInt64();
	bot_event_base.anonymous.name = js_anonymous.get("name",defstr).asString();
	bot_event_base.anonymous.flag = js_anonymous.get("flag",defstr).asString();

	bot_event_base.raw_message = root.get("raw_message", defstr).asString();

	Json::Value msg = root.get("message", defstr);
	if (msg.isArray())
	{
		bot_event_base.message = CvtJsonToStrMsg(msg);
	}
	else
	{
		bot_event_base.message = msg.asString();
	}
	if (bot_event_base.message == "")
	{
		bot_event_base.message = bot_event_base.raw_message;
	}

	bot_event_base.font = root.get("font",defint).asInt();

	Json::Value js_sender = root.get("sender",Json::Value());
	bot_event_base.sender.user_id = js_sender.get("user_id",defint).asInt64();
	bot_event_base.sender.nickname = js_sender.get("nickname",defstr).asString();
	bot_event_base.sender.card = js_sender.get("card",defstr).asString();
	std::string sexstr = js_sender.get("sex",defstr).asString();
	if(sexstr == "male")
	{
		bot_event_base.sender.sex = SbotEventMessageGroup::Sender::SexType::MALE;
	}
	else if(sexstr == "female")
	{
		bot_event_base.sender.sex = SbotEventMessageGroup::Sender::SexType::FEMALE;
	}
	else if(sexstr == "unknown")
	{
		bot_event_base.sender.sex = SbotEventMessageGroup::Sender::SexType::UNKNOWN;
	}
	bot_event_base.sender.age = js_sender.get("age",defint).asInt();
	bot_event_base.sender.area = js_sender.get("area",defstr).asString();
	bot_event_base.sender.level = js_sender.get("level",defstr).asString();
	std::string role = js_sender.get("role",defstr).asString();
	if(role == "owner")
	{
		bot_event_base.sender.role = SbotEventMessageGroup::Sender::RoleType::OWNER;
	}
	else if(role == "admin")
	{
		bot_event_base.sender.role = SbotEventMessageGroup::Sender::RoleType::ADMIN;
	}
	else if(role == "member")
	{
		bot_event_base.sender.role = SbotEventMessageGroup::Sender::RoleType::MEMBER;
	}
	bot_event_base.sender.title = js_sender.get("title",defstr).asString();
	
	user_event_message_group(root,bot_event_base);
	return 0;
}

static int deal_event_message(const Json::Value & root,const SBotEventBase & base)
{
	Json::Value defstr = "";
	std::string message_type = root.get("message_type",defstr).asString();
	SbotEventMessageBase bot_event_base;
	bot_event_base.copy_init_pre(base);
	if(message_type == "private")
	{
		bot_event_base.message_type = SbotEventMessageBase::MessageType::PRIVATR;
		deal_event_message_private(root,bot_event_base);
	}
	else if(message_type == "group")
	{
		bot_event_base.message_type = SbotEventMessageBase::MessageType::GROUP;
		deal_event_message_group(root,bot_event_base);
	}
	return 0;
}

static int deal_event_notice_group_upload(const Json::Value & root,const SbotEventNoticeBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventNoticeGroupUpload bot_event_base;
	bot_event_base.copy_init_pre(base);
	bot_event_base.group_id = root.get("group_id",defint).asInt64();
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	Json::Value js_file = root.get("file",Json::Value());
	bot_event_base.file.id =  js_file.get("id",defstr).asString();
	bot_event_base.file.name =  js_file.get("name",defstr).asString();
	bot_event_base.file.size =  js_file.get("size",defint).asInt64();
	bot_event_base.file.busid =  js_file.get("busid",defint).asInt64();
	user_event_notice_group_upload(root,bot_event_base);
	return 0;
}

static int deal_event_notice_group_admin(const Json::Value & root,const SbotEventNoticeBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventNoticeGroupAdmin bot_event_base;
	bot_event_base.copy_init_pre(base);
	std::string sub_type = root.get("sub_type",defstr).asString();
	if(sub_type == "set")
	{
		bot_event_base.sub_type = SbotEventNoticeGroupAdmin::SubType::SET;
	}
	else if(sub_type == "unset")
	{
		bot_event_base.sub_type = SbotEventNoticeGroupAdmin::SubType::UNSET;
	}
	bot_event_base.group_id = root.get("group_id",defint).asInt64();
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	user_event_notice_group_admin(root,bot_event_base);
	return 0;
}

static int deal_event_notice_group_decrease(const Json::Value & root,const SbotEventNoticeBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventNoticeGroupDecrease bot_event_base;
	bot_event_base.copy_init_pre(base);
	std::string sub_type = root.get("sub_type",defstr).asString();
	if(sub_type == "leave")
	{
		bot_event_base.sub_type = SbotEventNoticeGroupDecrease::SubType::LEAVE;
	}
	else if(sub_type == "kick")
	{
		bot_event_base.sub_type = SbotEventNoticeGroupDecrease::SubType::KICK;
	}
	else if(sub_type == "kick_me")
	{
		bot_event_base.sub_type = SbotEventNoticeGroupDecrease::SubType::KICK_ME;
	}
	bot_event_base.group_id = root.get("group_id",defint).asInt64();
	bot_event_base.operator_id = root.get("operator_id",defint).asInt64();
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	user_event_notice_group_decrease(root,bot_event_base);
	return 0;
}

static int deal_event_notice_group_increase(const Json::Value & root,const SbotEventNoticeBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventNoticeGroupIncrease bot_event_base;
	bot_event_base.copy_init_pre(base);
	std::string sub_type = root.get("sub_type",defstr).asString();
	if(sub_type == "approve")
	{
		bot_event_base.sub_type = SbotEventNoticeGroupIncrease::SubType::APPROVE;
	}
	else if(sub_type == "invite")
	{
		bot_event_base.sub_type = SbotEventNoticeGroupIncrease::SubType::INVITE;
	}
	bot_event_base.group_id = root.get("group_id",defint).asInt64();
	bot_event_base.operator_id = root.get("operator_id",defint).asInt64();
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	user_event_notice_group_increase(root,bot_event_base);
	return 0;
}

static int deal_event_notice_group_ban(const Json::Value & root,const SbotEventNoticeBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventNoticeGroupBan bot_event_base;
	bot_event_base.copy_init_pre(base);
	std::string sub_type = root.get("sub_type",defstr).asString();
	if(sub_type == "ban")
	{
		bot_event_base.sub_type = SbotEventNoticeGroupBan::SubType::BAN;
	}
	else if(sub_type == "lift_ban")
	{
		bot_event_base.sub_type = SbotEventNoticeGroupBan::SubType::LIFT_BAN;
	}
	bot_event_base.group_id = root.get("group_id",defint).asInt64();
	bot_event_base.operator_id = root.get("operator_id",defint).asInt64();
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	bot_event_base.duration = root.get("duration",defint).asInt64();
	user_event_notice_group_ban(root,bot_event_base);
	return 0;
}

static int deal_event_notice_friend_add(const Json::Value & root,const SbotEventNoticeBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventNoticeFriendAdd bot_event_base;
	bot_event_base.copy_init_pre(base);
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	user_event_notice_friend_add(root,bot_event_base);
	return 0;
}

static int deal_event_notice_group_recall(const Json::Value & root,const SbotEventNoticeBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventNoticeGroupRecall bot_event_base;
	bot_event_base.copy_init_pre(base);
	bot_event_base.group_id = root.get("group_id",defint).asInt64();
	bot_event_base.operator_id = root.get("operator_id",defint).asInt64();
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	bot_event_base.message_id = root.get("message_id",defint).asInt();
	user_event_notice_group_recall(root,bot_event_base);
	return 0;
}

static int deal_event_notice_friend_recall(const Json::Value & root,const SbotEventNoticeBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventNoticeFriendRecall bot_event_base;
	bot_event_base.copy_init_pre(base);
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	bot_event_base.message_id = root.get("message_id",defint).asInt();
	user_event_notice_friend_recall(root,bot_event_base);
	return 0;
}

static int deal_event_notice_notify_poke(const Json::Value & root,const SbotEventNoticeBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventNoticeNotifyPoke bot_event_base;
	bot_event_base.copy_init_pre(base);
	bot_event_base.group_id = root.get("group_id",defint).asInt64();
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	bot_event_base.target_id = root.get("target_id",defint).asInt64();
	user_event_notice_notify_poke(root,bot_event_base);
	return 0;
}

static int deal_event_notice_notify_lucky_king(const Json::Value & root,const SbotEventNoticeBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventNoticeNotifyLuckyKing bot_event_base;
	bot_event_base.copy_init_pre(base);
	bot_event_base.group_id = root.get("group_id",defint).asInt64();
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	bot_event_base.target_id = root.get("target_id",defint).asInt64();
	user_event_notice_notify_lucky_king(root,bot_event_base);
	return 0;
}

static int deal_event_notice_notify_honor(const Json::Value & root,const SbotEventNoticeBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventNoticeNotifyHonor bot_event_base;
	bot_event_base.copy_init_pre(base);
	std::string honor_type = root.get("honor_type",defstr).asString();
	if(honor_type == "talkative")
	{
		bot_event_base.honor_type = SbotEventNoticeNotifyHonor::HonorType::TALKATIVE;
	}
	else if(honor_type == "performer")
	{
		bot_event_base.honor_type = SbotEventNoticeNotifyHonor::HonorType::PERFORMER;
	}
	else if(honor_type == "emotion")
	{
		bot_event_base.honor_type = SbotEventNoticeNotifyHonor::HonorType::EMOTION;
	}
	bot_event_base.group_id = root.get("group_id",defint).asInt64();
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	user_event_notice_notify_honor(root,bot_event_base);
	return 0;
}

static int deal_event_request_friend(const Json::Value & root,const SbotEventRequestBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventRequestFriend bot_event_base;
	bot_event_base.copy_init_pre(base);
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	bot_event_base.comment = root.get("comment",defstr).asString();
	bot_event_base.flag = root.get("flag",defstr).asString();
	user_event_request_friend(root,bot_event_base);
	return 0;
}

static int deal_event_request_group(const Json::Value & root,const SbotEventRequestBase & base)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	SbotEventRequestGroup bot_event_base;
	bot_event_base.copy_init_pre(base);
	std::string sub_type = root.get("sub_type",defstr).asString();
	if(sub_type == "add")
	{
		bot_event_base.sub_type = SbotEventRequestGroup::SubType::ADD;
	}
	else if(sub_type == "invite")
	{
		bot_event_base.sub_type = SbotEventRequestGroup::SubType::INVITE;
	}
	bot_event_base.group_id = root.get("group_id",defint).asInt64();
	bot_event_base.user_id = root.get("user_id",defint).asInt64();
	bot_event_base.comment = root.get("comment",defstr).asString();
	bot_event_base.flag = root.get("flag",defstr).asString();
	user_event_request_group(root,bot_event_base);
	return 0;
}

static int deal_event_notice(const Json::Value & root,const SBotEventBase & base)
{
	Json::Value defstr = "";
	std::string notice_type = root.get("notice_type",defstr).asString();
	SbotEventNoticeBase bot_event_base;
	bot_event_base.copy_init_pre(base);
	if(notice_type == "group_upload")
	{
		bot_event_base.notice_type = SbotEventNoticeBase::NoticeType::GROUP_UPLOAD;
		deal_event_notice_group_upload(root,bot_event_base);
	}
	else if(notice_type == "group_admin")
	{
		bot_event_base.notice_type = SbotEventNoticeBase::NoticeType::GROUP_ADMIN;
		deal_event_notice_group_admin(root,bot_event_base);
	}
	else if(notice_type == "group_decrease")
	{
		bot_event_base.notice_type = SbotEventNoticeBase::NoticeType::GROUP_DECREASE;
		deal_event_notice_group_decrease(root,bot_event_base);
	}
	else if(notice_type == "group_increase")
	{
		bot_event_base.notice_type = SbotEventNoticeBase::NoticeType::GROUP_INCREASE;
		deal_event_notice_group_increase(root,bot_event_base);
	}
	else if(notice_type == "group_ban")
	{
		bot_event_base.notice_type = SbotEventNoticeBase::NoticeType::GROUP_BAN;
		deal_event_notice_group_ban(root,bot_event_base);
	}
	else if(notice_type == "friend_add")
	{
		bot_event_base.notice_type = SbotEventNoticeBase::NoticeType::FRIEND_ADD;
		deal_event_notice_friend_add(root,bot_event_base);
	}
	else if(notice_type == "group_recall")
	{
		bot_event_base.notice_type = SbotEventNoticeBase::NoticeType::GROUP_RECALL;
		deal_event_notice_group_recall(root,bot_event_base);
	}
	else if(notice_type == "friend_recall")
	{
		bot_event_base.notice_type = SbotEventNoticeBase::NoticeType::FRIEND_RECALL;
		deal_event_notice_friend_recall(root,bot_event_base);
	}
	else if(notice_type == "notify")
	{
		bot_event_base.notice_type = SbotEventNoticeBase::NoticeType::NOTIFY;
		std::string sub_type = root.get("notice_type",defstr).asString();
		if(sub_type == "poke")
		{
			deal_event_notice_notify_poke(root,bot_event_base);
		}
		else if(sub_type == "lucky_king")
		{
			deal_event_notice_notify_lucky_king(root,bot_event_base);
		}
		else if(sub_type == "honor")
		{
			deal_event_notice_notify_honor(root,bot_event_base);
		}
		
	}
	return 0;
}

static int deal_event_request(const Json::Value & root,const SBotEventBase & base)
{
	Json::Value defstr = "";
	std::string request_type = root.get("request_type",defstr).asString();
	SbotEventRequestBase bot_event_base;
	bot_event_base.copy_init_pre(base);
	if(request_type == "friend")
	{
		bot_event_base.request_type = SbotEventRequestBase::RequestType::FRIEND;
		deal_event_request_friend(root,bot_event_base);
	}
	else if(request_type == "group")
	{
		bot_event_base.request_type = SbotEventRequestBase::RequestType::GROUP;
		deal_event_request_group(root,bot_event_base);
	}
	return 0;
}

static int deal_eventjson(const Json::Value & root)
{
	Json::Value defstr = "";
	Json::Value defint = 0;
	std::string post_type = root.get("post_type",defstr).asString();
	SBotEventBase bot_event_base;
	bot_event_base._time = root.get("time",defint).asInt64();
	bot_event_base.self_id = root.get("self_id",defint).asInt64();
	if(post_type == "message")
	{
		bot_event_base.post_type = SBotEventBase::PostType::MESSAGE;
		deal_event_message(root,bot_event_base);
	}
	else if(post_type == "notice")
	{
		bot_event_base.post_type = SBotEventBase::PostType::NOTICE;
		deal_event_notice(root,bot_event_base);
	}
	else if(post_type == "request")
	{
		bot_event_base.post_type = SBotEventBase::PostType::REQUEST;
		deal_event_request(root,bot_event_base);
	}
	return 0;
}

/* 接收所有ws下发的事件，msg为gbk编码的json字符串 type = 1207 */
extern "C" int __stdcall event_all(const char * jsonmsg)
{

	Json::Value root;
	Json::Reader reader;
	if(!reader.parse(jsonmsg, root))
	{
		return 0;
	}
	try
	{
		deal_eventjson(root);
	}catch(std::exception & e)
	{
		SBotCore::add_info_log("error",e.what());
	}

	return 0;
}

static std::vector<std::string> tokenize(const std::string& s, char c) {
	std::string::const_iterator end = s.end();
	std::string::const_iterator start = end;

	std::vector<std::string> v;
	for (std::string::const_iterator it = s.begin(); it != end; ++it) {
		if (*it != c) {
			if (start == end)
				start = it;
			continue;
		}
		if (start != end) {
			v.push_back(std::string(start, it));
			start = end;
		}
	}
	if (start != end)
		v.push_back(std::string(start, end));
	return v;
}



/* 用于将字符串格式的消息转化为json数组格式,转化失败返回null */
Json::Value CvtStrMsgToJson(const std::string& strMsg)
{
	Json::Value jsonArr = Json::arrayValue;
	std::vector<std::string> strVec;
	bool iscq = false;
	for(size_t i = 0;i < strMsg.size();++i)
	{
		if (strMsg[i] == '[')
		{
			iscq = true;
			strVec.push_back(std::string(1, '['));
		}
		else
		{
			if (iscq && strMsg[i] == ' ')
			{
				continue;
			}
			if (strVec.size() == 0)
			{
				strVec.push_back("");
			}
			strVec[strVec.size() - 1].append(std::string(1, strMsg[i]));
			if (strMsg[i] == ']')
			{
				iscq = false;
				strVec.push_back("");
			}
		}
	}
	for(size_t i = 0;i < strVec.size();++i)
	{
		if (strVec[i] == "")
		{
			continue;
		}
		if (strVec[i][0] != '[')
		{
			Json::Value node;
			node["type"] = "text";
			Json::Value datNode;
			std::string t = strVec[i];
			replace_all_distinct(t, "&amp;", "&");
			replace_all_distinct(t, "&#91;", "[");
			replace_all_distinct(t, "&#93;", "]");
			replace_all_distinct(t, "&#44;", ",");
			datNode["text"] = t;
			node["data"] = datNode;
			jsonArr.append(node);
		}
		else
		{
			Json::Value node;
			Json::Value datNode = Json::objectValue;
			std::vector<std::string> strNode = tokenize(std::string(strVec[i].begin() + 1, strVec[i].end() - 1), ',');
			try
			{
				node["type"] = tokenize(strNode.at(0), ':').at(1);
				for (size_t i = 1; i < strNode.size(); ++i)
				{
					size_t pos = strNode.at(i).find_first_of("=");
					std::string type = strNode.at(i).substr(0, pos);
					std::string dat = strNode.at(i).substr(pos + 1);
					std::string t = dat;
					replace_all_distinct(t, "&amp;", "&");
					replace_all_distinct(t, "&#91;", "[");
					replace_all_distinct(t, "&#93;", "]");
					replace_all_distinct(t, "&#44;", ",");
					datNode[type] = t;
				}
				node["data"] = datNode;
				jsonArr.append(node);

			}
			catch (const std::exception& e)
			{
				SBotCore::add_debug_log("CvtStrMsgToJson", e.what());
				return Json::Value();
			}
		}
	}
	return jsonArr;
}







