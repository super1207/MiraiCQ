#include "MiraiHttpEventDeal.h"
#include "../../tool/StrTool.h"
#include "../../log/MiraiLog.h"

#include <list>
#include <string>

using namespace std;

MiraiHttpEventDeal::MiraiHttpEventDeal()
{
}

MiraiHttpEventDeal::~MiraiHttpEventDeal()
{
}
/*
	text -> Plain
	face -> Face
	image -> Image、FlashImage
	record -> Voice (MiraiCQ暂不支持)
	video -> ??? (MiraiCQ与mha都不支持)
	at -> At、AtAll
	rps -> ??? (MiraiCQ与mha都不支持)
	dice -> Dice
	shake -> ??? (MiraiCQ与mha都不支持)
	poke -> Poke
	anonymous -> ??? (MiraiCQ与mha都不支持)
	share -> ??? (MiraiCQ与mha都不支持)
	contact ->??? (MiraiCQ与mha都不支持)
	location -> ??? (MiraiCQ与mha都不支持)
	music -> MusicShare (仅支持自定义音乐分享)
	reply -> (MiraiCQ与mha都不支持)
	forward -> ForwardMessage (MiraiCQ暂不支持)
	xml -> Xml
	json -> Json
*/
static Json::Value deal_mha_msg(const Json::Value& chain)
{
	Json::Value ret_arr = Json::arrayValue;
	if (!chain.isArray())
	{
		return ret_arr;
	}
	for (size_t i = 0; i < chain.size(); i++)
	{
		string type = chain[i]["type"].asString();
		if (type == "Plain")
		{
			Json::Value node;
			node["type"] = "text";
			node["data"]["text"] = chain[i]["text"].asString();
			ret_arr.append(node);
		}
		else if (type == "Face")
		{
			Json::Value node;
			node["type"] = "face";
			node["data"]["id"] = to_string(chain[i]["faceId"].asInt());
			ret_arr.append(node);
		}
		else if (type == "Image")
		{
			Json::Value node;
			node["type"] = "image";
			/* MiraiCQ中的图片只需要url参数即可(并且此url必须为txqq的ur) */
			node["data"]["url"] = chain[i]["url"].asString();
			node["data"]["file"] = chain[i]["imageId"].asString();
			ret_arr.append(node);
		}
		else if (type == "FlashImage")
		{
			Json::Value node;
			node["type"] = "image";
			/* MiraiCQ中的图片只需要url参数即可(并且此url必须为txqq的ur) */
			node["data"]["url"] = chain[i]["url"].asString();
			node["data"]["type"] = "flash";
			node["data"]["file"] = chain[i]["imageId"].asString();
			ret_arr.append(node);
		}
		else if (type == "At")
		{
			Json::Value node;
			node["type"] = "at";
			node["data"]["qq"] = to_string(chain[i]["target"].asInt64());
			ret_arr.append(node);
		}
		else if (type == "AtAll")
		{
			Json::Value node;
			node["type"] = "at";
			node["data"]["qq"] = "all";
			ret_arr.append(node);
		}
		else if (type == "Dice")
		{
			Json::Value node;
			node["type"] = "dice";
			node["data"] = Json::objectValue;
			ret_arr.append(node);
		}
		else if (type == "Poke")
		{
			Json::Value node;
			node["type"] = "poke";
			string name = chain[i]["name"].asString();
			if (name == "Poke")
			{
				node["data"]["type"] = 1;
				node["data"]["id"] = -1;
			}
			else if (name == "ShowLove")
			{
				node["data"]["type"] = 2;
				node["data"]["id"] = -1;
			}
			else if (name == "ShowLove")
			{
				node["data"]["type"] = 3;
				node["data"]["id"] = -1;
			}
			else if (name == "Heartbroken")
			{
				node["data"]["type"] = 4;
				node["data"]["id"] = -1;
			}
			else if (name == "SixSixSix")
			{
				node["data"]["type"] = 5;
				node["data"]["id"] = -1;
			}
			else if (name == "FangDaZhao")
			{
				node["data"]["type"] = 6;
				node["data"]["id"] = -1;
			}
			else
			{
				/* 对于不支持的类型，按照普通戳一戳处理 */
				node["data"]["type"] = 1;
				node["data"]["id"] = -1;
			}
			ret_arr.append(node);
		}
		else if (type == "MusicShare")
		{
			Json::Value node;
			node["type"] = "music";
			node["data"]["type"] = "custom";
			node["data"]["url"] = chain[i]["jumpUrl"].asString();
			node["data"]["audio"] = chain[i]["musicUrl"].asString();
			node["data"]["title"] = chain[i]["title"].asString();
			node["data"]["content"] = chain[i]["brief"].asString();
			node["data"]["image"] = chain[i]["pictureUrl"].asString();
			ret_arr.append(node);
		}
		else if (type == "Xml")
		{
			Json::Value node;
			node["type"] = "xml";
			node["data"]["data"] = chain[i]["xml"].asString();
			ret_arr.append(node);
		}
		else if (type == "Json")
		{
			Json::Value node;
			node["type"] = "json";
			node["data"]["data"] = chain[i]["json"].asString();
			ret_arr.append(node);
		}
		else
		{
			MiraiLog::get_instance()->add_debug_log("MiraiHttp", "将忽略未知消息类型: " + type);
		}
	}
	return ret_arr;
}


static MiraiNet::NetStruct deal_group_message(const MiraiNet::NetStruct & root)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value());
	auto & chain = (*root)["data"]["messageChain"];
	auto& sender = (*root)["data"]["sender"];

	(*ret_json)["time"] = chain[0]["time"].asInt64();
	//(*ret_json)["self_id"] = 0; /* 从mha中无法得到self_id
	(*ret_json)["post_type"] = "message";
	(*ret_json)["message_type"] = "group";
	if (sender["id"].asInt64() == 80000000)
	{
		/* 说明是匿名消息 */
		(*ret_json)["sub_type"] = "anonymous";
	}else
	{
		(*ret_json)["sub_type"] = "normal";
		/* mha不支持onebot中的`系统提示（如「管理员已禁止群内匿名聊天」）是 notice`*/
		/* 酷Q插件也用不到*/
	}
	(*ret_json)["message_id"] = chain[0]["id"].asInt();
	(*ret_json)["group_id"] = sender["group"]["id"].asInt64();
	(*ret_json)["user_id"] = sender["id"].asInt64();
	if ((*ret_json)["sub_type"].asString() == "anonymous")
	{
		/* 匿名消息 */
		Json::Value anonymous;
		anonymous["id"] = 80000000;
		anonymous["name"] = sender["memberName"].asString();
		anonymous["flag"] = ""; /* mha中匿名用户不能得到flag，也就是说，不能被禁言 */
		(*ret_json)["anonymous"] = anonymous;
	}
	/* 把mha的chain变成cq的array */
	(*ret_json)["message"] = deal_mha_msg(chain);
	(*ret_json)["raw_message"] = StrTool::jsonarr_to_cq_str((*ret_json)["message"]);
	/* mha中没有字体 */
	(*ret_json)["font"] = 0;
	(*ret_json)["sender"]["user_id"] = sender["id"].asInt64();
	/* mha中没有昵称，所以使用群名片代替 */
	(*ret_json)["sender"]["nickname"] = sender["memberName"].asString();
	/* mha没有性别、年龄 */
	(*ret_json)["sender"]["sex"] = "unknown";
	(*ret_json)["sender"]["age"] = 0;
	return ret_json;
}


static MiraiNet::NetStruct deal_private_message(const MiraiNet::NetStruct& root)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value());
	auto& chain = (*root)["data"]["messageChain"];
	auto& sender = (*root)["data"]["sender"];

	(*ret_json)["time"] = chain[0]["time"].asInt64();
	//(*ret_json)["self_id"] = 0; /* 从mha中无法得到self_id
	(*ret_json)["post_type"] = "message";
	(*ret_json)["message_type"] = "private";
	if ((*root)["data"]["type"].asString() == "FriendMessage")
	{
		(*ret_json)["sub_type"] = "friend";
	}
	else if ((*root)["data"]["type"].asString() == "TempMessage")
	{
		(*ret_json)["sub_type"] = "group";
	}
	else
	{
		/* 不处理其它情况 */
	}
	(*ret_json)["message_id"] = chain[0]["id"].asInt();
	(*ret_json)["user_id"] = sender["id"].asInt64();
	
	/* 把mha的chain变成cq的array */
	(*ret_json)["message"] = deal_mha_msg(chain);
	(*ret_json)["raw_message"] = StrTool::jsonarr_to_cq_str((*ret_json)["message"]);
	/* mha中没有字体 */
	(*ret_json)["font"] = 0;
	(*ret_json)["sender"]["user_id"] = sender["id"].asInt64();
	/* mha中没有昵称，所以使用群名片代替 */
	if ((*ret_json)["sub_type"].asString() == "friend")
	{
		(*ret_json)["sender"]["nickname"] = sender["nickname"].asString();
	}
	else if((*ret_json)["sub_type"].asString() == "group")
	{
		(*ret_json)["sender"]["nickname"] = sender["memberName"].asString();
	}
	
	/* mha没有性别、年龄 */
	(*ret_json)["sender"]["sex"] = "unknown";
	(*ret_json)["sender"]["age"] = 0;
	return ret_json;
}


MiraiNet::NetStruct MiraiHttpEventDeal::deal_event(const Json::Value & root_,std::mutex & mx_msg_id_vec, std::list<std::pair<int, int>> & msg_id_vec, int& curr_msg_id)
{
	auto root = MiraiNet::NetStruct(new Json::Value(root_));
	MiraiNet::NetStruct ret_json;
	string type = (*root)["data"]["type"].asString();
	if (type == "GroupMessage")
	{
		ret_json = deal_group_message(root);
	}
	else if (type == "FriendMessage" || type == "TempMessage")
	{
		ret_json = deal_private_message(root);
	}
	return ret_json;
}
