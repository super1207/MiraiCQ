#pragma once

#include <string>
#include <memory>
#include <tuple>

#include "../tool/ThreadSafeMap.h"
#include "../tool/BlockQueue.h"

class InputStream
{ 
public:
	void deal_group_member_msg(const std::string& msg, int64_t user_id, int64_t group_id);

	void deal_group_msg(const std::string& msg, int64_t user_id, int64_t group_id);

	void deal_private_msg(const std::string& msg, int64_t user_id);

	void put_key(const std::string& key, std::shared_ptr<BlockQueue<std::tuple<std::string, int64_t, int64_t>>> q);

	void remove_key(const std::string& key);

	static InputStream* get_instance();
private:
	ThreadSafeMap<std::shared_ptr<BlockQueue<std::tuple<std::string,int64_t,int64_t>>>> mp;
};

