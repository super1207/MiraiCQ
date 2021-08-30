#include "OneBotApiDeal.h"
#include "../../tool/StrTool.h"

OneBotApiDeal::OneBotApiDeal()
{
}

OneBotApiDeal::~OneBotApiDeal()
{
}

MiraiNet::NetStruct OneBotApiDeal::deal_api(const Json::Value& root,std::mutex& mx_msg_id_vec, std::list<std::pair<int, int>>& msg_id_vec, int& curr_msg_id)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	auto id_json = (*ret_json).get("message_id",Json::nullValue);
	if (!id_json.isNull() && id_json.isInt())
	{
		/* 在此转换message_id */
		int id = id_json.asInt();
		std::lock_guard<std::mutex> lk(mx_msg_id_vec);
		for (auto i : msg_id_vec)
		{
			if (i.first == id)
			{
				(*ret_json)["message_id"] = i.second;
				break;
			}
		}
	}
	return ret_json;
}

MiraiNet::NetStruct OneBotApiDeal::deal_recv(const Json::Value & root, std::mutex& mx_msg_id_vec, std::list<std::pair<int, int>>& msg_id_vec, int& curr_msg_id)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	auto data_json = (*ret_json).get("data", Json::nullValue);
	if(data_json.isObject())
	{
		auto id_json = data_json.get("message_id", Json::nullValue);
		if (!id_json.isNull() && id_json.isInt())
		{
			/* 在此转换message_id */
			int id = id_json.asInt();
			std::lock_guard<std::mutex> lk(mx_msg_id_vec);
			++curr_msg_id;
			if (curr_msg_id == INT32_MAX)
			{
				curr_msg_id = 2;
			}
			msg_id_vec.push_back({ curr_msg_id ,id });
			/* 限制msg_id的缓存 */
			if (msg_id_vec.size() > 4096)
			{
				msg_id_vec.pop_front();
			}
			(*ret_json)["data"]["message_id"] = curr_msg_id;
		}
	}
	

	return ret_json;
}
