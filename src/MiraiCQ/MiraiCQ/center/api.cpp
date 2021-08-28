#include <Windows.h>
#include "center.h"
#include "../log/MiraiLog.h"
#include "../tool/PathTool.h"
#include "../tool/StrTool.h"
#include "../tool/TimeTool.h"
#include "../tool/BinTool.h"
#include "../tool/ImgTool.h"

#include <base64/base64.h>
#include <fstream>


enum class JSON_TYPE
{
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_NULL
};

template <class T>
class TP10086
{
};

static std::string RETERR(const TP10086<std::string>& v)
{
	return "";
}
static int RETERR(const TP10086<int>& v)
{
	return -1;
}
static int64_t RETERR(const TP10086<int64_t>& v)
{
	return -1;
}

template <typename TER_TYPE>
static TER_TYPE normal_call(
	int auth_code,
	std::weak_ptr<MiraiNet> net_,
	bool can_not_enable,
	std::function<void(MiraiNet::NetStruct json)> fun1,
	std::function<TER_TYPE(const Json::Value & data_json)> fun2,
	JSON_TYPE json_type)
{
	if (!can_not_enable)
	{
		auto plus = MiraiPlus::get_instance();
		//assert(plus);
		if (!plus->is_enable(auth_code))
		{
			return RETERR(TP10086<TER_TYPE>());
		}
	}
	else
	{
		auto ret = fun2(Json::nullValue);
		return ret;
	}
	MiraiNet::NetStruct json(new Json::Value);
	fun1(json);
	auto net = net_.lock();
	if (!net)
	{
		return RETERR(TP10086<TER_TYPE>());
	}
	MiraiNet::NetStruct ret_json = net->call_fun(json, 10000);
	net = nullptr; /* ��ʱ�ͷ�net */
	if (!ret_json)
	{
		return RETERR(TP10086<TER_TYPE>());
	}
	if (StrTool::get_int_from_json(*ret_json, "retcode", -1) != 0)
	{
		return RETERR(TP10086<TER_TYPE>());
	}
	Json::Value data_json = ret_json->get("data", Json::nullValue);
	if (json_type == JSON_TYPE::JSON_OBJECT)
	{
		if (!data_json.isObject())
		{
			return RETERR(TP10086<TER_TYPE>());
		}
	}
	else if (json_type == JSON_TYPE::JSON_ARRAY)
	{
		if (!data_json.isArray())
		{
			return RETERR(TP10086<TER_TYPE>());
		}
	}
	auto ret = fun2(data_json);
	return ret;
}

static Json::Value deal_cq_str(const std::string & cq_str)
{
	/* ���Ƚ�jsonarrת����utf8��ʽ */
	auto json_arr = StrTool::cq_str_to_jsonarr(cq_str);
	if (!json_arr.isArray())
	{
		return -1;
	}
	auto utf8_json_str = StrTool::to_utf8(Json::FastWriter().write(json_arr));
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(utf8_json_str, json_arr))
	{
		return Json::arrayValue;
	}
	for (auto & node : json_arr)
	{
		/* ��StrTool::cq_str_to_jsonarr ת��������jsonһ����type */
		if (node["type"].asString() == "image")
		{
			auto file = StrTool::get_str_from_json(node["data"], "file", "");
			if (file == "")
			{
				continue;
			}
			file = PathTool::get_exe_dir() +"data\\image\\" + StrTool::to_ansi(file);
			
			/* ����ļ������ڣ������ļ���cqimg������ڣ�����url��ʽ���� */
			if ((!PathTool::is_file_exist(file)) && (PathTool::is_file_exist((file + ".cqimg"))))
			{
				auto url = StrTool::get_str_from_ini(file + ".cqimg", "image", "url", "");
				if (url != "")
				{
					node["data"]["file"] = StrTool::to_utf8(url);
				}
				continue;
			}
			std::ifstream ifs;
			ifs.open(file, std::ios::binary | std::ios::ate);
			if (!ifs.is_open())
			{
				MiraiLog::get_instance()->add_debug_log("ApiCall", "��ͼƬ�ļ�ʧ��");
				continue;
			}
			std::streamsize size = ifs.tellg();
			ifs.seekg(0, std::ios::beg);
			std::vector<char> buffer((size_t)size);
			ifs.read(buffer.data(), size);
			ifs.close();
			if (buffer.size() != size || buffer.size() == 0)
			{
				MiraiLog::get_instance()->add_debug_log("ApiCall", "��ȡͼƬ�ļ�ʧ��");
				continue;
			}
			node["data"]["file"] = "base64://" + base64_encode((const unsigned char*)(&(buffer[0])), buffer.size());

		}
	}
	auto s = json_arr.toStyledString();
	return json_arr;
	
}

int Center::CQ_sendPrivateMsg(int auth_code, int64_t qq, const char* msg) 
{
	if (!msg)
	{
		return -1;
	}
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code,net,false,
		[&](MiraiNet::NetStruct json) 
	{
		(*json)["action"] = "send_private_msg";
		(*json)["params"]["user_id"] = qq;
		(*json)["params"]["message"] = deal_cq_str(msg);
		
	}, [&](const Json::Value& data_json) 
	{
		return StrTool::get_int_from_json(data_json, "message_id", 0);
	}, 
	JSON_TYPE::JSON_OBJECT);
}

int Center::CQ_sendGroupMsg(int auth_code, int64_t group_id, const char* msg) 
{
	if (!msg)
	{
		return -1;
	}
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "send_group_msg";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["message"] = deal_cq_str(msg);

	}, [&](const Json::Value& data_json)
	{
		return StrTool::get_int_from_json(data_json, "message_id", 0);
	},
		JSON_TYPE::JSON_OBJECT);
}

int Center::CQ_deleteMsg(int auth_code, int64_t msg_id) 
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "delete_msg";
		(*json)["params"]["message_id"] = msg_id;

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_sendLike(int auth_code, int64_t qq,int times) 
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "send_like";
		(*json)["params"]["user_id"] = qq;
		(*json)["params"]["times"] = times;

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setGroupKick(int auth_code, int64_t group_id, int64_t qq, int reject_add_request) 
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "set_group_kick";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["user_id"] = qq;
		(*json)["params"]["reject_add_request"] = (reject_add_request ? true : false);

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setGroupBan(int auth_code, int64_t group_id, int64_t qq, int64_t duration) 
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "set_group_ban";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["user_id"] = qq;
		(*json)["params"]["duration"] = duration;

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);

}

int Center::CQ_setGroupAnonymousBan(int auth_code, int64_t group_id, const char* anonymous, int64_t duration) 
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "set_group_anonymous_ban";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["duration"] = duration;
		std::vector<char> vec;
		std::string str = base64_decode((anonymous ? anonymous : ""));
		BinTool bin(std::vector<char>(str.begin(), str.end()));
		bin.int64_pop();
		bin.string_pop();
		(*json)["params"]["flag"] = bin.string_pop();

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setGroupWholeBan(int auth_code, int64_t group_id, int enable) 
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "set_group_whole_ban";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["enable"] = (enable ? true : false);

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setGroupAdmin(int auth_code, __int64 group_id, __int64 qq, int set) 
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "set_group_admin";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["user_id"] = qq;
		(*json)["params"]["enable"] = (set ? true : false);

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setGroupAnonymous(int auth_code, __int64 group_id, int enable) 
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "set_group_anonymous";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["enable"] = (enable ? true : false);

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setGroupCard(int auth_code, __int64 group_id, __int64 qq, const char* new_card) 
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "set_group_card";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["user_id"] = qq;
		(*json)["params"]["card"] = StrTool::to_utf8(new_card ? new_card : "");

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setGroupLeave(int auth_code, __int64 group_id, int is_dismiss)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "set_group_leave";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["is_dismiss"] = (is_dismiss ? true : false);

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setGroupSpecialTitle(int auth_code, __int64 group_id, __int64 qq, const char* new_special_title, __int64 duration)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "set_group_special_title";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["user_id"] = qq;
		(*json)["params"]["special_title"] = StrTool::to_utf8(new_special_title ? new_special_title : "");
		(*json)["params"]["duration"] = duration;

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setFriendAddRequest(__int32 auth_code, const char* response_flag, __int32 response_operation, const char* remark)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "set_friend_add_request";
		(*json)["params"]["flag"] = (response_flag ? response_flag : "");
		if (response_operation == 1)
		{
			(*json)["params"]["approve"] = true;
		}
		else
		{
			(*json)["params"]["approve"] = false;
		}
		(*json)["params"]["remark"] = StrTool::to_utf8(remark ? remark : "");

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setGroupAddRequest(__int32 auth_code, const char* response_flag, int request_type, int response_operation, const char* reason)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "set_group_add_request";
		(*json)["params"]["flag"] = (response_flag ? response_flag : "");
		if (request_type == 1)
		{
			(*json)["params"]["sub_type"] = "add";
		}
		else
		{
			(*json)["params"]["sub_type"] = "invite";
		}
		if (response_operation == 1)
		{
			(*json)["params"]["approve"] = true;
		}
		else
		{
			(*json)["params"]["approve"] = false;
		}
		(*json)["params"]["reason"] = StrTool::to_utf8(reason ? reason : "");

	}, [&](const Json::Value& data_json)
	{
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int64_t Center::CQ_getLoginQQ(int auth_code)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int64_t>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "get_login_info";

	}, [&](const Json::Value& data_json)
	{
		return StrTool::get_int64_from_json(data_json, "user_id", -1);
	},
		JSON_TYPE::JSON_OBJECT);
}

std::string Center::CQ_getLoginNick(int auth_code)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<std::string>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "get_login_info";

	}, [&](const Json::Value& data_json)
	{
		std::string login_nick = StrTool::get_str_from_json(data_json, "nickname", "");
		return StrTool::to_ansi(login_nick);
	},
		JSON_TYPE::JSON_OBJECT);
}

std::string Center::CQ_getStrangerInfo(int auth_code, __int64 qq, int no_cache)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<std::string>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "get_stranger_info";
		(*json)["params"]["user_id"] = qq;
		(*json)["params"]["no_cache"] = (no_cache ? true : false);

	}, [&](const Json::Value& data_json)
	{
		int64_t user_id = StrTool::get_int64_from_json(data_json, "user_id", 0);
		std::string nickname = StrTool::get_str_from_json(data_json, "nickname", "");
		int sex;
		{
			std::string sex_str = StrTool::get_str_from_json(data_json, "sex", "");
			if (sex_str == "male")
			{
				sex = 0;
			}
			else if (sex_str == "female")
			{
				sex = 1;
			}
			else
			{
				sex = 255; /* δ֪ */
			}
		}
		int age = StrTool::get_int_from_json(data_json, "age", 0);
		BinTool bin_pack;
		bin_pack.int64_push(user_id);
		bin_pack.string_push(StrTool::to_ansi(nickname));
		bin_pack.int32_push(sex);
		bin_pack.int32_push(age);
		std::string ret_str = base64_encode((const unsigned char*)(&(bin_pack.content[0])), bin_pack.content.size());
		return ret_str;
	},
		JSON_TYPE::JSON_OBJECT);
}

std::string Center::CQ_getFriendList(int auth_code, int reserved)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<std::string>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "get_friend_list";

	}, [&](const Json::Value& data_arr)
	{
		BinTool bin_pack;
		bin_pack.int32_push(data_arr.size());
		for (size_t i = 0; i < data_arr.size(); ++i)
		{
			if (!data_arr[i].isObject())
			{
				continue;
			}
			BinTool bin_pack_child;
			bin_pack_child.int64_push(StrTool::get_int64_from_json(data_arr[i], "user_id", 0));
			bin_pack_child.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_arr[i], "nickname", "")));
			bin_pack_child.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_arr[i], "remark", "")));
			bin_pack.token_push(bin_pack_child.content);
		}
		std::string ret_str = base64_encode((const unsigned char*)(&(bin_pack.content[0])), bin_pack.content.size());
		return ret_str;
	},
		JSON_TYPE::JSON_ARRAY);
}

std::string Center::CQ_getGroupList(int auth_code)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<std::string>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "get_group_list";

	}, [&](const Json::Value& data_arr)
	{
		BinTool bin_pack;
		bin_pack.int32_push(data_arr.size());
		for (size_t i = 0; i < data_arr.size(); ++i)
		{
			BinTool bin_pack_child;
			bin_pack_child.int64_push(StrTool::get_int64_from_json(data_arr[i], "group_id", 0));
			bin_pack_child.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_arr[i], "group_name", "")));
			bin_pack_child.int32_push(StrTool::get_int_from_json(data_arr[i], "member_count", 0));
			bin_pack_child.int32_push(StrTool::get_int_from_json(data_arr[i], "max_member_count", 0));
			bin_pack.token_push(bin_pack_child.content);
		}
		std::string ret_str = base64_encode((const unsigned char*)(&(bin_pack.content[0])), bin_pack.content.size());
		return ret_str;
	},
		JSON_TYPE::JSON_ARRAY);
}

std::string Center::CQ_getGroupInfo(int auth_code, int64_t group_id, int no_cache)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<std::string>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "get_group_info";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["no_cache"] = (no_cache ? true : false);

	}, [&](const Json::Value& data_json)
	{
		BinTool bin_pack;
		bin_pack.int64_push(StrTool::get_int64_from_json(data_json, "group_id", 0));
		bin_pack.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_json, "group_name", "")));
		bin_pack.int32_push(StrTool::get_int_from_json(data_json, "member_count", 0));
		bin_pack.int32_push(StrTool::get_int_from_json(data_json, "max_member_count", 0));
		std::string ret_str = base64_encode((const unsigned char*)(&(bin_pack.content[0])), bin_pack.content.size());
		return ret_str;
	},
		JSON_TYPE::JSON_OBJECT);
}

std::string Center::CQ_getGroupMemberList(int auth_code, int64_t group_id)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<std::string>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "get_group_member_list";
		(*json)["params"]["group_id"] = group_id;

	}, [&](const Json::Value& data_arr)
	{
		BinTool bin_pack;
		bin_pack.int32_push(data_arr.size());
		for (size_t i = 0; i < data_arr.size(); ++i)
		{
			BinTool bin_pack_child;
			bin_pack_child.int64_push(StrTool::get_int64_from_json(data_arr[i], "group_id", 0));
			bin_pack_child.int64_push(StrTool::get_int64_from_json(data_arr[i], "user_id", 0));

			bin_pack_child.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_arr[i], "nickname", "")));
			bin_pack_child.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_arr[i], "card", "")));

			{
				std::string sex = StrTool::get_str_from_json(data_arr[i], "sex", "");
				if (sex == "male")
				{
					bin_pack_child.int32_push(0);
				}
				else if (sex == "female")
				{
					bin_pack_child.int32_push(1);
				}
				else
				{
					bin_pack_child.int32_push(255);
				}
			}

			bin_pack_child.int32_push(StrTool::get_int_from_json(data_arr[i], "age", 0));
			bin_pack_child.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_arr[i], "area", "")));
			bin_pack_child.int32_push(StrTool::get_int_from_json(data_arr[i], "join_time", 0));
			bin_pack_child.int32_push(StrTool::get_int_from_json(data_arr[i], "last_sent_time", 0));
			bin_pack_child.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_arr[i], "level", "")));

			{
				std::string role = StrTool::get_str_from_json(data_arr[i], "role", "");
				if (role == "member")
				{
					bin_pack_child.int32_push(1);
				}
				else if (role == "admin")
				{
					bin_pack_child.int32_push(2);
				}
				else
				{
					bin_pack_child.int32_push(3);
				}
			}

			bin_pack_child.bool_push(StrTool::get_bool_from_json(data_arr[i], "unfriendly", false));

			bin_pack_child.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_arr[i], "title", "")));

			bin_pack_child.int32_push(StrTool::get_int_from_json(data_arr[i], "title_expire_time", 0));

			bin_pack_child.bool_push(StrTool::get_bool_from_json(data_arr[i], "card_changeable", false));

			bin_pack.token_push(bin_pack_child.content);

		}
		std::string ret_str = base64_encode((const unsigned char*)(&(bin_pack.content[0])), bin_pack.content.size());
		return ret_str;
	},
		JSON_TYPE::JSON_ARRAY);
}

std::string Center::CQ_getGroupMemberInfoV2(int auth_code, int64_t group_id, int64_t qq, int no_cache)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<std::string>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "get_group_member_info";
		(*json)["params"]["group_id"] = group_id;
		(*json)["params"]["user_id"] = qq;
		(*json)["params"]["no_cache"] = (no_cache ? true : false);

	}, [&](const Json::Value& data_json)
	{
		BinTool bin_pack;
		bin_pack.int64_push(StrTool::get_int64_from_json(data_json, "group_id", 0));
		bin_pack.int64_push(StrTool::get_int64_from_json(data_json, "user_id", 0));
		bin_pack.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_json, "nickname", "")));
		bin_pack.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_json, "card", "")));


		{
			std::string sex = StrTool::get_str_from_json(data_json, "sex", "");
			if (sex == "male")
			{
				bin_pack.int32_push(0);
			}
			else if (sex == "female")
			{
				bin_pack.int32_push(1);
			}
			else
			{
				bin_pack.int32_push(255);
			}
		}

		bin_pack.int32_push(StrTool::get_int_from_json(data_json, "age", 0));

		bin_pack.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_json, "area", "")));

		bin_pack.int32_push(StrTool::get_int_from_json(data_json, "join_time", 0));
		bin_pack.int32_push(StrTool::get_int_from_json(data_json, "last_sent_time", 0));

		bin_pack.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_json, "level", "")));


		{
			std::string role = StrTool::get_str_from_json(data_json, "role", "");
			if (role == "member")
			{
				bin_pack.int32_push(1);
			}
			else if (role == "admin")
			{
				bin_pack.int32_push(2);
			}
			else
			{
				bin_pack.int32_push(3);
			}
		}


		bin_pack.bool_push(StrTool::get_bool_from_json(data_json, "unfriendly", false));

		bin_pack.string_push(StrTool::to_ansi(StrTool::get_str_from_json(data_json, "title", "")));

		bin_pack.int32_push(StrTool::get_int_from_json(data_json, "title_expire_time", 0));

		bin_pack.bool_push(StrTool::get_bool_from_json(data_json, "card_changeable", false));

		std::string ret_str = base64_encode((const unsigned char*)(&(bin_pack.content[0])), bin_pack.content.size());

		return ret_str;
	},
		JSON_TYPE::JSON_OBJECT);

}

std::string Center::CQ_getCookies(int auth_code, const char* domain)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<std::string>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "get_cookies";
		if (std::string(domain ? domain : "") != "")
		{
			(*json)["params"]["domain"] = StrTool::to_utf8(domain ? domain : "");
		}

	}, [&](const Json::Value& data_json)
	{
		std::string cookies = StrTool::get_str_from_json(data_json, "cookies", "");
		return cookies;
	},
		JSON_TYPE::JSON_OBJECT);

}

int Center::CQ_getCsrfToken(int auth_code)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "get_csrf_token";

	}, [&](const Json::Value& data_json)
	{
		int token = StrTool::get_int_from_json(data_json, "token", -1);
		return token;
	},
		JSON_TYPE::JSON_OBJECT);
}

std::string Center::CQ_getAppDirectory(int auth_code)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<std::string>(auth_code, net, true,
		[&](MiraiNet::NetStruct json)
	{

	}, [&](const Json::Value& data_json) -> std::string
	{
		auto plus = MiraiPlus::get_instance();
		auto plus_def = plus->get_plus(auth_code);
		if (!plus_def)
		{
			return "";
		}
		std::string filename = plus_def->get_filename();
		plus_def = nullptr; /* �����ͷ�plus_def */
		std::string ret_path = PathTool::get_exe_dir() +"app\\"+ filename + "\\";
		if (!PathTool::is_dir_exist(ret_path))
		{
			if (!PathTool::create_dir(ret_path))
			{
				return "";
			}
		}
		return ret_path;
	},
		JSON_TYPE::JSON_NULL);
}

std::string Center::CQ_getRecord(int auth_code, const char* file, const char* out_format,bool full_path)
{
	//TODO
	return std::string();
}

std::string Center::CQ_getImage(int auth_code, const char* file)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<std::string>(auth_code, net, true,
		[&](MiraiNet::NetStruct json)
	{

	}, [&](const Json::Value& data_json) -> std::string
	{
		/* �������� */
		if (std::string(file ? file : "") == "")
		{
			return "";
		}
		auto img_dir = PathTool::get_exe_dir() + "data\\image\\";
		auto cqimg_path = img_dir + file + ".cqimg";
		if (!PathTool::is_file_exist(cqimg_path))
		{
			//��cqimg�ļ����޷���ȡͼƬ
			return "";
		}
		auto url = StrTool::get_str_from_ini(cqimg_path, "image", "url", "");
		if (url == "")
		{
			//û�ж�ȡ��url
			return "";
		}

		/* �����������ص��ļ� */
		static std::mutex mx;
		static std::set<std::string> downing_set;
		bool is_downloading = false;

		/* ����Ƿ�������������б����� */
		{
			std::lock_guard<std::mutex> lock(mx);
			/* û���������� */
			if (downing_set.find(file) == downing_set.end())
			{
				/* �����������ر�� */
				downing_set.insert(file);
				is_downloading = false;
			}
			else
			{
				is_downloading = true;
			}
		}

		/* û���������أ�˵�����Լ����� */
		if (!is_downloading)
		{
			/* ���ͼƬ�Ѿ����ڣ��Ͳ������������� */
			if (!PathTool::is_file_exist(img_dir + file))
			{
				/* ����ͼƬ */
				ImgTool::download_img(url, img_dir + file);
			}
			/* ͼƬ������ɣ�ɾ���������صı�� */
			std::lock_guard<std::mutex> lock(mx);
			downing_set.erase(file);
		}

		/* �ȴ��������صı����ʧ */
		while (true)
		{
			{
				std::lock_guard<std::mutex> lock(mx);
				if (downing_set.find(file) == downing_set.end())
				{
					break;
				}
			}
			/* ˯��һ��ʱ�� */
			TimeTool::sleep(0);
		}

		/* ����Ƿ����سɹ� */
		if (!PathTool::is_file_exist(img_dir + file))
		{
			//û�����سɹ�
			return "";
		}
		/* ���سɹ�������·�� */
		return img_dir + file;
	},
		JSON_TYPE::JSON_NULL);
}


int Center::CQ_canSendImage(int auth_code)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "can_send_image";

	}, [&](const Json::Value& data_json)
	{
		bool can = StrTool::get_bool_from_json(data_json, "yes", false);
		return (can ? 1 : 0);
	},
		JSON_TYPE::JSON_OBJECT);
}

int Center::CQ_canSendRecord(int auth_code)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, false,
		[&](MiraiNet::NetStruct json)
	{
		(*json)["action"] = "can_send_record";

	}, [&](const Json::Value& data_json)
	{
		bool can = StrTool::get_bool_from_json(data_json, "yes", false);
		return (can ? 1 : 0);
	},
		JSON_TYPE::JSON_OBJECT);
}



int Center::CQ_addLog(int auth_code, int log_level, const char* category, const char* log_msg)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, true,
		[&](MiraiNet::NetStruct json)
	{

	}, [&](const Json::Value& data_json)
	{
		auto plus = MiraiPlus::get_instance();
		auto plus_def = plus->get_plus(auth_code);
		if (!plus_def)
		{
			return -1;
		}
		std::string name = plus_def->get_name();
		plus_def = nullptr; /* �����ͷ�plus_def */
		MiraiLog::get_instance()->add_info_log(std::string("[") + name + std::string("] ") + (category ? category : ""), (log_msg ? log_msg : ""));
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setFatal(__int32 auth_code, const char* error_info)
{
	std::weak_ptr<MiraiNet> net;
	{
		std::shared_lock<std::shared_mutex> lk;
		net = this->net;
	}
	return normal_call<int>(auth_code, net, true,
		[&](MiraiNet::NetStruct json)
	{

	}, [&](const Json::Value& data_json)
	{
		auto plus = MiraiPlus::get_instance();
		auto plus_def = plus->get_plus(auth_code);
		if (!plus_def)
		{
			return -1;
		}
		std::string name = plus_def->get_name();
		plus_def = nullptr; /* �����ͷ�plus_def */
		MiraiLog::get_instance()->add_info_log(std::string("[") + name + std::string("] CQ_setFatal: "), (error_info ? error_info : ""));
		return 0;
	},
		JSON_TYPE::JSON_NULL);
}

int Center::CQ_setRestart(int auth_code)
{
	//TODO..
	return -1;
}