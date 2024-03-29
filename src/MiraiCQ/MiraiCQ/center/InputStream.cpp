#include "InputStream.h"

void InputStream::deal_group_member_msg(const std::string& msg, int64_t user_id, int64_t group_id) {
	std::string key = std::to_string(user_id) + "," + std::to_string(group_id);
	std::shared_ptr<BlockQueue<std::tuple<std::string, int64_t, int64_t>>> dat = nullptr;
	try {
		dat = mp.get(key);
	}
	catch (const std::exception&) {
		//do nothing
	}
	if (dat != nullptr) {
		dat->push({ msg,user_id,group_id });
	}
}

void InputStream::deal_group_msg(const std::string& msg, int64_t user_id, int64_t group_id) {
	std::string key = std::to_string(group_id);
	std::shared_ptr<BlockQueue<std::tuple<std::string, int64_t, int64_t>>> dat = nullptr;
	try {
		dat = mp.get(key);
	}
	catch (const std::exception&) {
		//do nothing
	}
	if (dat != nullptr) {
		dat->push({ msg,user_id,group_id });
	}
}

void InputStream::deal_private_msg(const std::string& msg, int64_t user_id) {
	std::string key = std::to_string(user_id);
	std::shared_ptr<BlockQueue<std::tuple<std::string, int64_t, int64_t>>> dat = nullptr;
	try {
		dat = mp.get(key);
	}
	catch (const std::exception&) {
		//do nothing
	}
	if (dat != nullptr) {
		dat->push({ msg,user_id,0 });
	}
}

void InputStream::put_key(const std::string& key, std::shared_ptr<BlockQueue<std::tuple<std::string, int64_t, int64_t>>> q)
{
	mp.put(key, q);
}

void InputStream::remove_key(const std::string& key) {
	mp.remove(key);
}

InputStream* InputStream::get_instance() {
	static InputStream ins;
	return &ins;
}
