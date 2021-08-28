#include "BinTool.h"

#include <algorithm>

void BinTool::string_push(const std::string& str)
{
	size_t len = str.length();
	if (len > 32767)
	{
		throw std::exception("binpack string_push error");
	}
	int16_push((__int16)len);
	for (size_t i = 0; i < str.length(); ++i)
	{
		content.push_back(str[i]);
	}
}

void BinTool::int16_push(__int16 num)
{
	content.push_back('\0');
	content.push_back('\0');
	std::memcpy(&content[content.size() - 2], &num, 2);
	std::reverse(content.end() - 2, content.end());
}

void BinTool::int64_push(__int64 num)
{
	content.push_back('\0');
	content.push_back('\0');
	content.push_back('\0');
	content.push_back('\0');
	content.push_back('\0');
	content.push_back('\0');
	content.push_back('\0');
	content.push_back('\0');
	std::memcpy(&content[content.size() - 8], &num, 8);
	std::reverse(content.end() - 8, content.end());
}

void BinTool::bool_push(bool bool_num)
{
	__int32 num = bool_num;
	content.push_back('\0');
	content.push_back('\0');
	content.push_back('\0');
	content.push_back('\0');
	std::memcpy(&content[content.size() - 4], &num, 4);
}

void BinTool::byte_push(const char* bt, size_t len)
{
	for (size_t i = 0; i < len; ++i)
	{
		content.push_back(bt[i]);
	}
}

void BinTool::token_push(const std::vector<char>& token)
{
	size_t len = token.size();
	if (len > 32767)
	{
		throw std::exception("binpack token_push error");
	}
	int16_push((__int16)len);
	if (len == 0)
	{
		return;
	}
	byte_push(&token[0], len);
}

BinTool::BinTool(const std::vector<char>& content)
{
	this->content = content;
	curr = 0;
}

BinTool::BinTool()
{
	curr = 0;
}

__int64 BinTool::int64_pop()
{
	std::vector<char> s(content.begin() + curr, content.begin() + curr + 8);
	std::reverse(s.begin(), s.end());
	__int64 result;
	std::memcpy(static_cast<void*>(&result), &s.at(0), s.size());
	curr += 8;
	return result;
}

__int16 BinTool::int16_pop()
{
	std::vector<char> s(content.begin() + curr, content.begin() + curr + 2);
	std::reverse(s.begin(), s.end());
	__int16 result;
	std::memcpy(static_cast<void*>(&result), &s.at(0), s.size());
	curr += 2;
	return result;
}

std::string BinTool::string_pop()
{
	__int16 len = int16_pop();
	if (len == 0) {
		return std::string();
	}
	std::string result;
	for (size_t i = curr; i < curr + len; ++i)
	{
		result += (content.at(i));
	}
	curr += len;
	return result;
}

void BinTool::int32_push(__int32 num)
{
	content.push_back('\0');
	content.push_back('\0');
	content.push_back('\0');
	content.push_back('\0');
	std::memcpy(&content[content.size() - 4], &num, 4);
	std::reverse(content.end() - 4, content.end());
}
