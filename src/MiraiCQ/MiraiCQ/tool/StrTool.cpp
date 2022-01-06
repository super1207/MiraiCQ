#include "StrTool.h"

#include <objbase.h>
#include <mutex>
#include <vector>
#include <pcre.h>
#include <set>

#pragma comment(lib,"Ole32.lib")

std::string StrTool::tolower(const std::string& str)
{
	std::string ret;
	for (const auto ch : str)
	{
		ret += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
	}
	return ret;
}

std::string StrTool::to_ansi(const std::string& utf8_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), utf8_str.size(), NULL, 0);
	std::wstring unicode_buf(len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), utf8_str.size(), unicode_buf.data(), len);
	len = WideCharToMultiByte(CP_ACP, 0, unicode_buf.data(), unicode_buf.size(), NULL, 0, NULL, NULL);
	std::string ansi_buf(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, unicode_buf.data(), unicode_buf.size(), ansi_buf.data(), len, NULL, NULL);
	return ansi_buf;
}

std::string StrTool::to_utf8(const std::string& ansi_str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, ansi_str.c_str(), ansi_str.size(), NULL, 0);
	std::wstring unicode_buf(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, ansi_str.c_str(), ansi_str.size(), unicode_buf.data(), len);
	len = WideCharToMultiByte(CP_UTF8, 0, unicode_buf.data(), unicode_buf.size(), NULL, 0, NULL, NULL);
	std::string utf8_buf(len, '\0');
	WideCharToMultiByte(CP_UTF8, 0, unicode_buf.data(), unicode_buf.size(), utf8_buf.data(), len, NULL, NULL);
	return utf8_buf;
}

std::string StrTool::gen_uuid()
{
	char buf[64] = { 0 };
	GUID guid;
	if (CoCreateGuid(&guid) != S_OK)
	{
		return "";
	}
	sprintf_s(buf,64,
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	return buf;
}

int StrTool::gen_ac()
{
	static std::mutex mx;
	static int ac = 0;
	std::lock_guard<std::mutex> lock(mx);
	++ac;
	return ac;
}

bool StrTool::end_with(const std::string& str, const std::string& end_str)
{
	return str.rfind(end_str) == str.length() - end_str.length();
}

std::string StrTool::remove_suffix(const std::string& file_str)
{
	return file_str.substr(0, file_str.rfind('.', file_str.length()));
}

std::string StrTool::get_str_from_json(const Json::Value& json, const std::string& key, const std::string& default_value)
{
	auto json_value = json.get(key, Json::Value());
	if (json_value.isString())
	{
		return json_value.asString();
	}
	return default_value;
}

int StrTool::get_int_from_json(const Json::Value& json, const std::string& key, int default_value)
{
	auto json_value = json.get(key, Json::Value());
	if (json_value.isInt())
	{
		return json_value.asInt();
	}
	return default_value;
}

int64_t StrTool::get_int64_from_json(const Json::Value& json, const std::string& key, int64_t default_value)
{
	auto json_value = json.get(key, Json::Value());
	if (json_value.isInt64())
	{
		return json_value.asInt64();
	}
	return default_value;
}

bool StrTool::get_bool_from_json(const Json::Value& json, const std::string& key, bool default_value)
{
	auto json_value = json.get(key, Json::Value());
	if (json_value.isBool())
	{
		return json_value.asBool();
	}
	return default_value;
}


//这个表来自:https://github.com/kyubotics/coolq-http-api/blob/master/src/cqsdk/message.h
const std::set<std::string> cq_code_type = {
	"text",
	"emoji",
	"face",
	"image",
	"record",
	"at",
	"rps",
	"dice",
	"anonymous",
	"shake",
	"share",
	"contact",
	"location",
	"music"
};

Json::Value StrTool::cq_str_to_jsonarr(const std::string& cq_str,int mode)
{
	// `cq_str`的规则
	// (\\[CQ:[^\\[\\],]+?(,[^\\[\\],]+?=[^\\[\\],]*?)*?\\])|([^\\[\\]]+)
	int stat = 0;//0:text 1:cqcode_type 2:cqcode_key 3:cqcode_val
	Json::Value jsonarr;
	jsonarr.resize(0);
	std::string text;
	std::string type;
	std::string key;
	std::string val;
	Json::Value cqcode = {};
	for (size_t i = 0;i < cq_str.size();++i)
	{
		char cur_ch = cq_str[i];
		if (stat == 0) //text mode
		{
			if (cur_ch == '[') // to cqcode
			{
				//[CQ:image,file=abc.jpg,url=ddd.html]
				std::string t = cq_str.substr(i, 4);
				if (t == "[CQ:")
				{
					if (text.size() != 0)
					{
						Json::Value node;
						node["type"] = "text";
						node["data"]["text"] = text;
						jsonarr.append(node);
						text.clear();
					}
					stat = 1; //to cqcode_type
					i += 3;
				}
			}
			else if (cur_ch == '&') // escape
			{
				std::string t = cq_str.substr(i, 5);
				if (t == "&#91;") // [
				{
					text += '[';
					i += 4;
				}
				else if (t == "&#93;") // ]
				{
					text += ']';
					i += 4;
				}
				else if (t == "&amp;") // &
				{
					text += '&';
					i += 4;
				}
			}
			else //normal ch
			{
				text += cq_str[i];
			}
		}
		else if (stat == 1) // cqcode_type
		{
			if (cur_ch == ',')//to cqcode_key
			{
				stat = 2;
			}
			else if (cur_ch == '&') // escape
			{
				std::string t = cq_str.substr(i, 5);
				if (t == "&#91;") // [
				{
					type += '[';
					i += 4;
				}
				else if (t == "&#93;") // ]
				{
					type += ']';
					i += 4;
				}
				else if (t == "&amp;") // &
				{
					type += '&';
					i += 4;
				}
				else if (t == "&#44;") // ,
				{
					type += ',';
					i += 4;
				}
			}
			else //normal ch
			{
				type += cq_str[i];
			}
		}
		else if (stat == 2) // cqcode_key
		{
			if (cur_ch == '=')//to cqcode_val
			{
				stat = 3;
			}
			else if (cur_ch == '&') // escape
			{
				std::string t = cq_str.substr(i, 5);
				if (t == "&#91;") // [
				{
					key += '[';
					i += 4;
				}
				else if (t == "&#93;") // ]
				{
					key += ']';
					i += 4;
				}
				else if (t == "&amp;") // &
				{
					key += '&';
					i += 4;
				}
				else if (t == "&#44;") // ,
				{
					key += ',';
					i += 4;
				}
			}
			else //normal ch
			{
				key += cq_str[i];
			}
		}
		else if (stat == 3) // cqcode_val
		{
			if (cur_ch == ']')//to cqcode
			{
				if ((mode == 0) || 
					((mode == 1) && (cq_code_type.find(type) != cq_code_type.end())))
				{
					Json::Value node;
					cqcode[key] = val;
					node["type"] = type;
					node["data"] = cqcode;
					jsonarr.append(node);
				}
				key.clear();
				val.clear();
				type.clear();
				cqcode.clear();
				stat = 0;
			}
			else if (cur_ch == ',') // to cqcode_key
			{
				cqcode[key] = val;
				key.clear();
				val.clear();
				stat = 2;
			}
			else if (cur_ch == '&') // escape
			{
				std::string t = cq_str.substr(i, 5);
				if (t == "&#91;") // [
				{
					val += '[';
					i += 4;
				}
				else if (t == "&#93;") // ]
				{
					val += ']';
					i += 4;
				}
				else if (t == "&amp;") // &
				{
					val += '&';
					i += 4;
				}
				else if (t == "&#44;") // ,
				{
					val += ',';
					i += 4;
				}
			}
			else //normal ch
			{
				val += cq_str[i];
			}
		}
	}
	if (text.size() != 0) //if not end with cqcode
	{
		Json::Value node;
		node["type"] = "text";
		node["data"]["text"] = text;
		jsonarr.append(node);
	}
	return jsonarr;
}

std::string StrTool::jsonarr_to_cq_str(const Json::Value& jsonarr,int mode)
{
	std::string ret_str;
	if (!jsonarr.isArray())
	{
		return "";
	}
	for (const auto& node : jsonarr)
	{
		if (!node.isObject())
		{
			continue;
		}
		std::string type = get_str_from_json(node, "type", "");
		auto data_json = node.get("data", Json::Value());
		if (!data_json.isObject())
		{
			continue;
		}
		if (type == "")
		{
			continue;
		}
		if ((mode == 1) && (cq_code_type.find(type) == cq_code_type.end()))
		{
			continue;
		}
		if (type == "text")
		{
			std::string text = get_str_from_json(data_json, "text", "");
			replace_all_distinct(text, "&", "&amp;");
			replace_all_distinct(text, "[", "&#91;");
			replace_all_distinct(text, "]", "&#93;");
			ret_str.append(text);
		}
		else
		{
			replace_all_distinct(type, "&", "&amp;");
			replace_all_distinct(type, "[", "&#91;");
			replace_all_distinct(type, "]", "&#93;");
			replace_all_distinct(type, ",", "&#44;");
			std::string cq_str = "[CQ:" + type;
			Json::Value::Members member = data_json.getMemberNames();
			for (std::vector<std::string>::iterator iter = member.begin(); iter != member.end(); iter++)
			{
				Json::Value dat_json = data_json[(*iter)];
				std::string dat;
				if (dat_json.isNull())
				{
					/* 空字段不处理 */
					continue;
				}
				if (dat_json.isString())
				{
					dat = dat_json.asString();
					replace_all_distinct(dat, "&", "&amp;");
					replace_all_distinct(dat, "[", "&#91;");
					replace_all_distinct(dat, "]", "&#93;");
					replace_all_distinct(dat, ",", "&#44;");
				}
				else if (dat_json.isInt())
				{
					dat = std::to_string(dat_json.asInt());
				}
				else if (dat_json.isInt64())
				{
					dat = std::to_string(dat_json.asInt64());
				}
				else if (dat_json.isUInt())
				{
					dat = std::to_string(dat_json.asUInt());
				}
				else if (dat_json.isUInt64())
				{
					dat = std::to_string(dat_json.asUInt64());
				}
				cq_str.append("," + (*iter) + "=" + dat);
			}
			cq_str.append("]");
			ret_str.append(cq_str);
		}
	}
	return ret_str;

}

void StrTool::replace_all_distinct(std::string& str, const std::string& old_value, const std::string& new_value)
{
	for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) != std::string::npos)
		{
			str.replace(pos, old_value.length(), new_value);
		}
		else
		{
			break;
		}
	}
}

std::string StrTool::get_str_from_ini(const std::string& file, const std::string& section, const std::string& key, const std::string& default_value)
{
	std::string retStr;
	char* buf = (char*)malloc(4096);
	if (!buf)
	{
		return default_value;
	}
	DWORD n = GetPrivateProfileStringA(section.c_str(), key.c_str(), default_value.c_str(), buf, 4096, file.c_str());
	if (n == 0)
	{
		free(buf);
		return default_value;
	}
	retStr = buf;
	free(buf);
	return retStr;
}

std::vector<std::string> StrTool::match(const std::string& content, const std::string& pattern)
{
	auto all = match_all(content, pattern);
	if (all.size() > 0)
	{
		return all[0];
	}
	return std::vector<std::string>();
}

std::vector<std::vector<std::string>> StrTool::match_all(const std::string& content, const std::string& pattern)
{
	const char* error;
	int  erroffset;
	int  ovector[3 * 14];
	std::vector<std::vector<std::string>> ret_vec;
	pcre* re = pcre_compile(pattern.c_str(), 0, &error, &erroffset, NULL);
	if (!re)
	{
		return {};
	}
	int offsetcount;
	offsetcount = pcre_exec(re, NULL, content.c_str(), content.length(), 0, 0, ovector, 3 * 14);
	while (offsetcount > 0)
	{

		std::vector <std::string> temp_vec;
		for (int i = 0; i < offsetcount; ++i)
		{
			const char* result = 0;
			if (pcre_get_substring(content.c_str(), ovector, offsetcount, i, &result) >= 0) {
				temp_vec.push_back(result);
				pcre_free_substring(result);

			}
		}
		ret_vec.push_back(temp_vec);
		offsetcount = pcre_exec(re, NULL, content.c_str(), content.length(), ovector[1], 0, ovector, 3 * 14);
	}
	pcre_free(re);
	return ret_vec;
}

StrTool::StrTool()
{
}
