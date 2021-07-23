#include "../core/stdafx.h"

#include "../jsoncpp/json.h"

#include "../core/PicHP.h"



int user_event_message_private(const Json::Value & raw_json,const SbotEventMessagePrivate & base)
{
	Json::Value root;
	root["action"] = "send_private_msg";
	Json::Value params;
	params["user_id"] = base.user_id;
	params["message"] = base.message;
	root["params"] = params;
	SBotCore::send_ws(Json::FastWriter().write(root));
	return 0;
}

int user_event_message_group(const Json::Value & raw_json,const SbotEventMessageGroup & base)
{
	return 0;
}

int user_event_notice_group_upload(const Json::Value & raw_json,const SbotEventNoticeGroupUpload & base)
{
	return 0;
}

int user_event_notice_group_admin(const Json::Value & raw_json,const SbotEventNoticeGroupAdmin & base)
{
	return 0;
}
int user_event_notice_group_decrease(const Json::Value & raw_json,const SbotEventNoticeGroupDecrease & base)
{
	return 0;
}

int user_event_notice_group_increase(const Json::Value & raw_json,const SbotEventNoticeGroupIncrease & base)
{
	return 0;
}
int user_event_notice_group_ban(const Json::Value & raw_json,const SbotEventNoticeGroupBan & base)
{
	return 0;
}

int user_event_notice_friend_add(const Json::Value & raw_json,const SbotEventNoticeFriendAdd & base)
{
	return 0;
}
int user_event_notice_group_recall(const Json::Value & raw_json,const SbotEventNoticeGroupRecall & base)
{
	return 0;
}
int user_event_notice_friend_recall(const Json::Value & raw_json,const SbotEventNoticeFriendRecall & base)
{
	return 0;
}
int user_event_notice_notify_poke(const Json::Value & raw_json,const SbotEventNoticeNotifyPoke & base)
{
	return 0;
}
int user_event_notice_notify_lucky_king(const Json::Value & raw_json,const SbotEventNoticeNotifyLuckyKing & base)
{
	return 0;
}
int user_event_notice_notify_honor(const Json::Value & raw_json,const SbotEventNoticeNotifyHonor & base)
{
	return 0;
}
int user_event_request_friend(const Json::Value & raw_json,const SbotEventRequestFriend & base)
{
	return 0;
}

int user_event_request_group(const Json::Value & raw_json,const SbotEventRequestGroup & base)
{
	return 0;
}



/* 框架启动事件 type = 1001 */
extern "C" int __stdcall event_coolq_start()
{
	return 0;
}

/* 插件启用事件 type = 1003 */
extern "C" int __stdcall event_enable()
{
	return 0;
}


/* 插件停用事件 type = 1004 永不调用 */
extern "C" int __stdcall event_disable()
{
	return 0;
}

/* 框架退出事件 type = 1002 */
extern "C" int __stdcall event_coolq_exit()
{
	return 0;
}

/* menu-检查更新事件 */
extern "C" int __stdcall menu_check_update()
{
	MessageBoxA(NULL,"检查更新按钮被点击","INFO",MB_OK);
	return 0;
}


