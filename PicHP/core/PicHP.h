#pragma once

#include <string>

class SBotCore
{
public:
	static void add_info_log(const std::string & category, const std::string &  log_msg);
	static void add_debug_log(const std::string & category, const std::string &  log_msg);
	static std::string get_app_dir();
	static std::string send_ws(const std::string & js_str,unsigned int timeout = 10000);

};

struct SBotEventBase
{
	struct PostType {enum T{MESSAGE,NOTICE,REQUEST/* ,META_EVENT */};};
	SBotEventBase(){}
	int copy_init(const SBotEventBase & e)
	{
		this->post_type = e.post_type;
		this->self_id = e.self_id;
		this->_time = e._time;
		return 0;
	}
	long long int _time;
	long long int self_id;
	PostType::T post_type;
};

struct SbotEventMessageBase:public SBotEventBase
{
	struct MessageType {enum T{PRIVATR,GROUP};};
	SbotEventMessageBase(){}
	int copy_init_pre(const SBotEventBase & e)
	{
		SBotEventBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventMessageBase & e)
	{
		copy_init_pre(e);
		this->message_type = e.message_type;
		return 0;
	}
	MessageType::T message_type;
};

struct SbotEventMessagePrivate:public SbotEventMessageBase
{
	struct SubType {enum T{FRIEND,GROUP,OTHER};};
	struct Sender
	{
		struct SexType {enum T{MALE,FEMALE,UNKNOWN};};
		long long user_id;
		std::string nickname;
		SexType::T sex;
		int age;
	};
	SbotEventMessagePrivate(){}
	int copy_init_pre(const SbotEventMessageBase & e)
	{
		SbotEventMessageBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventMessagePrivate & e)
	{
		copy_init_pre(e);
		sub_type = e.sub_type;
		message_id = e.message_id;
		user_id = e.user_id;
		message = e.message;
		raw_message = e.raw_message;
		font = e.font;
		sender = e.sender;
		return 0;
	}
	SubType::T sub_type;
	int message_id;
	long long user_id;
	std::string message;
	std::string raw_message;
	int font;
	Sender sender;
};

struct SbotEventMessageGroup:public SbotEventMessageBase
{
	struct SubType {enum T{NORMAL,ANONYMOUS,NOTICE};};
	struct Anonymous
	{
		long long id;
		std::string name;
		std::string flag;
	};
	struct Sender
	{
		struct SexType {enum T{MALE,FEMALE,UNKNOWN};};
		struct RoleType {enum T{OWNER,ADMIN,MEMBER};};
		long long user_id;
		std::string nickname;
		std::string card;
		SexType::T sex;
		int age;
		std::string area;
		std::string level;
		RoleType::T role;
		std::string title;
	};
	int copy_init_pre(const SbotEventMessageBase & e)
	{
		SbotEventMessageBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventMessageGroup & e)
	{
		copy_init_pre(e);
		sub_type = e.sub_type;
		message_id = e.message_id;
		group_id = e.group_id;
		user_id = e.user_id;
		anonymous = e.anonymous;
		message = e.message;
		raw_message = e.raw_message;
		font = e.font;
		sender = e.sender;
		return 0;
	}
	SubType::T sub_type;
	int message_id;
	long long int group_id;
	long long user_id;
	Anonymous anonymous;
	std::string message;
	std::string raw_message;
	int font;
	Sender sender;
};

struct SbotEventNoticeBase:public SBotEventBase
{
	struct NoticeType {enum T{GROUP_UPLOAD,GROUP_ADMIN,GROUP_DECREASE,GROUP_INCREASE
		,GROUP_BAN,FRIEND_ADD,GROUP_RECALL,FRIEND_RECALL,NOTIFY};};
	SbotEventNoticeBase(){}
	int copy_init_pre(const SBotEventBase & e)
	{
		SBotEventBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeBase & e)
	{
		copy_init_pre(e);
		this->notice_type = e.notice_type;
		return 0;
	}
	NoticeType::T notice_type;
};

struct SbotEventNoticeGroupUpload:public SbotEventNoticeBase
{
	struct FileType {
		std::string id;
		std::string name;
		long long int size;
		long long int busid;
	};
	SbotEventNoticeGroupUpload(){}
	int copy_init_pre(const SbotEventNoticeBase & e)
	{
		SbotEventNoticeBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeGroupUpload & e)
	{
		copy_init_pre(e);
		group_id = e.group_id;
		user_id = e.user_id;
		file = e.file;
		return 0;
	}
	long long int group_id;
	long long int user_id;
	FileType file;
};

struct SbotEventNoticeGroupAdmin:public SbotEventNoticeBase
{
	struct SubType {enum T{SET,UNSET};};
	SbotEventNoticeGroupAdmin(){}
	int copy_init_pre(const SbotEventNoticeBase & e)
	{
		SbotEventNoticeBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeGroupAdmin & e)
	{
		copy_init_pre(e);
		group_id = e.group_id;
		user_id = e.user_id;
		sub_type = e.sub_type;
		return 0;
	}
	SubType::T sub_type;
	long long int group_id;
	long long int user_id;
};

struct SbotEventNoticeGroupDecrease:public SbotEventNoticeBase
{
	struct SubType {enum T{LEAVE,KICK,KICK_ME};};
	SbotEventNoticeGroupDecrease(){}
	int copy_init_pre(const SbotEventNoticeBase & e)
	{
		SbotEventNoticeBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeGroupDecrease & e)
	{
		copy_init_pre(e);
		group_id = e.group_id;
		user_id = e.user_id;
		operator_id = e.operator_id;
		sub_type = e.sub_type;
		return 0;
	}
	SubType::T sub_type;
	long long int group_id;
	long long int operator_id;
	long long int user_id;
};

struct SbotEventNoticeGroupIncrease:public SbotEventNoticeBase
{
	struct SubType {enum T{APPROVE,INVITE};};
	SbotEventNoticeGroupIncrease(){}
	int copy_init_pre(const SbotEventNoticeBase & e)
	{
		SbotEventNoticeBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeGroupIncrease & e)
	{
		copy_init_pre(e);
		group_id = e.group_id;
		user_id = e.user_id;
		operator_id = e.operator_id;
		sub_type = e.sub_type;
		return 0;
	}
	SubType::T sub_type;
	long long int group_id;
	long long int operator_id;
	long long int user_id;
};

struct SbotEventNoticeGroupBan:public SbotEventNoticeBase
{
	struct SubType {enum T{BAN,LIFT_BAN};};
	SbotEventNoticeGroupBan(){}
	int copy_init_pre(const SbotEventNoticeBase & e)
	{
		SbotEventNoticeBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeGroupBan & e)
	{
		copy_init_pre(e);
		group_id = e.group_id;
		user_id = e.user_id;
		operator_id = e.operator_id;
		sub_type = e.sub_type;
		duration = e.duration;
		return 0;
	}
	SubType::T sub_type;
	long long int group_id;
	long long int operator_id;
	long long int user_id;
	long long int duration;
};

struct SbotEventNoticeFriendAdd:public SbotEventNoticeBase
{
	SbotEventNoticeFriendAdd(){}
	int copy_init_pre(const SbotEventNoticeBase & e)
	{
		SbotEventNoticeBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeFriendAdd & e)
	{
		copy_init_pre(e);
		user_id = e.user_id;
		return 0;
	}
	long long int user_id;
};

struct SbotEventNoticeGroupRecall:public SbotEventNoticeBase
{
	SbotEventNoticeGroupRecall(){}
	int copy_init_pre(const SbotEventNoticeBase & e)
	{
		SbotEventNoticeBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeGroupRecall & e)
	{
		copy_init_pre(e);
		group_id = e.group_id;
		user_id = e.user_id;
		operator_id = e.operator_id;
		message_id = e.message_id;
		return 0;
	}
	long long int group_id;
	long long int operator_id;
	long long int user_id;
	int message_id;
};

struct SbotEventNoticeFriendRecall:public SbotEventNoticeBase
{
	SbotEventNoticeFriendRecall(){}
	int copy_init_pre(const SbotEventNoticeBase & e)
	{
		SbotEventNoticeBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeFriendRecall & e)
	{
		copy_init_pre(e);
		user_id = e.user_id;
		message_id = e.message_id;
		return 0;
	}
	long long int user_id;
	int message_id;
};

struct SbotEventNoticeNotifyPoke:public SbotEventNoticeBase
{
	SbotEventNoticeNotifyPoke(){}
	int copy_init_pre(const SbotEventNoticeBase & e)
	{
		SbotEventNoticeBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeNotifyPoke & e)
	{
		copy_init_pre(e);
		user_id = e.user_id;
		group_id = e.group_id;
		target_id = e.target_id;
		return 0;
	}
	long long int group_id;
	long long int user_id;
	long long int target_id;
};

struct SbotEventNoticeNotifyLuckyKing:public SbotEventNoticeBase
{
	SbotEventNoticeNotifyLuckyKing(){}
	int copy_init_pre(const SbotEventNoticeBase & e)
	{
		SbotEventNoticeBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeNotifyLuckyKing & e)
	{
		copy_init_pre(e);
		user_id = e.user_id;
		group_id = e.group_id;
		target_id = e.target_id;
		return 0;
	}
	long long int group_id;
	long long int user_id;
	long long int target_id;
};

struct SbotEventNoticeNotifyHonor:public SbotEventNoticeBase
{
	struct HonorType {enum T{TALKATIVE,PERFORMER,EMOTION};};
	SbotEventNoticeNotifyHonor(){}
	int copy_init_pre(const SbotEventNoticeBase & e)
	{
		SbotEventNoticeBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventNoticeNotifyHonor & e)
	{
		copy_init_pre(e);
		user_id = e.user_id;
		group_id = e.group_id;
		honor_type = e.honor_type;
		return 0;
	}
	long long int group_id;
	long long int user_id;
	HonorType::T honor_type;
};


struct SbotEventRequestBase:public SBotEventBase
{
	struct RequestType {enum T{FRIEND,GROUP};};
	SbotEventRequestBase(){}
	int copy_init_pre(const SBotEventBase & e)
	{
		SBotEventBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventRequestBase & e)
	{
		copy_init_pre(e);
		this->request_type = e.request_type;
		return 0;
	}
	RequestType::T request_type;
};

struct SbotEventRequestFriend:public SBotEventBase
{
	SbotEventRequestFriend(){}
	int copy_init_pre(const SBotEventBase & e)
	{
		SBotEventBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventRequestFriend & e)
	{
		copy_init_pre(e);
		user_id = e.user_id;
		comment = e.comment;
		flag = e.flag;
		return 0;
	}
	long long int user_id;
	std::string comment;
	std::string flag;
};

struct SbotEventRequestGroup:public SBotEventBase
{
	struct SubType {enum T{ADD,INVITE};};
	SbotEventRequestGroup(){}
	int copy_init_pre(const SBotEventBase & e)
	{
		SBotEventBase::copy_init(e);
		return 0;
	}
	int copy_init(const SbotEventRequestGroup & e)
	{
		copy_init_pre(e);
		sub_type = e.sub_type;
		group_id = e.group_id;
		user_id = e.user_id;
		comment = e.comment;
		flag = e.flag;
		return 0;
	}
	SubType::T sub_type;
	long long int group_id;
	long long int user_id;
	std::string comment;
	std::string flag;
};


int user_event_message_private(const Json::Value & raw_json,const SbotEventMessagePrivate & base);

int user_event_message_group(const Json::Value & raw_json,const SbotEventMessageGroup & base);

int user_event_notice_group_upload(const Json::Value & raw_json,const SbotEventNoticeGroupUpload & base);

int user_event_notice_group_admin(const Json::Value & raw_json,const SbotEventNoticeGroupAdmin & base);

int user_event_notice_group_decrease(const Json::Value & raw_json,const SbotEventNoticeGroupDecrease & base);

int user_event_notice_group_increase(const Json::Value & raw_json,const SbotEventNoticeGroupIncrease & base);

int user_event_notice_group_ban(const Json::Value & raw_json,const SbotEventNoticeGroupBan & base);

int user_event_notice_friend_add(const Json::Value & raw_json,const SbotEventNoticeFriendAdd & base);

int user_event_notice_group_recall(const Json::Value & raw_json,const SbotEventNoticeGroupRecall & base);

int user_event_notice_friend_recall(const Json::Value & raw_json,const SbotEventNoticeFriendRecall & base);

int user_event_notice_notify_poke(const Json::Value & raw_json,const SbotEventNoticeNotifyPoke & base);

int user_event_notice_notify_lucky_king(const Json::Value & raw_json,const SbotEventNoticeNotifyLuckyKing & base);

int user_event_notice_notify_honor(const Json::Value & raw_json,const SbotEventNoticeNotifyHonor & base);

int user_event_request_friend(const Json::Value & raw_json,const SbotEventRequestFriend & base);

int user_event_request_group(const Json::Value & raw_json,const SbotEventRequestGroup & base);
