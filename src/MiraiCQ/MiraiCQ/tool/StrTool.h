#pragma once

#include <string>
#include <jsoncpp/json.h>

class StrTool
{
public:
	/*
	* 描述：将字符串中的大写字母转为小写
	* 参数`str`：要转换的字符串
	* 返回值：转换后的字符串
	*/
	static std::string tolower(const std::string& str)noexcept;
	/*
	* 描述：将utf8字符串转为ansi
	* 参数`utf8_str`：要转换的字符串
	* 返回值：转换后的字符串
	*/
	static std::string to_ansi(const std::string& utf8_str)noexcept;
	/*
	* 描述：将ansi字符串转为utf8
	* 参数`ansi_str`：要转换的字符串
	* 返回值：转换后的字符串
	*/
	static std::string to_utf8(const std::string& ansi_str)noexcept;
	/*
	* 描述：生成uuid
	* 返回值：成功返回uuid，失败返回`""`
	*/
	static std::string gen_uuid() noexcept;

	/*
	* 描述：生成ac
	* 返回值：返回ac,不会失败,不会重复
	*/
	static int gen_ac() noexcept;

	/*
	* 描述：判断字符串是否以某字符串结尾
	* 参数`str`：要判断的字符串
	* 参数`end_str`：结尾字符串
	* 返回值：若`str`以`end_str`结尾，则返回`true`,否则返回`false`
	*/
	static bool end_with(const std::string& str, const std::string& end_str) noexcept;

	/*
	* 描述：去掉文件名后缀
	* 参数`file_str`：要去掉后缀的文件名
	* 返回值：去掉后缀后的文件名，若无后缀，则返回文件名
	*/
	static std::string remove_suffix(const std::string& file_str) noexcept;

	/*
	* 描述：从json中获取字符串
	* 参数`json`：json
	* 参数`key`：键
	* 参数`default_value`：若发生错误，返回 `default_value`
	* 返回值：`key`所对应的value
	*/
	static std::string get_str_from_json(const Json::Value & json,const std::string & key, const std::string& default_value) noexcept;

	/*
	* 描述：从json中获取int
	* 参数`json`：json
	* 参数`key`：键
	* 参数`default_value`：若发生错误，返回 `default_value`
	* 返回值：`key`所对应的value
	*/
	static int get_int_from_json(const Json::Value& json, const std::string& key, int default_value) noexcept;
	
	/*
	* 描述：从json中获取int64
	* 参数`json`：json
	* 参数`key`：键
	* 参数`default_value`：若发生错误，返回 `default_value`
	* 返回值：`key`所对应的value
	*/
	static int64_t get_int64_from_json(const Json::Value& json, const std::string& key, int64_t default_value) noexcept;

	/*
	* 描述：从json中获取bool
	* 参数`json`：json
	* 参数`key`：键
	* 参数`default_value`：若发生错误，返回 `default_value`
	* 返回值：`key`所对应的value
	*/
	static bool get_bool_from_json(const Json::Value& json, const std::string& key, bool default_value) noexcept;


	/*
	* 描述：将cq的str消息格式转换为jsonarr
	* 参数`cq_str`：要转换的cq_str
	* 返回值：成功返回jsonarr，失败返回`Json::nullValue`
	*/
	static Json::Value cq_str_to_jsonarr(const std::string & cq_str) noexcept;

	/*
	* 描述：将jsonarr转换为cq的str消息格式
	* 参数`jsonarr`：要转换的jsonarr
	* 返回值：成功返回cqstr，失败返回`""`
	*/
	static std::string jsonarr_to_cq_str(const Json::Value& jsonarr) noexcept;

	/*
	* 描述：字符串替换
	* 参数`str`：要替换的字符串
	* 参数`old_value`：旧值
	* 参数`new_value`：新值
	*/
	static void replace_all_distinct(std::string& str, const std::string& old_value, const std::string& new_value) noexcept;

	/*
	* 描述：从ini文件中读取字符串
	* 参数`file`：ini文件路径
	* 参数`section`：略
	* 参数`key`：略
	* 参数`key`：若读取失败，返回的默认值
	* 返回值：读取的字符串
	*/
	static std::string get_str_from_ini(const std::string& file, const std::string& section, const std::string& key, const std::string& default_value) noexcept;

private:
	StrTool();
};

