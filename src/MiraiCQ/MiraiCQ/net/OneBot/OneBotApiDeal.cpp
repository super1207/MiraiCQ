#include "OneBotApiDeal.h"
#include "../../tool/StrTool.h"
#include "../../log/MiraiLog.h"
#include "../../tool/EmojiTool.h"

OneBotApiDeal::OneBotApiDeal()
{
}

OneBotApiDeal::~OneBotApiDeal()
{
}

MiraiNet::NetStruct OneBotApiDeal::deal_api(const Json::Value& root)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	auto params_json = ret_json->get("params", Json::nullValue);
	/* 处理emoji */
	Json::Value msg_json = Json::nullValue;
	if (params_json != Json::nullValue)
	{
		msg_json = params_json.get("message", Json::nullValue);
	}
	if (msg_json != Json::nullValue)
	{
		if (msg_json.isString())
		{
			std::string t = EmojiTool::unescape_cq_emoji(msg_json.asString());
			(*ret_json)["params"]["message"] = StrTool::cq_str_to_jsonarr(t);
		}
		else
		{
			std::string t = StrTool::jsonarr_to_cq_str(msg_json);
			std::string t2 = EmojiTool::unescape_cq_emoji(t);
			(*ret_json)["params"]["message"] = StrTool::cq_str_to_jsonarr(t2);
		}
	}
	//auto id_json = params_json.get("message_id",Json::nullValue);
	///* 只有撤回消息之类的api才有msg_id,所以，如果没有msg_id，则不做处理 */
	//if (id_json.isInt())
	//{
	//	/* 在此转换message_id */
	//	int id = id_json.asInt();
	//	bool is_find = false;
	//	std::lock_guard<std::mutex> lk(mx_msg_id_vec);
	//	for (auto &  i : msg_id_vec)
	//	{
	//		if (i.first == id)
	//		{
	//			(*ret_json)["params"]["message_id"] = i.second;
	//			is_find = true;
	//			break;
	//		}
	//	}
	//	if (is_find == false)
	//	{
	//		MiraiLog::get_instance()->add_debug_log("ApiCall", "没有找到对应的message_id");
	//		ret_json = nullptr;
	//	}
	//}
	return ret_json;
}

MiraiNet::NetStruct OneBotApiDeal::deal_recv(const Json::Value & root)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	//auto data_json = (*ret_json).get("data", Json::nullValue);
	//if(data_json.isObject())
	//{
	//	auto id_json = data_json.get("message_id", Json::nullValue);
	//	if (id_json.isInt())
	//	{
	//		/* 在此转换message_id */
	//		int id = id_json.asInt();
	//		std::lock_guard<std::mutex> lk(mx_msg_id_vec);
	//		++curr_msg_id;
	//		if (curr_msg_id == INT32_MAX)
	//		{
	//			curr_msg_id = 2;
	//		}
	//		msg_id_vec.push_back({ curr_msg_id ,id });
	//		/* 限制msg_id的缓存 */
	//		if (msg_id_vec.size() > 4096)
	//		{
	//			msg_id_vec.pop_front();
	//		}
	//		(*ret_json)["data"]["message_id"] = curr_msg_id;
	//	}
	//}
	

	return ret_json;
}
