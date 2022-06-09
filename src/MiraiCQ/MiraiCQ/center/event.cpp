#include "center.h"

#include <cassert>
#include <Windows.h>
#include "../log/MiraiLog.h"
#include "../tool/PathTool.h"
#include "../tool/StrTool.h"
#include "../tool/TimeTool.h"
#include "../tool/BinTool.h"
#include "../tool/ImgTool.h"
#include "../tool/MsgIdTool.h"
#include <websocketpp/base64/base64.hpp>
#include "../tool/IPCTool.h"
#include "../scriptrun/ScriptRun.h"


using namespace std;

void Center::deal_event(MiraiNet::NetStruct evt) 
{
	assert(evt);
	MiraiLog::get_instance()->add_debug_log("Center", "收到的消息:"+ StrTool::to_ansi(Json::FastWriter().write(*evt)));
	if (!(*evt).isObject())
	{
		return;
	}
	auto post_type = StrTool::get_str_from_json((*evt), "post_type", "");
	if (post_type == "")
	{
		MiraiLog::get_instance()->add_warning_log("Center", "post_type不存在");
		/* 失败，不再继续处理 */
		return;
	}
	bool is_pass = ScriptRun::get_instance()->onebot_event_filter(Json::FastWriter().write(*evt).c_str());
	if (!is_pass) {
		/* 被过滤，不再继续处理 */
		return;
	}
	try
	{
		/* 1207号事件暂时单独处理 */
		deal_1207_event(*evt);

		deal_ex_event(*evt);

		if (post_type == "message")
		{
			deal_type_message(*evt);
		}
		else if (post_type == "notice")
		{
			deal_type_notice(*evt);
		}
		else if (post_type == "request")
		{
			deal_type_request(*evt);
		}
		else if (post_type == "meta_event")
		{
			deal_type_meta_event(*evt);
		}
		else
		{
			MiraiLog::get_instance()->add_warning_log("Center", "未知的post_type:"+ post_type);
		}
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_warning_log("Center", string("处理事件时发送错误：") + e.what());
	}

}

static void IPC_SendEvent_T(const char * msg)
{
	if (!msg) {
		return ;
	}
	auto plus = MiraiPlus::get_instance()->get_all_plus();
	for (auto p : plus) {
		if (p.second->is_enable()) 
		{
			IPC_SendEvent(p.second->get_uuid().c_str(), msg);
		}
	}
}

void Center::deal_type_message(Json::Value& evt)
{
	// 处理message事件中的message_id
	Json::Value webid = evt.get("message_id", Json::Value());
	evt["message_id"] = MsgIdTool::getInstance()->to_cqid(webid);

	auto message_type = StrTool::get_str_from_json(evt, "message_type", "");
	if (message_type == "private")
	{
		deal_type_message_private(evt);
	}
	else if (message_type == "group")
	{
		deal_type_message_group(evt);
	}
	else
	{
		MiraiLog::get_instance()->add_warning_log("Center", "未知的message_type:"+ message_type);
	}
}

void Center::deal_type_notice(Json::Value& evt)
{
	auto notice_type = StrTool::get_str_from_json(evt, "notice_type", "");
	if (notice_type == "group_upload")
	{
		deal_type_notice_group_upload(evt);
	}
	else if (notice_type == "group_admin")
	{
		deal_type_notice_group_admin(evt);
	}
	else if (notice_type == "group_decrease")
	{
		deal_type_notice_group_decrease(evt);
	}
	else if (notice_type == "group_increase")
	{
		deal_type_notice_group_increase(evt);
	}
	else if (notice_type == "group_ban")
	{
		deal_type_notice_group_ban(evt);
	}
	else if (notice_type == "friend_add")
	{
		deal_type_notice_friend_add(evt);
	}
	else
	{
		//MiraiLog::get_instance()->add_debug_log("Center", "未知的notice_type:"+ notice_type);
	}
}

void Center::deal_type_notice_group_upload(Json::Value& evt)
{
	std::string file_base64;

	Json::Value file = evt.get("file", Json::Value());
	if (!file.isObject())
	{
		MiraiLog::get_instance()->add_warning_log("Center", "错误的deal_type_notice_group_upload 事件");
		return;
	}
	std::string id = StrTool::to_ansi(StrTool::get_str_from_json(file, "id", ""));
	std::string name = StrTool::to_ansi(StrTool::get_str_from_json(file, "name", ""));
	int64_t size = StrTool::get_int64_from_json(file, "size", 0);
	int64_t busid = StrTool::get_int64_from_json(file, "busid", 0);
	BinTool bin_pack;
	bin_pack.string_push(id);
	bin_pack.string_push(name);
	bin_pack.int64_push(size);
	bin_pack.int64_push(busid);
	file_base64 = websocketpp::base64_encode((const unsigned char*)(&(bin_pack.content[0])), bin_pack.content.size());
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);

	Json::Value to_send;
	to_send["event_type"] = "cq_event_group_upload";
	to_send["data"]["sub_type"] = 1;
	to_send["data"]["send_time"] = time_;
	to_send["data"]["from_group"] = group_id;
	to_send["data"]["from_qq"] = user_id;
	to_send["data"]["file_base64"] = file_base64;
	IPC_SendEvent_T(Json::FastWriter().write(to_send).c_str());
}

void Center::deal_type_notice_group_admin(Json::Value& evt)
{
	std::string subtype = StrTool::to_ansi(StrTool::get_str_from_json(evt, "sub_type", ""));
	__int32 sub_type;
	if (subtype == "set")
	{
		sub_type = 2;
	}
	else
	{
		sub_type = 1;
	}
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);

	Json::Value to_send;
	to_send["event_type"] = "cq_event_group_admin";
	to_send["data"]["sub_type"] = sub_type;
	to_send["data"]["send_time"] = time_;
	to_send["data"]["from_group"] = group_id;
	to_send["data"]["being_operate_qq"] = user_id;
	IPC_SendEvent_T(Json::FastWriter().write(to_send).c_str());
}

void Center::deal_type_notice_group_decrease(Json::Value& evt)
{
	std::string subtype = StrTool::to_ansi(StrTool::get_str_from_json(evt, "sub_type", ""));
	__int32 sub_type;
	if (subtype == "leave")
	{
		sub_type = 1;
	}
	else if (subtype == "kick")
	{
		sub_type = 2;
	}
	else
	{
		sub_type = 3; //kick self
	}
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
	int64_t operator_id = StrTool::get_int64_from_json(evt, "operator_id", 0);

	Json::Value to_send;
	to_send["event_type"] = "cq_event_group_member_decrease";
	to_send["data"]["sub_type"] = sub_type;
	to_send["data"]["send_time"] = time_;
	to_send["data"]["from_group"] = group_id;
	to_send["data"]["from_qq"] = operator_id;
	to_send["data"]["being_operate_qq"] = user_id;
	IPC_SendEvent_T(Json::FastWriter().write(to_send).c_str());
}

void Center::deal_type_notice_group_increase(Json::Value& evt)
{
	std::string subtype = StrTool::to_ansi(StrTool::get_str_from_json(evt, "sub_type", ""));
	__int32 sub_type;
	if (subtype == "approve")
	{
		sub_type = 1;
	}
	else
	{
		sub_type = 2; //invite
	}
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
	int64_t operator_id = StrTool::get_int64_from_json(evt, "operator_id", 0);

	Json::Value to_send;
	to_send["event_type"] = "cq_event_group_member_increase";
	to_send["data"]["sub_type"] = sub_type;
	to_send["data"]["send_time"] = time_;
	to_send["data"]["from_group"] = group_id;
	to_send["data"]["from_qq"] = operator_id;
	to_send["data"]["being_operate_qq"] = user_id;
	IPC_SendEvent_T(Json::FastWriter().write(to_send).c_str());
}

void Center::deal_type_notice_group_ban(Json::Value& evt)
{
	std::string subtype = StrTool::to_ansi(StrTool::get_str_from_json(evt, "sub_type", ""));
	__int32 sub_type;
	if (subtype == "lift_ban")
	{
		sub_type = 1;
	}
	else
	{
		sub_type = 2; //ban
	}
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
	int64_t operator_id = StrTool::get_int64_from_json(evt, "operator_id", 0);
	int64_t duration = StrTool::get_int64_from_json(evt, "duration", 0);

	Json::Value to_send;
	to_send["event_type"] = "cq_event_group_ban";
	to_send["data"]["sub_type"] = sub_type;
	to_send["data"]["send_time"] = time_;
	to_send["data"]["from_group"] = group_id;
	to_send["data"]["from_qq"] = operator_id;
	to_send["data"]["being_operate_qq"] = user_id;
	to_send["data"]["duration"] = duration;
	IPC_SendEvent_T(Json::FastWriter().write(to_send).c_str());
}

void Center::deal_type_notice_friend_add(Json::Value& evt)
{
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);

	Json::Value to_send;
	to_send["event_type"] = "cq_event_friend_add";
	to_send["data"]["sub_type"] = 1;
	to_send["data"]["send_time"] = time_;
	to_send["data"]["from_qq"] = user_id;
	IPC_SendEvent_T(Json::FastWriter().write(to_send).c_str());
}

void Center::deal_type_request_friend(Json::Value& evt)
{
	std::string comment = StrTool::to_ansi(StrTool::get_str_from_json(evt, "comment", ""));
	std::string response_flag = StrTool::to_ansi(StrTool::get_str_from_json(evt, "flag", ""));
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);

	Json::Value to_send;
	to_send["event_type"] = "cq_event_friend_request";
	to_send["data"]["sub_type"] = 1;
	to_send["data"]["send_time"] = time_;
	to_send["data"]["from_qq"] = user_id;
	to_send["data"]["msg"] = comment;
	to_send["data"]["response_flag"] = response_flag;
	IPC_SendEvent_T(Json::FastWriter().write(to_send).c_str());
}

void Center::deal_type_request_group(Json::Value& evt)
{
	std::string comment = StrTool::to_ansi(StrTool::get_str_from_json(evt, "comment", ""));
	std::string response_flag = StrTool::to_ansi(StrTool::get_str_from_json(evt, "flag", ""));
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
	std::string subtype = StrTool::to_ansi(StrTool::get_str_from_json(evt, "sub_type", ""));
	__int32 sub_type;
	if (subtype == "add")
	{
		sub_type = 1;
	}
	else
	{
		sub_type = 2; //invite
	}

	Json::Value to_send;
	to_send["event_type"] = "cq_event_group_request";
	to_send["data"]["sub_type"] = sub_type;
	to_send["data"]["send_time"] = time_;
	to_send["data"]["from_group"] = group_id;
	to_send["data"]["from_qq"] = user_id;
	to_send["data"]["msg"] = comment;
	to_send["data"]["response_flag"] = response_flag;
	IPC_SendEvent_T(Json::FastWriter().write(to_send).c_str());
}

void Center::deal_type_request(Json::Value& evt)
{
	std::string request_type = StrTool::to_ansi(StrTool::get_str_from_json(evt, "request_type", ""));
	if (request_type == "friend")
	{
		deal_type_request_friend(evt);
	}
	else if (request_type == "group")
	{
		deal_type_request_group(evt);
	}
	else
	{
		MiraiLog::get_instance()->add_warning_log("Center", "未知的request_type:" + request_type);
	}
}

void Center::deal_type_meta_event(Json::Value& evt)
{
}

static std::string get_md5_from_imgurl(const std::string & url)
{
	auto ret_vec = StrTool::match(url, "http.*?-([a-zA-Z0-9]{32}).*");
	if (ret_vec.size() >= 2)
	{
		return StrTool::toupper(ret_vec[1]);
	}
	else
	{
		return "";
	}
}

static std::string get_md5_from_file_str(const std::string& file_str)
{
	auto ret_vec = StrTool::match(file_str, "^([a-zA-Z0-9]{32}).*");
	if (ret_vec.size() >= 2)
	{
		return StrTool::toupper(ret_vec[1]);
	}
	else
	{
		return "";
	}
}


/* 判断url是否是qq的url */
static bool is_qq_url(const Json::Value & dat_json) 
{
	std::string url = StrTool::get_str_from_json(dat_json, "url", "");
	if (url.find("gchat") != url.npos) {
		return true;
	}
	if (url.find("c2cpicdw") != url.npos) {
		return true;
	}
	return false;
}

static bool deal_json_array(Json::Value & json_arr)
{
	if (!json_arr.isArray())
	{
		return false;
	}
	for (auto& node : json_arr)
	{
		if (!node.isObject())
		{
			/* 不是object,说明json格式错误,将这节消息删除 */
			node = Json::Value();
			continue;
		}
		std::string type_str = StrTool::get_str_from_json(node, "type", "");
		if (type_str == "")
		{
			/* 将这节消息删除 */
			node = Json::Value();
			continue;
		}
		if (type_str == "image")
		{
			Json::Value dat_json = node.get("data", Json::Value());
			std::string md5_str;
			std::string url;
			std::string file_str = StrTool::get_str_from_json(dat_json, "file", "");
			if (is_qq_url(dat_json)) {
				md5_str = get_md5_from_file_str(file_str);
				if (md5_str == "") {
					md5_str = get_md5_from_imgurl(StrTool::get_str_from_json(dat_json, "url", ""));
				}
				url = "https://gchat.qpic.cn/gchatpic_new/0/0-0-" + md5_str + "/0?term=2";
			}
			else {
				md5_str = get_md5_from_file_str(file_str);
				url = StrTool::get_str_from_json(dat_json, "url", "");
			}
			
			if (md5_str == "") 
			{
				MiraiLog::get_instance()->add_warning_log("Center", "无法从file字段获取图片的md5");
				node = Json::Value();
				continue;
			}

			if (url == "") {
				MiraiLog::get_instance()->add_warning_log("Center", "无法构造图片url");
				node = Json::Value();
				continue;
			}
			
			/* 获得图片信息需要下载一部分图片 */
			ImgTool::ImgInfo info;
			/* 这里进行两次尝试，增大成功概率 */
			if (!ImgTool::get_info(url, info) && !ImgTool::get_info(url, info))
			{
				MiraiLog::get_instance()->add_warning_log("Center", "无法从url中获取图片信息:" + url);
				/* 
					即使无法获得图片信息，也需要写入url,md5等信息到cqimg文件
					node = Json::Value();
					continue;
				*/
				/* 无法获得图片信息，则类型使用image，之后若使用`CQ_getImage`获取图片，可能会没有正确的后缀名 */
				info.height = info.width = info.size = 0;
				info.type = "image";
			}
			std::string cqimg_name = md5_str + "." + info.type;
			/* 创建目录 */
			std::string exe_dir = PathTool::get_exe_dir();
			PathTool::create_dir(exe_dir + "data");
			PathTool::create_dir(exe_dir + "data\\image");
			std::string cqimg_path = exe_dir + "data\\image\\" + cqimg_name + ".cqimg";
			/* 此处将图片信息直接写入cqimg文件即可 */
			WritePrivateProfileStringA("image", "md5", md5_str.c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("image", "width", std::to_string(info.width).c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("image", "height", std::to_string(info.height).c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("image", "size", std::to_string(info.size).c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("image", "url", url.c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("image", "addtime", std::to_string(time(0)).c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("notice", "致开发者", "由于消息服务器的更新，本文件中的 url 已被弃用，并将在未来被移除，请勿直接读取本文件。请更新至最新版 SDK，并使用 Api（CQ_getImage, 接收图片）读取本图片。", cqimg_path.c_str());
			if (!PathTool::is_file_exist(cqimg_path))
			{
				/* 再次检查cqmig是否存在 */
				MiraiLog::get_instance()->add_warning_log("Center", "cqimg文件写入失败，检查`data\\image`下的文件权限");
				node = Json::Value();
				continue;
			}
			/* 重新构造data字段，删除多余的键 */
			Json::Value v;
			v["file"] = cqimg_name;
			node["data"] = v;
		}
		else if (type_str == "reply")
		{
			/* 这里将web的msg_id变为cq的msgid */
			Json::Value id_json = node["data"]["id"];
			node["data"]["id"] = MsgIdTool::getInstance()->to_cqid(id_json);
		}
		//else if (type_str == "face") 
		//{
		//	// [CQ:face,id=324,type=sticker] -> [CQ:face,id=324]
		//	Json::Value id_json;
		//	id_json["id"] = node["data"]["id"];
		//	node["data"] = id_json;
		//}
	}
	return true;
}

void Center::deal_type_message_private(Json::Value& evt)
{
	Json::Value jsonarr = evt.get("message", Json::Value());
	/* 处理json array,比如要生成cqimg文件，或者要将多余的字段去掉 */
	if (!deal_json_array(jsonarr))
	{
		MiraiLog::get_instance()->add_warning_log("Center", "jsonarr预处理失败");
		return;
	}
	auto s = Json::FastWriter().write(jsonarr);
	std::string cq_str = StrTool::to_ansi(StrTool::jsonarr_to_cq_str(jsonarr,0));
	if (cq_str == "")
	{
		return;
	}
	//MiraiLog::get_instance()->add_debug_log("Center", "传入PrivateEvent的Msg:\n"+ cq_str);
	std::string sub_type_str = StrTool::to_ansi(StrTool::get_str_from_json(evt, "sub_type", ""));
	int subtype_int;
	if (sub_type_str == "friend")
	{
		subtype_int = 11;
	}
	else if (sub_type_str == "group")
	{
		subtype_int = 2;
	}
	else if (sub_type_str == "other")
	{
		subtype_int = 1; //coolq中表示来自在线状态
		return;
	}
	else
	{
		MiraiLog::get_instance()->add_warning_log("Center", "未知的私聊消息来源:"+ sub_type_str +"，不进行处理");
		return;
	}
	int message_id = StrTool::get_int_from_json(evt, "message_id", 0);
	int font = StrTool::get_int_from_json(evt, "font", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);

	Json::Value to_send;
	to_send["event_type"] = "cq_event_private_message";
	to_send["data"]["sub_type"] = subtype_int;
	to_send["data"]["msg_id"] = message_id;
	to_send["data"]["from_qq"] = user_id;
	to_send["data"]["msg"] = cq_str;
	to_send["data"]["font"] = font;
	IPC_SendEvent_T(Json::FastWriter().write(to_send).c_str());
}



void Center::deal_type_message_group(Json::Value& evt)
{
	std::string from_anonymous_base64;
	Json::Value anonymous = evt.get("anonymous", Json::Value());
	if (anonymous.isObject())
	{
		int64_t id = StrTool::get_int64_from_json(anonymous, "id", 0);
		std::string name = StrTool::to_ansi(StrTool::get_str_from_json(anonymous, "name", ""));
		std::string flag = StrTool::to_ansi(StrTool::get_str_from_json(anonymous, "flag", ""));
		BinTool bin_pack;
		bin_pack.int64_push(id);
		bin_pack.string_push(name);
		bin_pack.token_push(std::vector<char>(flag.begin(), flag.end()));
		from_anonymous_base64 = websocketpp::base64_encode((const unsigned char*)(&(bin_pack.content[0])), bin_pack.content.size());
	}
	Json::Value jsonarr = evt.get("message", Json::Value());
	/* 处理json array,比如要生成cqimg文件，或者要将多余的字段去掉 */
	if (!deal_json_array(jsonarr))
	{
		MiraiLog::get_instance()->add_warning_log("Center", "jsonarr预处理失败");
		return;
	}
	auto s = Json::FastWriter().write(jsonarr);
	std::string cq_str = StrTool::to_ansi(StrTool::jsonarr_to_cq_str(jsonarr, 0));
	if (cq_str == "")
	{
		return;
	}
	//MiraiLog::get_instance()->add_debug_log("Center", "传入GroupEvent的Msg:\n" + cq_str);
	int message_id = StrTool::get_int_from_json(evt, "message_id", 0);
	int font = StrTool::get_int_from_json(evt, "font", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
	int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);


	Json::Value to_send;
	to_send["event_type"] = "cq_event_group_message";
	to_send["data"]["sub_type"] = 1;
	to_send["data"]["msg_id"] = message_id;
	to_send["data"]["from_group"] = group_id;
	to_send["data"]["from_qq"] = user_id;
	to_send["data"]["anonymous"] = from_anonymous_base64;
	to_send["data"]["msg"] = cq_str;
	to_send["data"]["font"] = font;
	IPC_SendEvent_T(Json::FastWriter().write(to_send).c_str());
}


void Center::deal_1207_event(Json::Value& evt)
{
	Json::Value to_send;
	to_send["event_type"] = "cq_1207_event";
	to_send["data"]["msg"] = Json::FastWriter().write(evt);
	IPC_SendEvent_T(Json::FastWriter().write(to_send).c_str());
}

