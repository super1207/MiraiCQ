#include "OneBotEventDeal.h"
#include "../../log/MiraiLog.h"
#include "../../tool/StrTool.h"
#include "../../tool/EmojiTool.h"

#include <list>

OneBotEventDeal::OneBotEventDeal()
{
}

OneBotEventDeal::~OneBotEventDeal()
{
}

MiraiNet::NetStruct OneBotEventDeal::deal_event(const Json::Value & root_)
{
	auto root = MiraiNet::NetStruct(new Json::Value(root_));
	if (StrTool::get_str_from_json(*root, "post_type", "") == "message")
	{
		auto msg_json = root->get("message", Json::Value());
		/* 在此将message变为数组格式 */
		if (msg_json.isString())
		{
			(*root)["message"] = StrTool::cq_str_to_jsonarr(msg_json.asString());
		}
	}
	return root;
}
