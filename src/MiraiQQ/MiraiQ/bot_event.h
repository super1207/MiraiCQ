#ifndef __BOT_EVENT_H__BBBKADOAAORUBXTX
#define __BOT_EVENT_H__BBBKADOAAORUBXTX

#include <jsoncpp/json.h>
#include <boost/shared_ptr.hpp>
#include "plus.h"

extern std::map<std::string,std::map<std::string,__int32(*)(const Json::Value &,boost::shared_ptr<Plus>)> > g_message_map;

//__int32 call_event_enable(const Json::Value & ,boost::shared_ptr<Plus>);
//__int32 call_event_disable(const Json::Value & ,boost::shared_ptr<Plus>);
//__int32 call_event_coolq_start(const Json::Value & ,boost::shared_ptr<Plus>);
//__int32 call_event_coolq_exit(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_private_message(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_group_message(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_discuss_message(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_group_upload(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_group_admin(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_group_member_decrease(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_group_member_increase(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_group_ban(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_friend_add(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_notify(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_friend_request(const Json::Value & ,boost::shared_ptr<Plus>);
__int32 call_event_group_request(const Json::Value & ,boost::shared_ptr<Plus>);

void init_event_map();


#endif