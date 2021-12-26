#pragma once

#include <string>

class EmojiTool
{
public:
	/*
	* 描述：将字符串中的 emoji 转为 cq emoji 码
	* 参数`str`：要转换的字符串
	* 返回值：转换后的字符串
	*/
	static std::string escape_cq_emoji(const std::string& str);
	/*
	* 描述：将字符串中的 cq emoji 码 转为 emoji 
	* 参数`str`：要转换的字符串
	* 返回值：转换后的字符串
	*/
	static std::string unescape_cq_emoji(const std::string& str);

private:
	EmojiTool();
};

