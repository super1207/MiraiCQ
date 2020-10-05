#ifndef __BOT_H__LGQKADOYHORUBXTX
#define __BOT_H__LGQKADOYHORUBXTX

#include <string>
#include <jsoncpp/json.h>
#include <boost/shared_ptr.hpp>

class Bot
{
public:
	virtual bool is_connect() = 0;
	virtual bool connect() = 0;
	virtual bool disconnect() = 0;
	virtual Json::Value get_message() = 0;
	virtual Json::Value send(Json::Value & outroot,__int32 milliseconds = 1000) = 0;
	virtual Json::Value send_group_msg(unsigned __int64 group_id,const std::string & message,bool auto_escape = false,__int32 timeout_mills = 5000) = 0;
	virtual Json::Value send_private_msg(unsigned __int64 user_id,const std::string & message,bool auto_escape = false,__int32 timeout_mills = 5000) = 0;
	virtual Json::Value delete_msg(__int32 message_id,__int32 timeout_mills = 5000) = 0;
	static boost::shared_ptr<Bot> getInstance(const std::string & ws_url = "ws://localhost:6700",size_t buffer_size = 1024);
protected:
	virtual ~Bot(){};
};



#endif