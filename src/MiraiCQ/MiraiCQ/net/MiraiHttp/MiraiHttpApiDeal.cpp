#include "MiraiHttpApiDeal.h"
#include "../../tool/StrTool.h"
#include "../../log/MiraiLog.h"

using namespace std;

MiraiHttpApiDeal::MiraiHttpApiDeal()
{
}

MiraiHttpApiDeal::~MiraiHttpApiDeal()
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

static Json::Value deal_cq_msg(const Json::Value& chain)
{
	Json::Value ret_arr = Json::arrayValue;
	string s = chain.toStyledString();
	for (size_t i = 0; i < chain.size(); i++)
	{
		string type = chain[i]["type"].asString();
		if (type == "text")
		{
			Json::Value node;
			node["type"] = "Plain";
			node["text"] = chain[i]["data"]["text"].asString();
			ret_arr.append(node);
		}
		else if (type == "at")
		{
			Json::Value node;
			node["type"] = "At";
			node["target"] = stoll(chain[i]["data"]["qq"].asString());
			ret_arr.append(node);
		}
		else if (type == "face")
		{
			Json::Value node;
			node["type"] = "Face";
			node["faceId"] = stoi(chain[i]["data"]["id"].asString());
			ret_arr.append(node);
		}
		else if (type == "image")
		{
			string stype = StrTool::get_str_from_json(chain[i]["data"], "type", "");
			Json::Value node;
			if (stype == "")
			{
				/* 普通文件 */
				node["type"] = "Image";
			}
			else if (stype == "flash")
			{
				/* 闪照 */
				node["type"] = "FlashImage";
			}
			else
			{
				MiraiLog::get_instance()->add_debug_log("MiraiHttp", "将忽略未知图片类型: " + stype);
				continue;
			}
			string file = chain[i]["data"]["file"].asString();
			/* MiraiCQ只会以这两种方式发图 */
			if (file.at(0) == 'h')
			{
				/* 说明是以url发送 */
				node["url"] = chain[i]["data"]["file"].asString();
			}
			else
			{
				/* 说明是以base64发送 */
				node["base64"] = chain[i]["data"]["file"].asString().substr(9);
			}
			ret_arr.append(node);
		}
		else if (type == "dice")
		{
			Json::Value node;
			node["type"] = "Dice";
			/* 这里随机点数 */
			node["value"] = time(NULL) % 6 + 1;
			ret_arr.append(node);
		}
		else if (type == "poke")
		{
			Json::Value node;
			node["type"] = "Poke";
			string stype = chain[i]["data"]["type"].asString();
			if (stype == "1")
			{
				node["name"] = "Poke";
			}
			else if (stype == "2")
			{
				node["name"] = "ShowLove";
			}
			else if (stype == "3")
			{
				node["name"] = "Like";
			}
			else if (stype == "4")
			{
				node["name"] = "Heartbroken";
			}
			else if (stype == "5")
			{
				node["name"] = "SixSixSix";
			}
			else if (stype == "6")
			{
				node["name"] = "FangDaZhao";
			}
			else
			{
				MiraiLog::get_instance()->add_debug_log("MiraiHttp", "将忽略未知Poke类型: " + stype);
				continue;
			}
			ret_arr.append(node);
		}
		else if (type == "music")
		{
			Json::Value node;
			node["type"] = "MusicShare";
			node["kind"] = "custom";
			node["title"] = StrTool::get_str_from_json(chain[i]["data"],"title","");
			node["summary"] = StrTool::get_str_from_json(chain[i]["data"], "content", "");
			node["jumpUrl"] = StrTool::get_str_from_json(chain[i]["data"], "url", "");
			node["pictureUrl"] = StrTool::get_str_from_json(chain[i]["data"], "image", "");
			node["musicUrl"] = StrTool::get_str_from_json(chain[i]["data"], "audio", "");
			node["brief"] = StrTool::get_str_from_json(chain[i]["data"], "content", "");
			ret_arr.append(node);
		}
		else if (type == "xml")
		{
			Json::Value node;
			node["type"] = "Xml";
			node["xml"] = node["data"]["data"].asString();
			ret_arr.append(node);
		}
		else if (type == "json")
		{
			Json::Value node;
			node["type"] = "Json";
			node["json"] = node["data"]["data"].asString();
			ret_arr.append(node);
		}
		else
		{
			MiraiLog::get_instance()->add_debug_log("MiraiHttp", "将忽略未知消息类型: " + type);
		}
	}
	return ret_arr;
}
MiraiNet::NetStruct MiraiHttpApiDeal::deal_api(const Json::Value& root,std::mutex& mx_msg_id_vec, std::list<std::pair<int, int>>& msg_id_vec, int& curr_msg_id)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	string action = root["action"].asString();
	if (action == "send_group_msg")
	{
		(*ret_json)["action"] = "sendGroupMessage";
		(*ret_json)["target"] = root["params"]["group_id"].asInt64();
		(*ret_json)["messageChain"] = deal_cq_msg(root["params"]["message"]);
	}
	else if(action == "send_private_msg")
	{
		(*ret_json)["action"] = "sendFriendMessage";
		(*ret_json)["target"] = root["params"]["user_id"].asInt64();
		(*ret_json)["messageChain"] = deal_cq_msg(root["params"]["message"]);
	}
	else
	{
		ret_json = nullptr;
	}
	return ret_json;
}

MiraiNet::NetStruct MiraiHttpApiDeal::deal_recv(const Json::Value & root, std::mutex& mx_msg_id_vec, std::list<std::pair<int, int>>& msg_id_vec, int& curr_msg_id)
{
	auto ret_json = MiraiNet::NetStruct(new Json::Value(root));
	if (root["code"].asInt() != 0)
	{
		return nullptr;
	}
	(*ret_json)["retcode"] = 0;
	(*ret_json)["status"] = "ok";
	(*ret_json)["data"]["message_id"] = root["messageId"].asInt();
	return ret_json;
}
