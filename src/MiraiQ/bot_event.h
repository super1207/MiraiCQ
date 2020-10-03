#ifndef __BOT_EVENT_H__BBBKADOAAORUBXTX
#define __BOT_EVENT_H__BBBKADOAAORUBXTX

#include <jsoncpp/json.h>

extern std::map<std::string,std::map<std::string,__int32(*)(const Json::Value &)> > g_message_map;

//__int32 call_event_enable(const Json::Value & root);
//__int32 call_event_disable(const Json::Value & root);
//__int32 call_event_coolq_start(const Json::Value & root);
//__int32 call_event_coolq_exit(const Json::Value & root);
__int32 call_event_private_message(const Json::Value & root);
__int32 call_event_group_message(const Json::Value & root);
__int32 call_event_discuss_message(const Json::Value & root);
__int32 call_event_group_upload(const Json::Value & root);
__int32 call_event_group_admin(const Json::Value & root);
__int32 call_event_group_member_decrease(const Json::Value & root);
__int32 call_event_group_member_increase(const Json::Value & root);
__int32 call_event_group_ban(const Json::Value & root);
__int32 call_event_friend_add(const Json::Value & root);
__int32 call_event_friend_request(const Json::Value & root);
__int32 call_event_group_request(const Json::Value & root);

void init_event_map();


#endif