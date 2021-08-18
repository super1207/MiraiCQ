#include <afx.h>
#include <atlimage.h>
#include "bot_event.h"
#include "bot.h"
#include "plus.h"
#include "binpack.h"
#include "base64.h"
#include "msg_id_convert.h"
#include "httphelp.h"

#include <boost/locale/encoding.hpp>
#include <atlstr.h>
#include <boost\cast.hpp>
#include <boost\lexical_cast.hpp> 


#define to_gbk(U8) boost::locale::conv::between((U8), "GBK", "UTF-8")
#define to_u8(GBK) boost::locale::conv::between((GBK), "UTF-8", "GBK").c_str()

std::map<std::string,std::map<std::string,__int32(*)(const Json::Value &,boost::shared_ptr<Plus>)> > g_message_map;

#define EVENT_IGNORE 0
#define EVENT_BLOCK 1

#define TEMP_EVENT_FUN(x) static __int32 temp_call_##x##(const std::map<__int32,Plus::PlusDef>::iterator & iter,const Json::Value & root,const Json::Value & root_)
#define EVENT_FUN_ID(x) Plus::cq_##x##_id
#define EVENT_FUN_TYPE(x) Plus::cq_##x##_funtype
#define GET_FUNPTR(FUNTYPE) \
	BOOST_AUTO(fun_ptr,((EVENT_FUN_TYPE(FUNTYPE))Plus::get_plus_function(iter->second,EVENT_FUN_ID(FUNTYPE))));\
	if(!fun_ptr){return EVENT_BLOCK;}


static std::string& replace_all_distinct(std::string& str, const std::string& old_value, const std::string& new_value)
{
	for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) != std::string::npos)
			str.replace(pos, old_value.length(), new_value);
		else   break;
	}
	return   str;
}

/* 用于将json数组格式的消息转化为字符串格式消息,转化失败返回"" */
static std::string CvtJsonToStrMsg(const Json::Value& jsonArr)
{
	std::string retStr;
	Json::Value defstr = Json::Value("");
	if (!jsonArr.isArray())
	{
		return "";
	}
	try
	{
		for(size_t i = 0;i < jsonArr.size();++i)
		{
		/*for (const auto& node : jsonArr)
		{*/
			std::string type = jsonArr[i].get("type", Json::Value("")).asString();
			if (type == "")
			{
				return "";
			}
			if (type == "text")
			{
				std::string temStr = jsonArr[i].get("data", defstr).get("text", defstr).asString();
				replace_all_distinct(temStr, "&", "&amp;");
				replace_all_distinct(temStr, "[", "&#91;");
				replace_all_distinct(temStr, "]", "&#93;");
				replace_all_distinct(temStr, ",", "&#44;");
				retStr.append(temStr);
			}
			else
			{
				std::string cqStr = "[CQ:" + type;
				Json::Value datObj = jsonArr[i].get("data", defstr);
				if (!datObj.isObject())
				{
					return "";
				}
				Json::Value::Members member = datObj.getMemberNames();
				for (std::vector<std::string>::iterator iter = member.begin(); iter != member.end(); iter++)
				{
					std::string dat = datObj[(*iter)].asString();
					replace_all_distinct(dat, "&", "&amp;");
					replace_all_distinct(dat, "[", "&#91;");
					replace_all_distinct(dat, "]", "&#93;");
					replace_all_distinct(dat, ",", "&#44;");
					cqStr.append("," + (*iter) + "=" + dat);
				}
				cqStr.append("]");
				retStr.append(cqStr);

			}
		}
		return retStr;

	}
	catch (const std::exception&)
	{
		return "";
	}

}

static std::vector<std::string> tokenize(const std::string& s, char c) {
	std::string::const_iterator end = s.end();
	std::string::const_iterator start = end;

	std::vector<std::string> v;
	for (std::string::const_iterator it = s.begin(); it != end; ++it) {
		if (*it != c) {
			if (start == end)
				start = it;
			continue;
		}
		if (start != end) {
			v.push_back(std::string(start, it));
			start = end;
		}
	}
	if (start != end)
		v.push_back(std::string(start, end));
	return v;
}



/* 用于将字符串格式的消息转化为json数组格式,转化失败返回null */
Json::Value CvtStrMsgToJson(const std::string& strMsg)
{
	Json::Value jsonArr = Json::arrayValue;
	std::vector<std::string> strVec;
	bool iscq = false;
	for (size_t i = 0; i < strMsg.size(); ++i)
	{
		if (strMsg[i] == '[')
		{
			iscq = true;
			strVec.push_back(std::string(1, '['));
		}
		else
		{
			if (iscq && strMsg[i] == ' ')
			{
				continue;
			}
			if (strVec.size() == 0)
			{
				strVec.push_back("");
			}
			strVec[strVec.size() - 1].append(std::string(1, strMsg[i]));
			if (strMsg[i] == ']')
			{
				iscq = false;
				strVec.push_back("");
			}
		}
	}
	for (size_t i = 0; i < strVec.size(); ++i)
	{
		if (strVec[i] == "")
		{
			continue;
		}
		if (strVec[i][0] != '[')
		{
			Json::Value node;
			node["type"] = "text";
			Json::Value datNode;
			std::string t = strVec[i];
			replace_all_distinct(t, "&amp;", "&");
			replace_all_distinct(t, "&#91;", "[");
			replace_all_distinct(t, "&#93;", "]");
			replace_all_distinct(t, "&#44;", ",");
			datNode["text"] = t;
			node["data"] = datNode;
			jsonArr.append(node);
		}
		else
		{
			Json::Value node;
			Json::Value datNode = Json::objectValue;
			std::vector<std::string> strNode = tokenize(std::string(strVec[i].begin() + 1, strVec[i].end() - 1), ',');
			try
			{
				node["type"] = tokenize(strNode.at(0), ':').at(1);
				for (size_t i = 1; i < strNode.size(); ++i)
				{
					size_t pos = strNode.at(i).find_first_of("=");
					std::string type = strNode.at(i).substr(0, pos);
					std::string dat = strNode.at(i).substr(pos + 1);
					std::string t = dat;
					replace_all_distinct(t, "&amp;", "&");
					replace_all_distinct(t, "&#91;", "[");
					replace_all_distinct(t, "&#93;", "]");
					replace_all_distinct(t, "&#44;", ",");
					datNode[type] = t;
				}
				node["data"] = datNode;
				jsonArr.append(node);

			}
			catch (const std::exception&)
			{
				return Json::Value();
			}
		}
	}
	return jsonArr;
}

static bool IsFileExistent(const boost::filesystem::path& path) 
{ 
	return boost::filesystem::is_regular_file(path); 
}

static std::string GetPhotoType(const std::string & path)	
{
	FILE *fp = fopen(path.c_str(), "rb");
	if(!fp)
	{
		BOOST_LOG_TRIVIAL(debug) << "File can`t open:" << path;
		return "";
	}
	char buffer[32];
	size_t n = fread(buffer, 1, 30, fp);
	fclose(fp);
	if(n != 30)
	{
		BOOST_LOG_TRIVIAL(debug) << "n not 30" << n;
		return "";
	}
	if ((unsigned char)buffer[0] == 0xff && (unsigned char)buffer[1] == 0xd8)
	{
		return "jpg";
	}
	else if (!memcmp(&buffer[0], "GIF89a", 6))
	{
		return "gif";
	}else if (!memcmp(&buffer[0], "GIF87a", 6))
	{
		return "gif";
	}
	else if (!memcmp(&buffer[1], "PNG", 3))
	{
		return "png" ;
	}
	else if((unsigned char)buffer[0] == 0x42 && (unsigned char)buffer[1] == 0x4d)
	{
		return "bmp";
	}
	BOOST_LOG_TRIVIAL(debug) << "can`t match any";
	return "";
}

static bool GetImg(const std::string & url,const std::string & file,int & width,int & height,std::string & md5Str,ULONGLONG & sz)
{
	std::string filepath = "data\\image\\" + file;
	std::string ct;
	if(!HttpGet(url,filepath,ct,5000))
	{
		return false;;
	}
	std::string tp = GetPhotoType(filepath);
	if(tp == "")
	{
		return false;
	}
	try
	{
		boost::filesystem::rename(filepath,filepath + "." + tp);
	}
	catch(const std::exception &)
	{
		return false;
	}

	CString strFilePath  = (filepath + "." + tp).c_str();  
	CFileStatus fileStatus;  
	if (CFile::GetStatus(strFilePath, fileStatus))  
	{  
		sz = fileStatus.m_size;  
	}else
	{
		BOOST_LOG_TRIVIAL(debug) << "get image file size error";
		return false;
	}

	CImage image;
	if(image.Load((filepath + "." + tp).c_str()) != S_OK)
	{
		BOOST_LOG_TRIVIAL(debug) << "img load error";
		return false;
	}
	width = image.GetWidth();
	height = image.GetHeight();
	try
	{
		md5Str = boost::to_upper_copy(std::string(file.begin(),file.begin()+32));
	}
	catch(const std::exception &)
	{
		BOOST_LOG_TRIVIAL(debug) << "get image md5 error";
		image.Destroy();
		return false;
	}
	image.Destroy();
	if(md5Str.length() != 32)
	{
		BOOST_LOG_TRIVIAL(debug) << "get image md5 error";
		return false;
	}
	return true;
}

static void DealCqMsg(Json::Value & root)
{
	Json::Value jsonArr = root["message"];
	Json::Value defstr = Json::Value("");
	if(jsonArr.isNull())
	{
		root["message"] = "";
		return ;
	}
	if(jsonArr.isString())
	{
		jsonArr = CvtStrMsgToJson(jsonArr.asString());
		if(jsonArr.isNull())
		{
			root["message"] = "";
			return ;
		}
	}
	std::string retStr;
	if(!jsonArr.isArray())
	{
		root["message"] = "";
		return ;
	}
	for(size_t i = 0;i < jsonArr.size();++i)
	{
		std::string type = jsonArr[i].get("type", Json::Value("")).asString();
		if(type == "image")
		{
			std::string url = jsonArr[i].get("data", defstr).get("url", defstr).asString();
			std::string file = jsonArr[i].get("data", defstr).get("file", defstr).asString();
			if(url == "" || file == "")
			{
				continue;
			}
			std::string fileimg = file + ".cqimg";
			std::string inifile = "data/image/" + fileimg;
			if(!IsFileExistent(inifile))
			{
				int width = 0,height = 0;
				ULONGLONG sz;
				std::string md5Str;
				if(!GetImg(url,file,width,height,md5Str,sz))
				{
					BOOST_LOG_TRIVIAL(debug) << "get img info error";
				}else
				{
					WritePrivateProfileStringA("image","md5",md5Str.c_str(),inifile.c_str()); 
					WritePrivateProfileStringA("image","width",boost::lexical_cast<std::string>(width).c_str(),inifile.c_str()); 
					WritePrivateProfileStringA("image","height",boost::lexical_cast<std::string>(height).c_str(),inifile.c_str()); 
					WritePrivateProfileStringA("image","size",boost::lexical_cast<std::string>(sz).c_str(),inifile.c_str());
				}

				BOOL isOk = WritePrivateProfileStringA("image","url",url.c_str(),inifile.c_str()); 
				if(isOk != TRUE)
				{
					BOOST_LOG_TRIVIAL(debug) << "cqimg read error，check data\\image";
				}
				WritePrivateProfileStringA("image","addtime",boost::lexical_cast<std::string>(time(0)).c_str(),inifile.c_str());
			}
			Json::Value jdat;
			jdat["file"] = file;
			jsonArr[i]["data"] = jdat;

		}
			
	}
	root["message"] = to_gbk(CvtJsonToStrMsg(jsonArr));
}

TEMP_EVENT_FUN(event_private_message)
{
	GET_FUNPTR(event_private_message)
	std::string  msg = root["message"].asString();
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "friend")
	{
		sub_type = 11;
	}else if(subtype == "group")
	{
		sub_type = 2;
	}else
	{
		sub_type = 1;
	}
	__int32 ret = fun_ptr(sub_type,root["message_id"].asInt(),root["user_id"].asInt64(),msg.c_str(),root["font"].asInt());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK; //
}

TEMP_EVENT_FUN(event_group_message)
{
	GET_FUNPTR(event_group_message)
	std::string from_anonymous_base64;

	Json::Value anonymous = root["anonymous"];
	if(anonymous.isObject())
	{
		__int64 id = anonymous["id"].asInt64();
		std::string name = to_gbk(anonymous["name"].asString());
		std::string flag = anonymous["flag"].asString();
		BinPack bin_pack;
		bin_pack.int64_push(id);
		bin_pack.string_push(name);
		bin_pack.token_push(std::vector<char>(flag.begin(),flag.end()));
		from_anonymous_base64 = base64_encode((const unsigned char *)(&(bin_pack.content[0])),bin_pack.content.size());

	}

	std::string  msg = root["message"].asString();

	__int32 ret = fun_ptr(1,root["message_id"].asInt(),root["group_id"].asInt64(),root["user_id"].asInt64(),from_anonymous_base64.c_str(),msg.c_str(),root["font"].asInt());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_discuss_message)
{
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_upload)
{
	GET_FUNPTR(event_group_upload)

	std::string file_base64;

	Json::Value file = root["file"];

	if(file.isObject())
	{
		std::string id = file["id"].asString();
		std::string name = to_gbk(file["name"].asString());
		__int64 size = file["size"].asInt64();
		__int64 busid = file["busid"].asInt64();
		BinPack bin_pack;
		bin_pack.string_push(id);
		bin_pack.string_push(name);
		bin_pack.int64_push(size);
		bin_pack.int64_push(busid);
		file_base64 = base64_encode((const unsigned char *)(&(bin_pack.content[0])),bin_pack.content.size());

	}
	__int32 ret = fun_ptr(1, root["time"].asInt(),root["group_id"].asInt64(),root["user_id"].asInt64(),file_base64.c_str());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_admin)
{
	GET_FUNPTR(event_group_admin)
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "set")
	{
		sub_type = 2;
	}else
	{
		sub_type = 1;
	}
	__int32 ret = fun_ptr(sub_type,root["time"].asInt(),root["group_id"].asInt64(),root["user_id"].asInt64());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_member_decrease)
{
	GET_FUNPTR(event_group_member_decrease)
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "leave")
	{
		sub_type = 1;
	}else if(subtype == "kick")
	{
		sub_type = 2;
	}else
	{
		sub_type = 3; //kick self
	}
	__int32 ret = fun_ptr(sub_type,root["time"].asInt(),root["group_id"].asInt64(),root["user_id"].asInt64(),root["operator_id"].asInt64());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_member_increase)
{
	GET_FUNPTR(event_group_member_increase)
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "approve")
	{
		sub_type = 1;
	}else
	{
		sub_type = 2; //invite
	}
	__int32 ret = fun_ptr(sub_type,root["time"].asInt(),root["group_id"].asInt64(),root["operator_id"].asInt64(),root["user_id"].asInt64());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_ban)
{
	GET_FUNPTR(event_group_ban)
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "lift_ban")
	{
		sub_type = 1;
	}else
	{
		sub_type = 2; //ban
	}
	__int32 ret = fun_ptr(sub_type,root["time"].asInt(),root["group_id"].asInt64(),root["user_id"].asInt64(),root["operator_id"].asInt64(),root["duration"].asInt64());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_friend_add)
{
	GET_FUNPTR(event_friend_add)
	__int32 ret = fun_ptr(1,root["time"].asInt(),root["user_id"].asInt64());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_recall)
{
	GET_FUNPTR(event_group_recall)
	MsgIdConvert * msgid_convert = MsgIdConvert::getInstance();
	assert(msgid_convert);
	__int32 ret = fun_ptr(root["group_id"].asInt64(),root["user_id"].asInt64(),root["operator_id"].asInt64(),msgid_convert->get_cq(root["message_id"].asInt()));
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_friend_recall)
{
	GET_FUNPTR(event_friend_recall)
	MsgIdConvert * msgid_convert = MsgIdConvert::getInstance();
	assert(msgid_convert);
	__int32 ret = fun_ptr(root["user_id"].asInt64(),msgid_convert->get_cq(root["message_id"].asInt()));
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_notify)
{
	if(root["sub_type"] == "poke")
	{
		GET_FUNPTR(event_notify_pock)
		__int32 ret = fun_ptr(root["group_id"].asInt64(),root["user_id"].asInt64(),root["target_id"].asInt64());
		if(ret == 0)
		{
			return EVENT_IGNORE;
		}
		return EVENT_BLOCK;
	}else if(root["sub_type"] == "lucky_king")
	{
		GET_FUNPTR(event_notify_lucky_king)
		__int32 ret = fun_ptr(root["group_id"].asInt64(),root["user_id"].asInt64(),root["target_id"].asInt64());
		if(ret == 0)
		{
			return EVENT_IGNORE;
		}
		return EVENT_BLOCK;
	}else if(root["sub_type"] == "honor")
	{
		
		GET_FUNPTR(event_notify_honor)
			__int32 ret = fun_ptr(root["group_id"].asInt64(),root["honor_type"].asCString(),root["user_id"].asInt64());
		if(ret == 0)
		{
			return EVENT_IGNORE;
		}
		return EVENT_BLOCK;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_friend_request)
{
	GET_FUNPTR(event_friend_request)
	std::string msg = boost::locale::conv::between(root["comment"].asString(), "GBK", "UTF-8");
	std::string response_flag = root["flag"].asString();
	__int32 ret = fun_ptr(1,root["time"].asInt(),root["user_id"].asInt64(),msg.c_str(),response_flag.c_str());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_group_request)
{
	GET_FUNPTR(event_group_request)
	std::string msg = boost::locale::conv::between(root["comment"].asString(), "GBK", "UTF-8");
	std::string response_flag = root["flag"].asString();
	std::string subtype = root["sub_type"].asString();
	__int32 sub_type;
	if(subtype == "add")
	{
		sub_type = 1;
	}else
	{
		sub_type = 2; //invite
	}
	__int32 ret = fun_ptr(sub_type,root["time"].asInt(),root["group_id"].asInt64(),root["user_id"].asInt64(),msg.c_str(),response_flag.c_str());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}

TEMP_EVENT_FUN(event_all)
{
	GET_FUNPTR(event_all)
	std::string outstr = Json::FastWriter().write(root_);
	__int32 ret = fun_ptr(to_gbk(outstr).c_str());
	if(ret == 0)
	{
		return EVENT_IGNORE;
	}
	return EVENT_BLOCK;
}





#define CALL_FUN_EVENT(X,M1,M2)  __int32 call_##X (const Json::Value & root_,boost::shared_ptr<Plus> plus) \
{ \
	Json::Value root = root_;\
	try{\
		if(root["post_type"].asString() == "message")\
		{\
			DealCqMsg(root);\
			{\
				MsgIdConvert * msgid_convert = MsgIdConvert::getInstance();\
				assert(msgid_convert);\
				root["message_id"] = msgid_convert->to_cq(root["message_id"].asInt());\
			}\
		}\
	}\
	catch(const std::exception & e)\
	{\
		BOOST_LOG_TRIVIAL(info) <<"crashed on call fun temp_call_" << #X << ":" << e.what();\
	}\
	std::map<__int32,Plus::PlusDef> plus_map = plus->get_plus_map();\
	std::map<__int32,Plus::PlusDef>::iterator iter;\
	for(iter = plus_map.begin();iter != plus_map.end();++iter)\
	{\
		if(iter->second.is_enable == false)\
		{\
			continue;\
		}\
		__int32 ret;try{\
			temp_call_event_all(iter,root,root_);\
			ret = temp_call_##X(iter,root,root_);}\
		catch(const std::exception & e){\
		BOOST_LOG_TRIVIAL(info) <<"crashed on call fun temp_call_" << #X << ":" << e.what();}\
	}\
	return 0;\
}


//CALL_FUN_EVENT(event_enable,_,_)
//CALL_FUN_EVENT(event_disable,_,_)
//CALL_FUN_EVENT(event_coolq_start,_,_)
//CALL_FUN_EVENT(event_coolq_exit,_,_)
CALL_FUN_EVENT(event_private_message,_,_)
CALL_FUN_EVENT(event_group_message,message,group)
CALL_FUN_EVENT(event_discuss_message,_,_)
CALL_FUN_EVENT(event_group_upload,_,_)
CALL_FUN_EVENT(event_group_admin,_,_)
CALL_FUN_EVENT(event_group_member_decrease,_,_)
CALL_FUN_EVENT(event_group_member_increase,_,_)
CALL_FUN_EVENT(event_group_ban,_,_)
CALL_FUN_EVENT(event_friend_add,_,_)
CALL_FUN_EVENT(event_group_recall,_,_)
CALL_FUN_EVENT(event_friend_recall,_,_)
CALL_FUN_EVENT(event_notify,_,_)
CALL_FUN_EVENT(event_friend_request,_,_)
CALL_FUN_EVENT(event_group_request,_,_)
CALL_FUN_EVENT(event_all,_,_)

#define CALL_FUNNAME(X)  call_##X

void init_event_map()
{
	//g_message_map["_" ]["_"] = CALL_FUNNAME(event_enable);
	//g_message_map["_"]["_"] = CALL_FUNNAME(event_disable);
	//g_message_map["_"]["_"] = CALL_FUNNAME(event_coolq_start);
	//g_message_map["_"]["_"] = CALL_FUNNAME(event_coolq_exit);
	g_message_map["message"]["private"] = CALL_FUNNAME(event_private_message);
	g_message_map["message"]["group"] = CALL_FUNNAME(event_group_message);
	g_message_map["message"]["_"] = CALL_FUNNAME(event_discuss_message);
	g_message_map["notice"]["group_upload"] = CALL_FUNNAME(event_group_upload);
	g_message_map["notice"]["group_admin"] = CALL_FUNNAME(event_group_admin);
	g_message_map["notice"]["group_decrease"] = CALL_FUNNAME(event_group_member_decrease);
	g_message_map["notice"]["group_increase"] = CALL_FUNNAME(event_group_member_increase);
	g_message_map["notice"]["group_ban"] = CALL_FUNNAME(event_group_ban);
	g_message_map["notice"]["friend_add"] = CALL_FUNNAME(event_friend_add);
	g_message_map["notice"]["group_recall"] = CALL_FUNNAME(event_group_recall);
	g_message_map["notice"]["friend_recall"] = CALL_FUNNAME(event_friend_recall);
	g_message_map["notice"]["notify"] = CALL_FUNNAME(event_notify);
	g_message_map["request"]["friend"] = CALL_FUNNAME(event_friend_request);
	g_message_map["request"]["group"] = CALL_FUNNAME(event_group_request);
	g_message_map["all_event"][""] = CALL_FUNNAME(event_all);

};



