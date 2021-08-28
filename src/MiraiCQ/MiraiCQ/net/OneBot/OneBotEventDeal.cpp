#include "OneBotEventDeal.h"
#include "../../tool/StrTool.h"

#include <list>

OneBotEventDeal::OneBotEventDeal()
{
}

OneBotEventDeal::~OneBotEventDeal()
{
}

MiraiNet::NetStruct OneBotEventDeal::deal_event(const Json::Value & root_,std::mutex & mx_msg_id_vec, std::list<std::pair<int, int>> & msg_id_vec, int& curr_msg_id)
{
	auto root = MiraiNet::NetStruct(new Json::Value(root_));
	if (StrTool::get_str_from_json(*root, "post_type", "") == "message")
	{
		auto msg_json = root->get("message", Json::nullValue);
		/* 在此将message变为数组格式 */
		if (msg_json.isString())
		{
			(*root)["message"] = StrTool::cq_str_to_jsonarr(msg_json.asString());
		}
		/* 在此转换message_id */
		int id = StrTool::get_int_from_json(*root,"message_id",0);
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
		(*root)["message_id"] = curr_msg_id;
	}
	return root;
}
