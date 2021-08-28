#include "MiraiNet.h"

#include "../../tool/StrTool.h"

#include "OneBot/OneBotImpl.h"


using namespace std;

MiraiNet::MiraiNet()
{
}

std::map<std::string, std::string> MiraiNet::get_all_config() noexcept
{
	shared_lock<shared_mutex> lock(mx_config_map);
	return config_map;
}

std::vector<MiraiNet::NetStruct> MiraiNet::get_event()noexcept
{
	lock_guard<mutex> lock(mx_event_vec);
	auto ret_vec = event_vec;
	event_vec.clear();
	return ret_vec;
}
void MiraiNet::add_event(NetStruct event)noexcept
{
	lock_guard<mutex> lock(mx_event_vec);
	event_vec.push_back(event);
	if (event_vec.size() > buf_size)
	{
		event_vec.erase(event_vec.begin());
	}
}

MiraiNet::~MiraiNet()
{
}

void MiraiNet::set_config(const std::string& key, const std::string& value) noexcept
{
	unique_lock<shared_mutex> lock(mx_config_map);
	config_map[key] = value;
}

void MiraiNet::set_all_config(const std::map<std::string, std::string>& all_config) noexcept
{
	unique_lock<shared_mutex> lock(mx_config_map);
	config_map = all_config;
}

std::string MiraiNet::get_config(const std::string& key)noexcept
{
	shared_lock<shared_mutex> lock(mx_config_map);
	const auto iter = config_map.find(key);
	if (iter != config_map.cend())
	{
		return iter->second;
	}
	return "";
}

std::shared_ptr<MiraiNet> MiraiNet::get_instance(const std::string & type) noexcept
{
	if (StrTool::tolower(type) == "onebot")
	{
		auto ret =  std::make_shared<OneBotNetImpl>();
		ret->set_config("net_type", "onebot");
		return ret;
	}
	return std::shared_ptr<MiraiNet>();
}
