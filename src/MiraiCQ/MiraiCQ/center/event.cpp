#include "center.h"

#include <cassert>
#include <Windows.h>
#include <regex>
#include "../log/MiraiLog.h"
#include "../tool/PathTool.h"
#include "../tool/StrTool.h"
#include "../tool/TimeTool.h"
#include "../tool/BinTool.h"
#include "../tool/ImgTool.h"
#include <base64/base64.h>


using namespace std;

void Center::deal_event(MiraiNet::NetStruct evt) noexcept
{
	assert(evt);
	const std::string print_utf8_str = evt->toStyledString();
	const std::string print_ansi_str = StrTool::to_ansi(print_utf8_str);
	MiraiLog::get_instance()->add_debug_log("Center", print_ansi_str);
	Json::Value ansi_json;
	Json::Reader reader;
	if (!reader.parse(print_ansi_str, ansi_json))
	{
		MiraiLog::get_instance()->add_debug_log("Center", "jsonת��Ϊansiʧ��");
		/* ʧ�ܣ����ټ������� */
		return;
	}
	auto post_type = StrTool::get_str_from_json(ansi_json, "post_type", "");
	if (post_type == "")
	{
		MiraiLog::get_instance()->add_debug_log("Center", "post_type������");
		/* ʧ�ܣ����ټ������� */
		return;
	}
	try
	{
		if (post_type == "message")
		{
			deal_type_message(ansi_json);
		}
		else if (post_type == "notice")
		{
			deal_type_notice(ansi_json);
		}
		else if (post_type == "request")
		{
			deal_type_request(ansi_json);
		}
		else if (post_type == "meta_event")
		{
			deal_type_meta_event(ansi_json);
		}
		else
		{
			MiraiLog::get_instance()->add_debug_log("Center", "δ֪��post_type");
		}
	}
	catch (const std::exception& e)
	{
		MiraiLog::get_instance()->add_debug_log("Center", string("�����¼�ʱ���ʹ���") + e.what());
	}

}

void Center::deal_type_message(Json::Value& evt)
{
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
		MiraiLog::get_instance()->add_debug_log("Center", "δ֪��message_type");
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
		MiraiLog::get_instance()->add_debug_log("Center", "δ֪��notice_type");
	}
}

void Center::deal_type_notice_group_upload(Json::Value& evt)
{
	std::string file_base64;

	Json::Value file = evt.get("file",Json::nullValue);
	if (!file.isObject())
	{
		MiraiLog::get_instance()->add_debug_log("Center", "�����deal_type_message_group_upload �¼�");
		return;
	}
	std::string id = StrTool::get_str_from_json(file, "id", "");
	std::string name = StrTool::get_str_from_json(file, "name", "");
	int64_t size = StrTool::get_int64_from_json(file, "size", 0);
	int64_t busid = StrTool::get_int64_from_json(file, "busid", 0);
	BinTool bin_pack;
	bin_pack.string_push(id);
	bin_pack.string_push(name);
	bin_pack.int64_push(size);
	bin_pack.int64_push(busid);
	file_base64 = base64_encode((const unsigned char*)(&(bin_pack.content[0])), bin_pack.content.size());
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
	/* �����¼����� */
	normal_cal_plus_fun(11, [&](const void* fun_ptr, void* user_data)->int {
		typedef int(__stdcall* cq_event_group_upload)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, const char* file_base64);
		return ((cq_event_group_upload)fun_ptr)(1, time_, group_id, user_id, file_base64.c_str());
	}, 0);
	
}

void Center::deal_type_notice_group_admin(Json::Value& evt)
{
	std::string subtype = StrTool::get_str_from_json(evt, "sub_type", "");
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
	/* �����¼����� */
	normal_cal_plus_fun(101, [&](const void* fun_ptr, void* user_data)->int {
		typedef int(__stdcall* cq_event_group_admin)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 being_operate_qq);
		return ((cq_event_group_admin)fun_ptr)(sub_type, time_, group_id, user_id);
	}, 0);
}

void Center::deal_type_notice_group_decrease(Json::Value& evt)
{
	std::string subtype = StrTool::get_str_from_json(evt, "sub_type", "");
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
	/* �����¼����� */
	normal_cal_plus_fun(102, [&](const void* fun_ptr, void* user_data)->int {
		typedef int(__stdcall* cq_event_group_member_decrease)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, __int64 being_operate_qq);
		return ((cq_event_group_member_decrease)fun_ptr)(sub_type, time_, group_id, user_id, operator_id);
	}, 0);
}

void Center::deal_type_notice_group_increase(Json::Value& evt)
{
	std::string subtype = StrTool::get_str_from_json(evt, "sub_type", "");
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
	/* �����¼����� */
	normal_cal_plus_fun(103, [&](const void* fun_ptr, void* user_data)->int {
		typedef int(__stdcall* cq_event_group_member_increase)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, __int64 being_operate_qq);
		return ((cq_event_group_member_increase)fun_ptr)(sub_type, time_, group_id,operator_id,user_id);
	}, 0);
}

void Center::deal_type_notice_group_ban(Json::Value& evt)
{
	std::string subtype = StrTool::get_str_from_json(evt, "sub_type", "");
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
	/* �����¼����� */
	normal_cal_plus_fun(104, [&](const void* fun_ptr, void* user_data)->int {
		typedef int(__stdcall* cq_event_group_ban)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, __int64 being_operate_qq, __int64 duration);
		return ((cq_event_group_ban)fun_ptr)(sub_type, time_, group_id, user_id, operator_id, duration);
	}, 0);
}

void Center::deal_type_notice_friend_add(Json::Value& evt)
{
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
	/* �����¼����� */
	normal_cal_plus_fun(201, [&](const void* fun_ptr, void* user_data)->int {
		typedef int(__stdcall* cq_event_friend_add)(__int32 sub_type, __int32 send_time, __int64 from_qq);
		return ((cq_event_friend_add)fun_ptr)(1,time_, user_id);
	}, 0);
}

void Center::deal_type_request_friend(Json::Value& evt)
{
	std::string comment = StrTool::get_str_from_json(evt, "comment", "");
	std::string response_flag = StrTool::get_str_from_json(evt, "flag", "");
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
	/* �����¼����� */
	normal_cal_plus_fun(301, [&](const void* fun_ptr, void* user_data)->int {
		typedef int(__stdcall* cq_event_friend_request)(__int32 sub_type, __int32 send_time, __int64 from_qq, const char* msg, const char* response_flag);
		return ((cq_event_friend_request)fun_ptr)(1,time_, user_id, comment.c_str(), response_flag.c_str());
	}, 0);
}

void Center::deal_type_request_group(Json::Value& evt)
{
	std::string comment = StrTool::get_str_from_json(evt, "comment", "");
	std::string response_flag = StrTool::get_str_from_json(evt, "flag", "");
	int time_ = StrTool::get_int_from_json(evt, "time", 0);
	int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
	std::string subtype = StrTool::get_str_from_json(evt, "sub_type", "");
	__int32 sub_type;
	if (subtype == "add")
	{
		sub_type = 1;
	}
	else
	{
		sub_type = 2; //invite
	}
	/* �����¼����� */
	normal_cal_plus_fun(302, [&](const void* fun_ptr, void* user_data)->int {
		typedef int(__stdcall* cq_event_group_request)(__int32 sub_type, __int32 send_time, __int64 from_group, __int64 from_qq, const char* msg, const char* response_flag);
		return ((cq_event_group_request)fun_ptr)(sub_type, time_, group_id, user_id, comment.c_str(), response_flag.c_str());
	}, 0);
}

void Center::deal_type_request(Json::Value& evt)
{
	auto request_type = StrTool::get_str_from_json(evt, "request_type", "");
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
		MiraiLog::get_instance()->add_debug_log("Center", "δ֪��request_type");
	}
}

void Center::deal_type_meta_event(Json::Value& evt)
{
}

static std::string get_md5_from_imgurl(const std::string & url)
{
	std::regex r("http.*?-([a-zA-Z0-9]{32}).*");
	std::smatch sm;
	if (std::regex_search(url, sm, r))
	{
		if (sm.size() == 2)
		{
			return sm[1].str();
		}
	}
	return "";
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
			/* ����object,˵��json��ʽ����,�������Ϣɾ�� */
			node = Json::nullValue;
			continue;
		}
		std::string type_str = StrTool::get_str_from_json(node, "type", "");
		if (type_str == "")
		{
			/* �������Ϣɾ�� */
			node = Json::nullValue;
			continue;
		}
		if (type_str == "image")
		{
			Json::Value dat_json = node.get("data", Json::nullValue);
			std::string url = StrTool::get_str_from_json(dat_json, "url", "");
			if (url == "")
			{
				/* 
					û��url�ֶΣ�MiraiCQ �޷�������
					MiraiCQĿǰ��Ҫ��ͼƬ��url�л�ȡͼƬ��md5������������С����ʽ 
				*/
				MiraiLog::get_instance()->add_debug_log("Center", "��⵽ͼƬ��û��url");
				node = Json::nullValue;
				continue;
			}
			std::string md5_str = get_md5_from_imgurl(url);
			if (md5_str == "")
			{
				MiraiLog::get_instance()->add_debug_log("Center", "�޷���url�л�ȡmd5:"+url);
				node = Json::nullValue;
				continue;
			}
			/* ���ͼƬ��Ϣ��Ҫ����һ����ͼƬ */
			ImgTool::ImgInfo info;
			if (!ImgTool::get_info(url, info))
			{
				MiraiLog::get_instance()->add_debug_log("Center", "�޷���url�л�ȡͼƬ��Ϣ:" + url);
				/* 
					��ʹ�޷����ͼƬ��Ϣ��Ҳ��Ҫд��url,md5����Ϣ��cqimg�ļ�
					node = Json::nullValue;
					continue;
				*/
			}
			std::string cqimg_name = md5_str + "." + info.type;
			/* ����Ŀ¼ */
			std::string exe_dir = PathTool::get_exe_dir();
			PathTool::create_dir(exe_dir + "data");
			PathTool::create_dir(exe_dir + "data\\image");
			std::string cqimg_path = exe_dir + "data\\image\\" + cqimg_name + ".cqimg";
			/* �˴���ͼƬ��Ϣֱ��д��cqimg�ļ����� */
			WritePrivateProfileStringA("image", "md5", md5_str.c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("image", "width", std::to_string(info.width).c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("image", "height", std::to_string(info.height).c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("image", "size", std::to_string(info.size).c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("image", "url", url.c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("image", "addtime", std::to_string(time(0)).c_str(), cqimg_path.c_str());
			WritePrivateProfileStringA("notice", "�¿�����", "������Ϣ�������ĸ��£����ļ��е� url �ѱ����ã�������δ�����Ƴ�������ֱ�Ӷ�ȡ���ļ�������������°� SDK����ʹ�� Api��CQ_getImage, ����ͼƬ����ȡ��ͼƬ��", cqimg_path.c_str());
			if (!PathTool::is_file_exist(cqimg_path))
			{
				/* �ٴμ��cqmig�Ƿ���� */
				MiraiLog::get_instance()->add_debug_log("Center", "cqimg�ļ�д��ʧ�ܣ����`data\\image`�µ��ļ�Ȩ��");
				node = Json::nullValue;
				continue;
			}
			/* ���¹���data�ֶΣ�ɾ������ļ� */
			Json::Value v;
			v["file"] = cqimg_name;
			node["data"] = v;
			/* Test  CQ_getImage_T:
				std::string full_path = CQ_getImage_T(1, cqimg_name.c_str());
				MiraiLog::get_instance()->add_debug_log("TestCQ_GetImage", full_path);
			*/
		}
	}
	return true;
}

void Center::deal_type_message_private(Json::Value& evt)
{
	Json::Value jsonarr = evt.get("message", Json::nullValue);
	/* ����json array,����Ҫ����cqimg�ļ�������Ҫ��������ֶ�ȥ�� */
	if (!deal_json_array(jsonarr))
	{
		MiraiLog::get_instance()->add_debug_log("Center", "jsonarrԤ����ʧ��");
		return;
	}
	auto s = jsonarr.toStyledString();
	std::string cq_str = StrTool::jsonarr_to_cq_str(jsonarr);
	if (cq_str == "")
	{
		MiraiLog::get_instance()->add_debug_log("Center", "jsonarrת��ʧ��");
		return;
	}
	MiraiLog::get_instance()->add_debug_log("Center", "����PrivateEvent��Msg:"+ cq_str);
	std::string sub_type_str = StrTool::get_str_from_json(evt, "sub_type", "");
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
		subtype_int = 1; //coolq�б�ʾ��������״̬
		return;
	}
	else
	{
		MiraiLog::get_instance()->add_debug_log("Center", "δ֪��˽����Ϣ��Դ�������д���");
		return;
	}
	int message_id = StrTool::get_int_from_json(evt, "message_id", 0);
	int font = StrTool::get_int_from_json(evt, "font", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);

	/* �����¼����� */
	normal_cal_plus_fun(21, [&](const void* fun_ptr, void* user_data)->int {
		typedef int(__stdcall* cq_event_private_message)(int sub_type, int msg_id, int64_t from_qq, const char* msg, int font);
		return ((cq_event_private_message)fun_ptr)(subtype_int, message_id, user_id, cq_str.c_str(), font);
	}, 0);
}



void Center::deal_type_message_group(Json::Value& evt)
{
	std::string from_anonymous_base64;
	Json::Value anonymous = evt.get("anonymous", Json::nullValue);
	if (anonymous.isObject())
	{
		int64_t id = StrTool::get_int64_from_json(anonymous, "id", 0);
		std::string name = StrTool::get_str_from_json(anonymous, "name", "");
		std::string flag = StrTool::get_str_from_json(anonymous, "flag", "");
		BinTool bin_pack;
		bin_pack.int64_push(id);
		bin_pack.string_push(name);
		bin_pack.token_push(std::vector<char>(flag.begin(), flag.end()));
		from_anonymous_base64 = base64_encode((const unsigned char*)(&(bin_pack.content[0])), bin_pack.content.size());

	}
	Json::Value jsonarr = evt.get("message", Json::nullValue);
	/* ����json array,����Ҫ����cqimg�ļ�������Ҫ��������ֶ�ȥ�� */
	if (!deal_json_array(jsonarr))
	{
		MiraiLog::get_instance()->add_debug_log("Center", "jsonarrԤ����ʧ��");
		return;
	}
	std::string cq_str = StrTool::jsonarr_to_cq_str(jsonarr);
	if (cq_str == "")
	{
		MiraiLog::get_instance()->add_debug_log("Center", "jsonarrת��ʧ��");
		return;
	}
	MiraiLog::get_instance()->add_debug_log("Center", "����GroupEvent��Msg:" + cq_str);
	int message_id = StrTool::get_int_from_json(evt, "message_id", 0);
	int font = StrTool::get_int_from_json(evt, "font", 0);
	int64_t user_id = StrTool::get_int64_from_json(evt, "user_id", 0);
	int64_t group_id = StrTool::get_int64_from_json(evt, "group_id", 0);

	/* �����¼����� */
	normal_cal_plus_fun(2, [&](const void* fun_ptr, void* user_data)->int {
		typedef int(__stdcall* cq_event_group_message)(int sub_type, int msg_id, int64_t from_group,int64_t from_qq, const char* anonymous,const char* msg, int font);
		return ((cq_event_group_message)fun_ptr)(1, message_id,group_id, user_id, from_anonymous_base64.c_str(), cq_str.c_str(), font);
	}, 0);
	
}